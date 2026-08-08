#include "AMSimPassengerTerminalSimulation.h"

#include "Algo/Count.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 FnvOffset = 1469598103934665603ull;
		constexpr uint64 FnvPrime = 1099511628211ull;
		constexpr uint64 Phase3IdBase = 1ull << 48;

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
			const FTCHARToUTF8 Utf8(*Value);
			HashBytes(Hash, Utf8.Get(), Utf8.Length());
		}

		template <typename RecordType, typename IdType>
		bool ContainsId(const TArray<RecordType>& Records, const IdType Id)
		{
			return Records.ContainsByPredicate(
				[Id](const RecordType& Record) { return Record.Id == Id; });
		}

		bool HasTerminalRoomBoundary(
			const FTerminalLayoutState& Layout,
			const FTerminalCellCoord From,
			const FTerminalCellCoord To)
		{
			FTerminalCellCoord EdgeFrom;
			FTerminalCellCoord EdgeTo;
			if (To.X != From.X)
			{
				const int32 BoundaryX = FMath::Max(From.X, To.X);
				EdgeFrom = {BoundaryX, From.Y};
				EdgeTo = {BoundaryX, From.Y + 1};
			}
			else
			{
				const int32 BoundaryY = FMath::Max(From.Y, To.Y);
				EdgeFrom = {From.X, BoundaryY};
				EdgeTo = {From.X + 1, BoundaryY};
			}
			return Layout.Edges.ContainsByPredicate(
				[EdgeFrom, EdgeTo](const FTerminalEdgeRecord& Edge)
				{
					return (Edge.From == EdgeFrom && Edge.To == EdgeTo) ||
						(Edge.From == EdgeTo && Edge.To == EdgeFrom);
				});
		}

		TArray<FTerminalSpatialRoomRecord> InferTerminalRooms(
			const FTerminalLayoutState& Layout)
		{
			TArray<FTerminalSpatialRoomRecord> Rooms;
			TSet<FTerminalCellCoord> Visited;
			for (const FTerminalFloorCellRecord& Seed : Layout.FloorCells)
			{
				if (Visited.Contains(Seed.Cell))
				{
					continue;
				}
				FTerminalSpatialRoomRecord Room;
				Room.Function = Seed.Kind;
				Room.bOperational = true;
				TArray<FTerminalCellCoord> Frontier{Seed.Cell};
				Visited.Add(Seed.Cell);
				for (int32 Index = 0; Index < Frontier.Num(); ++Index)
				{
					const FTerminalCellCoord Current = Frontier[Index];
					Room.Cells.Add(Current);
					const FTerminalFloorCellRecord* CurrentCell =
						Layout.FloorCells.FindByPredicate(
							[Current](const FTerminalFloorCellRecord& Cell)
							{
								return Cell.Cell == Current;
							});
					Room.bOperational = Room.bOperational && CurrentCell &&
						CurrentCell->bBuilt && !CurrentCell->bLocallyClosed;
					const FTerminalCellCoord Neighbors[] = {
						{Current.X + 1, Current.Y}, {Current.X - 1, Current.Y},
						{Current.X, Current.Y + 1}, {Current.X, Current.Y - 1}};
					for (const FTerminalCellCoord Neighbor : Neighbors)
					{
						if (Visited.Contains(Neighbor) ||
							HasTerminalRoomBoundary(Layout, Current, Neighbor))
						{
							continue;
						}
						const FTerminalFloorCellRecord* NeighborCell =
							Layout.FloorCells.FindByPredicate(
								[Neighbor, &Room](const FTerminalFloorCellRecord& Cell)
								{
									return Cell.Cell == Neighbor &&
										Cell.Kind == Room.Function;
								});
						if (NeighborCell)
						{
							Visited.Add(Neighbor);
							Frontier.Add(Neighbor);
						}
					}
				}
				const FTerminalCellCoord Anchor = Room.Cells[0];
				Room.StableId =
					(static_cast<uint64>(Room.Function) + 1ull) << 56 |
					(static_cast<uint64>(Anchor.X + 128) & 0xffffull) << 16 |
					(static_cast<uint64>(Anchor.Y + 128) & 0xffffull);
				Rooms.Add(MoveTemp(Room));
			}
			return Rooms;
		}

		FString FeaturedRouteText(const bool bAccessible)
		{
			return bAccessible
				? TEXT("Entrance -> Bag drop -> Security -> Gate A1 · accessible")
				: TEXT("Entrance -> Bag drop -> Security -> Gate A1");
		}
	}

	uint64 FPassengerTerminalSimulation::CalculateChecksum() const
	{
		uint64 Hash = FnvOffset;
		HashBytes(Hash, &State.bInitialized, sizeof(State.bInitialized));
		HashBytes(Hash, &State.MasterSeed, sizeof(State.MasterSeed));
		HashBytes(Hash, &State.RandomStreamState, sizeof(State.RandomStreamState));
		HashBytes(Hash, &State.NextDomainId, sizeof(State.NextDomainId));
		HashBytes(Hash, &State.NextEventSequence, sizeof(State.NextEventSequence));
		HashBytes(Hash, &State.TerminalStage, sizeof(State.TerminalStage));
		HashBytes(Hash, &State.bTerminalOpen, sizeof(State.bTerminalOpen));
		for (const FTerminalRoomRecord& Room : State.Rooms)
		{
			HashBytes(Hash, &Room.Id.Value, sizeof(uint64));
			HashString(Hash, Room.DefinitionId.ToString());
			HashBytes(Hash, &Room.bBuilt, sizeof(bool));
			HashBytes(Hash, &Room.bOpen, sizeof(bool));
		}
		for (const FTerminalRouteRecord& Route : State.Routes)
		{
			HashBytes(Hash, &Route.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Route.Kind, sizeof(Route.Kind));
			HashBytes(Hash, &Route.bConnected, sizeof(bool));
			HashBytes(Hash, &Route.bControlledTransition, sizeof(bool));
			HashBytes(Hash, &Route.bAccessible, sizeof(bool));
			HashBytes(Hash, &Route.bIntroducesSecurityBypass, sizeof(bool));
		}
		HashBytes(Hash, &State.Checkpoint.Id.Value, sizeof(uint64));
		HashBytes(Hash, &State.Checkpoint.bOpen, sizeof(bool));
		HashBytes(Hash, &State.Checkpoint.ProcessedCount, sizeof(int32));
		for (const FPassengerPartyRecord& Party : State.Parties)
		{
			HashBytes(Hash, &Party.Id.Value, sizeof(uint64));
			HashString(Hash, Party.DisplayName);
			HashBytes(Hash, &Party.bAssistanceAssigned, sizeof(bool));
			for (const FPassengerId Member : Party.Members)
			{
				HashBytes(Hash, &Member.Value, sizeof(uint64));
			}
		}
		for (const FPassengerRecord& Passenger : State.Passengers)
		{
			HashBytes(Hash, &Passenger.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Passenger.PartyId.Value, sizeof(uint64));
			HashBytes(Hash, &Passenger.FlightId.Value, sizeof(uint64));
			HashString(Hash, Passenger.DisplayName);
			HashBytes(Hash, &Passenger.Direction, sizeof(Passenger.Direction));
			HashBytes(Hash, &Passenger.JourneyState, sizeof(Passenger.JourneyState));
			HashBytes(Hash, &Passenger.SecurityState, sizeof(Passenger.SecurityState));
			HashBytes(Hash, &Passenger.bUsedAccessibleRoute, sizeof(bool));
			HashBytes(Hash, &Passenger.BagCount, sizeof(int32));
		}
		for (const FBagRecord& Bag : State.Bags)
		{
			HashBytes(Hash, &Bag.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Bag.PassengerId.Value, sizeof(uint64));
			HashBytes(Hash, &Bag.JourneyState, sizeof(Bag.JourneyState));
			HashBytes(Hash, &Bag.bScreened, sizeof(bool));
			HashBytes(Hash, &Bag.bReconciled, sizeof(bool));
		}
		for (const FTerminalFloorCellRecord& Cell : State.TerminalLayout.FloorCells)
		{
			HashBytes(Hash, &Cell.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Cell.Cell.X, sizeof(int32));
			HashBytes(Hash, &Cell.Cell.Y, sizeof(int32));
			HashBytes(Hash, &Cell.Kind, sizeof(Cell.Kind));
			HashBytes(Hash, &Cell.bBuilt, sizeof(bool));
			HashBytes(Hash, &Cell.bLocallyClosed, sizeof(bool));
		}
		for (const FTerminalEdgeRecord& Edge : State.TerminalLayout.Edges)
		{
			HashBytes(Hash, &Edge.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Edge.From.X, sizeof(int32));
			HashBytes(Hash, &Edge.From.Y, sizeof(int32));
			HashBytes(Hash, &Edge.To.X, sizeof(int32));
			HashBytes(Hash, &Edge.To.Y, sizeof(int32));
			HashBytes(Hash, &Edge.Kind, sizeof(Edge.Kind));
			HashBytes(Hash, &Edge.bBuilt, sizeof(bool));
		}
		for (const FTerminalPlacedObjectRecord& Object : State.TerminalLayout.Objects)
		{
			HashBytes(Hash, &Object.Id.Value, sizeof(uint64));
			HashString(Hash, Object.DefinitionId.ToString());
			HashBytes(Hash, &Object.Anchor.X, sizeof(int32));
			HashBytes(Hash, &Object.Anchor.Y, sizeof(int32));
			HashBytes(Hash, &Object.QuarterTurns, sizeof(int32));
			HashBytes(Hash, &Object.bOperational, sizeof(bool));
		}
		for (const FTerminalConstructionJobRecord& Job : State.TerminalLayout.ConstructionJobs)
		{
			HashBytes(Hash, &Job.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Job.ElementId.Value, sizeof(uint64));
			HashBytes(Hash, &Job.Stage, sizeof(Job.Stage));
			HashBytes(Hash, &Job.ProgressPercent, sizeof(int32));
		}
		for (const FTerminalVisitorRecord& Visitor : State.TerminalLayout.Visitors)
		{
			HashBytes(Hash, &Visitor.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Visitor.FlightId.Value, sizeof(uint64));
			HashBytes(Hash, &Visitor.Activity, sizeof(Visitor.Activity));
			HashBytes(Hash, &Visitor.Cell.X, sizeof(int32));
			HashBytes(Hash, &Visitor.Cell.Y, sizeof(int32));
			HashBytes(Hash, &Visitor.bActive, sizeof(bool));
		}
		HashBytes(Hash, &State.Flight.Id.Value, sizeof(uint64));
		HashBytes(Hash, &State.Flight.State, sizeof(State.Flight.State));
		HashBytes(Hash, &State.Flight.bPassengerReconciled, sizeof(bool));
		HashBytes(Hash, &State.Flight.bBagReconciled, sizeof(bool));
		HashBytes(Hash, &State.CompletedPassengerCount, sizeof(int32));
		HashBytes(Hash, &State.CompletedBagCount, sizeof(int32));
		HashBytes(Hash, &State.TotalPassengerRevenueCredits, sizeof(int64));
		for (const FPhase3Event& Event : State.Events)
		{
			HashBytes(Hash, &Event.Sequence, sizeof(uint64));
			HashBytes(Hash, &Event.Type, sizeof(Event.Type));
			HashBytes(Hash, &Event.SubjectId, sizeof(uint64));
		}
		return Hash;
	}

	bool FPassengerTerminalSimulation::RestoreState(
		const FPhase3State& InState,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State)
	{
		if (InState.MasterSeed == 0 ||
			InState.NextDomainId < Phase3IdBase ||
			InState.NextEventSequence == 0 ||
			InState.Rooms.Num() > 10000 ||
			InState.Routes.Num() > 10000 ||
			InState.Parties.Num() > 10000 ||
			InState.Passengers.Num() > 10000 ||
			InState.Bags.Num() > 20000 ||
			InState.Teams.Num() > 10000 ||
			InState.Events.Num() > 100000 ||
			InState.CompletedPassengerCount < 0 ||
			InState.CompletedPassengerCount > InState.Passengers.Num() ||
			InState.CompletedBagCount < 0 ||
			InState.CompletedBagCount > InState.Bags.Num())
		{
			return false;
		}
		if (InState.bInitialized &&
			(!Phase1State.bInitialized || !Phase1State.bAirportOpen ||
				!Phase2State.bInitialized))
		{
			return false;
		}
		if (InState.bTerminalOpen &&
			InState.TerminalStage != ETerminalConstructionStage::Operational)
		{
			return false;
		}

		TSet<uint64> SeenIds;
		auto RegisterId = [&SeenIds, &InState](const uint64 Id)
		{
			return Id >= Phase3IdBase &&
				Id < InState.NextDomainId &&
				!SeenIds.Contains(Id) &&
				(SeenIds.Add(Id), true);
		};
		for (const FTerminalRoomRecord& Room : InState.Rooms)
		{
			if (!RegisterId(Room.Id.Value) ||
				Room.DefinitionId.IsNone() ||
				Room.DisplayName.IsEmpty() ||
				Room.Capacity <= 0)
			{
				return false;
			}
		}
		TSet<int32> RouteKinds;
		for (const FTerminalRouteRecord& Route : InState.Routes)
		{
			if (!RegisterId(Route.Id.Value) ||
				Route.DefinitionId.IsNone() ||
				Route.Capacity <= 0 ||
				RouteKinds.Contains(static_cast<int32>(Route.Kind)))
			{
				return false;
			}
			RouteKinds.Add(static_cast<int32>(Route.Kind));
		}
		for (const FPhase3StaffTeamRecord& Team : InState.Teams)
		{
			if (!RegisterId(Team.Id.Value) ||
				Team.RoleId.IsNone() ||
				Team.ZoneId.IsNone() ||
				Team.TeamSize <= 0 ||
				Team.WorkloadPercent < 0 ||
				Team.WorkloadPercent > 100)
			{
				return false;
			}
		}
		if (InState.Checkpoint.Id.IsValid() &&
			(!RegisterId(InState.Checkpoint.Id.Value) ||
				InState.Checkpoint.DefinitionId.IsNone() ||
				!ContainsId(InState.Teams, InState.Checkpoint.TeamId) ||
				InState.Checkpoint.ThroughputPerBucket <= 0 ||
				InState.Checkpoint.QueueCapacity <= 0))
		{
			return false;
		}
		if (InState.Tenant.Id.IsValid() &&
			(!RegisterId(InState.Tenant.Id.Value) ||
				InState.Tenant.TenantContentId.IsNone() ||
				InState.Tenant.DisplayName.IsEmpty() ||
				InState.Tenant.SatisfactionPercent < 0 ||
				InState.Tenant.SatisfactionPercent > 100))
		{
			return false;
		}
		if (InState.Flight.Id.IsValid() &&
			(!RegisterId(InState.Flight.Id.Value) ||
				InState.Flight.FlightCode.IsNone() ||
				InState.Flight.OperatorContentId.IsNone() ||
				InState.Flight.AircraftContentId.IsNone() ||
				InState.Flight.GateId.IsNone()))
		{
			return false;
		}
		for (const FPassengerPartyRecord& Party : InState.Parties)
		{
			if (!RegisterId(Party.Id.Value) ||
				Party.DisplayName.IsEmpty() ||
				Party.Members.IsEmpty() ||
				Party.TimeConfidencePercent < 0 ||
				Party.TimeConfidencePercent > 100)
			{
				return false;
			}
		}
		for (const FPassengerRecord& Passenger : InState.Passengers)
		{
			if (!RegisterId(Passenger.Id.Value) ||
				!ContainsId(InState.Parties, Passenger.PartyId) ||
				(InState.Flight.Id.IsValid() &&
					Passenger.FlightId != InState.Flight.Id) ||
				Passenger.DisplayName.IsEmpty() ||
				Passenger.AgeBand.IsNone() ||
				Passenger.BagCount < 0 ||
				Passenger.BagCount > 8 ||
				Passenger.TimeConfidencePercent < 0 ||
				Passenger.TimeConfidencePercent > 100 ||
				Passenger.PatiencePercent < 0 ||
				Passenger.PatiencePercent > 100)
			{
				return false;
			}
		}
		for (const FBagRecord& Bag : InState.Bags)
		{
			if (!RegisterId(Bag.Id.Value) ||
				!ContainsId(InState.Passengers, Bag.PassengerId) ||
				Bag.FlightId != InState.Flight.Id)
			{
				return false;
			}
		}
		FString LayoutFailure;
		if (!ValidateTerminalLayout(InState.TerminalLayout, LayoutFailure))
		{
			return false;
		}
		for (const FTerminalFloorCellRecord& Cell : InState.TerminalLayout.FloorCells)
		{
			if (!RegisterId(Cell.Id.Value)) return false;
		}
		for (const FTerminalEdgeRecord& Edge : InState.TerminalLayout.Edges)
		{
			if (!RegisterId(Edge.Id.Value)) return false;
		}
		for (const FTerminalPlacedObjectRecord& Object : InState.TerminalLayout.Objects)
		{
			if (!RegisterId(Object.Id.Value)) return false;
		}
		TSet<uint64> LayoutTransactionIds;
		for (const FTerminalEditTransactionRecord& Transaction :
			InState.TerminalLayout.EditTransactions)
		{
			if (!RegisterId(Transaction.Id.Value)) return false;
			LayoutTransactionIds.Add(Transaction.Id.Value);
		}
		for (const FTerminalConstructionJobRecord& Job :
			InState.TerminalLayout.ConstructionJobs)
		{
			if (!RegisterId(Job.Id.Value) ||
				!LayoutTransactionIds.Contains(Job.TransactionId.Value))
			{
				return false;
			}
		}
		for (const FTerminalVisitorRecord& Visitor :
			InState.TerminalLayout.Visitors)
		{
			if (!RegisterId(Visitor.Id.Value) || !Visitor.FlightId.IsValid() ||
				Visitor.ActivityChangedAtGameMilliseconds < 0)
			{
				return false;
			}
		}
		for (const FPhase3Event& Event : InState.Events)
		{
			if (Event.Sequence == 0 ||
				Event.Sequence >= InState.NextEventSequence ||
				Event.GameTimeMilliseconds < 0)
			{
				return false;
			}
		}

		bool bAccessible = false;
		FString Failure;
		if (InState.bTerminalOpen &&
			(!ValidateSecurityTopology(InState, bAccessible, Failure) ||
				!bAccessible))
		{
			return false;
		}
		if (!ValidateReconciliation(InState, Failure))
		{
			return false;
		}
		const int32 DerivedCompletedPassengers = Algo::CountIf(
			InState.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.JourneyState ==
						EPassengerJourneyState::Completed;
				});
		const int32 DerivedCompletedBags = Algo::CountIf(
			InState.Bags,
			[](const FBagRecord& Bag)
				{
					return (Bag.Direction == EPassengerDirection::Departing &&
							Bag.JourneyState == EBagJourneyState::OnAircraft) ||
						(Bag.Direction == EPassengerDirection::Arriving &&
							Bag.JourneyState == EBagJourneyState::Collected);
				});
		if (DerivedCompletedPassengers != InState.CompletedPassengerCount ||
			DerivedCompletedBags != InState.CompletedBagCount)
		{
			return false;
		}

		State = InState;
		PendingCommands.Reset();
		return true;
	}

	FPhase3QuerySnapshot FPassengerTerminalSimulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State) const
	{
		FPhase3QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bUnlocked =
			Phase1State.bInitialized &&
			Phase1State.bAirportOpen &&
			Phase2State.bInitialized;
		Query.bInitialized = State.bInitialized;
		Query.bTerminalOpen = State.bTerminalOpen;
		Query.TerminalStage = State.TerminalStage;
		Query.FlightState = State.Flight.State;
		Query.RequiredConnectionCount = State.Routes.Num();
		Query.ConnectedCount = Algo::CountIf(
			State.Routes,
			[](const FTerminalRouteRecord& Route) { return Route.bConnected; });
		Query.PassengerCount = State.Passengers.Num();
		Query.DepartingPassengerCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.Direction == EPassengerDirection::Departing;
				});
		Query.ArrivingPassengerCount =
			Query.PassengerCount - Query.DepartingPassengerCount;
		Query.CompletedPassengerCount = State.CompletedPassengerCount;
		Query.BagCount = State.Bags.Num();
		Query.CompletedBagCount = State.CompletedBagCount;
		Query.SecurityQueueCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.JourneyState ==
							EPassengerJourneyState::SecurityQueue ||
						Passenger.JourneyState ==
							EPassengerJourneyState::Screening;
				});
		Query.BoardedCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.Direction == EPassengerDirection::Departing &&
						(Passenger.JourneyState ==
								EPassengerJourneyState::OnAircraft ||
							Passenger.JourneyState ==
								EPassengerJourneyState::Completed);
				});
		Query.ReclaimCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.JourneyState ==
						EPassengerJourneyState::BaggageReclaim;
				});
		Query.LandsideExitCount = Algo::CountIf(
			State.Passengers,
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.Direction == EPassengerDirection::Arriving &&
						Passenger.JourneyState ==
							EPassengerJourneyState::Completed;
				});
		Query.VisiblePassengerTarget = FMath::Min(
			Query.PassengerCount,
			GetPhase3Fixture().VisibleScaleProxyCount);
		Query.bSecurityIntegrityValid = State.bSecurityIntegrityValid;
		Query.bAccessibleRouteValid = State.bAccessibleRouteValid;
		Query.bPassengerReconciled = State.Flight.bPassengerReconciled;
		Query.bBagReconciled = State.Flight.bBagReconciled;
		Query.FlightCode = State.Flight.FlightCode.ToString();
		Query.FlightStatus = Phase3FlightDisplayName(State.Flight.State);

		const FPassengerPartyRecord* Featured = State.Parties.FindByPredicate(
			[](const FPassengerPartyRecord& Party)
				{
					return Party.bRequiresAccessibleRoute;
				});
		if (!Featured && !State.Parties.IsEmpty())
		{
			Featured = &State.Parties[0];
		}
		if (Featured)
		{
			Query.FeaturedPartyName = Featured->DisplayName;
			Query.FeaturedPartyMembers = Featured->Members.Num();
			Query.FeaturedNeeds = Featured->CompactNeeds;
			Query.FeaturedTimeConfidencePercent =
				Featured->TimeConfidencePercent;
			Query.bFeaturedAccessible = Featured->bRequiresAccessibleRoute;
			if (!Featured->Members.IsEmpty())
			{
				if (const FPassengerRecord* Passenger =
					State.Passengers.FindByPredicate(
						[Featured](const FPassengerRecord& Candidate)
							{
								return Candidate.Id == Featured->Members[0];
							}))
				{
					Query.FeaturedStep =
						PassengerJourneyDisplayName(Passenger->JourneyState);
				}
			}
			Query.FeaturedRoute = FeaturedRouteText(
				Featured->bRequiresAccessibleRoute);
		}
		Query.BaggageSummary = Query.BagCount == 0
			? TEXT("No checked bags")
			: FString::Printf(
				TEXT("%d/%d bags at their reconciled destination"),
				Query.CompletedBagCount,
				Query.BagCount);
		int32 ArrivedByRoad = 0;
		int32 ExitedByRoad = 0;
		for (const FLandsideAccessRecord& Access : State.Landside)
		{
			ArrivedByRoad += Access.ArrivedPassengerCount;
			ExitedByRoad += Access.DepartedPassengerCount;
		}
		Query.LandsideSummary = FString::Printf(
			TEXT("%d arrived by car/taxi/bus · %d exited to curb"),
			ArrivedByRoad,
			ExitedByRoad);

		if (!State.bInitialized)
		{
			Query.PrimaryStatus = Query.bUnlocked
				? TEXT("Passenger terminal available")
				: TEXT("Living-airport operations required");
			Query.Cause = Query.bUnlocked
				? TEXT("")
				: TEXT("Phase 2 is not active.");
			Query.Remedy = Query.bUnlocked
				? TEXT("Open passenger-airport planning.")
				: TEXT("Start Phase 2 first.");
		}
		else if (State.TerminalStage == ETerminalConstructionStage::None)
		{
			Query.PrimaryStatus = TEXT("Plan the domestic terminal");
			Query.Remedy = TEXT("Fund the terminal shell.");
		}
		else if (State.TerminalStage < ETerminalConstructionStage::ShellReady)
		{
			Query.PrimaryStatus = TerminalStageDisplayName(State.TerminalStage);
			Query.Remedy = TEXT("Construction teams are completing the shell.");
		}
		else if (!State.bTerminalOpen)
		{
			Query.PrimaryStatus = FString::Printf(
				TEXT("Connect terminal flows · %d/%d"),
				Query.ConnectedCount,
				Query.RequiredConnectionCount);
			if (!State.Checkpoint.bOpen)
			{
				Query.Cause = TEXT("Domestic security is closed.");
				Query.Remedy = TEXT("Open the security lane.");
			}
			else if (Query.ConnectedCount < Query.RequiredConnectionCount)
			{
				Query.Cause = TEXT("A required route is disconnected.");
				Query.Remedy = TEXT("Connect the next highlighted flow.");
			}
			else
			{
				Query.Remedy = TEXT("Open the terminal.");
			}
		}
		else if (State.Flight.State == EPhase3FlightState::Unscheduled)
		{
			Query.PrimaryStatus = TEXT("Terminal ready");
			Query.Remedy = TEXT("Schedule the first passenger service.");
		}
		else if (State.Flight.State == EPhase3FlightState::Completed)
		{
			Query.PrimaryStatus = TEXT("Passenger turnaround complete");
			Query.Remedy = TEXT("Inspect the final reconciliation.");
		}
		else
		{
			Query.PrimaryStatus = FString::Printf(
				TEXT("%s · %s"),
				*Query.FlightCode,
				*Query.FlightStatus);
			Query.Remedy = Query.SecurityQueueCount > 0
				? TEXT("Keep domestic security open.")
				: TEXT("Follow passengers, bags, and Gate A1.");
		}
		Query.TerminalLayout.Revision = State.TerminalLayout.Revision;
		Query.TerminalLayout.FloorCells = State.TerminalLayout.FloorCells;
		Query.TerminalLayout.Edges = State.TerminalLayout.Edges;
		Query.TerminalLayout.Objects = State.TerminalLayout.Objects;
		Query.TerminalLayout.ConstructionJobs =
			State.TerminalLayout.ConstructionJobs;
		Query.TerminalLayout.Visitors = State.TerminalLayout.Visitors;
		Query.TerminalLayout.Rooms = InferTerminalRooms(State.TerminalLayout);
		Query.TerminalLayout.bReady = State.TerminalLayout.bReady;
		Query.TerminalLayout.ValidAirsideGateCount =
			State.TerminalLayout.ValidAirsideGateCount;
		Query.StateChecksum = CalculateChecksum();
		return Query;
	}
}
