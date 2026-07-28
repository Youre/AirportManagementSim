#include "AMSimLivingAirportSimulation.h"
#include "AMSimDeterminism.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr int64 Phase2IdBase = 1ll << 32;

		FName RoleForSpecialization(const EAirportSpecialization Specialization)
		{
			switch (Specialization)
			{
			case EAirportSpecialization::FlightSchool: return TEXT("AircraftRole.BasicTrainer");
			case EAirportSpecialization::Charter: return TEXT("AircraftRole.BusinessTurboprop");
			default: return TEXT("AircraftRole.TouringPiston");
			}
		}

		int64 RewardFor(const EAirportSpecialization Specialization)
		{
			switch (Specialization)
			{
			case EAirportSpecialization::FlightSchool: return 220;
			case EAirportSpecialization::Charter: return 720;
			default: return 340;
			}
		}

		FName RequiredRoleForService(const FName ServiceId)
		{
			if (ServiceId == TEXT("Service.CargoHandling"))
			{
				return TEXT("StaffRole.CargoHandling");
			}
			if (ServiceId == TEXT("Service.Inspection"))
			{
				return TEXT("StaffRole.Maintenance");
			}
			return TEXT("StaffRole.GroundService");
		}

		bool ContainsId(const TArray<FPhase2FlightRecord>& Records, const FFlightId Id)
		{
			return Records.ContainsByPredicate(
				[Id](const FPhase2FlightRecord& Record) { return Record.Id == Id; });
		}

		bool ContainsId(const TArray<FPhase2StaffTeamRecord>& Records, const FStaffTeamId Id)
		{
			return Records.ContainsByPredicate(
				[Id](const FPhase2StaffTeamRecord& Record) { return Record.Id == Id; });
		}

		bool ContainsId(const TArray<FPhase2ParcelRecord>& Records, const FParcelId Id)
		{
			return Records.ContainsByPredicate(
				[Id](const FPhase2ParcelRecord& Record) { return Record.Id == Id; });
		}

		bool IsServiceComplete(const FPhase2ServiceTaskRecord& Task)
		{
			return Task.State == EPhase2ServiceState::Completed;
		}
	}

	FLivingAirportSimulation::FLivingAirportSimulation(const uint64 MasterSeed)
	{
		Reset(MasterSeed);
	}

	void FLivingAirportSimulation::EnsureCargoServiceResources(
		const int64 CurrentGameMilliseconds)
	{
		if (!State.bInitialized)
		{
			return;
		}
		const TArray<FName> CargoVehicleIds = {
			TEXT("Vehicle.Cargo.Forklift"),
			TEXT("Vehicle.Cargo.DollyTug"),
			TEXT("Vehicle.Cargo.DeliveryTruck"),
			TEXT("Vehicle.Cargo.HighLoader"),
			TEXT("Vehicle.Cargo.SupportUnit")};
		for (const FName VehicleId : CargoVehicleIds)
		{
			if (!State.Vehicles.ContainsByPredicate(
				[VehicleId](const FPhase2VehicleRecord& Vehicle)
				{
					return Vehicle.VehicleContentId == VehicleId;
				}))
			{
				FPhase2VehicleRecord Vehicle;
				Vehicle.Id = {AllocateDomainId()};
				Vehicle.VehicleContentId = VehicleId;
				Vehicle.CapabilityId = TEXT("Service.CargoHandling");
				Vehicle.HomeDepotId = TEXT("Facility.Cargo.EquipmentBay");
				Vehicle.State = EPhase2VehicleState::AtDepot;
				Vehicle.StateChangedAtGameMilliseconds =
					CurrentGameMilliseconds;
				State.Vehicles.Add(Vehicle);
			}
		}
		for (int32 TeamIndex = 1; TeamIndex <= CargoVehicleIds.Num();
			++TeamIndex)
		{
			const FName ZoneId(*FString::Printf(
				TEXT("Zone.Cargo.Team%d"),
				TeamIndex));
			if (!State.Teams.ContainsByPredicate(
				[ZoneId](const FPhase2StaffTeamRecord& Team)
				{
					return Team.RoleId == TEXT("StaffRole.CargoHandling") &&
						Team.ZoneId == ZoneId;
				}))
			{
				FPhase2StaffTeamRecord Team;
				Team.Id = {AllocateDomainId()};
				Team.RoleId = TEXT("StaffRole.CargoHandling");
				Team.ZoneId = ZoneId;
				Team.TeamSize = 4;
				Team.WorkloadPercent = 20;
				Team.MoralePercent = 85;
				Team.bOnShift = true;
				State.Teams.Add(Team);
			}
		}
	}

	void FLivingAirportSimulation::EnsureExternalServiceResource(
		const FName ServiceId,
		const int64 CurrentGameMilliseconds)
	{
		if (!State.bInitialized || ServiceId.IsNone())
		{
			return;
		}
		if (!State.Vehicles.ContainsByPredicate(
			[ServiceId](const FPhase2VehicleRecord& Vehicle)
			{
				return Vehicle.CapabilityId == ServiceId;
			}))
		{
			FPhase2VehicleRecord Vehicle;
			Vehicle.Id = {AllocateDomainId()};
			Vehicle.VehicleContentId = FName(*FString::Printf(
				TEXT("Vehicle.Major.%s"),
				*ServiceId.ToString().Replace(TEXT("."), TEXT("_"))));
			Vehicle.CapabilityId = ServiceId;
			Vehicle.HomeDepotId = TEXT("Facility.Major.Service.Depot");
			Vehicle.State = EPhase2VehicleState::AtDepot;
			Vehicle.StateChangedAtGameMilliseconds =
				CurrentGameMilliseconds;
			State.Vehicles.Add(Vehicle);
		}
		const FName RoleId = RequiredRoleForService(ServiceId);
		if (!State.Teams.ContainsByPredicate(
			[RoleId](const FPhase2StaffTeamRecord& Team)
			{
				return Team.RoleId == RoleId &&
					Team.ZoneId == TEXT("Zone.Major.Service");
			}))
		{
			FPhase2StaffTeamRecord Team;
			Team.Id = {AllocateDomainId()};
			Team.RoleId = RoleId;
			Team.ZoneId = TEXT("Zone.Major.Service");
			Team.TeamSize = 6;
			Team.WorkloadPercent = 15;
			Team.MoralePercent = 88;
			Team.bOnShift = true;
			State.Teams.Add(Team);
		}
	}

	FServiceTaskId FLivingAirportSimulation::RegisterExternalServiceTask(
		const FName OwnerDomain,
		const uint64 OwnerId,
		const FName OperationId,
		const int32 Quantity,
		const int64 DurationMilliseconds,
		const int64 CurrentGameMilliseconds)
	{
		return RegisterExternalServiceTask(
			OwnerDomain,
			OwnerId,
			TEXT("Service.CargoHandling"),
			OperationId,
			Quantity,
			DurationMilliseconds,
			CurrentGameMilliseconds,
			{});
	}

	FServiceTaskId FLivingAirportSimulation::RegisterExternalServiceTask(
		const FName OwnerDomain,
		const uint64 OwnerId,
		const FName ServiceId,
		const FName OperationId,
		const int32 Quantity,
		const int64 DurationMilliseconds,
		const int64 CurrentGameMilliseconds,
		const FServiceTaskId PrerequisiteTaskId)
	{
		if (!State.bInitialized ||
			OwnerDomain.IsNone() ||
			OwnerId == 0 ||
			ServiceId.IsNone() ||
			OperationId.IsNone() ||
			Quantity <= 0 ||
			DurationMilliseconds <= 0)
		{
			return {};
		}
		const FPhase2ServiceTaskRecord* Existing =
			State.ServiceTasks.FindByPredicate(
				[OwnerDomain, OwnerId, OperationId](
					const FPhase2ServiceTaskRecord& Task)
				{
					return Task.OwnerDomain == OwnerDomain &&
						Task.OwnerId == OwnerId &&
						Task.OperationId == OperationId;
				});
		if (Existing)
		{
			return Existing->Id;
		}
		if (ServiceId == TEXT("Service.CargoHandling"))
		{
			EnsureCargoServiceResources(CurrentGameMilliseconds);
		}
		else
		{
			EnsureExternalServiceResource(
				ServiceId,
				CurrentGameMilliseconds);
		}
		FPhase2ServiceTaskRecord Task;
		Task.Id = {AllocateDomainId()};
		Task.ServiceId = ServiceId;
		Task.OwnerDomain = OwnerDomain;
		Task.OwnerId = OwnerId;
		Task.OperationId = OperationId;
		Task.Quantity = Quantity;
		Task.PrerequisiteTaskId = PrerequisiteTaskId;
		Task.State = EPhase2ServiceState::Queued;
		Task.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Task.DurationMilliseconds = DurationMilliseconds;
		State.ServiceTasks.Add(Task);
		return Task.Id;
	}

	bool FLivingAirportSimulation::IsServiceTaskComplete(
		const FServiceTaskId TaskId) const
	{
		const FPhase2ServiceTaskRecord* Task = State.ServiceTasks.FindByPredicate(
			[TaskId](const FPhase2ServiceTaskRecord& Candidate)
			{
				return Candidate.Id == TaskId;
			});
		return Task && Task->State == EPhase2ServiceState::Completed;
	}

	void FLivingAirportSimulation::Reset(const uint64 MasterSeed)
	{
		State = {};
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase2.LivingAirport"));
		State.NextDomainId = Phase2IdBase;
		State.NextEventSequence = 1;
		PendingCommands.Reset();
	}

	EPhase2CommandResult FLivingAirportSimulation::QueueCommand(
		const FPhase2Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State)
	{
		const FPhase2Validation Validation = ValidateCommand(
			Command,
			CurrentGameMilliseconds,
			Phase1State);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		if (Command.Id.Value == 0 || CurrentGameMilliseconds < 0)
		{
			return EPhase2CommandResult::RejectedInvalidCommand;
		}
		PendingCommands.Add(Command);
		return EPhase2CommandResult::Accepted;
	}

	FPhase2Validation FLivingAirportSimulation::ValidateCommand(
		const FPhase2Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State) const
	{
		FPhase2Validation Result;
		Result.Result = EPhase2CommandResult::RejectedInvalidCommand;
		if (!Command.Id.IsValid())
		{
			Result.Cause = TEXT("The command has no stable ID.");
			Result.Remedy = TEXT("Submit it through the simulation command adapter.");
			return Result;
		}

		if (Command.Type == EPhase2CommandType::InitializeLivingAirport)
		{
			if (State.bInitialized)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidState;
				Result.Cause = TEXT("Living-airport systems are already active.");
				Result.Remedy = TEXT("Choose a specialization or inspect current operations.");
				return Result;
			}
			if (!Phase1State.bInitialized ||
				!Phase1State.bAirportOpen ||
				Phase1State.Flight.State != EFlightState::Completed)
			{
				Result.Result = EPhase2CommandResult::RejectedPhase1NotReady;
				Result.Cause = TEXT("The starter airport has not completed its first visit.");
				Result.Remedy = TEXT("Open the airfield and complete the Phase 1 flight.");
				return Result;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;
		}

		if (!State.bInitialized)
		{
			Result.Result = EPhase2CommandResult::RejectedInvalidState;
			Result.Cause = TEXT("Living-airport systems are not active.");
			Result.Remedy = TEXT("Complete and initialize the starter airport first.");
			return Result;
		}

		switch (Command.Type)
		{
		case EPhase2CommandType::SelectSpecialization:
			if (Command.Specialization == EAirportSpecialization::Unselected)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidCommand;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::AcceptContract:
		{
			const FPhase2ContractRecord* Contract = State.Contracts.FindByPredicate(
				[&Command](const FPhase2ContractRecord& Candidate)
				{
					return Candidate.Id == Command.ContractId;
				});
			if (!Contract)
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				Result.Cause = TEXT("The selected contract no longer exists.");
				Result.Remedy = TEXT("Choose a contract shown in the tenant panel.");
				break;
			}
			if (Contract->Specialization != State.SelectedSpecialization)
			{
				Result.Result = EPhase2CommandResult::RejectedIncompatible;
				Result.Cause = TEXT("This contract belongs to a different airport identity.");
				Result.Remedy = TEXT("Select its specialization first.");
				break;
			}
			if (Contract->bAccepted)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidState;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;
		}

		case EPhase2CommandType::CancelContract:
		{
			const FPhase2ContractRecord* Contract = State.Contracts.FindByPredicate(
				[&Command](const FPhase2ContractRecord& Candidate)
					{
						return Candidate.Id == Command.ContractId;
					});
			if (!Contract || !Contract->bAccepted)
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				break;
			}
			const bool bHasActiveFlight = State.Flights.ContainsByPredicate(
				[Contract](const FPhase2FlightRecord& Flight)
					{
						return Flight.ContractId == Contract->Id &&
							Flight.State != EPhase2FlightState::Scheduled &&
							Flight.State != EPhase2FlightState::Completed &&
							Flight.State != EPhase2FlightState::Cancelled;
					});
			if (bHasActiveFlight)
			{
				Result.Result = EPhase2CommandResult::RejectedConflict;
				Result.Cause = TEXT("An aircraft is already operating this contract.");
				Result.Remedy = TEXT("Wait until it completes before ending the agreement.");
				break;
			}
			if (Phase1State.Credits < Contract->CancellationCostCredits)
			{
				Result.Result = EPhase2CommandResult::RejectedInsufficientCredits;
				Result.QuotedCredits = Contract->CancellationCostCredits;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			Result.QuotedCredits = Contract->CancellationCostCredits;
			return Result;
		}

		case EPhase2CommandType::RescheduleFlight:
		{
			const FPhase2FlightRecord* Flight = State.Flights.FindByPredicate(
				[&Command](const FPhase2FlightRecord& Candidate)
					{
						return Candidate.Id == Command.FlightId;
					});
			if (!Flight || Flight->State != EPhase2FlightState::Scheduled)
			{
				Result.Result = EPhase2CommandResult::RejectedNotReady;
				break;
			}
			if (Command.RequestedGameTimeMilliseconds <
				CurrentGameMilliseconds + 30000)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidCommand;
				Result.Cause = TEXT("The new slot does not preserve arrival preparation.");
				Result.Remedy = TEXT("Choose a slot at least thirty game-seconds ahead.");
				break;
			}
			const int64 NewStart = Command.RequestedGameTimeMilliseconds - 15000;
			const int64 NewEnd =
				Command.RequestedGameTimeMilliseconds +
				GetPhase2Fixture().FlightCycleMilliseconds;
			const bool bConflicts = State.Reservations.ContainsByPredicate(
				[Flight, NewStart, NewEnd](const FPhase2MovementReservation& Reservation)
					{
						return Reservation.FlightId != Flight->Id &&
							Reservation.BlockId == Flight->AssignedStandId &&
							Reservation.StartGameMilliseconds < NewEnd &&
							NewStart < Reservation.EndGameMilliseconds;
					});
			if (bConflicts)
			{
				Result.Result = EPhase2CommandResult::RejectedConflict;
				Result.Cause = TEXT("The requested stand buffer overlaps another flight.");
				Result.Remedy = TEXT("Choose a later slot or another compatible stand.");
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;
		}

		case EPhase2CommandType::SetActiveRunway:
			if (Command.RunwayDirection != TEXT("06") && Command.RunwayDirection != TEXT("24"))
			{
				Result.Result = EPhase2CommandResult::RejectedIncompatible;
				Result.Cause = TEXT("Only published runway directions 06 and 24 are available.");
				Result.Remedy = TEXT("Select 06 or 24.");
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::AssignTeamZone:
			if (!ContainsId(State.Teams, Command.TeamId) || Command.ZoneId.IsNone())
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::DispatchNextService:
			if (Command.FlightId.IsValid() && !ContainsId(State.Flights, Command.FlightId))
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::RequestTow:
			if (!ContainsId(State.Flights, Command.FlightId))
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::PurchaseParcel:
		{
			const FPhase2ParcelRecord* Parcel = State.Parcels.FindByPredicate(
				[&Command](const FPhase2ParcelRecord& Candidate)
				{
					return Candidate.Id == Command.ParcelId;
				});
			if (!Parcel)
			{
				Result.Result = EPhase2CommandResult::RejectedMissingReference;
				break;
			}
			if (Parcel->bOwned)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidState;
				break;
			}
			if (Phase1State.Credits < Parcel->PurchaseCostCredits)
			{
				Result.Result = EPhase2CommandResult::RejectedInsufficientCredits;
				Result.QuotedCredits = Parcel->PurchaseCostCredits;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			Result.QuotedCredits = Parcel->PurchaseCostCredits;
			return Result;
		}

		case EPhase2CommandType::StartExpansion:
			if (!Command.ParcelId.IsValid() ||
				!ContainsId(State.Parcels, Command.ParcelId) ||
				State.Expansion.Stage != EExpansionStage::None)
			{
				Result.Result = EPhase2CommandResult::RejectedInvalidState;
				break;
			}
			if (Phase1State.Credits < GetPhase2Fixture().ExpansionCostCredits)
			{
				Result.Result = EPhase2CommandResult::RejectedInsufficientCredits;
				Result.QuotedCredits = GetPhase2Fixture().ExpansionCostCredits;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			Result.QuotedCredits = GetPhase2Fixture().ExpansionCostCredits;
			return Result;

		case EPhase2CommandType::RespondToIncident:
			if (!State.Incident.Id.IsValid() ||
				State.Incident.Id != Command.IncidentId ||
				State.Incident.State != EIncidentState::Reported)
			{
				Result.Result = EPhase2CommandResult::RejectedNotReady;
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		case EPhase2CommandType::RequestRecovery:
			if (Phase1State.Credits > 500 || State.RecoveryGrantCount >= 2)
			{
				Result.Result = EPhase2CommandResult::RejectedNotReady;
				Result.Cause = TEXT("Recovery support is reserved for a low-credit safe airport.");
				Result.Remedy = TEXT("Continue operating accepted contracts.");
				break;
			}
			Result = {true, EPhase2CommandResult::Accepted};
			return Result;

		default:
			break;
		}

		if (Result.Cause.IsEmpty())
		{
			Result.Cause = TEXT("The requested operation is not valid in the current state.");
		}
		if (Result.Remedy.IsEmpty())
		{
			Result.Remedy = TEXT("Refresh the panel and choose an available action.");
		}
		return Result;
	}

	void FLivingAirportSimulation::Step(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		PendingCommands.Sort([](const FPhase2Command& Left, const FPhase2Command& Right)
		{
			return Left.Id.Value < Right.Id.Value;
		});
		for (const FPhase2Command& Command : PendingCommands)
		{
			ApplyCommand(Command, CurrentGameMilliseconds, Phase1);
		}
		PendingCommands.Reset();

		if (!State.bInitialized)
		{
			return;
		}

		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		AdvanceOperatingDay(CurrentGameMilliseconds, Phase1);
		AdvanceServices(CurrentGameMilliseconds);
		AdvanceFlights(CurrentGameMilliseconds, Phase1);
		AdvanceExpansion(CurrentGameMilliseconds);
		AdvanceIncident(CurrentGameMilliseconds, Phase1);
		RefreshRatings(CurrentGameMilliseconds);
	}

	void FLivingAirportSimulation::ApplyCommand(
		const FPhase2Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		switch (Command.Type)
		{
		case EPhase2CommandType::InitializeLivingAirport:
			InitializeLivingAirport(CurrentGameMilliseconds);
			EmitEvent(
				EPhase2EventType::LivingAirportInitialized,
				Command.Id,
				0,
				TEXT("Living general-aviation systems are ready."),
				CurrentGameMilliseconds);
			break;

		case EPhase2CommandType::SelectSpecialization:
			State.SelectedSpecialization = Command.Specialization;
			for (FPhase2TenantRecord& Tenant : State.Tenants)
			{
				Tenant.bActive = Tenant.Specialization == Command.Specialization;
			}
			EmitEvent(
				EPhase2EventType::SpecializationSelected,
				Command.Id,
				0,
				FString::Printf(
					TEXT("%s identity selected; other paths remain available."),
					*SpecializationDisplayName(Command.Specialization)),
				CurrentGameMilliseconds);
			break;

		case EPhase2CommandType::AcceptContract:
			for (FPhase2ContractRecord& Contract : State.Contracts)
			{
				if (Contract.Id == Command.ContractId)
				{
					Contract.bAccepted = true;
					EmitEvent(
						EPhase2EventType::ContractAccepted,
						Command.Id,
						Contract.Id.Value,
						TEXT("Recurring contract accepted with explicit timetable buffers."),
						CurrentGameMilliseconds);
					CreateFlightsForDay(State.CurrentOperatingDay, CurrentGameMilliseconds);
					break;
				}
			}
			break;

		case EPhase2CommandType::CancelContract:
			for (FPhase2ContractRecord& Contract : State.Contracts)
			{
				if (Contract.Id != Command.ContractId)
				{
					continue;
				}
				if (Phase1.ApplyExternalEconomyChange(
					TEXT("ContractCancellation"),
					-Contract.CancellationCostCredits,
					0,
					TEXT("Recurring agreement ended before its next operation."),
					CurrentGameMilliseconds))
				{
					Contract.bAccepted = false;
					State.TotalPhase2CostCredits += Contract.CancellationCostCredits;
					for (FPhase2FlightRecord& Flight : State.Flights)
					{
						if (Flight.ContractId == Contract.Id &&
							Flight.State == EPhase2FlightState::Scheduled)
						{
							Flight.State = EPhase2FlightState::Cancelled;
						}
					}
					State.Reservations.RemoveAll(
						[this](const FPhase2MovementReservation& Reservation)
							{
								const FPhase2FlightRecord* Flight =
									State.Flights.FindByPredicate(
										[&Reservation](const FPhase2FlightRecord& Candidate)
											{
												return Candidate.Id == Reservation.FlightId;
											});
								return Flight &&
									Flight->State == EPhase2FlightState::Cancelled;
							});
					EmitEvent(
						EPhase2EventType::ContractCancelled,
						Command.Id,
						Contract.Id.Value,
						TEXT("Recurring agreement ended; cancellation cost itemized."),
						CurrentGameMilliseconds);
				}
				break;
			}
			break;

		case EPhase2CommandType::RescheduleFlight:
			for (FPhase2FlightRecord& Flight : State.Flights)
			{
				if (Flight.Id != Command.FlightId)
				{
					continue;
				}
				Flight.ScheduledArrivalGameMilliseconds =
					Command.RequestedGameTimeMilliseconds;
				Flight.StandStartGameMilliseconds =
					Command.RequestedGameTimeMilliseconds - 15000;
				Flight.StandEndGameMilliseconds =
					Command.RequestedGameTimeMilliseconds +
						GetPhase2Fixture().FlightCycleMilliseconds;
				Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
				for (FPhase2MovementReservation& Reservation : State.Reservations)
				{
					if (Reservation.FlightId == Flight.Id)
					{
						Reservation.StartGameMilliseconds =
							Flight.StandStartGameMilliseconds;
						Reservation.EndGameMilliseconds =
							Flight.StandEndGameMilliseconds;
					}
				}
				EmitEvent(
					EPhase2EventType::FlightRescheduled,
					Command.Id,
					Flight.Id.Value,
					TEXT("Flight moved with its protected stand buffer."),
					CurrentGameMilliseconds);
				break;
			}
			break;

		case EPhase2CommandType::SetActiveRunway:
			State.ActiveRunwayDirection = Command.RunwayDirection;
			State.ActiveRunwayReason = FString::Printf(
				TEXT("Runway %s selected for the current wind and published pattern."),
				*Command.RunwayDirection.ToString());
			EmitEvent(
				EPhase2EventType::RunwayChanged,
				Command.Id,
				0,
				State.ActiveRunwayReason,
				CurrentGameMilliseconds);
			break;

		case EPhase2CommandType::AssignTeamZone:
			for (FPhase2StaffTeamRecord& Team : State.Teams)
			{
				if (Team.Id == Command.TeamId)
				{
					Team.ZoneId = Command.ZoneId;
					EmitEvent(
						EPhase2EventType::TeamAssigned,
						Command.Id,
						Team.Id.Value,
						FString::Printf(
							TEXT("%s assigned to %s."),
							*Team.RoleId.ToString(),
							*Team.ZoneId.ToString()),
						CurrentGameMilliseconds);
					break;
				}
			}
			break;

		case EPhase2CommandType::DispatchNextService:
			for (FPhase2ServiceTaskRecord& Task : State.ServiceTasks)
			{
				if ((!Command.FlightId.IsValid() || Task.FlightId == Command.FlightId) &&
					(Task.State == EPhase2ServiceState::Queued ||
						Task.State == EPhase2ServiceState::Blocked) &&
					DispatchTask(Task, CurrentGameMilliseconds))
				{
					break;
				}
			}
			break;

		case EPhase2CommandType::RequestTow:
			State.Reservations.Add({
				TEXT("Taxi.TowRoute"),
				Command.FlightId,
				CurrentGameMilliseconds,
				CurrentGameMilliseconds + 30000,
				true
			});
			EmitEvent(
				EPhase2EventType::TowCompleted,
				Command.Id,
				Command.FlightId.Value,
				TEXT("Tug reserved a conflict-free tow route to the service stand."),
				CurrentGameMilliseconds);
			break;

		case EPhase2CommandType::PurchaseParcel:
			for (FPhase2ParcelRecord& Parcel : State.Parcels)
			{
				if (Parcel.Id == Command.ParcelId &&
					Phase1.ApplyExternalEconomyChange(
						TEXT("LandPurchase"),
						-Parcel.PurchaseCostCredits,
						0,
						TEXT("Purchased the neighboring east meadow parcel."),
						CurrentGameMilliseconds))
				{
					Parcel.bOwned = true;
					State.TotalPhase2CostCredits += Parcel.PurchaseCostCredits;
					EmitEvent(
						EPhase2EventType::ParcelPurchased,
						Command.Id,
						Parcel.Id.Value,
						TEXT("East meadow added to airport-owned land."),
						CurrentGameMilliseconds);
					break;
				}
			}
			break;

		case EPhase2CommandType::StartExpansion:
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("Construction"),
				-GetPhase2Fixture().ExpansionCostCredits,
				0,
				TEXT("Funded the east-apron expansion project."),
				CurrentGameMilliseconds))
			{
				State.TotalPhase2CostCredits += GetPhase2Fixture().ExpansionCostCredits;
				State.Expansion.Id = {AllocateDomainId()};
				State.Expansion.ParcelId = Command.ParcelId;
				State.Expansion.Stage = EExpansionStage::Funded;
				State.Expansion.FundedAtGameMilliseconds = CurrentGameMilliseconds;
				State.Expansion.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
				State.Expansion.QuotedCostCredits = GetPhase2Fixture().ExpansionCostCredits;
				State.Expansion.ClosureSummary =
					TEXT("East taxi spur closed; runway and Stand A1 remain available.");
				EmitEvent(
					EPhase2EventType::ExpansionStageChanged,
					Command.Id,
					State.Expansion.Id.Value,
					TEXT("East-apron expansion funded."),
					CurrentGameMilliseconds);
			}
			break;

		case EPhase2CommandType::RespondToIncident:
			for (const FPhase2StaffTeamRecord& Team : State.Teams)
			{
				if (Team.RoleId == TEXT("StaffRole.Emergency") && Team.bOnShift)
				{
					State.Incident.ResponseTeamId = Team.Id;
					SetIncidentState(EIncidentState::ResponseDispatched, CurrentGameMilliseconds);
					break;
				}
			}
			break;

		case EPhase2CommandType::RequestRecovery:
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("Recovery"),
				GetPhase2Fixture().RecoveryGrantCredits,
				0,
				TEXT("Phase 2 continuity grant; recurring-contract income remains required."),
				CurrentGameMilliseconds))
			{
				++State.RecoveryGrantCount;
				EmitEvent(
					EPhase2EventType::RecoveryGranted,
					Command.Id,
					0,
					TEXT("A labeled continuity grant restored a safe operating reserve."),
					CurrentGameMilliseconds);
			}
			break;
		}
	}

	void FLivingAirportSimulation::InitializeLivingAirport(const int64 CurrentGameMilliseconds)
	{
		State.bInitialized = true;
		State.InitializedAtGameMilliseconds = CurrentGameMilliseconds;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		State.CurrentOperatingDay = 0;
		State.LastEconomyOperatingDay = -1;
		State.ActiveRunwayDirection = TEXT("24");
		State.ActiveRunwayReason =
			TEXT("Runway 24 selected for the prevailing westerly wind; PAPI ready.");
		State.bApproachAidReady = true;

		auto AddAircraft = [this](
			const TCHAR* ContentId,
			const TCHAR* RoleId,
			const TCHAR* OperatorId,
			const TCHAR* TailNumber)
		{
			State.Aircraft.Add({
				{AllocateDomainId()},
				ContentId,
				RoleId,
				OperatorId,
				TailNumber,
				0,
				0,
				0,
				TEXT("No Phase 2 visits yet.")
			});
		};
		AddAircraft(
			TEXT("Aircraft.BasicTrainer.Riverbend"),
			TEXT("AircraftRole.BasicTrainer"),
			TEXT("Operator.SkywardSchool"),
			TEXT("N214ST"));
		AddAircraft(
			TEXT("Aircraft.BasicTrainer.Riverbend"),
			TEXT("AircraftRole.BasicTrainer"),
			TEXT("Operator.SkywardSchool"),
			TEXT("N228ST"));
		AddAircraft(
			TEXT("Aircraft.BasicTrainer.Riverbend"),
			TEXT("AircraftRole.BasicTrainer"),
			TEXT("Operator.SkywardSchool"),
			TEXT("N236ST"));
		AddAircraft(
			TEXT("Aircraft.TouringPiston.Wayfarer"),
			TEXT("AircraftRole.TouringPiston"),
			TEXT("Operator.MeadowAeroClub"),
			TEXT("N308MA"));
		AddAircraft(
			TEXT("Aircraft.TouringPiston.Wayfarer"),
			TEXT("AircraftRole.TouringPiston"),
			TEXT("Operator.MeadowAeroClub"),
			TEXT("N322MA"));
		AddAircraft(
			TEXT("Aircraft.BusinessTurboprop.Northstar"),
			TEXT("AircraftRole.BusinessTurboprop"),
			TEXT("Operator.NorthstarCharter"),
			TEXT("N612NC"));
		AddAircraft(
			TEXT("Aircraft.LightBusinessJet.Aster"),
			TEXT("AircraftRole.LightBusinessJet"),
			TEXT("Operator.AsterExecutive"),
			TEXT("N440AE"));

		auto AddTenantAndContract = [this](
			const EAirportSpecialization Specialization,
			const TCHAR* TenantId,
			const TCHAR* DisplayName,
			const TCHAR* Requirements)
		{
			FPhase2TenantRecord Tenant;
			Tenant.Id = {AllocateDomainId()};
			Tenant.TenantContentId = TenantId;
			Tenant.Specialization = Specialization;
			Tenant.DisplayName = DisplayName;
			Tenant.Requirements = Requirements;
			State.Tenants.Add(Tenant);

			State.Contracts.Add({
				{AllocateDomainId()},
				Tenant.Id,
				Specialization,
				ContractContentIdFor(Specialization),
				RoleForSpecialization(Specialization),
				Phase2FlightsPerOperatingDay(Specialization),
				RewardFor(Specialization),
				120,
				false,
				TEXT("Runway, Stand A1, staff coverage, and protected buffers are compatible.")
			});
		};
		AddTenantAndContract(
			EAirportSpecialization::GeneralAviation,
			TEXT("Tenant.MeadowAeroClub"),
			TEXT("Meadow Aero Club"),
			TEXT("Open grass runway, one touring stand, fuel and inspection coverage."));
		AddTenantAndContract(
			EAirportSpecialization::FlightSchool,
			TEXT("Tenant.SkywardSchool"),
			TEXT("Skyward Flight School"),
			TEXT("Three daily training slots, maintenance cover, safe circuit weather."));
		AddTenantAndContract(
			EAirportSpecialization::Charter,
			TEXT("Tenant.NorthstarCharter"),
			TEXT("Northstar Charter"),
			TEXT("Approach aid, priority turnaround, dependable timetable and deicing."));

		auto AddVehicle = [this](
			const TCHAR* ContentId,
			const TCHAR* Capability,
			const TCHAR* Depot)
		{
			State.Vehicles.Add({
				{AllocateDomainId()},
				ContentId,
				Capability,
				Depot
			});
		};
		AddVehicle(TEXT("Vehicle.OpsVan.Starter"), TEXT("Service.Inspection"), TEXT("Depot.Operations"));
		AddVehicle(TEXT("Vehicle.FuelTruck.Light"), TEXT("Service.Fuel"), TEXT("Depot.Fuel"));
		AddVehicle(TEXT("Vehicle.Deicer.Light"), TEXT("Service.Deicing"), TEXT("Depot.Service"));
		AddVehicle(TEXT("Vehicle.Tug.Light"), TEXT("Service.Tow"), TEXT("Depot.Service"));

		auto AddTeam = [this](const TCHAR* RoleId, const TCHAR* ZoneId, const int32 TeamSize)
		{
			State.Teams.Add({
				{AllocateDomainId()},
				RoleId,
				ZoneId,
				TeamSize
			});
		};
		AddTeam(TEXT("StaffRole.FlightOperations"), TEXT("Zone.Airside"), 2);
		AddTeam(TEXT("StaffRole.GroundService"), TEXT("Zone.StandA1"), 3);
		AddTeam(TEXT("StaffRole.Maintenance"), TEXT("Zone.Service"), 2);
		AddTeam(TEXT("StaffRole.Emergency"), TEXT("Zone.Airside"), 2);

		State.Parcels.Add({
			{AllocateDomainId()},
			TEXT("Parcel.EastMeadow"),
			GetPhase2Fixture().ParcelPurchaseCostCredits,
			false
		});

		static const FName RatingIds[] = {
			TEXT("Rating.PassengerExperience"),
			TEXT("Rating.TenantSatisfaction"),
			TEXT("Rating.StaffWelfare"),
			TEXT("Rating.SafetyCompliance"),
			TEXT("Rating.OperationalReliability")
		};
		for (const FName RatingId : RatingIds)
		{
			State.Ratings.Add({
				RatingId,
				70,
				TEXT("Starter-airfield readiness established."),
				TEXT("Keep resources available and resolve alerts promptly.")
			});
		}

		State.Achievements.Add({
			{AllocateDomainId()},
			TEXT("Achievement.FortnightOperator"),
			TEXT("Fortnight Operator")
		});
		State.CurrentWeather = {
			0,
			EWeatherCategory::Visual,
			240,
			5,
			10000,
			16,
			TEXT("Visual conditions; runway 24 preferred.")
		};
		for (int32 Day = 1; Day <= 3; ++Day)
		{
			FPhase2WeatherRecord Forecast = State.CurrentWeather;
			Forecast.OperatingDay = Day;
			Forecast.Category =
				Day == 2 ? EWeatherCategory::Wet : EWeatherCategory::Visual;
			Forecast.OperationalSummary = FString::Printf(
				TEXT("Day %d: %s."),
				Day + 1,
				*WeatherCategoryDisplayName(Forecast.Category));
			State.Forecast.Add(Forecast);
		}
	}

	void FLivingAirportSimulation::AdvanceOperatingDay(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		const FPhase2Fixture& Fixture = GetPhase2Fixture();
		const int32 NewOperatingDay = static_cast<int32>(
			(CurrentGameMilliseconds - State.InitializedAtGameMilliseconds) /
			Fixture.OperatingDayMilliseconds);
		while (State.CurrentOperatingDay < NewOperatingDay)
		{
			++State.CurrentOperatingDay;
			const int32 Pattern = State.CurrentOperatingDay % 6;
			State.CurrentWeather.OperatingDay = State.CurrentOperatingDay;
			State.CurrentWeather.Category =
				Pattern == 2 ? EWeatherCategory::Wet :
				Pattern == 3 ? EWeatherCategory::LowCloud :
				Pattern == 4 ? EWeatherCategory::ColdWet :
				Pattern == 5 ? EWeatherCategory::StrongCrosswind :
				EWeatherCategory::Visual;
			State.CurrentWeather.WindDirectionDegrees =
				State.CurrentWeather.Category == EWeatherCategory::StrongCrosswind ? 150 : 240;
			State.CurrentWeather.WindSpeedKnots =
				State.CurrentWeather.Category == EWeatherCategory::StrongCrosswind ? 18 : 6;
			State.CurrentWeather.VisibilityMeters =
				State.CurrentWeather.Category == EWeatherCategory::LowCloud ? 3500 : 10000;
			State.CurrentWeather.TemperatureCelsius =
				State.CurrentWeather.Category == EWeatherCategory::ColdWet ? 1 : 15;
			State.CurrentWeather.OperationalSummary = FString::Printf(
				TEXT("Day %d: %s; runway and service plans reviewed."),
				State.CurrentOperatingDay + 1,
				*WeatherCategoryDisplayName(State.CurrentWeather.Category));

			if (State.CurrentWeather.Category == EWeatherCategory::StrongCrosswind)
			{
				State.ActiveRunwayDirection = TEXT("06");
				State.ActiveRunwayReason =
					TEXT("Runway 06 selected to reduce the forecast crosswind component.");
			}
			else
			{
				State.ActiveRunwayDirection = TEXT("24");
				State.ActiveRunwayReason =
					TEXT("Runway 24 selected for the prevailing westerly wind.");
			}
			EmitEvent(
				EPhase2EventType::WeatherChanged,
				{},
				0,
				State.CurrentWeather.OperationalSummary,
				CurrentGameMilliseconds);

			CreateFlightsForDay(State.CurrentOperatingDay, CurrentGameMilliseconds);

			if (State.CurrentOperatingDay == 7 && !State.Incident.Id.IsValid())
			{
				State.Incident.Id = {AllocateDomainId()};
				State.Incident.Type = EIncidentType::DisabledAircraft;
				State.Incident.State = EIncidentState::Reported;
				State.Incident.ReportedAtGameMilliseconds = CurrentGameMilliseconds;
				State.Incident.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
				State.Incident.Cause =
					TEXT("A visiting aircraft reported a flat tire clear of the runway.");
				State.Incident.Remedy =
					TEXT("Dispatch the emergency-readiness team, contain the stand, then reopen it.");
				State.Incident.CostCredits = 300;
				EmitEvent(
					EPhase2EventType::IncidentStateChanged,
					{},
					State.Incident.Id.Value,
					State.Incident.Cause,
					CurrentGameMilliseconds);
			}
		}

		while (State.LastEconomyOperatingDay < State.CurrentOperatingDay)
		{
			++State.LastEconomyOperatingDay;
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("StaffAndLease"),
				-Fixture.DailyStaffAndLeaseCostCredits,
				0,
				FString::Printf(
					TEXT("Day %d staff shifts, vehicle depots, and tenant lease costs."),
					State.LastEconomyOperatingDay + 1),
				CurrentGameMilliseconds))
			{
				State.TotalPhase2CostCredits += Fixture.DailyStaffAndLeaseCostCredits;
				EmitEvent(
					EPhase2EventType::EconomyApplied,
					{},
					0,
					TEXT("Daily operating costs posted to the shared airport ledger."),
					CurrentGameMilliseconds);
			}
		}
	}

	void FLivingAirportSimulation::CreateFlightsForDay(
		const int32 OperatingDay,
		const int64 CurrentGameMilliseconds)
	{
		const FPhase2Fixture& Fixture = GetPhase2Fixture();
		const int64 DayStart =
			State.InitializedAtGameMilliseconds +
			static_cast<int64>(OperatingDay) * Fixture.OperatingDayMilliseconds;
		for (const FPhase2ContractRecord& Contract : State.Contracts)
		{
			if (!Contract.bAccepted)
			{
				continue;
			}
			const bool bAlreadyCreated = State.Flights.ContainsByPredicate(
				[OperatingDay, &Contract](const FPhase2FlightRecord& Flight)
				{
					return Flight.OperatingDay == OperatingDay &&
						Flight.ContractId == Contract.Id;
				});
			if (bAlreadyCreated)
			{
				continue;
			}

			TArray<FAircraftInstanceId> CompatibleAircraft;
			for (const FPhase2AircraftRecord& Aircraft : State.Aircraft)
			{
				if (Aircraft.RoleId == Contract.AircraftRoleId ||
					(Contract.Specialization == EAirportSpecialization::Charter &&
						Aircraft.RoleId == TEXT("AircraftRole.LightBusinessJet")))
				{
					CompatibleAircraft.Add(Aircraft.Id);
				}
			}
			if (CompatibleAircraft.IsEmpty())
			{
				continue;
			}

			for (int32 Index = 0; Index < Contract.FlightsPerOperatingDay; ++Index)
			{
				FPhase2FlightRecord Flight;
				Flight.Id = {AllocateDomainId()};
				Flight.ContractId = Contract.Id;
				Flight.AircraftId = CompatibleAircraft[Index % CompatibleAircraft.Num()];
				Flight.OperatingDay = OperatingDay;
				Flight.ScheduledArrivalGameMilliseconds =
					DayStart + (Index + 2) * Fixture.FlightSpacingMilliseconds;
				Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
				Flight.StandStartGameMilliseconds =
					Flight.ScheduledArrivalGameMilliseconds - 15000;
				Flight.StandEndGameMilliseconds =
					Flight.ScheduledArrivalGameMilliseconds + Fixture.FlightCycleMilliseconds;
				Flight.AssignedRunwayDirection = State.ActiveRunwayDirection;
				Flight.AssignedStandId = FName(*FString::Printf(
					TEXT("Facility.GAStand.A%d"),
					Index + 1));
				Flight.bRequiresDeicing =
					State.CurrentWeather.Category == EWeatherCategory::ColdWet;
				State.Flights.Add(Flight);
				State.Reservations.Add({
					Flight.AssignedStandId,
					Flight.Id,
					Flight.StandStartGameMilliseconds,
					Flight.StandEndGameMilliseconds,
					false
				});
				EmitEvent(
					EPhase2EventType::FlightCreated,
					{},
					Flight.Id.Value,
					FString::Printf(
						TEXT("Day %d flight scheduled with protected %s buffers."),
						OperatingDay + 1,
						*Flight.AssignedStandId.ToString()),
					CurrentGameMilliseconds);
			}
		}
	}

	void FLivingAirportSimulation::AdvanceFlights(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		for (FPhase2FlightRecord& Flight : State.Flights)
		{
			if (Flight.State == EPhase2FlightState::Completed ||
				Flight.State == EPhase2FlightState::Cancelled)
			{
				continue;
			}

			const int64 Offset = CurrentGameMilliseconds - Flight.ScheduledArrivalGameMilliseconds;
			switch (Flight.State)
			{
			case EPhase2FlightState::Scheduled:
				if (Offset >= -30000)
				{
					SetFlightState(Flight, EPhase2FlightState::Inbound, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::Inbound:
				if (Offset >= -15000)
				{
					SetFlightState(Flight, EPhase2FlightState::Approach, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::Approach:
				if (Offset >= 0)
				{
					SetFlightState(Flight, EPhase2FlightState::Landing, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::Landing:
				if (Offset >= 10000)
				{
					SetFlightState(Flight, EPhase2FlightState::TaxiIn, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::TaxiIn:
				if (Offset >= 25000)
				{
					SetFlightState(Flight, EPhase2FlightState::Turnaround, CurrentGameMilliseconds);
					CreateTurnaroundTasks(Flight, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::Turnaround:
			{
				bool bAllComplete = true;
				for (const FPhase2ServiceTaskRecord& Task : State.ServiceTasks)
				{
					if (Task.FlightId == Flight.Id && !IsServiceComplete(Task))
					{
						bAllComplete = false;
						Flight.Blocker = Task.Blocker.IsEmpty()
							? FString::Printf(
								TEXT("Waiting for %s."),
								*Task.ServiceId.ToString())
							: Task.Blocker;
						break;
					}
				}
				if (bAllComplete && Offset >= 80000)
				{
					Flight.Blocker.Reset();
					SetFlightState(
						Flight,
						EPhase2FlightState::ReadyToDepart,
						CurrentGameMilliseconds);
				}
				break;
			}
			case EPhase2FlightState::ReadyToDepart:
				if (Offset >= 90000)
				{
					SetFlightState(Flight, EPhase2FlightState::TaxiOut, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::TaxiOut:
				if (Offset >= 110000)
				{
					SetFlightState(Flight, EPhase2FlightState::Outbound, CurrentGameMilliseconds);
				}
				break;
			case EPhase2FlightState::Outbound:
				if (Offset >= GetPhase2Fixture().FlightCycleMilliseconds)
				{
					CompleteFlight(Flight, CurrentGameMilliseconds, Phase1);
				}
				break;
			default:
				break;
			}
		}
	}

	void FLivingAirportSimulation::CreateTurnaroundTasks(
		FPhase2FlightRecord& Flight,
		const int64 CurrentGameMilliseconds)
	{
		if (State.ServiceTasks.ContainsByPredicate(
			[&Flight](const FPhase2ServiceTaskRecord& Task)
				{
					return Task.FlightId == Flight.Id;
				}))
		{
			return;
		}

		FPhase2ServiceTaskRecord Inspection;
		Inspection.Id = {AllocateDomainId()};
		Inspection.FlightId = Flight.Id;
		Inspection.ServiceId = TEXT("Service.Inspection");
		Inspection.State = EPhase2ServiceState::Queued;
		Inspection.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Inspection.DurationMilliseconds = 10000;
		State.ServiceTasks.Add(Inspection);

		FPhase2ServiceTaskRecord Fuel;
		Fuel.Id = {AllocateDomainId()};
		Fuel.FlightId = Flight.Id;
		Fuel.ServiceId = TEXT("Service.Fuel");
		Fuel.State = EPhase2ServiceState::Blocked;
		Fuel.PrerequisiteTaskId = Inspection.Id;
		Fuel.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Fuel.DurationMilliseconds = 12000;
		Fuel.Blocker = TEXT("Inspection must finish before fueling.");
		State.ServiceTasks.Add(Fuel);

		if (Flight.bRequiresDeicing)
		{
			FPhase2ServiceTaskRecord Deicing;
			Deicing.Id = {AllocateDomainId()};
			Deicing.FlightId = Flight.Id;
			Deicing.ServiceId = TEXT("Service.Deicing");
			Deicing.State = EPhase2ServiceState::Blocked;
			Deicing.PrerequisiteTaskId = Fuel.Id;
			Deicing.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
			Deicing.DurationMilliseconds = 15000;
			Deicing.Blocker = TEXT("Fuel service must clear the stand before deicing.");
			State.ServiceTasks.Add(Deicing);
		}
	}

	bool FLivingAirportSimulation::DispatchTask(
		FPhase2ServiceTaskRecord& Task,
		const int64 CurrentGameMilliseconds)
	{
		if (Task.PrerequisiteTaskId.IsValid())
		{
			const FPhase2ServiceTaskRecord* Prerequisite = State.ServiceTasks.FindByPredicate(
				[&Task](const FPhase2ServiceTaskRecord& Candidate)
				{
					return Candidate.Id == Task.PrerequisiteTaskId;
				});
			if (!Prerequisite || Prerequisite->State != EPhase2ServiceState::Completed)
			{
				Task.State = EPhase2ServiceState::Blocked;
				return false;
			}
		}

		FPhase2VehicleRecord* Vehicle = State.Vehicles.FindByPredicate(
			[&Task](const FPhase2VehicleRecord& Candidate)
				{
					return Candidate.CapabilityId == Task.ServiceId &&
						Candidate.State == EPhase2VehicleState::AtDepot;
				});
		FPhase2StaffTeamRecord* Team = State.Teams.FindByPredicate(
			[&Task](const FPhase2StaffTeamRecord& Candidate)
				{
					return Candidate.RoleId == RequiredRoleForService(Task.ServiceId) &&
						Candidate.bOnShift &&
						!Candidate.AssignedTaskId.IsValid();
				});
		if (!Vehicle || !Team)
		{
			Task.State = EPhase2ServiceState::Queued;
			Task.Blocker = !Vehicle
				? TEXT("Compatible service vehicle is busy.")
				: TEXT("Qualified staff team is busy or off shift.");
			return false;
		}

		Task.State = EPhase2ServiceState::Dispatched;
		Task.AssignedVehicleId = Vehicle->Id;
		Task.AssignedTeamId = Team->Id;
		Task.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Task.Blocker.Reset();
		Vehicle->State = EPhase2VehicleState::Dispatched;
		Vehicle->AssignedTaskId = Task.Id;
		Vehicle->StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		Team->AssignedTaskId = Task.Id;
		Team->WorkloadPercent = 70;
		EmitEvent(
			EPhase2EventType::ServiceStateChanged,
			{},
			Task.Id.Value,
			FString::Printf(TEXT("%s dispatched."), *Task.ServiceId.ToString()),
			CurrentGameMilliseconds);
		return true;
	}

	void FLivingAirportSimulation::AdvanceServices(const int64 CurrentGameMilliseconds)
	{
		for (FPhase2ServiceTaskRecord& Task : State.ServiceTasks)
		{
			if (Task.State == EPhase2ServiceState::Blocked &&
				Task.PrerequisiteTaskId.IsValid())
			{
				const FPhase2ServiceTaskRecord* Prerequisite = State.ServiceTasks.FindByPredicate(
					[&Task](const FPhase2ServiceTaskRecord& Candidate)
						{
							return Candidate.Id == Task.PrerequisiteTaskId;
						});
				if (Prerequisite && Prerequisite->State == EPhase2ServiceState::Completed)
				{
					Task.State = EPhase2ServiceState::Queued;
					Task.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
					Task.Blocker.Reset();
				}
			}

			if (State.bAutomaticDispatch &&
				Task.State == EPhase2ServiceState::Queued &&
				CurrentGameMilliseconds - Task.StateChangedAtGameMilliseconds >= 2500)
			{
				DispatchTask(Task, CurrentGameMilliseconds);
			}
			else if (Task.State == EPhase2ServiceState::Dispatched &&
				CurrentGameMilliseconds - Task.StateChangedAtGameMilliseconds >= 5000)
			{
				Task.State = EPhase2ServiceState::InProgress;
				Task.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
				for (FPhase2VehicleRecord& Vehicle : State.Vehicles)
				{
					if (Vehicle.Id == Task.AssignedVehicleId)
					{
						Vehicle.State = EPhase2VehicleState::Working;
						Vehicle.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
						break;
					}
				}
			}
			else if (Task.State == EPhase2ServiceState::InProgress &&
				CurrentGameMilliseconds - Task.StateChangedAtGameMilliseconds >=
					Task.DurationMilliseconds)
			{
				Task.State = EPhase2ServiceState::Completed;
				Task.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
				for (FPhase2VehicleRecord& Vehicle : State.Vehicles)
				{
					if (Vehicle.Id == Task.AssignedVehicleId)
					{
						Vehicle.State = EPhase2VehicleState::AtDepot;
						Vehicle.AssignedTaskId = {};
						Vehicle.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
						++Vehicle.CompletedTaskCount;
						break;
					}
				}
				for (FPhase2StaffTeamRecord& Team : State.Teams)
				{
					if (Team.Id == Task.AssignedTeamId)
					{
						Team.AssignedTaskId = {};
						Team.WorkloadPercent = 20;
						Team.MoralePercent = FMath::Max(60, Team.MoralePercent - 1);
						break;
					}
				}
				EmitEvent(
					EPhase2EventType::ServiceStateChanged,
					{},
					Task.Id.Value,
					FString::Printf(TEXT("%s completed."), *Task.ServiceId.ToString()),
					CurrentGameMilliseconds);
			}
		}
	}

	void FLivingAirportSimulation::CompleteFlight(
		FPhase2FlightRecord& Flight,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		SetFlightState(Flight, EPhase2FlightState::Completed, CurrentGameMilliseconds);
		if (Flight.bRewardRecognized)
		{
			return;
		}
		const FPhase2ContractRecord* Contract = State.Contracts.FindByPredicate(
			[&Flight](const FPhase2ContractRecord& Candidate)
				{
					return Candidate.Id == Flight.ContractId;
				});
		if (!Contract)
		{
			Flight.Blocker = TEXT("Contract reference missing; revenue was not recognized.");
			return;
		}
		if (!Phase1.ApplyExternalEconomyChange(
			TEXT("Phase2FlightRevenue"),
			Contract->RewardPerFlightCredits,
			1,
			FString::Printf(
				TEXT("%s recurring flight completed with itemized service revenue."),
				*SpecializationDisplayName(Contract->Specialization)),
			CurrentGameMilliseconds))
		{
			Flight.Blocker = TEXT("Shared ledger rejected the flight reward.");
			return;
		}

		Flight.bRewardRecognized = true;
		++State.CompletedFlightCount;
		State.TotalPhase2RevenueCredits += Contract->RewardPerFlightCredits;
		for (FPhase2AircraftRecord& Aircraft : State.Aircraft)
		{
			if (Aircraft.Id == Flight.AircraftId)
			{
				++Aircraft.VisitCount;
				for (const FPhase2ServiceTaskRecord& Task : State.ServiceTasks)
				{
					if (Task.FlightId == Flight.Id &&
						Task.State == EPhase2ServiceState::Completed)
					{
						++Aircraft.ServiceCount;
					}
				}
				Aircraft.LastVisitGameMilliseconds = CurrentGameMilliseconds;
				Aircraft.HistorySummary = FString::Printf(
					TEXT("%d visits; %d completed services; last visit day %d."),
					Aircraft.VisitCount,
					Aircraft.ServiceCount,
					Flight.OperatingDay + 1);
				break;
			}
		}
		for (FPhase2TenantRecord& Tenant : State.Tenants)
		{
			if (Tenant.Id == Contract->TenantId)
			{
				Tenant.EvidencePoints += 1;
				break;
			}
		}
	}

	void FLivingAirportSimulation::SetFlightState(
		FPhase2FlightRecord& Flight,
		const EPhase2FlightState NewState,
		const int64 CurrentGameMilliseconds)
	{
		Flight.State = NewState;
		Flight.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		EmitEvent(
			EPhase2EventType::FlightStateChanged,
			{},
			Flight.Id.Value,
			FString::Printf(
				TEXT("Flight %llu advanced to state %d."),
				Flight.Id.Value,
				static_cast<int32>(NewState)),
			CurrentGameMilliseconds);
	}

	void FLivingAirportSimulation::AdvanceExpansion(const int64 CurrentGameMilliseconds)
	{
		if (State.Expansion.Stage == EExpansionStage::None ||
			State.Expansion.Stage == EExpansionStage::Operational)
		{
			return;
		}
		const int64 Elapsed =
			CurrentGameMilliseconds - State.Expansion.FundedAtGameMilliseconds;
		EExpansionStage Expected = EExpansionStage::Funded;
		if (Elapsed >= 240000)
		{
			Expected = EExpansionStage::Operational;
		}
		else if (Elapsed >= 180000)
		{
			Expected = EExpansionStage::Inspection;
		}
		else if (Elapsed >= 90000)
		{
			Expected = EExpansionStage::Building;
		}
		else if (Elapsed >= 30000)
		{
			Expected = EExpansionStage::Delivery;
		}
		if (Expected == State.Expansion.Stage)
		{
			return;
		}
		State.Expansion.Stage = Expected;
		State.Expansion.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
		State.Expansion.bDeliveryArrived =
			Expected >= EExpansionStage::Building;
		State.Expansion.bInspectionPassed =
			Expected == EExpansionStage::Operational;
		if (Expected == EExpansionStage::Operational)
		{
			State.Expansion.ClosureSummary = TEXT("East taxi spur reopened; new apron ready.");
		}
		EmitEvent(
			EPhase2EventType::ExpansionStageChanged,
			{},
			State.Expansion.Id.Value,
			FString::Printf(
				TEXT("East-apron project advanced to stage %d."),
				static_cast<int32>(Expected)),
			CurrentGameMilliseconds);
	}

	void FLivingAirportSimulation::AdvanceIncident(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		if (!State.Incident.Id.IsValid())
		{
			return;
		}
		const int64 Elapsed =
			CurrentGameMilliseconds - State.Incident.StateChangedAtGameMilliseconds;
		if (State.Incident.State == EIncidentState::Reported &&
			State.bAutomaticDispatch &&
			Elapsed >= 30000)
		{
			for (const FPhase2StaffTeamRecord& Team : State.Teams)
			{
				if (Team.RoleId == TEXT("StaffRole.Emergency") && Team.bOnShift)
				{
					State.Incident.ResponseTeamId = Team.Id;
					SetIncidentState(EIncidentState::ResponseDispatched, CurrentGameMilliseconds);
					break;
				}
			}
		}
		else if (State.Incident.State == EIncidentState::ResponseDispatched && Elapsed >= 30000)
		{
			SetIncidentState(EIncidentState::Contained, CurrentGameMilliseconds);
		}
		else if (State.Incident.State == EIncidentState::Contained && Elapsed >= 30000)
		{
			SetIncidentState(EIncidentState::Cleanup, CurrentGameMilliseconds);
		}
		else if (State.Incident.State == EIncidentState::Cleanup && Elapsed >= 30000)
		{
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("IncidentResponse"),
				-State.Incident.CostCredits,
				0,
				TEXT("Disabled-aircraft response, containment, and cleanup."),
				CurrentGameMilliseconds))
			{
				State.TotalPhase2CostCredits += State.Incident.CostCredits;
				SetIncidentState(EIncidentState::Resolved, CurrentGameMilliseconds);
				State.Incident.Remedy = TEXT("Stand reopened; review readiness and response time.");
			}
		}
	}

	void FLivingAirportSimulation::SetIncidentState(
		const EIncidentState NewState,
		const int64 CurrentGameMilliseconds)
	{
		State.Incident.State = NewState;
		State.Incident.StateChangedAtGameMilliseconds = CurrentGameMilliseconds;
		EmitEvent(
			EPhase2EventType::IncidentStateChanged,
			{},
			State.Incident.Id.Value,
			FString::Printf(
				TEXT("Incident response advanced to state %d."),
				static_cast<int32>(NewState)),
			CurrentGameMilliseconds);
	}

	void FLivingAirportSimulation::RefreshRatings(const int64 CurrentGameMilliseconds)
	{
		const bool bIncidentOpen =
			State.Incident.Id.IsValid() &&
			State.Incident.State != EIncidentState::Resolved;
		int32 BusyTeams = 0;
		int32 WorkloadTotal = 0;
		for (const FPhase2StaffTeamRecord& Team : State.Teams)
		{
			BusyTeams += Team.AssignedTaskId.IsValid() ? 1 : 0;
			WorkloadTotal += Team.WorkloadPercent;
		}
		for (FPhase2RatingRecord& Rating : State.Ratings)
		{
			if (Rating.ComponentId == TEXT("Rating.SafetyCompliance"))
			{
				Rating.Value = bIncidentOpen ? 62 : 82;
				Rating.RecentCause = bIncidentOpen
					? TEXT("A contained incident is still being resolved.")
					: TEXT("Runway checks, approach aid, and readiness coverage are current.");
			}
			else if (Rating.ComponentId == TEXT("Rating.OperationalReliability"))
			{
				Rating.Value = FMath::Clamp(70 + State.CompletedFlightCount / 2, 0, 95);
				Rating.RecentCause = TEXT("Completed recurring flights with protected buffers.");
			}
			else if (Rating.ComponentId == TEXT("Rating.StaffWelfare"))
			{
				const int32 AverageWorkload = State.Teams.IsEmpty()
					? 0
					: WorkloadTotal / State.Teams.Num();
				Rating.Value = FMath::Clamp(90 - AverageWorkload / 3, 55, 90);
				Rating.RecentCause = BusyTeams > 2
					? TEXT("Several teams are carrying active assignments.")
					: TEXT("Workload remains distributed across zones.");
			}
			else if (Rating.ComponentId == TEXT("Rating.TenantSatisfaction"))
			{
				Rating.Value = FMath::Clamp(68 + State.CompletedFlightCount, 0, 94);
				Rating.RecentCause = TEXT("Tenant evidence grows when contracted flights complete.");
			}
			else
			{
				Rating.Value = FMath::Clamp(65 + State.CompletedFlightCount / 3, 0, 90);
				Rating.RecentCause = TEXT("Small-airport service remains clear and dependable.");
			}
		}

		if (State.CurrentOperatingDay >= GetPhase2Fixture().RequiredOperatingDays &&
			!State.Achievements.IsEmpty() &&
			!State.Achievements[0].bEarned)
		{
			State.Achievements[0].bEarned = true;
			State.Achievements[0].EarnedAtGameMilliseconds = CurrentGameMilliseconds;
			EmitEvent(
				EPhase2EventType::AchievementEarned,
				{},
				State.Achievements[0].Id.Value,
				TEXT("Fortnight Operator earned from fourteen days of recorded evidence."),
				CurrentGameMilliseconds);
		}
	}

	void FLivingAirportSimulation::EmitEvent(
		const EPhase2EventType Type,
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

	uint64 FLivingAirportSimulation::AllocateDomainId()
	{
		return State.NextDomainId++;
	}

	FPhase2QuerySnapshot FLivingAirportSimulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds) const
	{
		FPhase2QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bInitialized = State.bInitialized;
		Query.OperatingDay = State.CurrentOperatingDay;
		Query.Specialization = State.SelectedSpecialization;
		Query.CompletedFlightCount = State.CompletedFlightCount;
		Query.ActiveRunwayDirection = State.ActiveRunwayDirection;
		Query.WeatherCategory = State.CurrentWeather.Category;
		Query.IncidentState = State.Incident.State;
		Query.ExpansionStage = State.Expansion.Stage;
		Query.StateChecksum = CalculateChecksum();

		for (const FPhase2FlightRecord& Flight : State.Flights)
		{
			if (Flight.State != EPhase2FlightState::Completed &&
				Flight.State != EPhase2FlightState::Cancelled)
			{
				++Query.ScheduledFlightCount;
			}
		}
		Query.ActiveAircraftCount = Query.ScheduledFlightCount;
		for (const FPhase2VehicleRecord& Vehicle : State.Vehicles)
		{
			Query.BusyVehicleCount +=
				Vehicle.State == EPhase2VehicleState::AtDepot ? 0 : 1;
		}
		int32 WorkloadTotal = 0;
		for (const FPhase2StaffTeamRecord& Team : State.Teams)
		{
			Query.BusyTeamCount += Team.AssignedTaskId.IsValid() ? 1 : 0;
			WorkloadTotal += Team.WorkloadPercent;
		}
		Query.AverageWorkloadPercent = State.Teams.IsEmpty()
			? 0
			: WorkloadTotal / State.Teams.Num();
		int32 RatingTotal = 0;
		for (const FPhase2RatingRecord& Rating : State.Ratings)
		{
			RatingTotal += Rating.Value;
		}
		Query.AverageRating = State.Ratings.IsEmpty()
			? 0
			: RatingTotal / State.Ratings.Num();

		if (!State.bInitialized)
		{
			Query.PrimaryStatus = TEXT("Living airport locked");
			Query.Cause = TEXT("The first starter-airfield visit must finish first.");
			Query.Remedy = TEXT("Complete the Phase 1 flight, then start Phase 2.");
			return Query;
		}

		Query.PrimaryStatus = State.SelectedSpecialization == EAirportSpecialization::Unselected
			? TEXT("Choose an airport identity")
			: FString::Printf(
				TEXT("Day %d · %s"),
				State.CurrentOperatingDay + 1,
				*SpecializationDisplayName(State.SelectedSpecialization));
		Query.OperationsSummary = FString::Printf(
			TEXT("%d active · %d completed · RWY %s · PAPI ready"),
			Query.ScheduledFlightCount,
			Query.CompletedFlightCount,
			*State.ActiveRunwayDirection.ToString());
		Query.WeatherSummary = State.CurrentWeather.OperationalSummary;
		Query.StaffSummary = FString::Printf(
			TEXT("%d/%d teams busy · %d%% average workload · %d/%d vehicles busy"),
			Query.BusyTeamCount,
			State.Teams.Num(),
			Query.AverageWorkloadPercent,
			Query.BusyVehicleCount,
			State.Vehicles.Num());
		Query.TenantSummary = FString::Printf(
			TEXT("%s · %d rating · three identity paths retained"),
			*SpecializationDisplayName(State.SelectedSpecialization),
			Query.AverageRating);
		Query.EconomySummary = FString::Printf(
			TEXT("+%lld revenue · -%lld operating and growth costs"),
			State.TotalPhase2RevenueCredits,
			State.TotalPhase2CostCredits);
		if (State.Incident.Id.IsValid() &&
			State.Incident.State != EIncidentState::Resolved)
		{
			Query.Cause = State.Incident.Cause;
			Query.Remedy = State.Incident.Remedy;
		}
		else
		{
			Query.Cause.Reset();
			Query.Remedy = TEXT("Inspect weather, workload, and the next timetable conflict.");
		}
		return Query;
	}

}
