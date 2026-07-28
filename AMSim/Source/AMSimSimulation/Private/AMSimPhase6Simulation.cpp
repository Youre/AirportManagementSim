#include "AMSimPhase6Simulation.h"

#include "AMSimDeterminism.h"
#include "AMSimLivingAirportSimulation.h"
#include "AMSimPhase5Fixture.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase6IdBase = 1ull << 61;
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

		bool HasEarnedBand(
			const FPhase5State& Phase5,
			const ESpecializationPath Path,
			const ECapabilityBand Required)
		{
			const FPhase5PathEvidenceRecord* Evidence =
				Phase5.Paths.FindByPredicate(
					[Path](const FPhase5PathEvidenceRecord& Candidate)
					{
						return Candidate.Path == Path;
					});
			if (!Evidence)
			{
				return false;
			}
			const ECapabilityBand Earned =
				Evidence->EarnedBand == ECapabilityBand::Unavailable
					? Evidence->Band
					: Evidence->EarnedBand;
			return Earned >= Required;
		}

		const FPhase5PathEvidenceRecord* FindPath(
			const FPhase5State& Phase5,
			const ESpecializationPath Path)
		{
			return Phase5.Paths.FindByPredicate(
				[Path](const FPhase5PathEvidenceRecord& Candidate)
				{
					return Candidate.Path == Path;
				});
		}

		bool IsIncidentActive(const ESeriousIncidentLifecycle Lifecycle)
		{
			return Lifecycle != ESeriousIncidentLifecycle::None &&
				Lifecycle != ESeriousIncidentLifecycle::Recovered;
		}

		void AddEvidence(
			FPhase6PathMetricRecord& Metric,
			const FName EvidenceId)
		{
			if (!EvidenceId.IsNone())
			{
				Metric.EvidenceIds.AddUnique(EvidenceId);
			}
		}
	}

	FPhase6Simulation::FPhase6Simulation(const uint64 MasterSeed)
	{
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase6.MajorAirport"));
		State.NextDomainId = Phase6IdBase;
		State.NextEventSequence = 1;
	}

	EPhase6CommandResult FPhase6Simulation::QueueCommand(
		const FPhase6Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State)
	{
		(void)CurrentGameMilliseconds;
		const FPhase6Validation Validation = ValidateCommand(
			Command,
			Phase1State,
			Phase2State,
			Phase3State,
			Phase4State,
			Phase5State);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		if (Validation.bRequiresConfirmation && !Command.bConfirmWarning)
		{
			return EPhase6CommandResult::AcceptedWithWarning;
		}
		PendingCommands.Add(Command);
		return EPhase6CommandResult::Accepted;
	}

	FPhase6Validation FPhase6Simulation::ValidateCommand(
		const FPhase6Command& Command,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State) const
	{
		(void)Phase2State;
		(void)Phase3State;
		(void)Phase4State;
		FPhase6Validation Result;
		if (!Command.Id.IsValid())
		{
			Result.Result = EPhase6CommandResult::RejectedInvalidCommand;
			Result.ReasonCode = TEXT("Phase6.InvalidCommandId");
			Result.Cause = TEXT("The command has no stable ID.");
			return Result;
		}
		if (Command.Type == EPhase6CommandType::InitializeMajorCapability)
		{
			if (State.bInitialized)
			{
				Result.Result = EPhase6CommandResult::RejectedInvalidState;
				Result.ReasonCode = TEXT("Phase6.AlreadyInitialized");
				return Result;
			}
			if (!Phase5State.bInitialized ||
				Phase5State.AdvancedPathCount == 0)
			{
				Result.Result = EPhase6CommandResult::RejectedPhase5NotReady;
				Result.ReasonCode = TEXT("Phase6.AdvancedCapabilityRequired");
				Result.Cause =
					TEXT("At least one Advanced path is required.");
				Result.Remedy =
					TEXT("Complete an Advanced capability path first.");
				return Result;
			}
			Result.bValid = true;
			Result.Result = EPhase6CommandResult::Accepted;
			return Result;
		}
		if (!State.bInitialized)
		{
			Result.Result = EPhase6CommandResult::RejectedInvalidState;
			Result.ReasonCode = TEXT("Phase6.NotInitialized");
			return Result;
		}

		switch (Command.Type)
		{
		case EPhase6CommandType::ConstructFacility:
		{
			const FMajorFacilityRecord* Facility =
				State.Facilities.FindByPredicate(
					[&Command](const FMajorFacilityRecord& Candidate)
					{
						return Candidate.Id == Command.FacilityId;
					});
			if (!Facility)
			{
				Result.Result =
					EPhase6CommandResult::RejectedMissingReference;
				Result.ReasonCode = TEXT("Phase6.FacilityMissing");
				return Result;
			}
			if (Facility->State != EMajorFacilityState::Proposed)
			{
				Result.Result = EPhase6CommandResult::RejectedInvalidState;
				Result.ReasonCode = TEXT("Phase6.FacilityNotProposed");
				return Result;
			}
			if (Phase1State.Credits < Facility->CostCredits)
			{
				Result.Result =
					EPhase6CommandResult::RejectedInsufficientFunds;
				Result.ReasonCode = TEXT("Phase6.InsufficientCredits");
				Result.Cause = FString::Printf(
					TEXT("%s costs %lld Credits."),
					*Facility->DisplayName,
					Facility->CostCredits);
				Result.Remedy =
					TEXT("Complete contracts or use a recovery option.");
				return Result;
			}
			break;
		}
		case EPhase6CommandType::ActivateParallelRunways:
			if (!State.Facilities.ContainsByPredicate(
				[](const FMajorFacilityRecord& Facility)
				{
					return Facility.Type ==
							EMajorFacilityType::ParallelRunway &&
						Facility.State ==
							EMajorFacilityState::Operational;
				}))
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode = TEXT("Phase6.ParallelRunwayNotReady");
				return Result;
			}
			break;
		case EPhase6CommandType::SetRunwayConfiguration:
		{
			const FPhase6RunwayRecord* Runway =
				State.Runways.FindByPredicate(
					[&Command](const FPhase6RunwayRecord& Candidate)
					{
						return Candidate.Id == Command.RunwayId;
					});
			if (!Runway ||
				(Command.RunwayEndId != Runway->PrimaryEndId &&
					Command.RunwayEndId != Runway->ReciprocalEndId))
			{
				Result.Result =
					EPhase6CommandResult::RejectedMissingReference;
				Result.ReasonCode = TEXT("Phase6.RunwayEndMissing");
				return Result;
			}
			if (!Runway->bOperational)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode = TEXT("Phase6.RunwayNotOperational");
				return Result;
			}
			const bool bOccupied =
				Command.RunwayEndId == Runway->PrimaryEndId
					? Runway->bPrimaryOccupied
					: Runway->bReciprocalOccupied;
			if (bOccupied && Command.RunwayUse == ERunwayUse::Closed)
			{
				Result.Result = EPhase6CommandResult::RejectedUnsafe;
				Result.ReasonCode = TEXT("Phase6.RunwayOccupied");
				Result.Cause =
					TEXT("The runway end is occupied by an active movement.");
				Result.Remedy =
					TEXT("Wait for the movement to clear before closing.");
				return Result;
			}
			break;
		}
		case EPhase6CommandType::AcceptLargeAircraftOffer:
		{
			const FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.State ==
							ELargeAircraftOperationState::Offered;
					});
			FString Cause;
			if (!Operation || !HasEarnedBand(
				Phase5State,
				ESpecializationPath::Passenger,
				ECapabilityBand::Advanced) ||
				!IsLargeAircraftCompatible(Cause))
			{
				Result.Result = EPhase6CommandResult::RejectedIncompatible;
				Result.ReasonCode = TEXT("Phase6.LargeAircraftIncompatible");
				Result.Cause = Cause.IsEmpty()
					? TEXT("Passenger Advanced capability is required.")
					: Cause;
				Result.Remedy =
					TEXT("Complete the listed runway, stand, terminal, baggage, service, and emergency capacity.");
				return Result;
			}
			break;
		}
		case EPhase6CommandType::StartLargeAircraftTurnaround:
		{
			const FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[&Command](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.Id ==
							Command.LargeAircraftOperationId;
					});
			if (!Operation ||
				Operation->State != ELargeAircraftOperationState::Arrived)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode =
					TEXT("Phase6.LargeAircraftNotAtStand");
				return Result;
			}
			break;
		}
		case EPhase6CommandType::AdvanceLargeAircraftOperation:
		{
			const FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[&Command](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.Id ==
							Command.LargeAircraftOperationId;
					});
			if (!Operation ||
				(Operation->State !=
						ELargeAircraftOperationState::Turnaround &&
					Operation->State !=
						ELargeAircraftOperationState::DepartureReady &&
					Operation->State !=
						ELargeAircraftOperationState::Departed))
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode =
					TEXT("Phase6.LargeAircraftCannotAdvance");
				return Result;
			}
			if (Operation->State ==
					ELargeAircraftOperationState::Turnaround &&
				(Operation->CompletedServiceTaskCount <
						Operation->RequiredServiceTaskCount ||
					!Operation->bPassengerReconciled ||
					!Operation->bBagReconciled))
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode =
					TEXT("Phase6.TurnaroundIncomplete");
				Result.Cause =
					TEXT("Mandatory service or reconciliation remains.");
				return Result;
			}
			break;
		}
		case EPhase6CommandType::RecordMajorOperation:
			if (Command.OperationKind.IsNone())
			{
				Result.Result =
					EPhase6CommandResult::RejectedInvalidCommand;
				Result.ReasonCode = TEXT("Phase6.OperationKindMissing");
				return Result;
			}
			break;
		case EPhase6CommandType::ResolveSharedResourceConflict:
			if (!State.Facilities.ContainsByPredicate(
				[](const FMajorFacilityRecord& Facility)
				{
					return Facility.Type ==
							EMajorFacilityType::OperationsCenter &&
						Facility.State ==
							EMajorFacilityState::Operational;
				}))
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode =
					TEXT("Phase6.OperationsCenterRequired");
				return Result;
			}
			break;
		case EPhase6CommandType::AcknowledgeSeriousRisk:
			if (State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::None)
			{
				Result.Result = EPhase6CommandResult::RejectedInvalidState;
				Result.ReasonCode =
					TEXT("Phase6.IncidentAlreadyActive");
				return Result;
			}
			Result.bRequiresConfirmation = true;
			Result.Cause =
				TEXT("A disclosed runway-capacity override reduces the response margin.");
			Result.Consequence =
				TEXT("The warned risk can produce aircraft loss and a bounded runway closure.");
			Result.Remedy =
				TEXT("Decline the override or confirm with emergency access protected.");
			break;
		case EPhase6CommandType::MaterializeSeriousIncident:
			if (State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::RiskAcknowledged)
			{
				Result.Result = EPhase6CommandResult::RejectedUnsafe;
				Result.ReasonCode =
					TEXT("Phase6.RiskNotAcknowledged");
				return Result;
			}
			break;
		case EPhase6CommandType::DispatchEmergencyResponse:
			if (State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Materialized &&
				State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Alerted)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::ProtectIncidentArea:
			if (State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::ResourcesDispatched)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::StabilizeIncident:
			if (State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::AreaProtected)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::InvestigateIncident:
			if (State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::Stabilized)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::ApplyIncidentRecovery:
			if (State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Investigating &&
				State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Repairing)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::BeginIncidentRepair:
			if (State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Investigating ||
				State.RepairProject.bFunded)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			if (Phase1State.Credits < State.RepairProject.CostCredits)
			{
				Result.Result =
					EPhase6CommandResult::RejectedInsufficientFunds;
				Result.ReasonCode =
					TEXT("Phase6.RecoverySupportRequired");
				Result.Remedy =
					TEXT("Apply recovery support before funding repair.");
				return Result;
			}
			break;
		case EPhase6CommandType::CompleteIncidentRepair:
			if (State.SeriousIncident.Lifecycle !=
					ESeriousIncidentLifecycle::Repairing ||
				!State.RepairProject.bFunded)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase6CommandType::ReopenIncidentArea:
			if (!State.RepairProject.bCompleted ||
				!State.SeriousIncident.bRecoveryFundingApplied)
			{
				Result.Result = EPhase6CommandResult::RejectedNotReady;
				Result.ReasonCode =
					TEXT("Phase6.RepairOrRecoveryIncomplete");
				return Result;
			}
			break;
		default:
			break;
		}

		Result.bValid = true;
		Result.Result = Result.bRequiresConfirmation
			? EPhase6CommandResult::AcceptedWithWarning
			: EPhase6CommandResult::Accepted;
		return Result;
	}

	void FPhase6Simulation::Step(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State)
	{
		PendingCommands.Sort(
			[](const FPhase6Command& Left, const FPhase6Command& Right)
			{
				return Left.Id.Value < Right.Id.Value;
			});
		for (const FPhase6Command& Command : PendingCommands)
		{
			ApplyCommand(
				Command,
				CurrentGameMilliseconds,
				Phase1,
				Phase2,
				Phase3State,
				Phase4State,
				Phase5State);
		}
		PendingCommands.Reset();
		if (!State.bInitialized)
		{
			return;
		}
		CompleteConstruction(
			CurrentGameMilliseconds,
			Phase1,
			Phase2);
		RefreshLargeAircraft(CurrentGameMilliseconds, Phase2);
		RefreshPathMetrics(Phase3State, Phase4State, Phase5State);
		RefreshScaleDiagnostics(
			Phase2.GetState(),
			Phase3State,
			Phase4State);
		State.MajorPathCount = CreateCapabilitySignals(
			Phase1.GetState(),
			Phase5State).MajorPathCount;
		State.bFixtureCompleted =
			State.MajorPathCount == 6 &&
			State.CompletedLargeAircraftCount > 0 &&
			State.RecoveredSeriousIncidentCount > 0;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
	}

	void FPhase6Simulation::Initialize(
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase5State& Phase5State)
	{
		(void)Phase1State;
		State.bInitialized = true;
		State.InitializedAtGameMilliseconds = CurrentGameMilliseconds;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;
		State.CurrentOperatingDay = FMath::Max(
			0,
			Phase5State.CurrentOperatingDay);

		FPhase6RunwayRecord Primary;
		Primary.Id = {AllocateId()};
		Primary.ContentId = TEXT("Runway.Major.Primary");
		Primary.DisplayName = TEXT("Runway 09L/27R");
		Primary.MagneticHeadingDegrees = 90;
		Primary.DeclaredLengthMeters = 3000;
		Primary.WidthMeters = 45;
		Primary.ParallelOrder = 0;
		Primary.Side = ERunwaySide::Left;
		Primary.PrimaryEndId = FName(*RunwayEndLabel(90, ERunwaySide::Left));
		Primary.ReciprocalEndId = FName(*RunwayEndLabel(
			270,
			ReciprocalRunwaySide(ERunwaySide::Left)));
		Primary.PrimaryUse = ERunwayUse::Arrival;
		Primary.ReciprocalUse = ERunwayUse::Available;
		Primary.bPaved = true;
		Primary.bOperational = true;
		Primary.bApproachAidOperational = true;
		Primary.Recommendation = TEXT("09L arrivals in current wind.");
		State.Runways.Add(Primary);

		FPhase6RunwayRecord Parallel;
		Parallel.Id = {AllocateId()};
		Parallel.ContentId = TEXT("Runway.Major.Parallel");
		Parallel.DisplayName = TEXT("Runway 09R/27L");
		Parallel.MagneticHeadingDegrees = 90;
		Parallel.DeclaredLengthMeters = 3000;
		Parallel.WidthMeters = 45;
		Parallel.ParallelOrder = 1;
		Parallel.Side = ERunwaySide::Right;
		Parallel.PrimaryEndId = FName(*RunwayEndLabel(
			90,
			ERunwaySide::Right));
		Parallel.ReciprocalEndId = FName(*RunwayEndLabel(
			270,
			ReciprocalRunwaySide(ERunwaySide::Right)));
		Parallel.PrimaryUse = ERunwayUse::Available;
		Parallel.ReciprocalUse = ERunwayUse::Departure;
		Parallel.bPaved = true;
		Parallel.bOperational = false;
		Parallel.bApproachAidOperational = true;
		Parallel.Recommendation =
			TEXT("Complete the parallel-runway project.");
		State.Runways.Add(Parallel);

		State.Facilities = MakeMajorFacilityDefinitions();
		for (FMajorFacilityRecord& Facility : State.Facilities)
		{
			Facility.Id = {AllocateId()};
		}
		for (int32 PathIndex = 0; PathIndex < 6; ++PathIndex)
		{
			FPhase6PathMetricRecord Metric;
			Metric.Path = static_cast<ESpecializationPath>(PathIndex);
			if (const FPhase5PathEvidenceRecord* Evidence =
				FindPath(Phase5State, Metric.Path))
			{
				Metric.CompletedOperations = Evidence->CompletedOperations;
				Metric.DistinctRolesOrClasses =
					Evidence->DistinctRolesOrClasses;
				Metric.CompletedPassengers = Evidence->CompletedPassengers;
				Metric.bPrimaryTenantActive =
					Evidence->bPrimaryTenantActive;
				Metric.bSecondaryProviderActive =
					Evidence->bSecondaryProviderActive;
				Metric.TenantRelationshipRating = FMath::Max(
					75,
					Evidence->TenantRelationshipRating);
			}
			State.PathMetrics.Add(Metric);
		}

		FLargeAircraftOperationRecord WideBody;
		WideBody.Id = {AllocateId()};
		WideBody.ContentId = TEXT("Operation.Major.Boeing787-9");
		WideBody.AircraftContentId = TEXT("Aircraft.Boeing787-9.Phase6");
		WideBody.OperatorId = TEXT("Operator.RiverbendLongreach");
		WideBody.OperatorDisplayName = TEXT("Riverbend Longreach");
		WideBody.FlightNumber = TEXT("RL 602");
		WideBody.PassengerCapacity =
			GetPhase6Fixture().LargeAircraftPassengerCapacity;
		WideBody.PassengerCount =
			GetPhase6Fixture().LargeAircraftRepresentativePassengers;
		WideBody.AcceptedBagCount =
			GetPhase6Fixture().LargeAircraftRepresentativeBags;
		WideBody.State = ELargeAircraftOperationState::Offered;
		State.LargeAircraftOperations.Add(WideBody);

		State.SeriousIncident.Id = {AllocateId()};
		State.SeriousIncident.OutcomeSeed =
			FDeterministicStream::SeedNamedStream(
				State.MasterSeed,
				TEXT("Phase6.SeriousIncident"));
		State.SeriousIncident.RiskFactor =
			TEXT("A disclosed capacity override reduced runway and response margin.");
		State.SeriousIncident.Warning =
			TEXT("Continuing may cause aircraft loss and a bounded runway closure.");
		State.SeriousIncident.Remedy =
			TEXT("Decline the override or protect emergency access before continuing.");
		State.RepairProject.Id = {AllocateId()};
		State.RepairProject.IncidentId = State.SeriousIncident.Id;
		State.RepairProject.ContentId =
			TEXT("Project.Phase6.IncidentRepair");
		State.RepairProject.CostCredits = 5000;

		Emit(
			EPhase6EventType::MajorCapabilityInitialized,
			{},
			0,
			CurrentGameMilliseconds,
			TEXT("Major capability planning is available for all six paths."));
	}

	void FPhase6Simulation::ApplyCommand(
		const FPhase6Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State)
	{
		(void)Phase3State;
		(void)Phase4State;
		switch (Command.Type)
		{
		case EPhase6CommandType::InitializeMajorCapability:
			Initialize(
				CurrentGameMilliseconds,
				Phase1.GetState(),
				Phase5State);
			break;
		case EPhase6CommandType::ConstructFacility:
			if (FMajorFacilityRecord* Facility =
				State.Facilities.FindByPredicate(
					[&Command](const FMajorFacilityRecord& Candidate)
					{
						return Candidate.Id == Command.FacilityId;
					}))
			{
				if (Phase1.ApplyExternalEconomyChange(
					TEXT("Phase6.MajorConstruction"),
					-Facility->CostCredits,
					0,
					FString::Printf(
						TEXT("Funded %s."),
						*Facility->DisplayName),
					CurrentGameMilliseconds))
				{
					Facility->State =
						EMajorFacilityState::UnderConstruction;
					Facility->FundedAtGameMilliseconds =
						CurrentGameMilliseconds;
					Facility->CompletesAtGameMilliseconds =
						CurrentGameMilliseconds +
						GetPhase6Fixture().
							FacilityConstructionMilliseconds;
					Emit(
						EPhase6EventType::EconomyApplied,
						Command.Id,
						Facility->Id.Value,
						CurrentGameMilliseconds,
						FString::Printf(
							TEXT("%s funded for %lld Credits."),
							*Facility->DisplayName,
							Facility->CostCredits));
				}
			}
			break;
		case EPhase6CommandType::ActivateParallelRunways:
			for (FPhase6RunwayRecord& Runway : State.Runways)
			{
				Runway.bOperational = true;
			}
			Emit(
				EPhase6EventType::ParallelRunwaysActivated,
				Command.Id,
				0,
				CurrentGameMilliseconds,
				TEXT("Parallel runway configuration activated."));
			break;
		case EPhase6CommandType::SetRunwayConfiguration:
			if (FPhase6RunwayRecord* Runway =
				State.Runways.FindByPredicate(
					[&Command](const FPhase6RunwayRecord& Candidate)
					{
						return Candidate.Id == Command.RunwayId;
					}))
			{
				if (Command.RunwayEndId == Runway->PrimaryEndId)
				{
					Runway->PrimaryUse = Command.RunwayUse;
					Runway->bPrimaryClosed =
						Command.RunwayUse == ERunwayUse::Closed;
				}
				else
				{
					Runway->ReciprocalUse = Command.RunwayUse;
					Runway->bReciprocalClosed =
						Command.RunwayUse == ERunwayUse::Closed;
				}
				Emit(
					EPhase6EventType::RunwayConfigurationChanged,
					Command.Id,
					Runway->Id.Value,
					CurrentGameMilliseconds,
					FString::Printf(
						TEXT("%s set to %s."),
						*Command.RunwayEndId.ToString(),
						*RunwayUseDisplayName(Command.RunwayUse)));
			}
			break;
		case EPhase6CommandType::AcceptLargeAircraftOffer:
			if (FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.State ==
							ELargeAircraftOperationState::Offered;
					}))
			{
				Operation->State =
					ELargeAircraftOperationState::Scheduled;
				Operation->RunwayId = State.Runways[0].Id;
				const FMajorFacilityRecord* Stand =
					State.Facilities.FindByPredicate(
						[](const FMajorFacilityRecord& Facility)
						{
							return Facility.Type ==
									EMajorFacilityType::LargeStand &&
								Facility.State ==
									EMajorFacilityState::Operational;
						});
				Operation->StandFacilityId = Stand ? Stand->Id : FMajorFacilityId{};
				Operation->ScheduledArrivalGameMilliseconds =
					CurrentGameMilliseconds + 30000;
				Operation->ScheduledDepartureGameMilliseconds =
					CurrentGameMilliseconds + 240000;
				Emit(
					EPhase6EventType::LargeAircraftAccepted,
					Command.Id,
					Operation->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Riverbend Longreach RL 602 accepted for Stand H1."));
			}
			break;
		case EPhase6CommandType::StartLargeAircraftTurnaround:
			if (FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[&Command](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.Id ==
							Command.LargeAircraftOperationId;
					}))
			{
				CreateLargeAircraftTasks(
					*Operation,
					CurrentGameMilliseconds,
					Phase2);
				Operation->State =
					ELargeAircraftOperationState::Turnaround;
				Emit(
					EPhase6EventType::LargeAircraftStateChanged,
					Command.Id,
					Operation->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Wide-body turnaround started with automatic dispatch."));
			}
			break;
		case EPhase6CommandType::AdvanceLargeAircraftOperation:
			if (FLargeAircraftOperationRecord* Operation =
				State.LargeAircraftOperations.FindByPredicate(
					[&Command](const FLargeAircraftOperationRecord& Candidate)
					{
						return Candidate.Id ==
							Command.LargeAircraftOperationId;
					}))
			{
				if (Operation->State ==
					ELargeAircraftOperationState::Turnaround)
				{
					Operation->State =
						ELargeAircraftOperationState::DepartureReady;
				}
				else if (Operation->State ==
					ELargeAircraftOperationState::DepartureReady)
				{
					Operation->State =
						ELargeAircraftOperationState::Departed;
				}
				else if (Operation->State ==
					ELargeAircraftOperationState::Departed)
				{
					Operation->State =
						ELargeAircraftOperationState::Completed;
					Operation->bRewardRecognized =
						Phase1.ApplyExternalEconomyChange(
							TEXT("Phase6.LargeAircraftCompleted"),
							15000,
							5,
							TEXT("Completed first Boeing 787-9 turnaround."),
							CurrentGameMilliseconds);
					++State.CompletedLargeAircraftCount;
					State.TotalMajorRevenueCredits += 15000;
					if (FPhase6PathMetricRecord* Passenger =
						State.PathMetrics.FindByPredicate(
							[](const FPhase6PathMetricRecord& Metric)
							{
								return Metric.Path ==
									ESpecializationPath::Passenger;
							}))
					{
						Passenger->bWideBodyTurnaroundCompleted = true;
						AddEvidence(
							*Passenger,
							TEXT("Aircraft.Boeing787-9.Turnaround"));
					}
				}
				Emit(
					EPhase6EventType::LargeAircraftStateChanged,
					Command.Id,
					Operation->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Wide-body operation advanced."));
			}
			break;
		case EPhase6CommandType::RecordMajorOperation:
			RecordMajorOperation(
				Command,
				CurrentGameMilliseconds,
				Phase1);
			break;
		case EPhase6CommandType::AdvanceMajorOperatingDay:
			++State.CurrentOperatingDay;
			for (FPhase6PathMetricRecord& Metric : State.PathMetrics)
			{
				if (Metric.CompletedOperations > 0 &&
					Metric.bSignatureFacilityOperational &&
					!Metric.bCriticalReadinessFailure)
				{
					++Metric.QualifyingOperatingDays;
					const FName DayEvidence = FName(*FString::Printf(
						TEXT("Major.Day.%d"),
						Metric.QualifyingOperatingDays));
					AddEvidence(Metric, DayEvidence);
					if (Metric.QualifyingOperatingDays == 8 ||
						Metric.QualifyingOperatingDays == 11 ||
						Metric.QualifyingOperatingDays == 14)
					{
						Phase1.ApplyExternalEconomyChange(
							TEXT("Phase6.QualifyingDay"),
							0,
							2,
							FString::Printf(
								TEXT("%s sustained Major-capability operations."),
								*SpecializationPathDisplayName(Metric.Path)),
							CurrentGameMilliseconds);
					}
				}
			}
			Emit(
				EPhase6EventType::MajorOperatingDayCompleted,
				Command.Id,
				State.CurrentOperatingDay,
				CurrentGameMilliseconds,
				TEXT("Major qualifying-day evidence evaluated."));
			break;
		case EPhase6CommandType::ResolveSharedResourceConflict:
			if (FPhase6PathMetricRecord* Mixed =
				State.PathMetrics.FindByPredicate(
					[](const FPhase6PathMetricRecord& Metric)
					{
						return Metric.Path == ESpecializationPath::Mixed;
					}))
			{
				Mixed->bSharedConflictRecovered = true;
				AddEvidence(
					*Mixed,
					TEXT("SharedConflict.Recovered"));
				Emit(
					EPhase6EventType::SharedResourceConflictResolved,
					Command.Id,
					0,
					CurrentGameMilliseconds,
					TEXT("Shared capacity conflict recovered without cancellation."));
			}
			break;
		case EPhase6CommandType::AcknowledgeSeriousRisk:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::RiskAcknowledged;
			State.SeriousIncident.OwningPath = Command.Path;
			State.SeriousIncident.bWarningShown = true;
			State.SeriousIncident.bRiskAcknowledged = true;
			State.SeriousIncident.WarningIssuedAtGameMilliseconds =
				CurrentGameMilliseconds;
			State.SeriousIncident.AcknowledgedDecision =
				TEXT("Player confirmed the disclosed risky capacity override.");
			Emit(
				EPhase6EventType::IncidentWarningIssued,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				State.SeriousIncident.Warning);
			Emit(
				EPhase6EventType::IncidentRiskAcknowledged,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				State.SeriousIncident.AcknowledgedDecision);
			break;
		case EPhase6CommandType::MaterializeSeriousIncident:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::Materialized;
			State.SeriousIncident.bAircraftLost = true;
			State.SeriousIncident.MaterializedAtGameMilliseconds =
				CurrentGameMilliseconds;
			State.SeriousIncident.HumanOutcome =
				(State.SeriousIncident.OutcomeSeed % 10) < 8
					? EAbstractHumanOutcome::NoInjuries
					: EAbstractHumanOutcome::InjuriesReported;
			State.SeriousIncident.AffectedRunwayId =
				State.Runways[0].Id;
			State.Runways[0].bPrimaryClosed = true;
			State.Runways[0].PrimaryUse = ERunwayUse::Closed;
			State.Runways[0].ClosureReason =
				TEXT("Aircraft-loss response and investigation.");
			Emit(
				EPhase6EventType::IncidentMaterialized,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Warned risk materialized; Runway 09L closed while parallel operations continue."));
			break;
		case EPhase6CommandType::DispatchEmergencyResponse:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::ResourcesDispatched;
			State.SeriousIncident.ResponseSummary =
				TEXT("Fire, medical, police, and operations teams dispatched.");
			Emit(
				EPhase6EventType::IncidentResponseChanged,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				State.SeriousIncident.ResponseSummary);
			break;
		case EPhase6CommandType::ProtectIncidentArea:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::AreaProtected;
			State.SeriousIncident.bAreaProtected = true;
			State.SeriousIncident.bOtherOperationsContinue =
				State.Runways.ContainsByPredicate(
					[](const FPhase6RunwayRecord& Runway)
					{
						return Runway.bOperational &&
							!Runway.bPrimaryClosed &&
							!Runway.bReciprocalClosed;
					});
			Emit(
				EPhase6EventType::IncidentResponseChanged,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Affected area protected; unrelated safe operations remain available."));
			break;
		case EPhase6CommandType::StabilizeIncident:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::Stabilized;
			State.SeriousIncident.ResponseSummary +=
				TEXT(" Area stabilized.");
			Emit(
				EPhase6EventType::IncidentResponseChanged,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Incident stabilized with abstract human outcome reporting."));
			break;
		case EPhase6CommandType::InvestigateIncident:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::Investigating;
			State.SeriousIncident.Report = FString::Printf(
				TEXT("The acknowledged capacity override and reduced response margin caused the aircraft-loss event. Outcome: %s."),
				State.SeriousIncident.HumanOutcome ==
						EAbstractHumanOutcome::NoInjuries
					? TEXT("No injuries")
					: TEXT("Injuries reported"));
			State.SeriousIncident.Prevention =
				TEXT("Preserve emergency access and reject operations beyond the disclosed runway margin.");
			Emit(
				EPhase6EventType::IncidentReportCreated,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				State.SeriousIncident.Report);
			break;
		case EPhase6CommandType::ApplyIncidentRecovery:
			if (!State.SeriousIncident.bRecoveryFundingApplied &&
				Phase1.ApplyExternalEconomyChange(
					TEXT("Phase6.RecoverySupport"),
					7000,
					0,
					TEXT("Applied capped aircraft-loss recovery support."),
					CurrentGameMilliseconds))
			{
				State.SeriousIncident.bRecoveryFundingApplied = true;
				State.TotalRecoverySupportCredits += 7000;
				Emit(
					EPhase6EventType::RecoveryApplied,
					Command.Id,
					State.SeriousIncident.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Recovery support preserves a viable airport loop."));
			}
			break;
		case EPhase6CommandType::BeginIncidentRepair:
			if (Phase1.ApplyExternalEconomyChange(
				TEXT("Phase6.IncidentRepair"),
				-State.RepairProject.CostCredits,
				0,
				TEXT("Funded bounded incident repair."),
				CurrentGameMilliseconds))
			{
				State.RepairProject.bFunded = true;
				State.RepairProject.StartedAtGameMilliseconds =
					CurrentGameMilliseconds;
				State.RepairProject.CompletesAtGameMilliseconds =
					CurrentGameMilliseconds +
					GetPhase6Fixture().SeriousIncidentRepairMilliseconds;
				State.SeriousIncident.Lifecycle =
					ESeriousIncidentLifecycle::Repairing;
				Emit(
					EPhase6EventType::RepairChanged,
					Command.Id,
					State.RepairProject.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Runway repair started."));
			}
			break;
		case EPhase6CommandType::CompleteIncidentRepair:
			if (CurrentGameMilliseconds >=
				State.RepairProject.CompletesAtGameMilliseconds)
			{
				State.RepairProject.ProgressPercent = 100;
				State.RepairProject.bCompleted = true;
				Emit(
					EPhase6EventType::RepairChanged,
					Command.Id,
					State.RepairProject.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Runway repair completed and awaits inspection/reopening."));
			}
			break;
		case EPhase6CommandType::ReopenIncidentArea:
			State.SeriousIncident.Lifecycle =
				ESeriousIncidentLifecycle::Recovered;
			State.SeriousIncident.bAreaReopened = true;
			State.SeriousIncident.RecoveredAtGameMilliseconds =
				CurrentGameMilliseconds;
			for (FPhase6RunwayRecord& Runway : State.Runways)
			{
				if (Runway.Id ==
					State.SeriousIncident.AffectedRunwayId)
				{
					Runway.bPrimaryClosed = false;
					Runway.PrimaryUse = ERunwayUse::Arrival;
					Runway.ClosureReason.Reset();
				}
			}
			++State.RecoveredSeriousIncidentCount;
			Emit(
				EPhase6EventType::AreaReopened,
				Command.Id,
				State.SeriousIncident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Affected runway reopened after investigation, repair, and recovery."));
			break;
		default:
			break;
		}
	}

}
