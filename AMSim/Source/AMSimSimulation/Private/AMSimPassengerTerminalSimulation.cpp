#include "AMSimPassengerTerminalSimulation.h"

#include "Algo/AllOf.h"
#include "Algo/Count.h"
#include "AMSimDeterminism.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase3IdBase = 1ull << 48;

		bool IsDepartureFinal(const FBagRecord& Bag)
		{
			return Bag.Direction == EPassengerDirection::Departing &&
				Bag.JourneyState == EBagJourneyState::OnAircraft;
		}

		bool IsArrivalFinal(const FBagRecord& Bag)
		{
			return Bag.Direction == EPassengerDirection::Arriving &&
				Bag.JourneyState == EBagJourneyState::Collected;
		}

		bool HasPassedSecurity(const FPassengerRecord& Passenger)
		{
			return Passenger.SecurityState == ESecurityClearanceState::Cleared ||
				Passenger.SecurityState == ESecurityClearanceState::SecondaryCleared;
		}

		template <typename RecordType, typename IdType>
		const RecordType* FindRecord(const TArray<RecordType>& Records, const IdType Id)
		{
			return Records.FindByPredicate(
				[Id](const RecordType& Record) { return Record.Id == Id; });
		}

		bool IsTerminalEditCommand(const EPhase3CommandType Type)
		{
			return Type >= EPhase3CommandType::PlaceTerminalFloor &&
				Type <= EPhase3CommandType::UndoTerminalEdit;
		}
	}

	FPassengerTerminalSimulation::FPassengerTerminalSimulation(const uint64 MasterSeed)
	{
		Reset(MasterSeed);
	}

	void FPassengerTerminalSimulation::Reset(const uint64 MasterSeed)
	{
		State = {};
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase3.PassengerTerminal"));
		State.NextDomainId = Phase3IdBase;
		State.NextEventSequence = 1;
		State.TerminalCostCredits = GetPhase3Fixture().TerminalCostCredits;
		SeedStarterTerminalLayout(true);
		PendingCommands.Reset();
	}

	EPhase3CommandResult FPassengerTerminalSimulation::QueueCommand(
		const FPhase3Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State)
	{
		const FPhase3Validation Validation = ValidateCommand(
			Command,
			CurrentGameMilliseconds,
			Phase1State,
			Phase2State);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		PendingCommands.Add(Command);
		return EPhase3CommandResult::Accepted;
	}

	FPhase3Validation FPassengerTerminalSimulation::ValidateCommand(
		const FPhase3Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State) const
	{
		FPhase3Validation Result;
		if (!Command.Id.IsValid() || CurrentGameMilliseconds < 0)
		{
			Result.Result = EPhase3CommandResult::RejectedInvalidCommand;
			Result.ReasonCode = TEXT("Phase3.InvalidCommand");
			Result.Cause = TEXT("This terminal action has no valid command identity.");
			Result.Remedy = TEXT("Try the action again.");
			return Result;
		}
		if (Command.Type == EPhase3CommandType::InitializePassengerAirport)
		{
			if (State.bInitialized)
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.ReasonCode = TEXT("Phase3.AlreadyInitialized");
				Result.Cause = TEXT("Passenger-airport systems are already active.");
				Result.Remedy = TEXT("Build the terminal shell.");
				return Result;
			}
			if (!Phase1State.bInitialized || !Phase1State.bAirportOpen ||
				!Phase2State.bInitialized)
			{
				Result.Result = EPhase3CommandResult::RejectedPhase2NotReady;
				Result.ReasonCode = TEXT("Phase3.LivingAirportRequired");
				Result.Cause = TEXT("The living airport must be operating first.");
				Result.Remedy = TEXT("Start Phase 2 and keep the airport open.");
				return Result;
			}
			Result.bValid = true;
			Result.Result = EPhase3CommandResult::Accepted;
			return Result;
		}
		if (IsTerminalEditCommand(Command.Type))
		{
			if (!Phase1State.bInitialized)
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.Cause = TEXT("Create the airport before remodeling its terminal.");
				Result.Remedy = TEXT("Finish airport creation first.");
				return Result;
			}
			if (!ValidateTerminalEdit(Command, Result))
			{
				return Result;
			}
			Result.QuotedCredits = QuoteTerminalEdit(Command);
			if (Phase1State.Credits < Result.QuotedCredits)
			{
				Result.bValid = false;
				Result.Result = EPhase3CommandResult::RejectedInsufficientCredits;
				Result.Cause = TEXT("The terminal edit is not affordable.");
				Result.Remedy = TEXT("Reduce the edit or earn more credits.");
				return Result;
			}
			return Result;
		}
		if (!State.bInitialized)
		{
			Result.Result = EPhase3CommandResult::RejectedInvalidState;
			Result.ReasonCode = TEXT("Phase3.NotInitialized");
			Result.Cause = TEXT("Passenger-airport systems are not active.");
			Result.Remedy = TEXT("Open passenger-airport planning.");
			return Result;
		}

		switch (Command.Type)
		{
		case EPhase3CommandType::FundTerminal:
			if (State.TerminalStage != ETerminalConstructionStage::None)
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.Cause = TEXT("The terminal project has already started.");
				Result.Remedy = TEXT("Wait for its current stage.");
				return Result;
			}
			if (Phase1State.Credits < State.TerminalCostCredits)
			{
				Result.Result = EPhase3CommandResult::RejectedInsufficientCredits;
				Result.Cause = TEXT("The terminal shell is not affordable.");
				Result.Remedy = TEXT("Complete flights or use recovery support.");
				Result.QuotedCredits = State.TerminalCostCredits;
				return Result;
			}
			break;

		case EPhase3CommandType::ConnectNextNetwork:
			if (State.TerminalStage != ETerminalConstructionStage::ShellReady)
			{
				Result.Result = EPhase3CommandResult::RejectedNotReady;
				Result.Cause = TEXT("The terminal shell is not ready for routes.");
				Result.Remedy = TEXT("Wait for inspection to finish.");
				return Result;
			}
			if (!State.Routes.ContainsByPredicate(
				[](const FTerminalRouteRecord& Route) { return !Route.bConnected; }))
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.Cause = TEXT("Every Phase 3 route is already connected.");
				Result.Remedy = TEXT("Open the terminal.");
				return Result;
			}
			break;

		case EPhase3CommandType::OpenTerminal:
		{
			if (State.TerminalStage != ETerminalConstructionStage::ShellReady)
			{
				Result.Result = EPhase3CommandResult::RejectedNotReady;
				Result.Cause = TEXT("The terminal is not ready to open.");
				Result.Remedy = TEXT("Finish construction and connect every flow.");
				return Result;
			}
			bool bAccessible = false;
			FString Failure;
			if (!ValidateSecurityTopology(State, bAccessible, Failure))
			{
				Result.Result = Failure.Contains(TEXT("bypass"))
					? EPhase3CommandResult::RejectedSecurityBypass
					: EPhase3CommandResult::RejectedDisconnected;
				Result.Cause = Failure;
				Result.Remedy = TEXT("Connect the highlighted controlled route.");
				return Result;
			}
			if (!bAccessible)
			{
				Result.Result = EPhase3CommandResult::RejectedNoAccessibleRoute;
				Result.Cause = TEXT("No complete accessible route reaches Gate A1.");
				Result.Remedy = TEXT("Open the accessible security lane and route.");
				return Result;
			}
			break;
		}

		case EPhase3CommandType::SchedulePassengerService:
			if (!State.bTerminalOpen ||
				State.Flight.State != EPhase3FlightState::Unscheduled)
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.Cause = State.bTerminalOpen
					? TEXT("The Phase 3 passenger service is already scheduled.")
					: TEXT("The terminal is not operational.");
				Result.Remedy = State.bTerminalOpen
					? TEXT("Follow the active passenger journey.")
					: TEXT("Open the terminal first.");
				return Result;
			}
			break;

		case EPhase3CommandType::RequestPassengerAssistance:
		{
			const FPassengerRecord* Passenger =
				FindRecord(State.Passengers, Command.PassengerId);
			if (!Passenger || !Passenger->bRequiresAccessibleRoute)
			{
				Result.Result = EPhase3CommandResult::RejectedMissingReference;
				Result.Cause = TEXT("That passenger does not need route assistance.");
				Result.Remedy = TEXT("Select the marked accessible-route party.");
				return Result;
			}
			const FPassengerPartyRecord* Party =
				FindRecord(State.Parties, Passenger->PartyId);
			if (!Party || Party->bAssistanceAssigned)
			{
				Result.Result = EPhase3CommandResult::RejectedInvalidState;
				Result.Cause = TEXT("Passenger assistance is already assigned.");
				Result.Remedy = TEXT("Follow the accessible route.");
				return Result;
			}
			break;
		}

		case EPhase3CommandType::ResolveBaggageException:
		{
			const FBagRecord* Bag = FindRecord(State.Bags, Command.BagId);
			if (!Bag || Bag->JourneyState != EBagJourneyState::Exception)
			{
				Result.Result = EPhase3CommandResult::RejectedMissingReference;
				Result.Cause = TEXT("No selected bag exception needs recovery.");
				Result.Remedy = TEXT("Inspect a bag marked Exception.");
				return Result;
			}
			break;
		}

		case EPhase3CommandType::ToggleSecurityLane:
			break;

		default:
			Result.Result = EPhase3CommandResult::RejectedInvalidCommand;
			return Result;
		}

		Result.bValid = true;
		Result.Result = EPhase3CommandResult::Accepted;
		return Result;
	}

	void FPassengerTerminalSimulation::Step(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		const FPhase2State& Phase2State)
	{
		PendingCommands.Sort(
			[](const FPhase3Command& Left, const FPhase3Command& Right)
				{
					return Left.Id.Value < Right.Id.Value;
				});
		for (const FPhase3Command& Command : PendingCommands)
		{
			ApplyCommand(Command, CurrentGameMilliseconds, Phase1);
		}
		PendingCommands.Reset();
		AdvanceTerminalConstruction(CurrentGameMilliseconds, Phase1);
		AdvanceGATerminalVisitors(CurrentGameMilliseconds, Phase1.GetState());
		if (!State.bInitialized || !Phase2State.bInitialized)
		{
			return;
		}
		AdvanceConstruction(CurrentGameMilliseconds);
		AdvancePassengers(CurrentGameMilliseconds);
		AdvanceBags(CurrentGameMilliseconds);
		AdvanceFlight(CurrentGameMilliseconds, Phase1);
		RefreshDerivedValidation();
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
	}

	void FPassengerTerminalSimulation::ApplyCommand(
		const FPhase3Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		switch (Command.Type)
		{
		case EPhase3CommandType::InitializePassengerAirport:
			InitializePassengerAirport(CurrentGameMilliseconds);
			EmitEvent(
				EPhase3EventType::PassengerAirportInitialized,
				Command.Id,
				0,
				TEXT("Passenger-airport planning is ready."),
				CurrentGameMilliseconds);
			break;

		case EPhase3CommandType::FundTerminal:
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("Phase3TerminalConstruction"),
				-State.TerminalCostCredits,
				0,
				TEXT("Domestic terminal shell and controlled-flow fit-out."),
				CurrentGameMilliseconds))
			{
				SetTerminalStage(
					ETerminalConstructionStage::Funded,
					CurrentGameMilliseconds);
			}
			break;

		case EPhase3CommandType::ConnectNextNetwork:
			for (FTerminalRouteRecord& Route : State.Routes)
			{
				if (Route.bConnected)
				{
					continue;
				}
				Route.bConnected = true;
				if (Route.Kind == ETerminalRouteKind::LandsideRoad)
				{
					for (FLandsideAccessRecord& Access : State.Landside)
					{
						Access.bRouteConnected = true;
					}
				}
				EmitEvent(
					EPhase3EventType::NetworkConnected,
					Command.Id,
					Route.Id.Value,
					FString::Printf(
						TEXT("%s connected with direction and capacity validation."),
						*Route.DefinitionId.ToString()),
					CurrentGameMilliseconds);
				break;
			}
			RefreshDerivedValidation();
			break;

		case EPhase3CommandType::OpenTerminal:
			State.bTerminalOpen = true;
			SetTerminalStage(
				ETerminalConstructionStage::Operational,
				CurrentGameMilliseconds);
			for (FTerminalRoomRecord& Room : State.Rooms)
			{
				Room.bOpen = true;
			}
			State.Tenant.bActive = true;
			EmitEvent(
				EPhase3EventType::TerminalOpened,
				Command.Id,
				0,
				TEXT("Domestic terminal opened with controlled and accessible routes."),
				CurrentGameMilliseconds);
			break;

		case EPhase3CommandType::SchedulePassengerService:
			CreatePassengerService(CurrentGameMilliseconds);
			EmitEvent(
				EPhase3EventType::PassengerServiceScheduled,
				Command.Id,
				State.Flight.Id.Value,
				TEXT("Riverbend Connect passenger service assigned to Gate A1."),
				CurrentGameMilliseconds);
			break;

		case EPhase3CommandType::ToggleSecurityLane:
			State.Checkpoint.bOpen = !State.Checkpoint.bOpen;
			EmitEvent(
				EPhase3EventType::SecurityLaneChanged,
				Command.Id,
				State.Checkpoint.Id.Value,
				State.Checkpoint.bOpen
					? TEXT("Domestic security lane opened.")
					: TEXT("Domestic security lane closed; waiting passengers remain landside."),
				CurrentGameMilliseconds);
			RefreshDerivedValidation();
			break;

		case EPhase3CommandType::RequestPassengerAssistance:
			if (FPassengerRecord* Passenger =
				State.Passengers.FindByPredicate(
					[&Command](const FPassengerRecord& Candidate)
						{
							return Candidate.Id == Command.PassengerId;
						}))
			{
				if (FPassengerPartyRecord* Party =
					State.Parties.FindByPredicate(
						[Passenger](const FPassengerPartyRecord& Candidate)
							{
								return Candidate.Id == Passenger->PartyId;
							}))
				{
					Party->bAssistanceAssigned = true;
					EmitEvent(
						EPhase3EventType::PassengerAssistanceAssigned,
						Command.Id,
						Party->Id.Value,
						TEXT("Customer-service assistance assigned to the accessible route."),
						CurrentGameMilliseconds);
				}
			}
			break;

		case EPhase3CommandType::ResolveBaggageException:
			if (FBagRecord* Bag = State.Bags.FindByPredicate(
				[&Command](const FBagRecord& Candidate)
					{
						return Candidate.Id == Command.BagId;
					}))
			{
				Bag->ExceptionReason.Reset();
				SetBagState(
					*Bag,
					Bag->Direction == EPassengerDirection::Departing
						? EBagJourneyState::Screened
						: EBagJourneyState::ArrivalInfeed,
					CurrentGameMilliseconds);
			}
			break;

		case EPhase3CommandType::PlaceTerminalFloor:
		case EPhase3CommandType::PlaceTerminalWall:
		case EPhase3CommandType::PlaceTerminalDoor:
		case EPhase3CommandType::PlaceTerminalObject:
		case EPhase3CommandType::RotateTerminalObject:
		case EPhase3CommandType::DemolishTerminalElement:
		case EPhase3CommandType::UndoTerminalEdit:
			ApplyTerminalEdit(Command, CurrentGameMilliseconds, Phase1);
			break;

		default:
			break;
		}
	}

	void FPassengerTerminalSimulation::InitializePassengerAirport(
		const int64 CurrentGameMilliseconds)
	{
		State.bInitialized = true;
		State.InitializedAtGameMilliseconds = CurrentGameMilliseconds;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		ExpandPassengerTerminalLayout(true);

		auto AddRoom = [this](
			const TCHAR* Id,
			const TCHAR* Display,
			const ETerminalZone Zone,
			const int32 Capacity,
			const bool bAccessible = true)
		{
			FTerminalRoomRecord Room;
			Room.Id = {AllocateDomainId()};
			Room.DefinitionId = Id;
			Room.DisplayName = Display;
			Room.Zone = Zone;
			Room.Capacity = Capacity;
			Room.bAccessible = bAccessible;
			State.Rooms.Add(MoveTemp(Room));
		};
		AddRoom(TEXT("Facility.Terminal.Entrance"), TEXT("Entrance"), ETerminalZone::Landside, 80);
		AddRoom(TEXT("Facility.Terminal.CheckIn"), TEXT("Check-in & bag drop"), ETerminalZone::Landside, 48);
		AddRoom(TEXT("Facility.Terminal.Security"), TEXT("Security"), ETerminalZone::Landside, 48);
		AddRoom(TEXT("Facility.Terminal.DepartureLounge"), TEXT("Departure lounge"), ETerminalZone::SterileDepartures, 90);
		AddRoom(TEXT("Facility.Terminal.GateA1"), TEXT("Gate A1"), ETerminalZone::SterileDepartures, 60);
		AddRoom(TEXT("Facility.Terminal.Arrivals"), TEXT("Arrivals corridor"), ETerminalZone::SterileArrivals, 60);
		AddRoom(TEXT("Facility.Terminal.BagMakeUp"), TEXT("Baggage make-up"), ETerminalZone::RestrictedBaggage, 36);
		AddRoom(TEXT("Facility.Terminal.Reclaim"), TEXT("Baggage reclaim"), ETerminalZone::Landside, 60);

		auto AddRoute = [this](
			const ETerminalRouteKind Kind,
			const TCHAR* Id,
			const ETerminalZone From,
			const ETerminalZone To,
			const bool bControlled,
			const int32 Capacity)
		{
			FTerminalRouteRecord Route;
			Route.Id = {AllocateDomainId()};
			Route.Kind = Kind;
			Route.DefinitionId = Id;
			Route.FromZone = From;
			Route.ToZone = To;
			Route.bControlledTransition = bControlled;
			Route.bAccessible = true;
			Route.Capacity = Capacity;
			State.Routes.Add(MoveTemp(Route));
		};
		AddRoute(
			ETerminalRouteKind::LandsideRoad,
			TEXT("Network.Landside.RoadCurbParkingBus"),
			ETerminalZone::Outside,
			ETerminalZone::Landside,
			false,
			120);
		AddRoute(
			ETerminalRouteKind::EntranceToCheckIn,
			TEXT("Network.Pedestrian.EntranceCheckIn"),
			ETerminalZone::Landside,
			ETerminalZone::Landside,
			false,
			80);
		AddRoute(
			ETerminalRouteKind::SecurityControlled,
			TEXT("Network.Controlled.DomesticSecurity"),
			ETerminalZone::Landside,
			ETerminalZone::SterileDepartures,
			true,
			48);
		AddRoute(
			ETerminalRouteKind::SterileGate,
			TEXT("Network.Pedestrian.SterileGateA1"),
			ETerminalZone::SterileDepartures,
			ETerminalZone::SterileDepartures,
			false,
			60);
		AddRoute(
			ETerminalRouteKind::ArrivalsToCurb,
			TEXT("Network.Controlled.ArrivalsReclaimCurb"),
			ETerminalZone::SterileArrivals,
			ETerminalZone::Landside,
			true,
			60);
		AddRoute(
			ETerminalRouteKind::BaggageOutbound,
			TEXT("Network.Baggage.Outbound"),
			ETerminalZone::Landside,
			ETerminalZone::RestrictedBaggage,
			true,
			40);
		AddRoute(
			ETerminalRouteKind::BaggageArrival,
			TEXT("Network.Baggage.Arrival"),
			ETerminalZone::RestrictedBaggage,
			ETerminalZone::Landside,
			true,
			40);

		auto AddTeam = [this](
			const TCHAR* Role,
			const TCHAR* Zone,
			const int32 Size)
		{
			FPhase3StaffTeamRecord Team;
			Team.Id = {AllocateDomainId()};
			Team.RoleId = Role;
			Team.ZoneId = Zone;
			Team.TeamSize = Size;
			State.Teams.Add(MoveTemp(Team));
		};
		AddTeam(TEXT("StaffRole.PassengerService"), TEXT("Zone.Terminal.Landside"), 3);
		AddTeam(TEXT("StaffRole.Security"), TEXT("Zone.Terminal.Checkpoint"), 4);
		AddTeam(TEXT("StaffRole.Baggage"), TEXT("Zone.Terminal.Baggage"), 4);
		AddTeam(TEXT("StaffRole.TerminalOperations"), TEXT("Zone.Terminal.All"), 2);

		State.Checkpoint.Id = {AllocateDomainId()};
		State.Checkpoint.DefinitionId = TEXT("Facility.Security.Domestic");
		State.Checkpoint.TeamId = State.Teams[1].Id;
		State.Checkpoint.bOpen = true;
		State.Checkpoint.bAccessibleLaneOpen = true;

		State.Tenant.Id = {AllocateDomainId()};
		State.Tenant.TenantContentId = TEXT("Tenant.RiverbendConnect");
		State.Tenant.DisplayName = TEXT("Riverbend Connect");
		State.Tenant.Requirements =
			TEXT("Operational Gate A1, domestic security, baggage, and curb access.");

		struct FLandsideSeed
		{
			ELandsideMode Mode;
			const TCHAR* FacilityId;
		};
		const FLandsideSeed LandsideSeeds[] = {
			{ELandsideMode::PrivateCar, TEXT("Facility.Landside.Parking")},
			{ELandsideMode::Taxi, TEXT("Facility.Landside.Curb")},
			{ELandsideMode::PublicBus, TEXT("Facility.Landside.BusStop")}
		};
		for (const FLandsideSeed& Entry : LandsideSeeds)
		{
			FLandsideAccessRecord Access;
			Access.Mode = Entry.Mode;
			Access.FacilityId = Entry.FacilityId;
			Access.Capacity =
				Entry.Mode == ELandsideMode::PrivateCar ? 70 :
				Entry.Mode == ELandsideMode::Taxi ? 36 : 48;
			State.Landside.Add(MoveTemp(Access));
		}
	}

	void FPassengerTerminalSimulation::SetTerminalStage(
		const ETerminalConstructionStage Stage,
		const int64 CurrentGameMilliseconds)
	{
		State.TerminalStage = Stage;
		State.TerminalStageChangedAtGameMilliseconds = CurrentGameMilliseconds;
		const bool bShellBuilt = Stage >= ETerminalConstructionStage::ShellReady;
		for (FTerminalRoomRecord& Room : State.Rooms)
		{
			Room.bBuilt = bShellBuilt;
		}
		EmitEvent(
			EPhase3EventType::TerminalStageChanged,
			{},
			0,
			FString::Printf(
				TEXT("Terminal: %s."),
				*TerminalStageDisplayName(Stage)),
			CurrentGameMilliseconds);
	}

	void FPassengerTerminalSimulation::AdvanceConstruction(
		const int64 CurrentGameMilliseconds)
	{
		const int64 Elapsed =
			CurrentGameMilliseconds - State.TerminalStageChangedAtGameMilliseconds;
		if (Elapsed < GetPhase3Fixture().ConstructionStageMilliseconds)
		{
			return;
		}
		switch (State.TerminalStage)
		{
		case ETerminalConstructionStage::Funded:
			SetTerminalStage(
				ETerminalConstructionStage::Building,
				CurrentGameMilliseconds);
			break;
		case ETerminalConstructionStage::Building:
			SetTerminalStage(
				ETerminalConstructionStage::Inspection,
				CurrentGameMilliseconds);
			break;
		case ETerminalConstructionStage::Inspection:
			SetTerminalStage(
				ETerminalConstructionStage::ShellReady,
				CurrentGameMilliseconds);
			break;
		default:
			break;
		}
	}

	void FPassengerTerminalSimulation::CreatePassengerService(
		const int64 CurrentGameMilliseconds)
	{
		State.Flight.Id = {AllocateDomainId()};
		State.Flight.FlightCode = TEXT("RB 304");
		State.Flight.OperatorContentId = TEXT("Operator.RiverbendConnect");
		State.Flight.AircraftContentId = TEXT("Aircraft.RegionalPassenger.Phase3");
		State.Flight.GateId = TEXT("Gate.A1");
		State.Flight.State = EPhase3FlightState::Scheduled;
		State.Flight.ScheduledAtGameMilliseconds = CurrentGameMilliseconds;
		State.Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		State.Flight.DepartingPassengerCount =
			GetPhase3Fixture().DepartingPassengerCount;
		State.Flight.ArrivingPassengerCount =
			GetPhase3Fixture().ArrivingPassengerCount;
		State.Flight.AcceptedDepartureBagCount =
			GetPhase3Fixture().DepartingBagCount;
		State.Flight.ArrivalBagCount = GetPhase3Fixture().ArrivingBagCount;
		CreateCohort(
			EPassengerDirection::Departing,
			State.Flight.DepartingPassengerCount,
			State.Flight.AcceptedDepartureBagCount,
			CurrentGameMilliseconds);
		CreateCohort(
			EPassengerDirection::Arriving,
			State.Flight.ArrivingPassengerCount,
			State.Flight.ArrivalBagCount,
			CurrentGameMilliseconds);
	}

	void FPassengerTerminalSimulation::CreateCohort(
		const EPassengerDirection Direction,
		const int32 PassengerCount,
		const int32 BagCount,
		const int64 CurrentGameMilliseconds)
	{
		static const TCHAR* Names[] = {
			TEXT("Maya"), TEXT("Jordan"), TEXT("Eli"), TEXT("Rowan"),
			TEXT("Avery"), TEXT("Sam"), TEXT("Nico"), TEXT("Priya"),
			TEXT("Theo"), TEXT("Lina"), TEXT("Noah"), TEXT("Zoe"),
			TEXT("Iris"), TEXT("Owen"), TEXT("Mina"), TEXT("Cal"),
			TEXT("June"), TEXT("Alex"), TEXT("Remy"), TEXT("Kai"),
			TEXT("Mila"), TEXT("Leo"), TEXT("Tess"), TEXT("Arlo"),
			TEXT("Nia"), TEXT("Finn"), TEXT("Cleo"), TEXT("Ben"),
			TEXT("Sora"), TEXT("Wren"), TEXT("Luca"), TEXT("Mae"),
			TEXT("Jules"), TEXT("Pia"), TEXT("Max"), TEXT("Ada"),
			TEXT("Rory"), TEXT("Evan"), TEXT("Skye"), TEXT("Drew"),
			TEXT("Esme"), TEXT("Hugo"), TEXT("Ruby"), TEXT("Amir"),
			TEXT("Elle"), TEXT("Dean"), TEXT("Nova"), TEXT("Cole"),
			TEXT("Lily"), TEXT("Moss"), TEXT("Aya"), TEXT("Gray")
		};
		int32 RemainingBags = BagCount;
		const int32 NameOffset =
			Direction == EPassengerDirection::Departing ? 0 : 28;
		for (int32 PartyStart = 0; PartyStart < PassengerCount; PartyStart += 4)
		{
			FPassengerPartyRecord Party;
			Party.Id = {AllocateDomainId()};
			Party.DisplayName =
				Direction == EPassengerDirection::Departing && PartyStart == 0
					? TEXT("Maya's party")
					: FString::Printf(
						TEXT("%s's party"),
						Names[(NameOffset + PartyStart) % UE_ARRAY_COUNT(Names)]);
			Party.ArrivalMode =
				static_cast<ELandsideMode>((PartyStart / 4) % 3);
			Party.bRequiresAccessibleRoute =
				Direction == EPassengerDirection::Departing && PartyStart == 0;
			Party.TimeConfidencePercent = 92 - (PartyStart / 4) * 2;
			Party.CompactNeeds = Party.bRequiresAccessibleRoute
				? TEXT("Accessible route · Food · Restroom")
				: TEXT("Information · Comfort");
			const FPassengerPartyId PartyId = Party.Id;
			State.Parties.Add(MoveTemp(Party));

			const int32 PartyEnd = FMath::Min(PartyStart + 4, PassengerCount);
			for (int32 Index = PartyStart; Index < PartyEnd; ++Index)
			{
				FPassengerRecord Passenger;
				Passenger.Id = {AllocateDomainId()};
				Passenger.PartyId = PartyId;
				Passenger.FlightId = State.Flight.Id;
				Passenger.DisplayName =
					Names[(NameOffset + Index) % UE_ARRAY_COUNT(Names)];
				Passenger.AgeBand = Index % 4 == 2 ? TEXT("Child") : TEXT("Adult");
				Passenger.Direction = Direction;
				Passenger.JourneyState =
					Direction == EPassengerDirection::Departing
						? EPassengerJourneyState::ApproachingAirport
						: EPassengerJourneyState::OnAircraft;
				Passenger.SecurityState =
					Direction == EPassengerDirection::Departing
						? ESecurityClearanceState::AwaitingScreening
						: ESecurityClearanceState::NotRequired;
				Passenger.LandsideMode = State.Parties.Last().ArrivalMode;
				Passenger.PatiencePercent = 88 - (Index % 5) * 3;
				Passenger.TimeConfidencePercent =
					State.Parties.Last().TimeConfidencePercent;
				Passenger.bRequiresAccessibleRoute =
					State.Parties.Last().bRequiresAccessibleRoute;
				Passenger.StateChangedAtGameMilliseconds =
					CurrentGameMilliseconds;
				if (RemainingBags > 0)
				{
					Passenger.BagCount = 1;
					--RemainingBags;
				}
				State.Parties.Last().Members.Add(Passenger.Id);
				State.Passengers.Add(MoveTemp(Passenger));
				if (State.Passengers.Last().BagCount > 0)
				{
					FBagRecord Bag;
					Bag.Id = {AllocateDomainId()};
					Bag.PassengerId = State.Passengers.Last().Id;
					Bag.FlightId = State.Flight.Id;
					Bag.Direction = Direction;
					Bag.JourneyState =
						Direction == EPassengerDirection::Departing
							? EBagJourneyState::Accepted
							: EBagJourneyState::OnAircraft;
					Bag.bScreened =
						Direction == EPassengerDirection::Arriving;
					Bag.StateChangedAtGameMilliseconds =
						CurrentGameMilliseconds;
					State.Bags.Add(MoveTemp(Bag));
				}
			}
		}
	}

	void FPassengerTerminalSimulation::SetPassengerState(
		FPassengerRecord& Passenger,
		const EPassengerJourneyState JourneyState,
		const int64 CurrentGameMilliseconds)
	{
		if (Passenger.JourneyState == JourneyState)
		{
			return;
		}
		Passenger.JourneyState = JourneyState;
		Passenger.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Passenger.Blocker.Reset();
		EmitEvent(
			EPhase3EventType::PassengerStateChanged,
			{},
			Passenger.Id.Value,
			FString::Printf(
				TEXT("%s: %s."),
				*Passenger.DisplayName,
				*PassengerJourneyDisplayName(JourneyState)),
			CurrentGameMilliseconds);
	}

	void FPassengerTerminalSimulation::AdvancePassengers(
		const int64 CurrentGameMilliseconds)
	{
		if (State.Flight.State == EPhase3FlightState::Unscheduled)
		{
			return;
		}
		const int64 Bucket = GetPhase3Fixture().JourneyBucketMilliseconds;
		for (FPassengerRecord& Passenger : State.Passengers)
		{
			if (CurrentGameMilliseconds - Passenger.StateChangedAtGameMilliseconds <
				Bucket)
			{
				continue;
			}
			if (Passenger.Direction == EPassengerDirection::Departing)
			{
				switch (Passenger.JourneyState)
				{
				case EPassengerJourneyState::ApproachingAirport:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::LandsideEntry,
						CurrentGameMilliseconds);
					for (FLandsideAccessRecord& Access : State.Landside)
					{
						if (Access.Mode == Passenger.LandsideMode)
						{
							++Access.ArrivedPassengerCount;
							break;
						}
					}
					break;
				case EPassengerJourneyState::LandsideEntry:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::CheckInBagDrop,
						CurrentGameMilliseconds);
					break;
				case EPassengerJourneyState::CheckInBagDrop:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::SecurityQueue,
						CurrentGameMilliseconds);
					break;
				case EPassengerJourneyState::SecurityQueue:
					if (State.Checkpoint.bOpen &&
						State.bSecurityIntegrityValid &&
						(!Passenger.bRequiresAccessibleRoute ||
							State.Checkpoint.bAccessibleLaneOpen))
					{
						SetPassengerState(
							Passenger,
							EPassengerJourneyState::Screening,
							CurrentGameMilliseconds);
					}
					else
					{
						Passenger.Blocker =
							Passenger.bRequiresAccessibleRoute
								? TEXT("Accessible security lane is closed.")
								: TEXT("Domestic security lane is closed.");
						Passenger.StateChangedAtGameMilliseconds =
							CurrentGameMilliseconds;
					}
					break;
				case EPassengerJourneyState::Screening:
					Passenger.SecurityState =
						Passenger.Id.Value % 11 == 0
							? ESecurityClearanceState::SecondaryCleared
							: ESecurityClearanceState::Cleared;
					Passenger.bUsedAccessibleRoute =
						Passenger.bRequiresAccessibleRoute;
					++State.Checkpoint.ProcessedCount;
					if (Passenger.SecurityState ==
						ESecurityClearanceState::SecondaryCleared)
					{
						++State.Checkpoint.SecondaryCount;
					}
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::GateArea,
						CurrentGameMilliseconds);
					break;
				case EPassengerJourneyState::GateArea:
				case EPassengerJourneyState::WaitingForRoute:
					if (HasPassedSecurity(Passenger) &&
						(State.Flight.State == EPhase3FlightState::Boarding ||
							State.Flight.State == EPhase3FlightState::AtGate))
					{
						SetPassengerState(
							Passenger,
							EPassengerJourneyState::Boarding,
							CurrentGameMilliseconds);
					}
					break;
				case EPassengerJourneyState::Boarding:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::OnAircraft,
						CurrentGameMilliseconds);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (Passenger.JourneyState)
				{
				case EPassengerJourneyState::OnAircraft:
					if (State.Flight.State >= EPhase3FlightState::AtGate)
					{
						SetPassengerState(
							Passenger,
							EPassengerJourneyState::ArrivalsCorridor,
							CurrentGameMilliseconds);
					}
					break;
				case EPassengerJourneyState::ArrivalsCorridor:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::BaggageReclaim,
						CurrentGameMilliseconds);
					break;
				case EPassengerJourneyState::BaggageReclaim:
				{
					const bool bOwnedBagPending = State.Bags.ContainsByPredicate(
						[&Passenger](const FBagRecord& Bag)
							{
								return Bag.PassengerId == Passenger.Id &&
									Bag.JourneyState != EBagJourneyState::Collected;
							});
					if (!bOwnedBagPending)
					{
						SetPassengerState(
							Passenger,
							EPassengerJourneyState::GroundTransport,
							CurrentGameMilliseconds);
					}
					break;
				}
				case EPassengerJourneyState::GroundTransport:
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::Completed,
						CurrentGameMilliseconds);
					for (FLandsideAccessRecord& Access : State.Landside)
					{
						if (Access.Mode == Passenger.LandsideMode)
						{
							++Access.DepartedPassengerCount;
							break;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		State.CompletedPassengerCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.JourneyState ==
						EPassengerJourneyState::Completed;
				});
	}

	void FPassengerTerminalSimulation::SetBagState(
		FBagRecord& Bag,
		const EBagJourneyState JourneyState,
		const int64 CurrentGameMilliseconds)
	{
		if (Bag.JourneyState == JourneyState)
		{
			return;
		}
		Bag.JourneyState = JourneyState;
		Bag.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		if (JourneyState == EBagJourneyState::Screened)
		{
			Bag.bScreened = true;
		}
		Bag.bReconciled = IsDepartureFinal(Bag) || IsArrivalFinal(Bag);
		EmitEvent(
			EPhase3EventType::BagStateChanged,
			{},
			Bag.Id.Value,
			TEXT("A checked bag advanced through its visible route."),
			CurrentGameMilliseconds);
	}

	void FPassengerTerminalSimulation::AdvanceBags(
		const int64 CurrentGameMilliseconds)
	{
		const int64 Bucket = GetPhase3Fixture().JourneyBucketMilliseconds;
		for (FBagRecord& Bag : State.Bags)
		{
			if (CurrentGameMilliseconds - Bag.StateChangedAtGameMilliseconds <
				Bucket)
			{
				continue;
			}
			if (Bag.Direction == EPassengerDirection::Departing)
			{
				switch (Bag.JourneyState)
				{
				case EBagJourneyState::Accepted:
					SetBagState(Bag, EBagJourneyState::Conveyor, CurrentGameMilliseconds);
					break;
				case EBagJourneyState::Conveyor:
					SetBagState(Bag, EBagJourneyState::Screened, CurrentGameMilliseconds);
					break;
				case EBagJourneyState::Screened:
					SetBagState(Bag, EBagJourneyState::Sorted, CurrentGameMilliseconds);
					break;
				case EBagJourneyState::Sorted:
					SetBagState(Bag, EBagJourneyState::MakeUp, CurrentGameMilliseconds);
					break;
				case EBagJourneyState::MakeUp:
					SetBagState(Bag, EBagJourneyState::OnAircraft, CurrentGameMilliseconds);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (Bag.JourneyState)
				{
				case EBagJourneyState::OnAircraft:
					if (State.Flight.State >= EPhase3FlightState::AtGate)
					{
						SetBagState(
							Bag,
							EBagJourneyState::ArrivalInfeed,
							CurrentGameMilliseconds);
					}
					break;
				case EBagJourneyState::ArrivalInfeed:
					SetBagState(Bag, EBagJourneyState::Reclaim, CurrentGameMilliseconds);
					break;
				case EBagJourneyState::Reclaim:
					SetBagState(Bag, EBagJourneyState::Collected, CurrentGameMilliseconds);
					break;
				default:
					break;
				}
			}
		}
		State.CompletedBagCount = Algo::CountIf(
			State.Bags,
			[](const FBagRecord& Bag)
				{
					return IsDepartureFinal(Bag) || IsArrivalFinal(Bag);
				});
	}

	void FPassengerTerminalSimulation::AdvanceFlight(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		if (State.Flight.State == EPhase3FlightState::Unscheduled)
		{
			return;
		}
		const int64 Elapsed =
			CurrentGameMilliseconds - State.Flight.StateChangedAtGameMilliseconds;
		const int64 Bucket = GetPhase3Fixture().JourneyBucketMilliseconds;
		auto SetFlightState = [this, CurrentGameMilliseconds](
			const EPhase3FlightState NewState)
		{
			State.Flight.State = NewState;
			State.Flight.StateChangedAtGameMilliseconds =
				CurrentGameMilliseconds;
		};
		if (State.Flight.State == EPhase3FlightState::Scheduled &&
			Elapsed >= Bucket)
		{
			SetFlightState(EPhase3FlightState::AtGate);
		}
		else if (State.Flight.State == EPhase3FlightState::AtGate)
		{
			const bool bAllDepartureAtGate = !State.Passengers.ContainsByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.Direction ==
								EPassengerDirection::Departing &&
							Passenger.JourneyState <
								EPassengerJourneyState::GateArea;
					});
			if (bAllDepartureAtGate)
			{
				SetFlightState(EPhase3FlightState::Boarding);
			}
		}
		else if (State.Flight.State == EPhase3FlightState::Boarding)
		{
			const int32 Boarded = Algo::CountIf(
				State.Passengers,
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.Direction ==
								EPassengerDirection::Departing &&
							Passenger.JourneyState ==
								EPassengerJourneyState::OnAircraft;
					});
			const int32 ArrivalsComplete = Algo::CountIf(
				State.Passengers,
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.Direction ==
								EPassengerDirection::Arriving &&
							Passenger.JourneyState ==
								EPassengerJourneyState::Completed;
					});
			const int32 DepartureBags = Algo::CountIf(State.Bags, IsDepartureFinal);
			const int32 ArrivalBags = Algo::CountIf(State.Bags, IsArrivalFinal);
			State.Flight.bPassengerReconciled =
				Boarded == State.Flight.DepartingPassengerCount &&
				ArrivalsComplete == State.Flight.ArrivingPassengerCount;
			State.Flight.bBagReconciled =
				DepartureBags == State.Flight.AcceptedDepartureBagCount &&
				ArrivalBags == State.Flight.ArrivalBagCount;
			if (State.Flight.bPassengerReconciled &&
				State.Flight.bBagReconciled)
			{
				++State.ReconciliationPassCount;
				SetFlightState(EPhase3FlightState::ReadyToDepart);
				EmitEvent(
					EPhase3EventType::ReconciliationPassed,
					{},
					State.Flight.Id.Value,
					TEXT("Passenger and checked-bag reconciliation is exact."),
					CurrentGameMilliseconds);
			}
		}
		else if (State.Flight.State == EPhase3FlightState::ReadyToDepart &&
			Elapsed >= Bucket)
		{
			SetFlightState(EPhase3FlightState::Outbound);
		}
		else if (State.Flight.State == EPhase3FlightState::Outbound &&
			Elapsed >= Bucket)
		{
			SetFlightState(EPhase3FlightState::Completed);
			for (FPassengerRecord& Passenger : State.Passengers)
			{
				if (Passenger.Direction == EPassengerDirection::Departing &&
					Passenger.JourneyState ==
						EPassengerJourneyState::OnAircraft)
				{
					SetPassengerState(
						Passenger,
						EPassengerJourneyState::Completed,
						CurrentGameMilliseconds);
				}
			}
			State.CompletedPassengerCount = State.Passengers.Num();
			if (!State.Flight.bRewardRecognized &&
				Phase1.ApplyExternalEconomyChange(
					TEXT("Phase3PassengerFlightRevenue"),
					GetPhase3Fixture().PassengerFlightRewardCredits,
					1,
					TEXT("Domestic passenger arrival and departure completed."),
					CurrentGameMilliseconds))
			{
				State.Flight.bRewardRecognized = true;
				State.TotalPassengerRevenueCredits +=
					GetPhase3Fixture().PassengerFlightRewardCredits;
				EmitEvent(
					EPhase3EventType::EconomyApplied,
					{},
					State.Flight.Id.Value,
					TEXT("Passenger-flight revenue and Airport Point recorded."),
					CurrentGameMilliseconds);
			}
			EmitEvent(
				EPhase3EventType::PassengerFlightCompleted,
				{},
				State.Flight.Id.Value,
				TEXT("RB 304 completed with every passenger and bag reconciled."),
				CurrentGameMilliseconds);
		}
	}

	bool FPassengerTerminalSimulation::ValidateSecurityTopology(
		const FPhase3State& Candidate,
		bool& bHasAccessibleRoute,
		FString& Failure) const
	{
		bHasAccessibleRoute = false;
		if (!Candidate.Checkpoint.Id.IsValid() ||
			!Candidate.Checkpoint.bOpen)
		{
			Failure = TEXT("The domestic security checkpoint is closed.");
			return false;
		}
		for (const FTerminalRouteRecord& Route : Candidate.Routes)
		{
			if (Route.bIntroducesSecurityBypass)
			{
				Failure = TEXT("A door creates a sterile-area security bypass.");
				return false;
			}
			if (Route.bConnected &&
				Route.FromZone == ETerminalZone::Landside &&
				Route.ToZone == ETerminalZone::SterileDepartures &&
				!Route.bControlledTransition)
			{
				Failure = TEXT("A landside route bypasses domestic security.");
				return false;
			}
		}
		for (int32 Kind = static_cast<int32>(ETerminalRouteKind::LandsideRoad);
			Kind <= static_cast<int32>(ETerminalRouteKind::BaggageArrival);
			++Kind)
		{
			if (!Candidate.Routes.ContainsByPredicate(
				[Kind](const FTerminalRouteRecord& Route)
					{
						return static_cast<int32>(Route.Kind) == Kind &&
							Route.bConnected;
					}))
			{
				Failure = TEXT("A required passenger or baggage route is disconnected.");
				return false;
			}
		}
		const bool bAccessibleLinks = Algo::AllOf(
			Candidate.Routes,
			[](const FTerminalRouteRecord& Route)
				{
					return !Route.bConnected || Route.bAccessible;
				});
		bHasAccessibleRoute =
			bAccessibleLinks && Candidate.Checkpoint.bAccessibleLaneOpen;
		Failure.Reset();
		return true;
	}

	void FPassengerTerminalSimulation::RefreshDerivedValidation()
	{
		bool bAccessible = false;
		FString Failure;
		State.bSecurityIntegrityValid =
			ValidateSecurityTopology(State, bAccessible, Failure);
		State.bAccessibleRouteValid =
			State.bSecurityIntegrityValid && bAccessible;
	}

	bool FPassengerTerminalSimulation::ValidateReconciliation(
		const FPhase3State& Candidate,
		FString& Failure) const
	{
		TSet<uint64> PassengerIds;
		TMap<uint64, int32> PartyMemberCounts;
		for (const FPassengerRecord& Passenger : Candidate.Passengers)
		{
			if (!Passenger.Id.IsValid() ||
				PassengerIds.Contains(Passenger.Id.Value) ||
				!FindRecord(Candidate.Parties, Passenger.PartyId) ||
				(Candidate.Flight.Id.IsValid() &&
					Passenger.FlightId != Candidate.Flight.Id))
			{
				Failure = TEXT("Passenger identity, party, or flight ownership is invalid.");
				return false;
			}
			PassengerIds.Add(Passenger.Id.Value);
			PartyMemberCounts.FindOrAdd(Passenger.PartyId.Value)++;
			if (Passenger.Direction == EPassengerDirection::Departing &&
				Passenger.JourneyState >= EPassengerJourneyState::GateArea &&
				Passenger.JourneyState <= EPassengerJourneyState::OnAircraft &&
				!HasPassedSecurity(Passenger))
			{
				Failure = TEXT("A departing passenger crossed the secure boundary without clearance.");
				return false;
			}
			if (Passenger.bRequiresAccessibleRoute &&
				Passenger.JourneyState >= EPassengerJourneyState::GateArea &&
				!Passenger.bUsedAccessibleRoute)
			{
				Failure = TEXT("A mobility-restricted passenger did not use an accessible route.");
				return false;
			}
		}
		for (const FPassengerPartyRecord& Party : Candidate.Parties)
		{
			TSet<uint64> ListedMembers;
			for (const FPassengerId Member : Party.Members)
			{
				if (!PassengerIds.Contains(Member.Value) ||
					ListedMembers.Contains(Member.Value))
				{
					Failure = TEXT("A party contains a missing or duplicate passenger.");
					return false;
				}
				ListedMembers.Add(Member.Value);
			}
			if (ListedMembers.Num() != PartyMemberCounts.FindRef(Party.Id.Value))
			{
				Failure = TEXT("Party membership does not reconcile.");
				return false;
			}
		}

		TSet<uint64> BagIds;
		TMap<uint64, int32> BagsPerPassenger;
		for (const FBagRecord& Bag : Candidate.Bags)
		{
			const FPassengerRecord* Owner =
				FindRecord(Candidate.Passengers, Bag.PassengerId);
			if (!Bag.Id.IsValid() ||
				BagIds.Contains(Bag.Id.Value) ||
				!Owner ||
				Bag.FlightId != Candidate.Flight.Id ||
				Bag.Direction != Owner->Direction)
			{
				Failure = TEXT("Checked-bag identity or ownership is invalid.");
				return false;
			}
			BagIds.Add(Bag.Id.Value);
			BagsPerPassenger.FindOrAdd(Bag.PassengerId.Value)++;
		}
		for (const FPassengerRecord& Passenger : Candidate.Passengers)
		{
			if (BagsPerPassenger.FindRef(Passenger.Id.Value) != Passenger.BagCount)
			{
				Failure = TEXT("A passenger's checked-bag count does not reconcile.");
				return false;
			}
		}

		if (Candidate.Flight.Id.IsValid())
		{
			const int32 Departing = Algo::CountIf(
				Candidate.Passengers,
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.Direction == EPassengerDirection::Departing;
					});
			const int32 Arriving = Candidate.Passengers.Num() - Departing;
			const int32 DepartureBags = Algo::CountIf(
				Candidate.Bags,
				[](const FBagRecord& Bag)
					{
						return Bag.Direction == EPassengerDirection::Departing;
					});
			if (Departing != Candidate.Flight.DepartingPassengerCount ||
				Arriving != Candidate.Flight.ArrivingPassengerCount ||
				DepartureBags != Candidate.Flight.AcceptedDepartureBagCount ||
				Candidate.Bags.Num() - DepartureBags !=
					Candidate.Flight.ArrivalBagCount)
			{
				Failure = TEXT("Flight passenger or bag totals do not match their records.");
				return false;
			}
		}
		Failure.Reset();
		return true;
	}

	void FPassengerTerminalSimulation::EmitEvent(
		const EPhase3EventType Type,
		const FCommandId Cause,
		const uint64 SubjectId,
		const FString& Message,
		const int64 CurrentGameMilliseconds)
	{
		State.Events.Add({
			State.NextEventSequence++,
			CurrentGameMilliseconds,
			Type,
			Cause,
			SubjectId,
			Message
		});
	}

	uint64 FPassengerTerminalSimulation::AllocateDomainId()
	{
		return State.NextDomainId++;
	}
}
