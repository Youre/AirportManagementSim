#include "AMSimStarterAirfieldSimulation.h"
#include "AMSimContextHelp.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 FnvOffset = 1469598103934665603ull;
		constexpr uint64 FnvPrime = 1099511628211ull;

		void HashBytes(uint64& Hash, const void* Data, const SIZE_T Size)
		{
			const uint8* Bytes = static_cast<const uint8*>(Data);
			for (SIZE_T Index = 0; Index < Size; ++Index)
			{
				Hash ^= Bytes[Index];
				Hash *= FnvPrime;
			}
		}

		void HashString(uint64& Hash, const FString& Value)
		{
			FTCHARToUTF8 Utf8(*Value);
			HashBytes(Hash, Utf8.Get(), Utf8.Length());
		}

		FString ConstructionStageText(const EConstructionStage Stage)
		{
			switch (Stage)
			{
			case EConstructionStage::Funded: return TEXT("Starter airfield funded");
			case EConstructionStage::AwaitingDelivery: return TEXT("Waiting for materials and workers");
			case EConstructionStage::Building: return TEXT("Construction team is building the airfield");
			case EConstructionStage::Inspection: return TEXT("Automatic safety and connection inspection");
			case EConstructionStage::ReadyToOpen: return TEXT("Starter airfield is ready to open");
			case EConstructionStage::Operational: return TEXT("Starter airfield is operational");
			default: return TEXT("Plan the starter grass airfield");
			}
		}

		FString FlightStateText(const EFlightState State)
		{
			switch (State)
			{
			case EFlightState::Scheduled: return TEXT("First GA visit is scheduled");
			case EFlightState::Inbound: return TEXT("Riverbend 21 is inbound");
			case EFlightState::Approach: return TEXT("Riverbend 21 is on final approach");
			case EFlightState::Landing: return TEXT("Riverbend 21 is landing");
			case EFlightState::RunwayRoll: return TEXT("Riverbend 21 is slowing on Runway 09");
			case EFlightState::TaxiIn: return TEXT("Riverbend 21 is taxiing to Stand A1");
			case EFlightState::Parked: return TEXT("Riverbend 21 is parked at Stand A1");
			case EFlightState::Turnaround: return TEXT("Inspection and fueling are in progress");
			case EFlightState::Ready: return TEXT("Riverbend 21 is ready to depart");
			case EFlightState::TaxiOut: return TEXT("Riverbend 21 is taxiing to Runway 09");
			case EFlightState::Takeoff: return TEXT("Riverbend 21 is taking off");
			case EFlightState::Outbound: return TEXT("Riverbend 21 is outbound");
			case EFlightState::Completed: return TEXT("First aircraft visit complete");
			default: return TEXT("No flight scheduled");
			}
		}
	}

	FStarterAirfieldSimulation::FStarterAirfieldSimulation(const uint64 MasterSeed)
	{
		Reset(MasterSeed);
	}

	void FStarterAirfieldSimulation::Reset(const uint64 MasterSeed)
	{
		State = {};
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase1.StarterAirfield"));
		State.Credits = GetPhase1Fixture().StartingCredits;
		PendingCommands.Reset();
	}

	EPhase1CommandResult FStarterAirfieldSimulation::QueueCommand(
		const FPhase1Command& Command,
		const int64 CurrentGameMilliseconds)
	{
		const FPhase1Validation Validation = ValidateCommand(Command, CurrentGameMilliseconds);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		PendingCommands.Add(Command);
		return EPhase1CommandResult::Accepted;
	}

	FPhase1Validation FStarterAirfieldSimulation::ValidateCommand(
		const FPhase1Command& Command,
		const int64 CurrentGameMilliseconds) const
	{
		FPhase1Validation Result;
		if (!Command.Id.IsValid())
		{
			Result.Result = EPhase1CommandResult::RejectedInvalidCommand;
			return Result;
		}

		switch (Command.Type)
		{
		case EPhase1CommandType::CreateAirport:
			if (State.bInitialized)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			if (Command.AirportName.TrimStartAndEnd().IsEmpty() || Command.AirportName.Len() > 40)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidName;
				return Result;
			}
			if (Command.MapId != GetPhase1Fixture().MapId)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidCommand;
				return Result;
			}
			break;

		case EPhase1CommandType::CommitStarterPlan:
			if (!State.bInitialized || State.Project.Stage != EConstructionStage::None)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			Result = ValidateStarterPlan(Command.Proposal);
			if (!Result.bValid)
			{
				return Result;
			}
			if (State.Credits < Result.QuotedCost)
			{
				Result.bValid = false;
				Result.Result = EPhase1CommandResult::RejectedInsufficientCredits;
				return Result;
			}
			break;

		case EPhase1CommandType::CancelStarterPlan:
			if (State.Project.Stage != EConstructionStage::Funded &&
				State.Project.Stage != EConstructionStage::AwaitingDelivery)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;

		case EPhase1CommandType::OpenAirport:
			if (State.Project.Stage != EConstructionStage::ReadyToOpen || State.bAirportOpen)
			{
				Result.Result = EPhase1CommandResult::RejectedNotReady;
				return Result;
			}
			break;

		case EPhase1CommandType::CloseAirport:
			if (!State.bAirportOpen ||
				State.Project.Stage != EConstructionStage::Operational ||
				(State.Flight.State != EFlightState::None &&
					State.Flight.State != EFlightState::Completed))
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;

		case EPhase1CommandType::PinStarterOffer:
		case EPhase1CommandType::DeclineStarterOffer:
			if (State.Offer.State != EOfferState::Available)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;

		case EPhase1CommandType::AcceptStarterOffer:
			if (State.Offer.State != EOfferState::Available)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;

		case EPhase1CommandType::ScheduleStarterFlight:
			if (State.Offer.State != EOfferState::Accepted)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			if (Command.RequestedStandDefinitionId != TEXT("Facility.GAStand.Starter"))
			{
				Result.Result = EPhase1CommandResult::RejectedIncompatible;
				return Result;
			}
			if (Command.ScheduledArrivalGameMilliseconds < CurrentGameMilliseconds ||
				Command.ScheduledArrivalGameMilliseconds % GetPhase1Fixture().TimetableIncrementMilliseconds != 0 ||
				Command.ScheduledArrivalGameMilliseconds - CurrentGameMilliseconds >
					GetPhase1Fixture().MaximumArrivalDelayMilliseconds)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidSchedule;
				return Result;
			}
			break;

		case EPhase1CommandType::RequestRecovery:
			if (!State.bAirportOpen || State.Credits != 0 || State.RecoveryGrantCount > 0)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;

		case EPhase1CommandType::SetSpeed:
			if (Command.SpeedMultiplier != 1 &&
				Command.SpeedMultiplier != 2 &&
				Command.SpeedMultiplier != 4 &&
				Command.SpeedMultiplier != 8)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidCommand;
				return Result;
			}
			break;

		case EPhase1CommandType::SetPaused:
			break;

		case EPhase1CommandType::AcknowledgeContextHelp:
			if (!FindPhase1ContextHelp(Command.ContextHelpId))
			{
				Result.Result =
					EPhase1CommandResult::RejectedInvalidCommand;
				return Result;
			}
			if (State.AcknowledgedContextHelp.Contains(
				Command.ContextHelpId))
			{
				Result.Result =
					EPhase1CommandResult::RejectedInvalidState;
				return Result;
			}
			break;
		}

		Result.bValid = true;
		Result.Result = EPhase1CommandResult::Accepted;
		return Result;
	}

	void FStarterAirfieldSimulation::Step(const int64 CurrentGameMilliseconds)
	{
		PendingCommands.Sort([](const FPhase1Command& Left, const FPhase1Command& Right)
		{
			return Left.Id.Value < Right.Id.Value;
		});
		for (const FPhase1Command& Command : PendingCommands)
		{
			ApplyCommand(Command, CurrentGameMilliseconds);
		}
		PendingCommands.Reset();
		AdvanceConstruction(CurrentGameMilliseconds);
		AdvanceFlight(CurrentGameMilliseconds);
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
	}

	void FStarterAirfieldSimulation::ApplyCommand(
		const FPhase1Command& Command,
		const int64 CurrentGameMilliseconds)
	{
		const FPhase1Fixture& Fixture = GetPhase1Fixture();
		switch (Command.Type)
		{
		case EPhase1CommandType::CreateAirport:
			State.bInitialized = true;
			State.bPaused = true;
			State.SpeedMultiplier = 0;
			State.AirportId = {AllocateDomainId()};
			State.MapId = Command.MapId;
			State.AirportName = Command.AirportName.TrimStartAndEnd();
			State.Objectives.Add({
				{AllocateDomainId()},
				TEXT("Objective.BuildStarterAirfield"),
				TEXT("Build a grass runway and connect taxiways to a terminal gate."),
				false
			});
			EmitEvent(
				EPhase1EventType::AirportCreated,
				Command.Id,
				TEXT("Temperate starter airport created while paused."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::CommitStarterPlan:
		{
			const FPhase1Validation Validation =
				ValidateStarterPlan(Command.Proposal);
			State.Project.Id = {AllocateDomainId()};
			State.Project.Proposal =
				NormalizeStarterPlanConnections(Command.Proposal);
			State.Project.QuotedCost = Validation.QuotedCost;
			State.Project.FundedAtGameMilliseconds = CurrentGameMilliseconds;
			State.Project.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
			State.Project.Stage = EConstructionStage::Funded;
			State.Credits -= Validation.QuotedCost;
			RecordTransaction(
				TEXT("Construction"),
				-Validation.QuotedCost,
				TEXT("Reserved for the complete starter grass-airfield project."),
				CurrentGameMilliseconds);
			EmitEvent(
				EPhase1EventType::ProjectCommitted,
				Command.Id,
				TEXT("Starter plan committed with deterministic geometry and price."),
				CurrentGameMilliseconds);
			ChangeConstructionStage(
				EConstructionStage::AwaitingDelivery,
				CurrentGameMilliseconds,
				TEXT("Materials and the construction team are on the way."));
			break;
		}

		case EPhase1CommandType::CancelStarterPlan:
		{
			const int64 Refund = State.Project.QuotedCost;
			State.Credits += Refund;
			State.Project = {};
			State.Facilities.Reset();
			State.Teams.Reset();
			for (FObjectiveRecord& Objective : State.Objectives)
			{
				if (Objective.ObjectiveId == TEXT("Objective.BuildStarterAirfield"))
				{
					Objective.bCompleted = false;
				}
			}
			RecordTransaction(
				TEXT("ConstructionRefund"),
				Refund,
				TEXT("Full refund: starter project cancelled before materials arrived."),
				CurrentGameMilliseconds);
			EmitEvent(
				EPhase1EventType::ProjectCancelled,
				Command.Id,
				TEXT("Starter project cancelled before delivery; all reserved Credits returned."),
				CurrentGameMilliseconds);
			break;
		}

		case EPhase1CommandType::OpenAirport:
		{
			const FRunwayDesignation Designation = CalculateRunwayDesignation(
				State.Project.Proposal.RunwayStart,
				State.Project.Proposal.RunwayEnd);
			State.bAirportOpen = true;
			State.bPaused = true;
			State.SpeedMultiplier = 0;
			ChangeConstructionStage(
				EConstructionStage::Operational,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("Runway %02d/%02d and the terminal-gate taxi network are open."),
					Designation.PrimaryNumber,
					Designation.ReciprocalNumber));
			for (FFacilityRecord& Facility : State.Facilities)
			{
				Facility.bOpen = true;
				Facility.Status = TEXT("Valid and open");
			}
			State.Offer.ContractId = {AllocateDomainId()};
			State.Offer.State = EOfferState::Available;
			State.Offer.AircraftContentId = Fixture.AircraftId;
			State.Offer.OperatorContentId = Fixture.OperatorId;
			State.Offer.CompatibilitySummary =
				TEXT("Compatible: grass runway, connected terminal gate, inspection, and fuel available.");
			State.Offer.RewardCredits = Fixture.FlightRewardCredits;
			State.Offer.bPinned = false;
			EmitEvent(
				EPhase1EventType::FacilityOpened,
				Command.Id,
				TEXT("Starter airfield opened."),
				CurrentGameMilliseconds);
			EmitEvent(
				EPhase1EventType::OfferAvailable,
				Command.Id,
				TEXT("Riverbend Flying Club offer is available immediately while paused."),
				CurrentGameMilliseconds);
			break;
		}

		case EPhase1CommandType::CloseAirport:
			State.bAirportOpen = false;
			State.bPaused = true;
			State.SpeedMultiplier = 0;
			State.Project.Stage = EConstructionStage::ReadyToOpen;
			State.Project.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
			for (FFacilityRecord& Facility : State.Facilities)
			{
				Facility.bOpen = false;
				Facility.Status = TEXT("Valid; closed by player");
			}
			State.Offer = {};
			EmitEvent(
				EPhase1EventType::FacilityClosed,
				Command.Id,
				TEXT("Starter airfield closed safely with no active flight."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::PinStarterOffer:
			State.Offer.bPinned = true;
			EmitEvent(
				EPhase1EventType::OfferPinned,
				Command.Id,
				TEXT("Riverbend Flying Club offer pinned for review."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::DeclineStarterOffer:
			State.Offer.State = EOfferState::Declined;
			State.Offer.bPinned = false;
			EmitEvent(
				EPhase1EventType::OfferDeclined,
				Command.Id,
				TEXT("Riverbend Flying Club offer declined. Close and reopen to request a fresh starter offer."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::AcceptStarterOffer:
			State.Offer.State = EOfferState::Accepted;
			for (FObjectiveRecord& Objective : State.Objectives)
			{
				if (Objective.ObjectiveId == TEXT("Objective.OpenAirport"))
				{
					Objective.bCompleted = true;
				}
			}
			State.Objectives.Add({
				{AllocateDomainId()},
				TEXT("Objective.CompleteFirstVisit"),
				TEXT("Schedule the compatible visit and observe its safe turnaround."),
				false
			});
			EmitEvent(
				EPhase1EventType::OfferAccepted,
				Command.Id,
				TEXT("One-time GA visit accepted. Choose the recommended slot and Stand A1."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::ScheduleStarterFlight:
			State.Offer.State = EOfferState::Scheduled;
			State.Airframe.Id = {AllocateDomainId()};
			State.Airframe.AircraftContentId = Fixture.AircraftId;
			State.Airframe.OperatorContentId = Fixture.OperatorId;
			State.Airframe.TailNumber = TEXT("RB-021");
			State.Airframe.HistorySummary = TEXT("First visit scheduled.");
			State.Flight.Id = {AllocateDomainId()};
			State.Flight.ContractId = State.Offer.ContractId;
			State.Flight.AirframeId = State.Airframe.Id;
			State.Flight.State = EFlightState::Scheduled;
			State.Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
			State.Flight.ScheduledArrivalGameMilliseconds = Command.ScheduledArrivalGameMilliseconds;
			State.Flight.StandOccupancyStartGameMilliseconds =
				Command.ScheduledArrivalGameMilliseconds - 300000;
			State.Flight.StandOccupancyEndGameMilliseconds =
				Command.ScheduledArrivalGameMilliseconds +
					Fixture.FlightCompletedOffsetMilliseconds + 300000;
			State.Flight.AssignedRunway = FindFacility(EFacilityType::GrassRunway);
			State.Flight.AssignedStand = FindFacility(EFacilityType::GAStand);
			EmitEvent(
				EPhase1EventType::FlightScheduled,
				Command.Id,
				TEXT("Riverbend 21 scheduled in an exact five-minute slot at Stand A1."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::RequestRecovery:
			State.Credits += 2000;
			++State.RecoveryGrantCount;
			RecordTransaction(
				TEXT("Recovery"),
				2000,
				TEXT("Clearly labeled starter-airfield recovery grant."),
				CurrentGameMilliseconds);
			EmitEvent(
				EPhase1EventType::RecoveryGranted,
				Command.Id,
				TEXT("Recovery grant restored a safe operating path."),
				CurrentGameMilliseconds);
			break;

		case EPhase1CommandType::SetPaused:
			State.bPaused = Command.bPaused;
			if (Command.bPaused)
			{
				State.SpeedMultiplier = 0;
			}
			break;

		case EPhase1CommandType::SetSpeed:
			State.bPaused = false;
			State.SpeedMultiplier = Command.SpeedMultiplier;
			break;

		case EPhase1CommandType::AcknowledgeContextHelp:
			State.AcknowledgedContextHelp.Add(Command.ContextHelpId);
			EmitEvent(
				EPhase1EventType::ContextHelpAcknowledged,
				Command.Id,
				FString::Printf(
					TEXT("Context help acknowledged: %s"),
					*Command.ContextHelpId.ToString()),
				CurrentGameMilliseconds);
			break;
		}
	}

	void FStarterAirfieldSimulation::AdvanceConstruction(const int64 CurrentGameMilliseconds)
	{
		if (State.Project.Stage == EConstructionStage::None ||
			State.Project.Stage == EConstructionStage::ReadyToOpen ||
			State.Project.Stage == EConstructionStage::Operational)
		{
			return;
		}

		const FPhase1Fixture& Fixture = GetPhase1Fixture();
		const int64 Elapsed = CurrentGameMilliseconds - State.Project.FundedAtGameMilliseconds;
		const bool bRoadAssisted =
			HasConstructionRoadBenefit(State.Project.Proposal);
		const int32 DurationPercent = bRoadAssisted
			? 100 - Fixture.ServiceRoadConstructionBonusPercent
			: 100;
		const auto Threshold = [DurationPercent](const int64 Value)
		{
			return Value * DurationPercent / 100;
		};
		if (Elapsed >= Threshold(Fixture.ReadyToOpenAtMilliseconds))
		{
			State.Project.bInspectionPassed = true;
			const FRunwayDesignation Designation = CalculateRunwayDesignation(
				State.Project.Proposal.RunwayStart,
				State.Project.Proposal.RunwayEnd);
			State.Facilities = {
				{{AllocateDomainId()}, TEXT("Facility.GrassRunway.Starter"), EFacilityType::GrassRunway, true, false, TEXT("Valid; closed by player")},
				{{AllocateDomainId()}, TEXT("Facility.GrassTaxiway.Starter"), EFacilityType::GrassTaxiway, true, false, TEXT("Runway-to-gate network connected; closed by player")},
				{{AllocateDomainId()}, TEXT("Facility.GAStand.Starter"), EFacilityType::GAStand, true, false, TEXT("Terminal gate compatible; closed by player")},
				{{AllocateDomainId()}, TEXT("Facility.OperationsHut.Starter"), EFacilityType::OperationsHut, true, false, TEXT("Basic operations terminal ready")},
				{{AllocateDomainId()}, TEXT("Facility.MarkingsWindsock.Starter"), EFacilityType::MarkingsAndWindsock, true, false, FString::Printf(TEXT("Runway %02d/%02d marked"), Designation.PrimaryNumber, Designation.ReciprocalNumber)}
			};
			if (State.Project.Proposal.AccessStart !=
				State.Project.Proposal.AccessEnd)
			{
				State.Facilities.Add({
					{AllocateDomainId()},
					TEXT("Facility.AccessConnection.Starter"),
					EFacilityType::AccessConnection,
					true,
					false,
					bRoadAssisted
						? TEXT("Service road shortened worker travel")
						: TEXT("Service road available")});
			}
			State.Teams = {
				{{AllocateDomainId()}, TEXT("Staff.Role.Construction"), 3, true, TEXT("Starter project complete")},
				{{AllocateDomainId()}, TEXT("Staff.Role.Ramp"), 2, true, TEXT("Available for inspection")},
				{{AllocateDomainId()}, TEXT("Staff.Role.Fueling"), 1, true, TEXT("Available for fueling")}
			};
			if (!State.Objectives.IsEmpty())
			{
				State.Objectives[0].bCompleted = true;
				State.Objectives.Add({
					{AllocateDomainId()},
					TEXT("Objective.OpenAirport"),
					TEXT("Open the inspected runway and accept the first compatible visit."),
					false
				});
			}
			ChangeConstructionStage(
				EConstructionStage::ReadyToOpen,
				CurrentGameMilliseconds,
				TEXT("Inspection passed. Open the airfield when ready."));
		}
		else if (Elapsed >= Threshold(Fixture.InspectionAtMilliseconds) &&
			State.Project.Stage != EConstructionStage::Inspection)
		{
			ChangeConstructionStage(
				EConstructionStage::Inspection,
				CurrentGameMilliseconds,
				TEXT("Checking runway clearance and network connections."));
		}
		else if (Elapsed >= Threshold(Fixture.BuildingAtMilliseconds) &&
			State.Project.Stage != EConstructionStage::Building)
		{
			ChangeConstructionStage(
				EConstructionStage::Building,
				CurrentGameMilliseconds,
				bRoadAssisted
					? TEXT("Workers reached the site by service road and are building the airfield network.")
					: TEXT("Workers walked from the terminal and are building the airfield network."));
		}
		else if (Elapsed >= Threshold(Fixture.DeliveryAtMilliseconds) &&
			!State.Project.bDeliveryArrived)
		{
			State.Project.bDeliveryArrived = true;
			EmitEvent(
				EPhase1EventType::ProjectStageChanged,
				{},
				bRoadAssisted
					? TEXT("Materials and workers arrived by the optional service road.")
					: TEXT("Materials arrived; workers walked from the starter terminal."),
				CurrentGameMilliseconds);
		}
	}

	void FStarterAirfieldSimulation::AdvanceFlight(const int64 CurrentGameMilliseconds)
	{
		if (State.Flight.State == EFlightState::None ||
			State.Flight.State == EFlightState::Completed)
		{
			return;
		}

		const int64 Arrival = State.Flight.ScheduledArrivalGameMilliseconds;
		EFlightState Desired = State.Flight.State;
		for (uint8 Value = static_cast<uint8>(EFlightState::Completed);
			Value >= static_cast<uint8>(EFlightState::Inbound);
			--Value)
		{
			const EFlightState Candidate = static_cast<EFlightState>(Value);
			if (CurrentGameMilliseconds >=
				Arrival + GetPhase1FlightStateOffsetMilliseconds(Candidate))
			{
				Desired = Candidate;
				break;
			}
		}

		while (static_cast<uint8>(State.Flight.State) < static_cast<uint8>(Desired))
		{
			const EFlightState NextState = static_cast<EFlightState>(
				static_cast<uint8>(State.Flight.State) + 1);
			ChangeFlightState(NextState, CurrentGameMilliseconds);
		}
	}

	void FStarterAirfieldSimulation::ChangeConstructionStage(
		const EConstructionStage NewStage,
		const int64 CurrentGameMilliseconds,
		const FString& Message)
	{
		State.Project.Stage = NewStage;
		State.Project.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
		EmitEvent(EPhase1EventType::ProjectStageChanged, {}, Message, CurrentGameMilliseconds);
	}

	void FStarterAirfieldSimulation::ChangeFlightState(
		const EFlightState NewState,
		const int64 CurrentGameMilliseconds)
	{
		const EFlightState Previous = State.Flight.State;
		State.Flight.State = NewState;
		State.Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		State.Reservations.Reset();

		if (NewState == EFlightState::Landing || NewState == EFlightState::RunwayRoll)
		{
			State.Reservations.Add({
				TEXT("Movement.Runway09"),
				State.Airframe.Id,
				CurrentGameMilliseconds,
				State.Flight.ScheduledArrivalGameMilliseconds +
					GetPhase1FlightStateEndOffsetMilliseconds(NewState)
			});
		}
		else if (NewState == EFlightState::TaxiIn || NewState == EFlightState::TaxiOut)
		{
			State.Reservations.Add({
				TEXT("Movement.TaxiA"),
				State.Airframe.Id,
				CurrentGameMilliseconds,
				State.Flight.ScheduledArrivalGameMilliseconds +
					GetPhase1FlightStateEndOffsetMilliseconds(NewState)
			});
		}
		else if (NewState == EFlightState::Parked || NewState == EFlightState::Turnaround ||
			NewState == EFlightState::Ready)
		{
			State.Reservations.Add({
				TEXT("Stand.A1"),
				State.Airframe.Id,
				CurrentGameMilliseconds,
				State.Flight.StandOccupancyEndGameMilliseconds
			});
		}

		if (NewState == EFlightState::Inbound)
		{
			QueuePhrase(
				TEXT("Radio.ArrivalContact"),
				TEXT("Riverbend 21"),
				TEXT("Riverbend Tower, Riverbend 21, inbound for landing."),
				1,
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::Approach)
		{
			QueuePhrase(
				TEXT("Radio.LandingClearance"),
				TEXT("Riverbend Tower"),
				TEXT("Riverbend 21, Runway 09 cleared to land."),
				2,
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::TaxiIn)
		{
			QueuePhrase(
				TEXT("Radio.TaxiRoute"),
				TEXT("Riverbend Tower"),
				TEXT("Riverbend 21, taxi via Taxiway A to Stand A1."),
				1,
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::Turnaround)
		{
			State.Flight.Inspection = EServiceTaskState::Active;
			State.Flight.Fueling = EServiceTaskState::Active;
			for (FStaffTeamRecord& Team : State.Teams)
			{
				if (Team.RoleId == TEXT("Staff.Role.Ramp"))
				{
					Team.CurrentTask = TEXT("Inspecting Riverbend 21");
				}
				else if (Team.RoleId == TEXT("Staff.Role.Fueling"))
				{
					Team.CurrentTask = TEXT("Fueling Riverbend 21");
				}
			}
			EmitEvent(
				EPhase1EventType::ServiceStateChanged,
				{},
				TEXT("Inspection and fueling started with compatible teams."),
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::Ready)
		{
			State.Flight.Inspection = EServiceTaskState::Completed;
			State.Flight.Fueling = EServiceTaskState::Completed;
			EmitEvent(
				EPhase1EventType::ServiceStateChanged,
				{},
				TEXT("Inspection and fueling complete; aircraft is departure-ready."),
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::TaxiOut)
		{
			QueuePhrase(
				TEXT("Radio.TaxiDeparture"),
				TEXT("Riverbend Tower"),
				TEXT("Riverbend 21, taxi via Taxiway A, hold short of Runway 09."),
				2,
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::Takeoff)
		{
			QueuePhrase(
				TEXT("Radio.TakeoffClearance"),
				TEXT("Riverbend Tower"),
				TEXT("Riverbend 21, Runway 09 cleared for takeoff."),
				2,
				CurrentGameMilliseconds);
		}
		else if (NewState == EFlightState::Completed)
		{
			if (Previous != EFlightState::Completed && !State.Flight.bRewardRecognized)
			{
				State.Flight.bRewardRecognized = true;
				State.Airframe.VisitCount = 1;
				State.Airframe.HistorySummary =
					TEXT("Completed one safe visit with inspection and fueling at Riverbend Field.");
				State.Credits += GetPhase1Fixture().FlightRewardCredits;
				State.AirportPoints += GetPhase1Fixture().FlightRewardAirportPoints;
				State.Offer.State = EOfferState::Completed;
				RecordTransaction(
					TEXT("FlightCompletion"),
					GetPhase1Fixture().FlightRewardCredits,
					TEXT("Landing, parking, inspection, and fueling reward for Riverbend 21."),
					CurrentGameMilliseconds);
				State.RatingContributions = {
					{TEXT("SafetyReadiness"), 90, TEXT("Runway, stand, inspection, and fuel requirements passed.")},
					{TEXT("OperationalReliability"), 90, TEXT("The first visit completed without controllable delay.")}
				};
				for (FObjectiveRecord& Objective : State.Objectives)
				{
					if (Objective.ObjectiveId == TEXT("Objective.CompleteFirstVisit"))
					{
						Objective.bCompleted = true;
					}
				}
				State.Objectives.Add({
					{AllocateDomainId()},
					TEXT("Objective.ObserveReward"),
					TEXT("Review the first flight reward and aircraft history."),
					false
				});
				EmitEvent(
					EPhase1EventType::CapabilityAwarded,
					{},
					TEXT("Established GA evidence awarded: 5 Airport Points."),
					CurrentGameMilliseconds);
			}
		}

		EmitEvent(
			EPhase1EventType::FlightStateChanged,
			{},
			FlightStateText(NewState),
			CurrentGameMilliseconds);
	}

	void FStarterAirfieldSimulation::QueuePhrase(
		const FName PhraseId,
		const FString& Speaker,
		const FString& Caption,
		const int32 Priority,
		const int64 CurrentGameMilliseconds)
	{
		const FPhraseIntentId Id{AllocateDomainId()};
		State.PhraseIntents.Add({
			Id,
			CurrentGameMilliseconds,
			PhraseId,
			Speaker,
			TEXT("Riverbend 21"),
			Caption,
			Priority,
			FString::Printf(TEXT("%s-%llu"), *PhraseId.ToString(), Id.Value)
		});
		EmitEvent(
			EPhase1EventType::PhraseQueued,
			{},
			Caption,
			CurrentGameMilliseconds);
	}

	void FStarterAirfieldSimulation::RecordTransaction(
		const FName Category,
		const int64 AmountCredits,
		const FString& Explanation,
		const int64 CurrentGameMilliseconds)
	{
		State.Transactions.Add({
			{AllocateDomainId()},
			CurrentGameMilliseconds,
			Category,
			AmountCredits,
			Explanation
		});
		EmitEvent(
			EPhase1EventType::TransactionRecorded,
			{},
			Explanation,
			CurrentGameMilliseconds);
	}

	void FStarterAirfieldSimulation::EmitEvent(
		const EPhase1EventType Type,
		const FCommandId Cause,
		const FString& Message,
		const int64 CurrentGameMilliseconds)
	{
		State.Events.Add({
			State.NextPhase1EventSequence++,
			CurrentGameMilliseconds,
			Type,
			Cause,
			Message
		});
	}

	uint64 FStarterAirfieldSimulation::AllocateDomainId()
	{
		return State.NextDomainId++;
	}

	FFacilityId FStarterAirfieldSimulation::FindFacility(const EFacilityType Type) const
	{
		for (const FFacilityRecord& Facility : State.Facilities)
		{
			if (Facility.Type == Type)
			{
				return Facility.Id;
			}
		}
		return {};
	}

	FPhase1QuerySnapshot FStarterAirfieldSimulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds) const
	{
		FPhase1QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bInitialized = State.bInitialized;
		Query.bPaused = State.bPaused;
		Query.bAirportOpen = State.bAirportOpen;
		Query.AirportName = State.AirportName;
		Query.MapDisplayName = TEXT("Riverbend Plains");
		Query.Credits = State.Credits;
		Query.AirportPoints = State.AirportPoints;
		Query.ConstructionStage = State.Project.Stage;
		Query.OfferState = State.Offer.State;
		Query.FlightState = State.Flight.State;
		Query.InspectionState = State.Flight.Inspection;
		Query.FuelingState = State.Flight.Fueling;
		Query.CompatibilitySummary = State.Offer.CompatibilitySummary;
		Query.StateChecksum = CalculateChecksum();

		if (!State.bInitialized)
		{
			Query.PrimaryStatus = TEXT("Create a new airport at Riverbend Plains");
			Query.Cause = TEXT("No active Phase 1 airport exists.");
			Query.Remedy = TEXT("Choose the temperate map and name the airport.");
			Query.CurrentObjective = TEXT("Start a new airport");
		}
		else if (State.Flight.State != EFlightState::None)
		{
			Query.PrimaryStatus = FlightStateText(State.Flight.State);
			Query.Cause = State.Flight.Blocker.IsEmpty()
				? TEXT("Routine ATC and ground services are automatic.")
				: State.Flight.Blocker;
			Query.Remedy = State.Flight.State == EFlightState::Completed
				? TEXT("Review the itemized reward, rating contributions, and aircraft history.")
				: State.Flight.State == EFlightState::Scheduled
				? TEXT("No action required. The aircraft appears when it enters the airport area.")
				: State.Flight.State == EFlightState::Turnaround
				? TEXT("No action required. Inspection and fueling dispatch automatically; use time controls to wait faster.")
				: State.Flight.Blocker.IsEmpty()
				? TEXT("No action required. Watch the captioned automated operation.")
				: TEXT("Select the highlighted route or service to inspect the blocker.");
		}
		else
		{
			Query.PrimaryStatus = ConstructionStageText(State.Project.Stage);
			Query.Cause = State.Project.Stage == EConstructionStage::None
				? TEXT("The parcel has no operational airfield.")
				: TEXT("");
			Query.Remedy = State.Project.Stage == EConstructionStage::None
				? TEXT("Build the complete starter airfield for 3,400 Credits.")
				: TEXT("");
		}

		for (const FObjectiveRecord& Objective : State.Objectives)
		{
			if (!Objective.bCompleted)
			{
				Query.CurrentObjective = Objective.Text;
				break;
			}
		}
		if (!State.PhraseIntents.IsEmpty())
		{
			Query.LatestCaption = State.PhraseIntents.Last().Caption;
		}
		return Query;
	}

	uint64 FStarterAirfieldSimulation::CalculateChecksum() const
	{
		uint64 Hash = FnvOffset;
		HashBytes(Hash, &State.bInitialized, sizeof(State.bInitialized));
		HashBytes(Hash, &State.bPaused, sizeof(State.bPaused));
		HashBytes(Hash, &State.bAirportOpen, sizeof(State.bAirportOpen));
		HashBytes(Hash, &State.AirportId.Value, sizeof(State.AirportId.Value));
		HashString(Hash, State.MapId.ToString());
		HashString(Hash, State.AirportName);
		HashBytes(Hash, &State.MasterSeed, sizeof(State.MasterSeed));
		HashBytes(Hash, &State.RandomStreamState, sizeof(State.RandomStreamState));
		HashBytes(Hash, &State.SpeedMultiplier, sizeof(State.SpeedMultiplier));
		HashBytes(Hash, &State.Credits, sizeof(State.Credits));
		HashBytes(Hash, &State.AirportPoints, sizeof(State.AirportPoints));
		HashBytes(Hash, &State.LastUpdatedGameMilliseconds, sizeof(State.LastUpdatedGameMilliseconds));
		HashBytes(Hash, &State.NextDomainId, sizeof(State.NextDomainId));
		const uint8 ProjectStage = static_cast<uint8>(State.Project.Stage);
		HashBytes(Hash, &ProjectStage, sizeof(ProjectStage));
		HashBytes(Hash, &State.Project.QuotedCost, sizeof(State.Project.QuotedCost));
		const FStarterPlanProposal& Proposal = State.Project.Proposal;
		for (const FPhase1Point Point : {
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			Proposal.TaxiStart,
			Proposal.TaxiEnd,
			Proposal.StandCenter,
			Proposal.AccessStart,
			Proposal.AccessEnd,
			Proposal.OperationsHutCenter})
		{
			HashBytes(Hash, &Point.X, sizeof(Point.X));
			HashBytes(Hash, &Point.Y, sizeof(Point.Y));
		}
		HashBytes(Hash, &Proposal.RunwayWidthCentimeters,
			sizeof(Proposal.RunwayWidthCentimeters));
		const int32 TaxiwaySegmentCount = Proposal.TaxiwaySegments.Num();
		HashBytes(Hash, &TaxiwaySegmentCount, sizeof(TaxiwaySegmentCount));
		for (const FTaxiwaySegment& Segment : Proposal.TaxiwaySegments)
		{
			for (const FPhase1Point Point : {Segment.Start, Segment.End})
			{
				HashBytes(Hash, &Point.X, sizeof(Point.X));
				HashBytes(Hash, &Point.Y, sizeof(Point.Y));
			}
		}
		const uint8 OfferState = static_cast<uint8>(State.Offer.State);
		const uint8 FlightState = static_cast<uint8>(State.Flight.State);
		HashBytes(Hash, &OfferState, sizeof(OfferState));
		HashBytes(Hash, &State.Offer.bPinned, sizeof(State.Offer.bPinned));
		HashBytes(Hash, &FlightState, sizeof(FlightState));
		HashBytes(Hash, &State.Flight.ScheduledArrivalGameMilliseconds, sizeof(int64));
		HashBytes(Hash, &State.Flight.StandOccupancyStartGameMilliseconds, sizeof(int64));
		HashBytes(Hash, &State.Flight.StandOccupancyEndGameMilliseconds, sizeof(int64));
		HashBytes(Hash, &State.Airframe.Id.Value, sizeof(uint64));
		HashBytes(Hash, &State.Airframe.VisitCount, sizeof(int32));
		for (const FTransactionRecord& Transaction : State.Transactions)
		{
			HashBytes(Hash, &Transaction.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Transaction.AmountCredits, sizeof(int64));
			HashString(Hash, Transaction.Explanation);
		}
		for (const FPhraseIntentRecord& Phrase : State.PhraseIntents)
		{
			HashBytes(Hash, &Phrase.Id.Value, sizeof(uint64));
			HashString(Hash, Phrase.Caption);
		}
		for (const FName HelpId : State.AcknowledgedContextHelp)
		{
			HashString(Hash, HelpId.ToString());
		}
		return Hash;
	}

	bool FStarterAirfieldSimulation::ApplyExternalEconomyChange(
		const FName Category,
		const int64 AmountCredits,
		const int32 AirportPoints,
		const FString& Explanation,
		const int64 CurrentGameMilliseconds)
	{
		if (!State.bInitialized ||
			Category.IsNone() ||
			Explanation.IsEmpty() ||
			AirportPoints < 0 ||
			(AmountCredits < 0 && State.Credits < -AmountCredits))
		{
			return false;
		}

		State.Credits += AmountCredits;
		State.AirportPoints += AirportPoints;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		RecordTransaction(Category, AmountCredits, Explanation, CurrentGameMilliseconds);
		if (AirportPoints > 0)
		{
			EmitEvent(
				EPhase1EventType::CapabilityAwarded,
				{},
				FString::Printf(
					TEXT("%d Airport Points awarded: %s"),
					AirportPoints,
					*Explanation),
				CurrentGameMilliseconds);
		}
		return true;
	}

	bool FStarterAirfieldSimulation::RestoreState(const FPhase1State& InState)
	{
		if (InState.MasterSeed == 0 ||
			InState.NextDomainId == 0 ||
			InState.NextPhase1EventSequence == 0 ||
			InState.AirportName.Len() > 40 ||
			InState.Facilities.Num() > 10000 ||
			InState.Teams.Num() > 10000 ||
			InState.Transactions.Num() > 100000 ||
			InState.PhraseIntents.Num() > 100000 ||
			InState.AcknowledgedContextHelp.Num() >
				GetPhase1ContextHelpCatalog().Num())
		{
			return false;
		}
		TSet<FName> SeenHelpIds;
		for (const FName HelpId : InState.AcknowledgedContextHelp)
		{
			if (!FindPhase1ContextHelp(HelpId) ||
				SeenHelpIds.Contains(HelpId))
			{
				return false;
			}
			SeenHelpIds.Add(HelpId);
		}
		if (InState.bInitialized && (!InState.AirportId.IsValid() || InState.MapId.IsNone()))
		{
			return false;
		}
		if (InState.Project.Stage != EConstructionStage::None &&
			!ValidateStarterPlan(InState.Project.Proposal).bValid)
		{
			return false;
		}
		if (InState.Flight.Id.IsValid() &&
			(InState.Flight.StandOccupancyStartGameMilliseconds >
				InState.Flight.ScheduledArrivalGameMilliseconds ||
				InState.Flight.StandOccupancyEndGameMilliseconds <
					InState.Flight.ScheduledArrivalGameMilliseconds))
		{
			return false;
		}

		TSet<uint64> SeenIds;
		auto RegisterId = [&SeenIds, &InState](const uint64 Id)
		{
			return Id == 0 || (Id < InState.NextDomainId && !SeenIds.Contains(Id) && (SeenIds.Add(Id), true));
		};
		if (InState.bInitialized && !RegisterId(InState.AirportId.Value))
		{
			return false;
		}
		if (InState.Project.Id.IsValid() && !RegisterId(InState.Project.Id.Value))
		{
			return false;
		}
		for (const FFacilityRecord& Facility : InState.Facilities)
		{
			if (!Facility.Id.IsValid() || !RegisterId(Facility.Id.Value))
			{
				return false;
			}
		}
		for (const FStaffTeamRecord& Team : InState.Teams)
		{
			if (!Team.Id.IsValid() || !RegisterId(Team.Id.Value))
			{
				return false;
			}
		}

		State = InState;
		PendingCommands.Reset();
		return true;
	}
}
