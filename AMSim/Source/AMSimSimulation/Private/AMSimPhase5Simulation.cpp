#include "AMSimPhase5Simulation.h"

#include "AMSimDeterminism.h"
#include "AMSimLivingAirportSimulation.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase5IdBase = 1ull << 60;
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

		const TArray<ECargoState>& FlowStates(const ECargoFlow Flow)
		{
			static const TArray<ECargoState> Outbound = {
				ECargoState::RoadReceipt,
				ECargoState::CheckAndSecure,
				ECargoState::Storage,
				ECargoState::BuildUp,
				ECargoState::AirsideTransfer,
				ECargoState::Loaded,
				ECargoState::InFlight,
				ECargoState::Completed};
			static const TArray<ECargoState> Inbound = {
				ECargoState::InFlight,
				ECargoState::Loaded,
				ECargoState::AirsideTransfer,
				ECargoState::Storage,
				ECargoState::CheckAndSecure,
				ECargoState::RoadReceipt,
				ECargoState::Completed};
			static const TArray<ECargoState> Transfer = {
				ECargoState::InFlight,
				ECargoState::Loaded,
				ECargoState::AirsideTransfer,
				ECargoState::Storage,
				ECargoState::BuildUp,
				ECargoState::AirsideTransfer,
				ECargoState::Loaded,
				ECargoState::InFlight,
				ECargoState::Completed};
			switch (Flow)
			{
			case ECargoFlow::Inbound: return Inbound;
			case ECargoFlow::Transfer: return Transfer;
			default: return Outbound;
			}
		}

		EWarehouseZoneType ZoneTypeForClass(const ECargoClass CargoClass)
		{
			switch (CargoClass)
			{
			case ECargoClass::MailExpress: return EWarehouseZoneType::Express;
			case ECargoClass::TemperatureSensitive:
				return EWarehouseZoneType::TemperatureControlled;
			case ECargoClass::Oversized: return EWarehouseZoneType::OversizedPad;
			default: return EWarehouseZoneType::General;
			}
		}

		bool IsEventOperational(const ESpecialEventState State)
		{
			return State == ESpecialEventState::Preparing ||
				State == ESpecialEventState::Active ||
				State == ESpecialEventState::PartialSuccess ||
				State == ESpecialEventState::Cleanup;
		}

		int32 CountCompletedForSpecialization(
			const FPhase2State& Phase2,
			const EAirportSpecialization Specialization)
		{
			int32 Count = 0;
			for (const FPhase2FlightRecord& Flight : Phase2.Flights)
			{
				if (Flight.State != EPhase2FlightState::Completed)
				{
					continue;
				}
				const FPhase2ContractRecord* Contract =
					Phase2.Contracts.FindByPredicate(
						[&Flight](const FPhase2ContractRecord& Candidate)
						{
							return Candidate.Id == Flight.ContractId;
						});
				if (Contract && Contract->Specialization == Specialization)
				{
					++Count;
				}
			}
			return Count;
		}

		int32 CountDistinctRoles(const FPhase2State& Phase2)
		{
			TSet<FName> Roles;
			for (const FPhase2AircraftRecord& Aircraft : Phase2.Aircraft)
			{
				Roles.Add(Aircraft.RoleId);
			}
			return Roles.Num();
		}

		template <typename RecordType, typename IdType>
		bool ContainsId(const TArray<RecordType>& Records, const IdType Id)
		{
			return Records.ContainsByPredicate(
				[Id](const RecordType& Record) { return Record.Id == Id; });
		}
	}

	FPhase5Simulation::FPhase5Simulation(const uint64 MasterSeed)
	{
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase5.CargoBreadth"));
		State.NextDomainId = Phase5IdBase;
		State.NextEventSequence = 1;
	}

	EPhase5CommandResult FPhase5Simulation::QueueCommand(
		const FPhase5Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		(void)CurrentGameMilliseconds;
		(void)Phase3State;
		(void)Phase4State;
		const FPhase5Validation Validation =
			ValidateCommand(Command, Phase1State, Phase2State);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		PendingCommands.Add(Command);
		return EPhase5CommandResult::Accepted;
	}

	FPhase5Validation FPhase5Simulation::ValidateCommand(
		const FPhase5Command& Command,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State) const
	{
		FPhase5Validation Validation;
		Validation.Result = EPhase5CommandResult::RejectedInvalidCommand;
		if (!Command.Id.IsValid())
		{
			Validation.Cause = TEXT("The command has no stable identity.");
			Validation.Remedy = TEXT("Submit the action again.");
			return Validation;
		}
		if (Command.Type == EPhase5CommandType::InitializeBreadth)
		{
			if (State.bInitialized)
			{
				Validation.Result = EPhase5CommandResult::RejectedInvalidState;
				Validation.Cause = TEXT("Phase 5 is already initialized.");
				return Validation;
			}
			if (!Phase1State.bAirportOpen || !Phase2State.bInitialized)
			{
				Validation.Result =
					EPhase5CommandResult::RejectedPhase2NotReady;
				Validation.Cause =
					TEXT("A living operational airport is required.");
				Validation.Remedy =
					TEXT("Open the airport and initialize living operations.");
				return Validation;
			}
			Validation.bValid = true;
			Validation.Result = EPhase5CommandResult::Accepted;
			return Validation;
		}
		if (!State.bInitialized)
		{
			Validation.Result = EPhase5CommandResult::RejectedInvalidState;
			Validation.Cause = TEXT("Phase 5 has not been initialized.");
			return Validation;
		}

		switch (Command.Type)
		{
		case EPhase5CommandType::AcceptNextCargoContract:
			Validation.bValid = State.CargoContracts.ContainsByPredicate(
				[](const FPhase5CargoContractRecord& Contract)
				{
					return !Contract.bAccepted;
				});
			break;
		case EPhase5CommandType::AdvanceNextCargoShipment:
			Validation.bValid = State.Shipments.ContainsByPredicate(
				[&Command](const FPhase5ShipmentRecord& Shipment)
				{
					return (!Command.ShipmentId.IsValid() ||
							Shipment.Id == Command.ShipmentId) &&
						Shipment.State != ECargoState::Offered &&
						Shipment.State != ECargoState::Completed;
				});
			break;
		case EPhase5CommandType::AcceptNextProviderTenant:
			Validation.bValid = State.ProviderTenants.ContainsByPredicate(
				[](const FPhase5ProviderTenantRecord& Tenant)
				{
					return Tenant.State == EProviderTenantState::Offered;
				});
			break;
		case EPhase5CommandType::TriggerTenantRequirementLoss:
			Validation.bValid = State.ProviderTenants.ContainsByPredicate(
				[](const FPhase5ProviderTenantRecord& Tenant)
				{
					return Tenant.State == EProviderTenantState::Active;
				});
			break;
		case EPhase5CommandType::RecoverTenant:
			Validation.bValid = State.ProviderTenants.ContainsByPredicate(
				[](const FPhase5ProviderTenantRecord& Tenant)
				{
					return Tenant.State == EProviderTenantState::Grace ||
						Tenant.State == EProviderTenantState::Suspended;
				});
			break;
		case EPhase5CommandType::AcceptNextEvent:
		case EPhase5CommandType::DeclineNextEvent:
			Validation.bValid = State.SpecialEvents.ContainsByPredicate(
				[](const FPhase5SpecialEventRecord& Event)
				{
					return Event.State == ESpecialEventState::Offered;
				});
			break;
		case EPhase5CommandType::AdvanceActiveEvent:
			Validation.bValid = State.SpecialEvents.ContainsByPredicate(
				[&Command](const FPhase5SpecialEventRecord& Event)
				{
					return (!Command.EventId.IsValid() ||
							Event.Id == Command.EventId) &&
						IsEventOperational(Event.State);
				});
			break;
		case EPhase5CommandType::ReplaceObjective:
			Validation.bValid =
				Command.ObjectiveIndex >= 0 &&
				Command.ObjectiveIndex < State.Objectives.Num();
			break;
		default:
			break;
		}
		Validation.Result = Validation.bValid
			? EPhase5CommandResult::Accepted
			: EPhase5CommandResult::RejectedNotReady;
		if (!Validation.bValid)
		{
			Validation.Cause = TEXT("No eligible item is ready for that action.");
			Validation.Remedy = TEXT("Complete the current prerequisite first.");
		}
		return Validation;
	}

	void FPhase5Simulation::Step(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		PendingCommands.Sort(
			[](const FPhase5Command& Left, const FPhase5Command& Right)
			{
				return Left.Id.Value < Right.Id.Value;
			});
		for (const FPhase5Command& Command : PendingCommands)
		{
			ApplyCommand(
				Command,
				CurrentGameMilliseconds,
				Phase1,
				Phase2,
				Phase3State,
				Phase4State);
		}
		PendingCommands.Reset();
		if (!State.bInitialized)
		{
			return;
		}
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		State.CurrentOperatingDay = static_cast<int32>(
			(CurrentGameMilliseconds - State.InitializedAtGameMilliseconds) /
			GetPhase5Fixture().OperatingDayMilliseconds);
		RefreshCargo(
			CurrentGameMilliseconds,
			Phase1,
			Phase2.GetState());
		RefreshTenants(CurrentGameMilliseconds);
		RefreshEvents(CurrentGameMilliseconds, Phase1);
		RefreshProgression(
			CurrentGameMilliseconds,
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3State,
			Phase4State);
	}

	void FPhase5Simulation::ApplyCommand(
		const FPhase5Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		if (Command.Type == EPhase5CommandType::InitializeBreadth)
		{
			Initialize(
				CurrentGameMilliseconds,
				Phase2,
				Phase3State,
				Phase4State);
			Emit(
				EPhase5EventType::BreadthInitialized,
				Command.Id,
				0,
				CurrentGameMilliseconds,
				TEXT("Cargo, providers, events, and non-exclusive paths are available."));
			return;
		}

		if (Command.Type == EPhase5CommandType::AcceptNextCargoContract)
		{
			FPhase5CargoContractRecord* Contract =
				State.CargoContracts.FindByPredicate(
					[](const FPhase5CargoContractRecord& Candidate)
					{
						return !Candidate.bAccepted;
					});
			if (!Contract)
			{
				return;
			}
			Contract->bAccepted = true;
			FPhase5ShipmentRecord* Shipment = State.Shipments.FindByPredicate(
				[Contract](const FPhase5ShipmentRecord& Candidate)
				{
					return Candidate.ContractId == Contract->Id;
				});
			if (Shipment)
			{
				Shipment->FlowStep = 0;
				Shipment->State = FlowStates(Shipment->Flow)[0];
				Shipment->CurrentLocation =
					CargoStateDisplayName(Shipment->State);
			}
			Emit(
				EPhase5EventType::CargoContractAccepted,
				Command.Id,
				Contract->Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s accepted."),
					*Contract->DisplayName));
			return;
		}

		if (Command.Type == EPhase5CommandType::AdvanceNextCargoShipment)
		{
			FPhase5ShipmentRecord* Shipment = State.Shipments.FindByPredicate(
				[&Command](const FPhase5ShipmentRecord& Candidate)
				{
					return (!Command.ShipmentId.IsValid() ||
							Candidate.Id == Command.ShipmentId) &&
						Candidate.State != ECargoState::Offered &&
						Candidate.State != ECargoState::Completed;
				});
			if (!Shipment)
			{
				return;
			}
			if (Shipment->State == ECargoState::Exception)
			{
				Shipment->State = FlowStates(Shipment->Flow)[
					FMath::Clamp(
						Shipment->FlowStep,
						0,
						FlowStates(Shipment->Flow).Num() - 1)];
				Shipment->DeadlineGameMilliseconds +=
					GetPhase5Fixture().OperatingDayMilliseconds;
				Shipment->ExceptionCause.Reset();
				Shipment->Remedy.Reset();
				return;
			}
			if (Shipment->ActiveServiceTaskId.IsValid())
			{
				return;
			}
			const TArray<ECargoState>& States = FlowStates(Shipment->Flow);
			if (!States.IsValidIndex(Shipment->FlowStep + 1))
			{
				return;
			}
			const ECargoState NextState = States[Shipment->FlowStep + 1];
			if (NextState == ECargoState::Storage)
			{
				const FPhase5WarehouseZoneRecord* Zone =
					State.WarehouseZones.FindByPredicate(
						[Shipment](const FPhase5WarehouseZoneRecord& Candidate)
						{
							return Candidate.Id == Shipment->WarehouseZoneId;
						});
				if (!Zone || !Zone->bOperational ||
					Zone->OccupiedLitres + Shipment->VolumeLitres >
						Zone->CapacityLitres)
				{
					Shipment->State = ECargoState::Exception;
					Shipment->ExceptionCause =
						TEXT("Compatible storage is unavailable or full.");
					Shipment->Remedy =
						TEXT("Restore the assigned zone or free compatible capacity.");
					return;
				}
			}
			const FName OperationId(*FString::Printf(
				TEXT("Cargo.Step.%d.%s"),
				Shipment->FlowStep + 1,
				*CargoStateDisplayName(NextState).Replace(TEXT(" "), TEXT(""))));
			Shipment->ActiveServiceTaskId =
				Phase2.RegisterExternalServiceTask(
					TEXT("Phase5"),
					Shipment->Id.Value,
					OperationId,
					FMath::Max(1, Shipment->Pieces),
					4000 + Shipment->Pieces * 250,
					CurrentGameMilliseconds);
			Emit(
				EPhase5EventType::CargoTaskRequested,
				Command.Id,
				Shipment->Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s work requested for %s."),
					*CargoStateDisplayName(NextState),
					*CargoClassDisplayName(Shipment->CargoClass)));
			return;
		}

		if (Command.Type == EPhase5CommandType::AcceptNextProviderTenant)
		{
			FPhase5ProviderTenantRecord* Tenant =
				State.ProviderTenants.FindByPredicate(
					[](const FPhase5ProviderTenantRecord& Candidate)
					{
						return Candidate.State == EProviderTenantState::Offered;
					});
			if (!Tenant)
			{
				return;
			}
			Tenant->State = EProviderTenantState::Active;
			Phase1.ApplyExternalEconomyChange(
				TEXT("TenantSupport"),
				-Tenant->OpeningCostCredits,
				0,
				FString::Printf(
					TEXT("%s opening support."),
					*Tenant->DisplayName),
				CurrentGameMilliseconds);
			Emit(
				EPhase5EventType::TenantAccepted,
				Command.Id,
				Tenant->Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(TEXT("%s opened."), *Tenant->DisplayName));
			return;
		}

		if (Command.Type ==
			EPhase5CommandType::TriggerTenantRequirementLoss)
		{
			FPhase5ProviderTenantRecord* Tenant =
				State.ProviderTenants.FindByPredicate(
					[](const FPhase5ProviderTenantRecord& Candidate)
					{
						return Candidate.State == EProviderTenantState::Active;
					});
			if (Tenant)
			{
				Tenant->State = EProviderTenantState::Grace;
				Tenant->GraceEndsAtGameMilliseconds =
					CurrentGameMilliseconds +
					GetPhase5Fixture().TenantGraceMilliseconds;
				Tenant->Cause = TEXT("A required operating capability is offline.");
				Tenant->Remedy =
					TEXT("Restore the capability before the grace period ends.");
				Emit(
					EPhase5EventType::TenantGraceStarted,
					Command.Id,
					Tenant->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Tenant recovery grace started."));
			}
			return;
		}

		if (Command.Type == EPhase5CommandType::RecoverTenant)
		{
			FPhase5ProviderTenantRecord* Tenant =
				State.ProviderTenants.FindByPredicate(
					[](const FPhase5ProviderTenantRecord& Candidate)
					{
						return Candidate.State == EProviderTenantState::Grace ||
							Candidate.State == EProviderTenantState::Suspended;
					});
			if (Tenant)
			{
				Tenant->State = EProviderTenantState::Active;
				Tenant->GraceEndsAtGameMilliseconds = 0;
				Tenant->Cause.Reset();
				Tenant->Remedy.Reset();
				Emit(
					EPhase5EventType::TenantRecovered,
					Command.Id,
					Tenant->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Tenant requirements restored."));
			}
			return;
		}

		if (Command.Type == EPhase5CommandType::AcceptNextEvent ||
			Command.Type == EPhase5CommandType::DeclineNextEvent)
		{
			FPhase5SpecialEventRecord* Event =
				State.SpecialEvents.FindByPredicate(
					[](const FPhase5SpecialEventRecord& Candidate)
					{
						return Candidate.State == ESpecialEventState::Offered;
					});
			if (!Event)
			{
				return;
			}
			if (Command.Type == EPhase5CommandType::DeclineNextEvent)
			{
				Event->State = ESpecialEventState::Cooldown;
				Event->CooldownEndsAtGameMilliseconds =
					CurrentGameMilliseconds +
					GetPhase5Fixture().EventCooldownMilliseconds;
				Emit(
					EPhase5EventType::EventDeclined,
					Command.Id,
					Event->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Event declined without penalty."));
				return;
			}

			Event->LinkedEntityIds.Reset();
			if (Event->Family == ESpecialEventFamily::CargoSurge)
			{
				Event->IntegrationDomain = TEXT("Cargo");
				for (const FPhase5ShipmentRecord& Shipment : State.Shipments)
				{
					if (Shipment.State != ECargoState::Offered)
					{
						Event->LinkedEntityIds.Add(Shipment.Id.Value);
					}
				}
			}
			else if (
				Event->Family == ESpecialEventFamily::HolidayPassengerSurge)
			{
				Event->IntegrationDomain = TEXT("Passenger");
				for (const FPassengerRecord& Passenger : Phase3State.Passengers)
				{
					Event->LinkedEntityIds.Add(Passenger.Id.Value);
				}
			}
			else if (!Phase4State.Flights.IsEmpty())
			{
				Event->IntegrationDomain = TEXT("Flight");
				Event->LinkedEntityIds.Add(Phase4State.Flights[0].Id.Value);
			}
			else
			{
				Event->IntegrationDomain = TEXT("LivingAirport");
				for (const FPhase2FlightRecord& Flight :
					Phase2.GetState().Flights)
				{
					Event->LinkedEntityIds.Add(Flight.Id.Value);
					if (Event->LinkedEntityIds.Num() >= 3)
					{
						break;
					}
				}
			}
			if (Event->LinkedEntityIds.IsEmpty() &&
				!Phase2.GetState().Vehicles.IsEmpty())
			{
				Event->IntegrationDomain = TEXT("OperatingCapacity");
				Event->LinkedEntityIds.Add(
					Phase2.GetState().Vehicles[0].Id.Value);
			}
			if (Event->LinkedEntityIds.IsEmpty())
			{
				return;
			}
			Event->State = ESpecialEventState::Preparing;
			Event->NoticeEndsAtGameMilliseconds =
				CurrentGameMilliseconds +
				GetPhase5Fixture().EventPreparationMilliseconds;
			Emit(
				EPhase5EventType::EventAccepted,
				Command.Id,
				Event->Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(TEXT("%s accepted."), *Event->DisplayName));
			return;
		}

		if (Command.Type == EPhase5CommandType::AdvanceActiveEvent)
		{
			FPhase5SpecialEventRecord* Event =
				State.SpecialEvents.FindByPredicate(
					[&Command](const FPhase5SpecialEventRecord& Candidate)
					{
						return (!Command.EventId.IsValid() ||
								Candidate.Id == Command.EventId) &&
							IsEventOperational(Candidate.State);
					});
			if (!Event)
			{
				return;
			}
			if (Event->State == ESpecialEventState::Preparing)
			{
				Event->State = ESpecialEventState::Active;
				Event->ActiveEndsAtGameMilliseconds =
					CurrentGameMilliseconds +
					GetPhase5Fixture().EventActiveMilliseconds;
			}
			else if (Event->State == ESpecialEventState::Active)
			{
				Event->SatisfiedDemand = FMath::Min(
					Event->ExpectedDemand,
					FMath::Max(
						1,
						Event->LinkedEntityIds.Num() * 20));
				Event->State = ESpecialEventState::PartialSuccess;
				Event->SuccessSummary = FString::Printf(
					TEXT("%d / %d demand served; cleanup remains."),
					Event->SatisfiedDemand,
					Event->ExpectedDemand);
			}
			else if (Event->State == ESpecialEventState::PartialSuccess)
			{
				Event->State = ESpecialEventState::Cleanup;
			}
			else
			{
				Event->State = ESpecialEventState::Completed;
				Event->SuccessSummary = FString::Printf(
					TEXT("%d / %d demand served; ordinary cleanup complete."),
					Event->SatisfiedDemand,
					Event->ExpectedDemand);
			}
			Emit(
				EPhase5EventType::EventStateChanged,
				Command.Id,
				Event->Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s advanced to state %d."),
					*Event->DisplayName,
					static_cast<int32>(Event->State)));
			return;
		}

		if (Command.Type == EPhase5CommandType::ReplaceObjective)
		{
			FPhase5ObjectiveRecord& Objective =
				State.Objectives[Command.ObjectiveIndex];
			const int32 Rotation =
				(static_cast<int32>(Objective.Path) + 1) % 6;
			Objective.Path = static_cast<ESpecializationPath>(Rotation);
			Objective.ContentId = FName(*FString::Printf(
				TEXT("Objective.Phase5.%s"),
				*SpecializationPathDisplayName(Objective.Path).
					Replace(TEXT(" "), TEXT(""))));
			Objective.Text = FString::Printf(
				TEXT("Build evidence for %s"),
				*SpecializationPathDisplayName(Objective.Path));
			Objective.Current = 0;
			Objective.Target = 1;
			Objective.bCompleted = false;
			Emit(
				EPhase5EventType::ObjectiveReplaced,
				Command.Id,
				Objective.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Suggested objective replaced without penalty."));
		}
	}

	void FPhase5Simulation::Initialize(
		const int64 CurrentGameMilliseconds,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		State.bInitialized = true;
		State.InitializedAtGameMilliseconds = CurrentGameMilliseconds;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		Phase2.EnsureCargoServiceResources(CurrentGameMilliseconds);

		const struct
		{
			const TCHAR* Id;
			EWarehouseZoneType Type;
			int64 Capacity;
		} Zones[] = {
			{TEXT("Warehouse.General"), EWarehouseZoneType::General, 180000},
			{TEXT("Warehouse.Express"), EWarehouseZoneType::Express, 80000},
			{TEXT("Warehouse.Cold"), EWarehouseZoneType::TemperatureControlled, 60000},
			{TEXT("Warehouse.Oversized"), EWarehouseZoneType::OversizedPad, 240000}};
		for (const auto& ZoneDefinition : Zones)
		{
			FPhase5WarehouseZoneRecord Zone;
			Zone.Id = {AllocateId()};
			Zone.ContentId = ZoneDefinition.Id;
			Zone.Type = ZoneDefinition.Type;
			Zone.CapacityLitres = ZoneDefinition.Capacity;
			State.WarehouseZones.Add(Zone);
		}

		const struct
		{
			const TCHAR* ContractId;
			const TCHAR* DisplayName;
			ECargoClass CargoClass;
			ECargoFlow Flow;
			int32 Pieces;
			int64 Mass;
			int64 Volume;
			const TCHAR* Aircraft;
			bool bBelly;
		} CargoDefinitions[] = {
			{TEXT("Contract.Cargo.GeneralOutbound"), TEXT("Meadow Freight Outbound"),
				ECargoClass::GeneralFreight, ECargoFlow::Outbound, 18, 2100, 16000,
				TEXT("Aircraft.FeederFreighter.Phase5"), false},
			{TEXT("Contract.Cargo.ExpressInbound"), TEXT("Swiftpost Express Inbound"),
				ECargoClass::MailExpress, ECargoFlow::Inbound, 32, 1100, 9000,
				TEXT("Aircraft.FeederFreighter.Phase5"), false},
			{TEXT("Contract.Cargo.ColdTransfer"), TEXT("Freshlink Cold Transfer"),
				ECargoClass::TemperatureSensitive, ECargoFlow::Transfer, 12, 2800, 12000,
				TEXT("Aircraft.RegionalFreighter.Phase5"), false},
			{TEXT("Contract.Cargo.OversizedOutbound"), TEXT("Northstar Project Cargo"),
				ECargoClass::Oversized, ECargoFlow::Outbound, 3, 7200, 48000,
				TEXT("Aircraft.RegionalFreighter.Phase5"), false},
			{TEXT("Contract.Cargo.BellyGeneral"), TEXT("Riverbend Belly Freight"),
				ECargoClass::GeneralFreight, ECargoFlow::Outbound, 8, 480, 3200,
				TEXT("Aircraft.RegionalTurboprop.Phase4"), true}};
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(CargoDefinitions); ++Index)
		{
			const auto& Definition = CargoDefinitions[Index];
			FPhase5CargoContractRecord Contract;
			Contract.Id = {AllocateId()};
			Contract.ContentId = Definition.ContractId;
			Contract.OperatorId = Index == 4
				? TEXT("Operator.RiverbendConnect.Phase4")
				: TEXT("Operator.MeadowFreight");
			Contract.DisplayName = Definition.DisplayName;
			Contract.CargoClass = Definition.CargoClass;
			Contract.Flow = Definition.Flow;
			Contract.AircraftContentId = Definition.Aircraft;
			Contract.RewardCredits = 350 + Index * 125;
			State.CargoContracts.Add(Contract);

			FPhase5ShipmentRecord Shipment;
			Shipment.Id = {AllocateId()};
			Shipment.ContractId = Contract.Id;
			Shipment.CargoClass = Definition.CargoClass;
			Shipment.Flow = Definition.Flow;
			Shipment.Pieces = Definition.Pieces;
			Shipment.MassKilograms = Definition.Mass;
			Shipment.VolumeLitres = Definition.Volume;
			Shipment.DeadlineGameMilliseconds =
				CurrentGameMilliseconds +
				GetPhase5Fixture().OperatingDayMilliseconds * 8;
			Shipment.bBellyFreight = Definition.bBelly;
			Shipment.AircraftContentId = Definition.Aircraft;
			if (Definition.bBelly && !Phase4State.Flights.IsEmpty())
			{
				Shipment.LinkedFlightId = Phase4State.Flights[0].Id;
			}
			FPhase5WarehouseZoneRecord* Zone =
				State.WarehouseZones.FindByPredicate(
					[&Definition](const FPhase5WarehouseZoneRecord& Candidate)
					{
						return Candidate.Type ==
							ZoneTypeForClass(Definition.CargoClass);
					});
			Shipment.WarehouseZoneId = Zone ? Zone->Id : FWarehouseZoneId{};
			Shipment.CurrentLocation = TEXT("Awaiting contract");
			State.Shipments.Add(Shipment);
		}

		const struct
		{
			const TCHAR* Id;
			const TCHAR* Name;
			const TCHAR* Type;
			const TCHAR* Footprint;
			const TCHAR* Capability;
			const TCHAR* Requirement;
			const TCHAR* Expectations;
			const TCHAR* Pattern;
			const TCHAR* SatisfactionDrivers;
			int64 Cost;
			int64 Rent;
			int32 Share;
			int32 Term;
		} TenantDefinitions[] = {
			{TEXT("Tenant.FBO.Riverbend"), TEXT("Riverbend Flight Services"),
				TEXT("FBO"), TEXT("Footprint.Apron.Small"), TEXT("Regional.GA"),
				TEXT("Operational apron and fuel access"),
				TEXT("Reliable fuel, parking, and crew handling"),
				TEXT("Daylight base with on-demand callouts"),
				TEXT("Dispatch time, fuel availability, and stand access"),
				900, 45, 8, 28},
			{TEXT("Tenant.Maintenance.Hearthside"), TEXT("Hearthside Maintenance"),
				TEXT("Maintenance"), TEXT("Footprint.Hangar.Small"),
				TEXT("Regional.GA"), TEXT("Reachable maintenance bay"),
				TEXT("Qualified coverage and parts continuity"),
				TEXT("Scheduled day shift with emergency callout"),
				TEXT("Work completion, bay access, and qualified coverage"),
				1100, 55, 6, 35},
			{TEXT("Tenant.Fuel.Meadow"), TEXT("Meadow Fuel Cooperative"),
				TEXT("Fuel"), TEXT("Footprint.FuelDepot.Small"),
				TEXT("Established"), TEXT("Fuel depot and safe stand route"),
				TEXT("Safe, timely replenishment for accepted aircraft"),
				TEXT("Daily delivery window and on-demand dispatch"),
				TEXT("Stock continuity, route access, and dispatch time"),
				850, 40, 7, 28},
			{TEXT("Tenant.Cargo.MeadowFreight"), TEXT("Meadow Freight"),
				TEXT("CargoOperator"), TEXT("Footprint.Warehouse.Medium"),
				TEXT("Regional.Cargo"),
				TEXT("Road-connected operational warehouse"),
				TEXT("Secure receipt, compatible storage, and flight handoff"),
				TEXT("Two road banks aligned with cargo departures"),
				TEXT("Deadline performance, storage fit, and dock access"),
				1400, 80, 10, 42},
			{TEXT("Tenant.Concession.SkyCafe"), TEXT("Sky Cafe"),
				TEXT("FoodConcession"), TEXT("Footprint.Concourse.Kiosk"),
				TEXT("Regional.Passenger"),
				TEXT("Open terminal or event visitor area"),
				TEXT("Clean service point with predictable visitor access"),
				TEXT("Flight banks and event peaks"),
				TEXT("Footfall, queue time, and operating continuity"),
				650, 35, 12, 28},
			{TEXT("Tenant.Concession.RunwayGoods"), TEXT("Runway Goods"),
				TEXT("RetailConcession"), TEXT("Footprint.Concourse.Kiosk"),
				TEXT("Regional.Passenger"),
				TEXT("Open terminal or event visitor area"),
				TEXT("Reliable retail service during published hours"),
				TEXT("Flight banks, weekends, and event peaks"),
				TEXT("Footfall, opening coverage, and stock continuity"),
				700, 38, 11, 35}};
		for (const auto& Definition : TenantDefinitions)
		{
			FPhase5ProviderTenantRecord Tenant;
			Tenant.Id = {AllocateId()};
			Tenant.ContentId = Definition.Id;
			Tenant.DisplayName = Definition.Name;
			Tenant.ProviderType = Definition.Type;
			Tenant.FootprintId = Definition.Footprint;
			Tenant.OpeningCostCredits = Definition.Cost;
			Tenant.RentCreditsPerOperatingDay = Definition.Rent;
			Tenant.RevenueSharePercent = Definition.Share;
			Tenant.ContractTermOperatingDays = Definition.Term;
			Tenant.RequiredCapability = Definition.Capability;
			Tenant.Requirement = Definition.Requirement;
			Tenant.ServiceExpectations = Definition.Expectations;
			Tenant.OperatingPattern = Definition.Pattern;
			Tenant.SatisfactionDrivers = Definition.SatisfactionDrivers;
			Tenant.Remedy = TEXT("Restore the required capability during grace.");
			State.ProviderTenants.Add(Tenant);
		}

		for (int32 Index = 0; Index < 6; ++Index)
		{
			FPhase5PathEvidenceRecord Path;
			Path.Path = static_cast<ESpecializationPath>(Index);
			Path.Band = ECapabilityBand::Established;
			Path.CurrentEvidence = TEXT("Evidence is building.");
			Path.NextRequirement = TEXT("Reach the Advanced capability requirements.");
			State.Paths.Add(Path);

			FPhase5AchievementRecord Achievement;
			Achievement.Id = {AllocateId()};
			Achievement.ContentId = FName(*FString::Printf(
				TEXT("Achievement.Advanced.%d"), Index));
			Achievement.Path = Path.Path;
			Achievement.DisplayName = FString::Printf(
				TEXT("Advanced %s"),
				*SpecializationPathDisplayName(Path.Path));
			State.Achievements.Add(Achievement);
		}

		for (int32 Index = 0; Index < 3; ++Index)
		{
			FPhase5ObjectiveRecord Objective;
			Objective.Id = {AllocateId()};
			Objective.ContentId = FName(*FString::Printf(
				TEXT("Objective.Phase5.%d"), Index));
			Objective.Path = static_cast<ESpecializationPath>(Index + 3);
			Objective.Text = FString::Printf(
				TEXT("Build evidence for %s"),
				*SpecializationPathDisplayName(Objective.Path));
			Objective.Target = 1;
			Objective.RewardAirportPoints = 2;
			State.Objectives.Add(Objective);
		}

		for (int32 Index = 0; Index < 8; ++Index)
		{
			FPhase5SpecialEventRecord Event;
			Event.Id = {AllocateId()};
			Event.Family = static_cast<ESpecialEventFamily>(Index);
			Event.ContentId = FName(*FString::Printf(
				TEXT("Event.Phase5.%d"), Index));
			Event.DisplayName = SpecialEventDisplayName(Event.Family);
			Event.ExpectedDemand = 40 + Index * 10;
			Event.RewardCredits = 600 + Index * 125;
			Event.Preview = FString::Printf(
				TEXT("%s: review movements, visitors, parking, staffing, safety, and temporary closures."),
				*Event.DisplayName);
			State.SpecialEvents.Add(Event);
		}

		for (const FName Component : {
			TEXT("Rating.SafetyReadiness"),
			TEXT("Rating.OperationalReliability"),
			TEXT("Rating.CustomerExperience"),
			TEXT("Rating.TenantRelationships"),
			TEXT("Rating.AccessCleanlinessAmenities")})
		{
			AddRatingContribution(
				Component,
				TEXT("MigrationBaseline"),
				20,
				CurrentGameMilliseconds,
				TEXT("Accepted Phase 1-4 operating baseline."));
		}
		(void)Phase3State;
	}

	void FPhase5Simulation::RefreshCargo(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		const FPhase2State& Phase2State)
	{
		for (FPhase5ShipmentRecord& Shipment : State.Shipments)
		{
			if (Shipment.State == ECargoState::Offered ||
				Shipment.State == ECargoState::Completed ||
				Shipment.State == ECargoState::Exception)
			{
				continue;
			}
			if (CurrentGameMilliseconds > Shipment.DeadlineGameMilliseconds)
			{
				Shipment.State = ECargoState::Exception;
				Shipment.ExceptionCause = TEXT("The cargo deadline was missed.");
				Shipment.Remedy =
					TEXT("Apply the recovery action to extend the handling window.");
				AddRatingContribution(
					TEXT("Rating.OperationalReliability"),
					TEXT("CargoException"),
					-3,
					CurrentGameMilliseconds,
					Shipment.ExceptionCause);
				continue;
			}
			if (!Shipment.ActiveServiceTaskId.IsValid())
			{
				continue;
			}
			const FPhase2ServiceTaskRecord* Task =
				Phase2State.ServiceTasks.FindByPredicate(
					[&Shipment](const FPhase2ServiceTaskRecord& Candidate)
					{
						return Candidate.Id ==
							Shipment.ActiveServiceTaskId;
					});
			if (!Task || Task->State != EPhase2ServiceState::Completed)
			{
				continue;
			}

			const TArray<ECargoState>& States = FlowStates(Shipment.Flow);
			const ECargoState PreviousState = Shipment.State;
			++Shipment.FlowStep;
			Shipment.State = States[
				FMath::Clamp(Shipment.FlowStep, 0, States.Num() - 1)];
			Shipment.CurrentLocation = CargoStateDisplayName(Shipment.State);
			Shipment.ActiveServiceTaskId = {};
			if (Shipment.State == ECargoState::CheckAndSecure ||
				PreviousState == ECargoState::CheckAndSecure)
			{
				Shipment.bSecurityCleared = true;
			}
			FPhase5WarehouseZoneRecord* Zone =
				State.WarehouseZones.FindByPredicate(
					[&Shipment](const FPhase5WarehouseZoneRecord& Candidate)
					{
						return Candidate.Id == Shipment.WarehouseZoneId;
					});
			if (Zone && Shipment.State == ECargoState::Storage &&
				PreviousState != ECargoState::Storage)
			{
				Zone->OccupiedLitres += Shipment.VolumeLitres;
			}
			if (Zone && PreviousState == ECargoState::Storage &&
				Shipment.State != ECargoState::Storage)
			{
				Zone->OccupiedLitres =
					FMath::Max<int64>(
						0,
						Zone->OccupiedLitres - Shipment.VolumeLitres);
			}
			Emit(
				EPhase5EventType::CargoStateChanged,
				{},
				Shipment.Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s moved to %s."),
					*CargoClassDisplayName(Shipment.CargoClass),
					*CargoStateDisplayName(Shipment.State)));

			if (Shipment.State == ECargoState::Completed &&
				!Shipment.bRewardRecognized)
			{
				const FPhase5CargoContractRecord* Contract =
					State.CargoContracts.FindByPredicate(
						[&Shipment](const FPhase5CargoContractRecord& Candidate)
						{
							return Candidate.Id == Shipment.ContractId;
						});
				const int64 Reward = Contract ? Contract->RewardCredits : 0;
				Shipment.bRewardRecognized = true;
				++State.CompletedShipmentCount;
				State.TotalCargoRevenueCredits += Reward;
				Phase1.ApplyExternalEconomyChange(
					TEXT("CargoCompletion"),
					Reward,
					1,
					FString::Printf(
						TEXT("%s cargo completed."),
						*CargoClassDisplayName(Shipment.CargoClass)),
					CurrentGameMilliseconds);
				AddRatingContribution(
					TEXT("Rating.OperationalReliability"),
					TEXT("CargoCompletion"),
					2,
					CurrentGameMilliseconds,
					TEXT("Cargo completed with traceable continuity."));
				Emit(
					EPhase5EventType::CargoCompleted,
					{},
					Shipment.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Cargo completed and revenue recognized."));
			}
		}

		TSet<ECargoClass> Classes;
		TSet<ECargoFlow> Flows;
		for (const FPhase5ShipmentRecord& Shipment : State.Shipments)
		{
			if (Shipment.State == ECargoState::Completed)
			{
				Classes.Add(Shipment.CargoClass);
				Flows.Add(Shipment.Flow);
			}
		}
		State.CompletedCargoClassCount = Classes.Num();
		State.CompletedCargoFlowCount = Flows.Num();

		const bool bHasAvailableOffer =
			State.Shipments.ContainsByPredicate(
				[](const FPhase5ShipmentRecord& Shipment)
				{
					return Shipment.State == ECargoState::Offered;
				});
		if (!bHasAvailableOffer &&
			State.Shipments.Num() >= 5 &&
			State.Shipments.Num() < 10000)
		{
			const int32 TemplateIndex =
				State.CompletedShipmentCount % 5;
			const FPhase5ShipmentRecord Template =
				State.Shipments[TemplateIndex];
			const FPhase5CargoContractRecord* TemplateContract =
				State.CargoContracts.FindByPredicate(
					[&Template](const FPhase5CargoContractRecord& Contract)
					{
						return Contract.Id == Template.ContractId;
					});

			FPhase5CargoContractRecord Contract;
			if (TemplateContract)
			{
				Contract = *TemplateContract;
			}
			Contract.Id = {AllocateId()};
			Contract.ContentId = FName(*FString::Printf(
				TEXT("Contract.Phase5.Recurring.%llu"),
				Contract.Id.Value));
			Contract.DisplayName = FString::Printf(
				TEXT("%s %d"),
				TemplateContract
					? *TemplateContract->DisplayName
					: TEXT("Recurring cargo"),
				State.CompletedShipmentCount + 1);
			Contract.bAccepted = false;
			State.CargoContracts.Add(Contract);

			FPhase5ShipmentRecord Shipment = Template;
			Shipment.Id = {AllocateId()};
			Shipment.ContractId = Contract.Id;
			Shipment.State = ECargoState::Offered;
			Shipment.FlowStep = 0;
			Shipment.DeadlineGameMilliseconds =
				CurrentGameMilliseconds + 900000;
			Shipment.bSecurityCleared = false;
			Shipment.CurrentLocation = TEXT("Awaiting contract");
			Shipment.ActiveServiceTaskId = {};
			Shipment.ExceptionCause.Reset();
			Shipment.Remedy.Reset();
			Shipment.bRewardRecognized = false;
			State.Shipments.Add(Shipment);
			Emit(
				EPhase5EventType::CargoStateChanged,
				{},
				Shipment.Id.Value,
				CurrentGameMilliseconds,
				TEXT("A recurring cargo offer entered the board."));
		}
	}

	void FPhase5Simulation::RefreshTenants(
		const int64 CurrentGameMilliseconds)
	{
		for (FPhase5ProviderTenantRecord& Tenant : State.ProviderTenants)
		{
			if (Tenant.State == EProviderTenantState::Grace &&
				CurrentGameMilliseconds >= Tenant.GraceEndsAtGameMilliseconds)
			{
				Tenant.State = EProviderTenantState::Suspended;
				Tenant.SatisfactionPercent =
					FMath::Max(40, Tenant.SatisfactionPercent - 10);
				Tenant.Cause =
					TEXT("The requirement was not restored during grace.");
				Tenant.Remedy =
					TEXT("Restore the capability and accept the recovery action.");
			}
		}
	}

	void FPhase5Simulation::RefreshEvents(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		for (FPhase5SpecialEventRecord& Event : State.SpecialEvents)
		{
			if (Event.State == ESpecialEventState::Completed &&
				!Event.bRewardRecognized)
			{
				const float Ratio = Event.ExpectedDemand > 0
					? static_cast<float>(Event.SatisfiedDemand) /
						static_cast<float>(Event.ExpectedDemand)
					: 0.0f;
				const int64 Reward =
					FMath::RoundToInt64(Event.RewardCredits * Ratio);
				Event.bRewardRecognized = true;
				Event.State = ESpecialEventState::Cooldown;
				Event.CooldownEndsAtGameMilliseconds =
					CurrentGameMilliseconds +
						GetPhase5Fixture().EventCooldownMilliseconds;
				State.TotalEventRevenueCredits += Reward;
				Phase1.ApplyExternalEconomyChange(
					TEXT("EventReward"),
					Reward,
					Ratio >= 0.5f ? 1 : 0,
					Event.SuccessSummary,
					CurrentGameMilliseconds);
			}
			else if (Event.State == ESpecialEventState::Cooldown &&
				CurrentGameMilliseconds >= Event.CooldownEndsAtGameMilliseconds)
			{
				Event.State = ESpecialEventState::Offered;
				Event.SatisfiedDemand = 0;
				Event.LinkedEntityIds.Reset();
				Event.bRewardRecognized = false;
				Event.SuccessSummary.Reset();
			}
		}
	}

	void FPhase5Simulation::RefreshProgression(
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		TMap<FName, int32> ComponentTotals;
		for (const FPhase5RatingContributionRecord& Contribution :
			State.RatingContributions)
		{
			ComponentTotals.FindOrAdd(Contribution.ComponentId) +=
				Contribution.Magnitude;
		}
		int32 RatingTotal = 0;
		for (const FName Component : {
			TEXT("Rating.SafetyReadiness"),
			TEXT("Rating.OperationalReliability"),
			TEXT("Rating.CustomerExperience"),
			TEXT("Rating.TenantRelationships"),
			TEXT("Rating.AccessCleanlinessAmenities")})
		{
			RatingTotal += FMath::Clamp(
				50 + ComponentTotals.FindRef(Component),
				0,
				100);
		}
		State.OverallRating = RatingTotal / 5;

		auto HasProvider = [this](const FName ProviderType)
		{
			return State.ProviderTenants.ContainsByPredicate(
				[ProviderType](const FPhase5ProviderTenantRecord& Tenant)
				{
					return Tenant.ProviderType == ProviderType &&
						Tenant.State == EProviderTenantState::Active;
				});
		};
		const int32 OperatingDays = FMath::Max(
			State.CurrentOperatingDay,
			FMath::Max(
				Phase2State.CurrentOperatingDay,
				Phase4State.CurrentOperatingDay));
		for (FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			Evidence.AirportPoints = Phase1State.AirportPoints;
			Evidence.OperatingDays = OperatingDays;
			Evidence.SafetyRating = State.OverallRating;
			Evidence.ReliabilityRating = State.OverallRating;
			Evidence.bSharedResourceDayCompleted = false;
			switch (Evidence.Path)
			{
			case ESpecializationPath::GeneralAviation:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::GeneralAviation);
				Evidence.DistinctRolesOrClasses =
					CountDistinctRoles(Phase2State);
				Evidence.bPrimaryTenantActive = HasProvider(TEXT("FBO"));
				Evidence.bSecondaryProviderActive =
					HasProvider(TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					Phase2State.bInitialized;
				break;
			case ESpecializationPath::FlightSchool:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::FlightSchool);
				Evidence.DistinctRolesOrClasses =
					Phase2State.Aircraft.Num() >= 2 ? 2 : Phase2State.Aircraft.Num();
				Evidence.bPrimaryTenantActive =
					Phase2State.Tenants.ContainsByPredicate(
						[](const FPhase2TenantRecord& Tenant)
						{
							return Tenant.Specialization ==
									EAirportSpecialization::FlightSchool &&
								Tenant.bActive;
						});
				Evidence.bSecondaryProviderActive =
					HasProvider(TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					Evidence.bPrimaryTenantActive;
				break;
			case ESpecializationPath::Charter:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::Charter);
				Evidence.bPrimaryTenantActive =
					Phase2State.Tenants.ContainsByPredicate(
						[](const FPhase2TenantRecord& Tenant)
						{
							return Tenant.Specialization ==
									EAirportSpecialization::Charter &&
								Tenant.bActive;
						});
				Evidence.bSecondaryProviderActive =
					HasProvider(TEXT("Fuel"));
				Evidence.bSignatureFacilityOperational =
					Evidence.bPrimaryTenantActive;
				break;
			case ESpecializationPath::Cargo:
				Evidence.CompletedOperations =
					State.CompletedShipmentCount;
				Evidence.DistinctRolesOrClasses =
					State.CompletedCargoClassCount;
				Evidence.bPrimaryTenantActive =
					HasProvider(TEXT("CargoOperator"));
				Evidence.bSecondaryProviderActive =
					HasProvider(TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					State.WarehouseZones.Num() == 4 &&
					State.WarehouseZones.ContainsByPredicate(
						[](const FPhase5WarehouseZoneRecord& Zone)
						{
							return Zone.bOperational &&
								Zone.bRoadConnected &&
								Zone.bAirsideConnected;
						});
				break;
			case ESpecializationPath::Passenger:
				Evidence.CompletedOperations =
					Phase4State.CompletedFlightCount;
				Evidence.CompletedPassengers =
					Phase3State.CompletedPassengerCount;
				Evidence.bPrimaryTenantActive =
					Phase3State.bTerminalOpen;
				Evidence.bSecondaryProviderActive =
					HasProvider(TEXT("FoodConcession")) ||
					HasProvider(TEXT("RetailConcession"));
				Evidence.bSignatureFacilityOperational =
					Phase3State.bTerminalOpen &&
					Phase3State.bSecurityIntegrityValid;
				break;
			case ESpecializationPath::Mixed:
				break;
			}
			if (Evidence.Path != ESpecializationPath::Mixed)
			{
				if (MeetsAdvancedRequirements(Evidence))
				{
					Evidence.Band = ECapabilityBand::Advanced;
				}
				else if (Evidence.bSignatureFacilityOperational &&
					Evidence.CompletedOperations > 0)
				{
					Evidence.Band = ECapabilityBand::Regional;
				}
				else if (Evidence.bSignatureFacilityOperational)
				{
					Evidence.Band = ECapabilityBand::Established;
				}
				else
				{
					Evidence.Band = ECapabilityBand::Unavailable;
				}
			}
		}

		int32 RegionalPathCount = 0;
		int32 ActivePathOperationCount = 0;
		for (const FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			if (Evidence.Path != ESpecializationPath::Mixed &&
				Evidence.Band >= ECapabilityBand::Regional)
			{
				++RegionalPathCount;
			}
			if (Evidence.Path != ESpecializationPath::Mixed &&
				Evidence.CompletedOperations > 0)
			{
				++ActivePathOperationCount;
			}
		}
		FPhase5PathEvidenceRecord* Mixed = State.Paths.FindByPredicate(
			[](const FPhase5PathEvidenceRecord& Evidence)
			{
				return Evidence.Path == ESpecializationPath::Mixed;
			});
		if (Mixed)
		{
			Mixed->AirportPoints = Phase1State.AirportPoints;
			Mixed->OperatingDays = OperatingDays;
			Mixed->SafetyRating = State.OverallRating;
			Mixed->ReliabilityRating = State.OverallRating;
			Mixed->RegionalPathCount = RegionalPathCount;
			Mixed->bPrimaryTenantActive = RegionalPathCount >= 2;
			Mixed->bSecondaryProviderActive =
				State.ProviderTenants.ContainsByPredicate(
					[](const FPhase5ProviderTenantRecord& Tenant)
					{
						return Tenant.State == EProviderTenantState::Active;
					});
			Mixed->bSignatureFacilityOperational = RegionalPathCount >= 2;
			Mixed->bSharedResourceDayCompleted =
				RegionalPathCount >= 2 &&
				ActivePathOperationCount >= 2 &&
				Mixed->bSecondaryProviderActive;
			Mixed->Band = MeetsAdvancedRequirements(*Mixed)
				? ECapabilityBand::Advanced
				: RegionalPathCount >= 2
					? ECapabilityBand::Regional
					: ECapabilityBand::Established;
		}

		State.AdvancedPathCount = 0;
		for (FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			Evidence.CurrentEvidence = FString::Printf(
				TEXT("%d AP • day %d • %d operations • rating %d"),
				Evidence.AirportPoints,
				Evidence.OperatingDays,
				Evidence.CompletedOperations,
				State.OverallRating);
			Evidence.NextRequirement =
				Evidence.Band == ECapabilityBand::Advanced
					? TEXT("Major remains future locked.")
					: TEXT("Reach 50 AP, day 7, rating 70, and path evidence.");
			if (Evidence.Band == ECapabilityBand::Advanced)
			{
				++State.AdvancedPathCount;
				FPhase5AchievementRecord* Achievement =
					State.Achievements.FindByPredicate(
						[&Evidence](const FPhase5AchievementRecord& Candidate)
						{
							return Candidate.Path == Evidence.Path;
						});
				if (Achievement && !Achievement->bEarned)
				{
					Achievement->bEarned = true;
					Achievement->EarnedAtGameMilliseconds =
						CurrentGameMilliseconds;
					Emit(
						EPhase5EventType::AchievementEarned,
						{},
						Achievement->Id.Value,
						CurrentGameMilliseconds,
						Achievement->DisplayName);
				}
			}
		}

		for (FPhase5ObjectiveRecord& Objective : State.Objectives)
		{
			const FPhase5PathEvidenceRecord* Evidence =
				State.Paths.FindByPredicate(
					[&Objective](const FPhase5PathEvidenceRecord& Candidate)
					{
						return Candidate.Path == Objective.Path;
					});
			Objective.Current =
				Evidence && Evidence->Band == ECapabilityBand::Advanced ? 1 : 0;
			Objective.bCompleted = Objective.Current >= Objective.Target;
		}
		State.bFixtureCompleted =
			State.CompletedCargoClassCount == 4 &&
			State.CompletedCargoFlowCount == 3 &&
			State.SpecialEvents.ContainsByPredicate(
				[](const FPhase5SpecialEventRecord& Event)
				{
					return Event.bRewardRecognized ||
						Event.State == ESpecialEventState::Cooldown;
				});
	}

	void FPhase5Simulation::AddRatingContribution(
		const FName ComponentId,
		const FName SourceGroup,
		const int32 Magnitude,
		const int64 CurrentGameMilliseconds,
		const FString& Explanation)
	{
		FPhase5RatingContributionRecord Contribution;
		Contribution.Id = {AllocateId()};
		Contribution.ComponentId = ComponentId;
		Contribution.SourceGroup = SourceGroup;
		Contribution.Magnitude = Magnitude;
		Contribution.AppliedAtGameMilliseconds = CurrentGameMilliseconds;
		Contribution.Explanation = Explanation;
		State.RatingContributions.Add(Contribution);
	}

	FPhase5QuerySnapshot FPhase5Simulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State) const
	{
		(void)Phase1State;
		(void)Phase3State;
		FPhase5QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bUnlocked =
			Phase2State.bInitialized &&
			(Phase4State.bFixtureCompleted || Phase2State.CompletedFlightCount > 0);
		Query.bInitialized = State.bInitialized;
		Query.bFixtureCompleted = State.bFixtureCompleted;
		Query.OperatingDay = State.CurrentOperatingDay;
		Query.ShipmentCount = State.Shipments.Num();
		Query.CompletedShipmentCount = State.CompletedShipmentCount;
		Query.AdvancedPathCount = State.AdvancedPathCount;
		Query.OverallRating = State.OverallRating;
		int64 Capacity = 0;
		int64 Occupied = 0;
		for (const FPhase5CargoContractRecord& Contract : State.CargoContracts)
		{
			Query.AcceptedCargoContractCount += Contract.bAccepted ? 1 : 0;
		}
		for (const FPhase5ShipmentRecord& Shipment : State.Shipments)
		{
			Query.ActiveShipmentCount +=
				Shipment.State != ECargoState::Offered &&
				Shipment.State != ECargoState::Completed ? 1 : 0;
		}
		for (const FPhase5WarehouseZoneRecord& Zone : State.WarehouseZones)
		{
			Capacity += Zone.CapacityLitres;
			Occupied += Zone.OccupiedLitres;
		}
		Query.WarehouseUtilizationPercent = Capacity > 0
			? static_cast<int32>(Occupied * 100 / Capacity)
			: 0;
		for (const FPhase5ProviderTenantRecord& Tenant : State.ProviderTenants)
		{
			Query.ActiveProviderCount +=
				Tenant.State == EProviderTenantState::Active ? 1 : 0;
			Query.GraceProviderCount +=
				Tenant.State == EProviderTenantState::Grace ||
				Tenant.State == EProviderTenantState::Suspended ? 1 : 0;
		}
		for (const FPhase5SpecialEventRecord& Event : State.SpecialEvents)
		{
			Query.ActiveEventCount += IsEventOperational(Event.State) ? 1 : 0;
		}
		Query.PrimaryStatus = !State.bInitialized
			? TEXT("Cargo and specialization breadth ready to initialize")
			: State.bFixtureCompleted
				? TEXT("Phase 5 fixture complete")
				: TEXT("Cargo, providers, events, and Advanced paths active");
		Query.CargoSummary = FString::Printf(
			TEXT("%d / %d shipments complete • %d / 4 classes • %d / 3 flows"),
			State.CompletedShipmentCount,
			State.Shipments.Num(),
			State.CompletedCargoClassCount,
			State.CompletedCargoFlowCount);
		const FPhase5ShipmentRecord* Selected =
			State.Shipments.FindByPredicate(
				[](const FPhase5ShipmentRecord& Shipment)
				{
					return Shipment.State != ECargoState::Offered &&
						Shipment.State != ECargoState::Completed;
				});
		if (!Selected && !State.Shipments.IsEmpty())
		{
			Selected = &State.Shipments[0];
		}
		if (Selected)
		{
			Query.SelectedShipment = FString::Printf(
				TEXT("%s • %s\n%d pieces • %lld kg • %lld L\n%s"),
				*CargoClassDisplayName(Selected->CargoClass),
				*CargoFlowDisplayName(Selected->Flow),
				Selected->Pieces,
				Selected->MassKilograms,
				Selected->VolumeLitres,
				*CargoStateDisplayName(Selected->State));
			Query.Cause = Selected->ExceptionCause;
			Query.Remedy = Selected->Remedy;
		}
		Query.WarehouseSummary = FString::Printf(
			TEXT("4 compatible zones • %d%% occupied • road + airside connected"),
			Query.WarehouseUtilizationPercent);
		Query.ProviderSummary = FString::Printf(
			TEXT("%d active providers • %d in recovery"),
			Query.ActiveProviderCount,
			Query.GraceProviderCount);
		Query.EventSummary = FString::Printf(
			TEXT("%d active • 8 authored families • decline has no penalty"),
			Query.ActiveEventCount);
		Query.ProgressionSummary = FString::Printf(
			TEXT("%d / 6 Advanced • Major future locked"),
			State.AdvancedPathCount);
		Query.StateChecksum = CalculateChecksum();
		return Query;
	}

	uint64 FPhase5Simulation::CalculateChecksum() const
	{
		uint64 Hash = FnvOffset;
		HashBytes(Hash, &State.bInitialized, sizeof(bool));
		HashBytes(Hash, &State.MasterSeed, sizeof(uint64));
		HashBytes(Hash, &State.NextDomainId, sizeof(uint64));
		HashBytes(Hash, &State.CurrentOperatingDay, sizeof(int32));
		for (const FPhase5ShipmentRecord& Shipment : State.Shipments)
		{
			HashBytes(Hash, &Shipment.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Shipment.State, sizeof(Shipment.State));
			HashBytes(Hash, &Shipment.FlowStep, sizeof(int32));
			HashBytes(Hash, &Shipment.MassKilograms, sizeof(int64));
			HashBytes(Hash, &Shipment.VolumeLitres, sizeof(int64));
			HashBytes(Hash, &Shipment.ActiveServiceTaskId.Value, sizeof(uint64));
			HashBytes(Hash, &Shipment.bRewardRecognized, sizeof(bool));
		}
		for (const FPhase5ProviderTenantRecord& Tenant : State.ProviderTenants)
		{
			HashBytes(Hash, &Tenant.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Tenant.State, sizeof(Tenant.State));
			HashBytes(Hash, &Tenant.SatisfactionPercent, sizeof(int32));
			HashBytes(
				Hash,
				&Tenant.ContractTermOperatingDays,
				sizeof(int32));
			HashString(Hash, Tenant.FootprintId.ToString());
			HashString(Hash, Tenant.RequiredCapability.ToString());
		}
		for (const FPhase5PathEvidenceRecord& Path : State.Paths)
		{
			HashBytes(Hash, &Path.Path, sizeof(Path.Path));
			HashBytes(Hash, &Path.Band, sizeof(Path.Band));
			HashBytes(Hash, &Path.CompletedOperations, sizeof(int32));
		}
		for (const FPhase5SpecialEventRecord& Event : State.SpecialEvents)
		{
			HashBytes(Hash, &Event.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Event.State, sizeof(Event.State));
			HashBytes(Hash, &Event.SatisfiedDemand, sizeof(int32));
		}
		for (const FPhase5RatingContributionRecord& Contribution :
			State.RatingContributions)
		{
			HashBytes(Hash, &Contribution.Id.Value, sizeof(uint64));
			HashString(Hash, Contribution.ComponentId.ToString());
			HashBytes(Hash, &Contribution.Magnitude, sizeof(int32));
		}
		for (const FPhase5Event& Event : State.Events)
		{
			HashBytes(Hash, &Event.Sequence, sizeof(uint64));
			HashBytes(Hash, &Event.Type, sizeof(Event.Type));
			HashBytes(Hash, &Event.SubjectId, sizeof(uint64));
		}
		return Hash;
	}

	bool FPhase5Simulation::RestoreState(
		const FPhase5State& Candidate,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		(void)Phase3State;
		(void)Phase4State;
		if (Candidate.MasterSeed == 0 ||
			Candidate.NextDomainId < Phase5IdBase ||
			Candidate.NextEventSequence == 0 ||
			Candidate.Shipments.Num() > 100000 ||
			Candidate.SpecialEvents.Num() > 10000 ||
			Candidate.RatingContributions.Num() > 100000 ||
			Candidate.Objectives.Num() > 3 ||
			Candidate.CurrentOperatingDay < 0 ||
			(Candidate.bInitialized &&
				(!Phase1State.bAirportOpen || !Phase2State.bInitialized)))
		{
			return false;
		}

		TSet<uint64> SeenIds;
		auto RegisterId = [&SeenIds, &Candidate](const uint64 Id)
		{
			return Id >= Phase5IdBase &&
				Id < Candidate.NextDomainId &&
				!SeenIds.Contains(Id) &&
				(SeenIds.Add(Id), true);
		};
		for (const FPhase5WarehouseZoneRecord& Zone : Candidate.WarehouseZones)
		{
			if (!RegisterId(Zone.Id.Value) ||
				Zone.ContentId.IsNone() ||
				Zone.CapacityLitres <= 0 ||
				Zone.OccupiedLitres < 0 ||
				Zone.OccupiedLitres > Zone.CapacityLitres)
			{
				return false;
			}
		}
		for (const FPhase5CargoContractRecord& Contract :
			Candidate.CargoContracts)
		{
			if (!RegisterId(Contract.Id.Value) ||
				Contract.ContentId.IsNone() ||
				Contract.AircraftContentId.IsNone() ||
				Contract.RewardCredits < 0)
			{
				return false;
			}
		}
		TMap<uint64, int64> ExpectedOccupancy;
		for (const FPhase5ShipmentRecord& Shipment : Candidate.Shipments)
		{
			if (!RegisterId(Shipment.Id.Value) ||
				!ContainsId(Candidate.CargoContracts, Shipment.ContractId) ||
				!ContainsId(Candidate.WarehouseZones, Shipment.WarehouseZoneId) ||
				Shipment.Pieces <= 0 ||
				Shipment.MassKilograms <= 0 ||
				Shipment.VolumeLitres <= 0 ||
				Shipment.FlowStep < 0 ||
				Shipment.FlowStep >= FlowStates(Shipment.Flow).Num() ||
				(Shipment.bBellyFreight &&
					Shipment.LinkedFlightId.IsValid() &&
					!ContainsId(Phase4State.Flights, Shipment.LinkedFlightId)) ||
				(Shipment.ActiveServiceTaskId.IsValid() &&
					!Phase2State.ServiceTasks.ContainsByPredicate(
						[&Shipment](const FPhase2ServiceTaskRecord& Task)
						{
							return Task.Id ==
									Shipment.ActiveServiceTaskId &&
								Task.OwnerDomain == TEXT("Phase5") &&
								Task.OwnerId == Shipment.Id.Value;
						})))
			{
				return false;
			}
			if (Shipment.State == ECargoState::Storage)
			{
				ExpectedOccupancy.FindOrAdd(Shipment.WarehouseZoneId.Value) +=
					Shipment.VolumeLitres;
			}
		}
		for (const FPhase5WarehouseZoneRecord& Zone : Candidate.WarehouseZones)
		{
			if (ExpectedOccupancy.FindRef(Zone.Id.Value) != Zone.OccupiedLitres)
			{
				return false;
			}
		}
		for (const FPhase5ProviderTenantRecord& Tenant :
			Candidate.ProviderTenants)
		{
			if (!RegisterId(Tenant.Id.Value) ||
				Tenant.ContentId.IsNone() ||
				Tenant.ProviderType.IsNone() ||
				Tenant.FootprintId.IsNone() ||
				Tenant.RequiredCapability.IsNone() ||
				Tenant.OpeningCostCredits < 0 ||
				Tenant.RentCreditsPerOperatingDay < 0 ||
				Tenant.ContractTermOperatingDays <= 0 ||
				Tenant.Requirement.IsEmpty() ||
				Tenant.ServiceExpectations.IsEmpty() ||
				Tenant.OperatingPattern.IsEmpty() ||
				Tenant.SatisfactionDrivers.IsEmpty() ||
				Tenant.SatisfactionPercent < 0 ||
				Tenant.SatisfactionPercent > 100)
			{
				return false;
			}
		}
		for (const FPhase5RatingContributionRecord& Contribution :
			Candidate.RatingContributions)
		{
			if (!RegisterId(Contribution.Id.Value) ||
				Contribution.ComponentId.IsNone() ||
				Contribution.SourceGroup.IsNone() ||
				Contribution.Magnitude < -100 ||
				Contribution.Magnitude > 100)
			{
				return false;
			}
		}
		for (const FPhase5ObjectiveRecord& Objective : Candidate.Objectives)
		{
			if (!RegisterId(Objective.Id.Value) ||
				Objective.ContentId.IsNone() ||
				Objective.Target <= 0 ||
				Objective.Current < 0)
			{
				return false;
			}
		}
		for (const FPhase5AchievementRecord& Achievement :
			Candidate.Achievements)
		{
			if (!RegisterId(Achievement.Id.Value) ||
				Achievement.ContentId.IsNone())
			{
				return false;
			}
		}
		for (const FPhase5SpecialEventRecord& Event : Candidate.SpecialEvents)
		{
			if (!RegisterId(Event.Id.Value) ||
				Event.ContentId.IsNone() ||
				Event.ExpectedDemand <= 0 ||
				Event.SatisfiedDemand < 0 ||
				Event.SatisfiedDemand > Event.ExpectedDemand ||
				(IsEventOperational(Event.State) &&
					Event.LinkedEntityIds.IsEmpty()))
			{
				return false;
			}
		}
		if (Candidate.bInitialized &&
			(Candidate.WarehouseZones.Num() != 4 ||
				Candidate.Paths.Num() != 6 ||
				Candidate.Objectives.Num() != 3 ||
				Candidate.SpecialEvents.Num() != 8))
		{
			return false;
		}
		State = Candidate;
		PendingCommands.Reset();
		return true;
	}

	void FPhase5Simulation::Emit(
		const EPhase5EventType Type,
		const FCommandId& Cause,
		const uint64 SubjectId,
		const int64 CurrentGameMilliseconds,
		const FString& Message)
	{
		State.Events.Add({
			State.NextEventSequence++,
			CurrentGameMilliseconds,
			Type,
			Cause,
			SubjectId,
			Message});
	}

	uint64 FPhase5Simulation::AllocateId()
	{
		return State.NextDomainId++;
	}
}
