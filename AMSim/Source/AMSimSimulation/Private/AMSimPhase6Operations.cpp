#include "AMSimPhase6Simulation.h"

#include "AMSimLivingAirportSimulation.h"
#include "AMSimPhase5Fixture.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
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

		bool IsIncidentActive(
			const ESeriousIncidentLifecycle Lifecycle)
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
	void FPhase6Simulation::CompleteConstruction(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2)
	{
		for (FMajorFacilityRecord& Facility : State.Facilities)
		{
			if (Facility.State != EMajorFacilityState::UnderConstruction ||
				CurrentGameMilliseconds <
					Facility.CompletesAtGameMilliseconds)
			{
				continue;
			}
			Facility.State = EMajorFacilityState::Operational;
			Facility.bStaffed = true;
			Phase1.ApplyExternalEconomyChange(
				TEXT("Phase6.MajorFacilityCompleted"),
				0,
				Facility.AirportPointReward,
				FString::Printf(
					TEXT("%s became operational."),
					*Facility.DisplayName),
				CurrentGameMilliseconds);
			FPhase6PathMetricRecord* Metric =
				State.PathMetrics.FindByPredicate(
					[&Facility](const FPhase6PathMetricRecord& Candidate)
					{
						return Candidate.Path == Facility.OwningPath;
					});
			if (Metric)
			{
				Metric->FacilityIds.AddUnique(Facility.ContentId);
				Metric->bSignatureFacilityOperational = true;
			}
			if (Facility.Type == EMajorFacilityType::ParallelRunway)
			{
				State.Runways[1].bOperational = true;
			}
			if (Facility.Type == EMajorFacilityType::InstructionCenter &&
				Metric)
			{
				AddEvidence(*Metric, TEXT("Team.Instructor.2"));
			}
			else if (Facility.Type == EMajorFacilityType::ExecutiveFacility &&
				Metric)
			{
				AddEvidence(*Metric, TEXT("Stand.BusinessJet.1"));
				AddEvidence(*Metric, TEXT("Stand.BusinessJet.2"));
				AddEvidence(*Metric, TEXT("Service.PremiumHandling"));
			}
			else if (Facility.Type == EMajorFacilityType::CargoHub &&
				Metric)
			{
				AddEvidence(*Metric, TEXT("Cargo.AllFlows"));
			}
			else if (Facility.Type == EMajorFacilityType::GroundAccessHub &&
				Metric)
			{
				AddEvidence(*Metric, TEXT("Transport.Public.2"));
			}
			if (Facility.Type == EMajorFacilityType::ServiceDepot)
			{
				for (const FName ServiceId : {
					TEXT("Service.Inspection"),
					TEXT("Service.PassengerBoarding"),
					TEXT("Service.Baggage"),
					TEXT("Service.Fueling"),
					TEXT("Service.Catering"),
					TEXT("Service.Cleaning"),
					TEXT("Service.PotableWater"),
					TEXT("Service.Lavatory"),
					TEXT("Service.Pushback")})
				{
					Phase2.EnsureExternalServiceResource(
						ServiceId,
						CurrentGameMilliseconds);
				}
			}
			else if (Facility.Type ==
				EMajorFacilityType::EmergencyStation)
			{
				Phase2.EnsureExternalServiceResource(
					TEXT("Service.EmergencyResponse"),
					CurrentGameMilliseconds);
			}
			Emit(
				EPhase6EventType::FacilityConstructed,
				{},
				Facility.Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s is operational."),
					*Facility.DisplayName));
		}
	}

	bool FPhase6Simulation::IsLargeAircraftCompatible(
		FString& OutCause) const
	{
		const FPhase6Fixture& Fixture = GetPhase6Fixture();
		const bool bRunway = State.Runways.ContainsByPredicate(
			[&Fixture](const FPhase6RunwayRecord& Runway)
			{
				return Runway.bOperational &&
					Runway.bPaved &&
					Runway.DeclaredLengthMeters >=
						Fixture.LargeAircraftMinimumRunwayLengthMeters &&
					Runway.WidthMeters >=
						Fixture.LargeAircraftMinimumRunwayWidthMeters &&
					Runway.bApproachAidOperational &&
					!Runway.bPrimaryClosed;
			});
		const auto HasOperationalFacility =
			[this](const EMajorFacilityType Type)
			{
				return State.Facilities.ContainsByPredicate(
					[Type](const FMajorFacilityRecord& Facility)
					{
						return Facility.Type == Type &&
							Facility.State ==
								EMajorFacilityState::Operational &&
							Facility.bStaffed;
					});
			};
		if (!bRunway)
		{
			OutCause =
				TEXT("No open paved 2,800 m x 45 m approach-capable runway.");
			return false;
		}
		for (const EMajorFacilityType Required : {
			EMajorFacilityType::LargeStand,
			EMajorFacilityType::TerminalConcourse,
			EMajorFacilityType::BaggageHall,
			EMajorFacilityType::ServiceDepot,
			EMajorFacilityType::EmergencyStation})
		{
			if (!HasOperationalFacility(Required))
			{
				OutCause = FString::Printf(
					TEXT("%s is not operational."),
					*MajorFacilityDisplayName(Required));
				return false;
			}
		}
		return true;
	}

	void FPhase6Simulation::CreateLargeAircraftTasks(
		FLargeAircraftOperationRecord& Operation,
		const int64 CurrentGameMilliseconds,
		FLivingAirportSimulation& Phase2)
	{
		struct FTaskTemplate
		{
			FName ServiceId;
			FName OperationId;
			int32 Quantity;
			int32 PrerequisiteIndex;
		};
		const TArray<FTaskTemplate> Templates = {
			{TEXT("Service.Inspection"), TEXT("WideBody.Inspection"), 1, INDEX_NONE},
			{TEXT("Service.PassengerBoarding"), TEXT("WideBody.Deboard"), Operation.PassengerCount, INDEX_NONE},
			{TEXT("Service.Baggage"), TEXT("WideBody.BaggageUnload"), Operation.AcceptedBagCount, INDEX_NONE},
			{TEXT("Service.Fueling"), TEXT("WideBody.Fueling"), 72, 0},
			{TEXT("Service.Catering"), TEXT("WideBody.Catering"), Operation.PassengerCount, INDEX_NONE},
			{TEXT("Service.Cleaning"), TEXT("WideBody.Cleaning"), Operation.PassengerCount, 1},
			{TEXT("Service.PotableWater"), TEXT("WideBody.Water"), 12, INDEX_NONE},
			{TEXT("Service.Lavatory"), TEXT("WideBody.Lavatory"), 6, INDEX_NONE},
			{TEXT("Service.Baggage"), TEXT("WideBody.BaggageLoad"), Operation.AcceptedBagCount, 2},
			{TEXT("Service.PassengerBoarding"), TEXT("WideBody.Boarding"), Operation.PassengerCount, 5},
			{TEXT("Service.Pushback"), TEXT("WideBody.Pushback"), 1, 9}};
		for (int32 Index = 0; Index < Templates.Num(); ++Index)
		{
			const FTaskTemplate& Template = Templates[Index];
			const FServiceTaskId Prerequisite =
				Template.PrerequisiteIndex == INDEX_NONE
					? FServiceTaskId{}
					: Operation.ServiceTaskIds[
						Template.PrerequisiteIndex];
			const FServiceTaskId TaskId =
				Phase2.RegisterExternalServiceTask(
					TEXT("Phase6"),
					Operation.Id.Value,
					Template.ServiceId,
					Template.OperationId,
					Template.Quantity,
					GetPhase6Fixture().
						LargeAircraftTurnaroundTaskMilliseconds,
					CurrentGameMilliseconds,
					Prerequisite);
			if (TaskId.IsValid())
			{
				Operation.ServiceTaskIds.Add(TaskId);
				Emit(
					EPhase6EventType::LargeAircraftTaskRequested,
					{},
					TaskId.Value,
					CurrentGameMilliseconds,
					FString::Printf(
						TEXT("%s requested through ordinary dispatch."),
						*Template.OperationId.ToString()));
			}
		}
		Operation.RequiredServiceTaskCount =
			Operation.ServiceTaskIds.Num();
	}

	void FPhase6Simulation::RefreshLargeAircraft(
		const int64 CurrentGameMilliseconds,
		FLivingAirportSimulation& Phase2)
	{
		for (FLargeAircraftOperationRecord& Operation :
			State.LargeAircraftOperations)
		{
			if (Operation.State ==
					ELargeAircraftOperationState::Scheduled &&
				CurrentGameMilliseconds >=
					Operation.ScheduledArrivalGameMilliseconds)
			{
				Operation.State =
					ELargeAircraftOperationState::Arrived;
				Emit(
					EPhase6EventType::LargeAircraftStateChanged,
					{},
					Operation.Id.Value,
					CurrentGameMilliseconds,
					TEXT("RL 602 arrived at Stand H1."));
			}
			if (Operation.State !=
				ELargeAircraftOperationState::Turnaround)
			{
				continue;
			}
			Operation.CompletedServiceTaskCount = 0;
			for (const FServiceTaskId TaskId : Operation.ServiceTaskIds)
			{
				if (Phase2.IsServiceTaskComplete(TaskId))
				{
					++Operation.CompletedServiceTaskCount;
				}
			}
			if (Operation.CompletedServiceTaskCount ==
				Operation.RequiredServiceTaskCount)
			{
				Operation.LoadedBagCount =
					Operation.AcceptedBagCount;
				Operation.bBagReconciled = true;
				Operation.bPassengerReconciled = true;
				Operation.Blocker.Reset();
			}
			else
			{
				Operation.Blocker = FString::Printf(
					TEXT("%d of %d mandatory tasks complete."),
					Operation.CompletedServiceTaskCount,
					Operation.RequiredServiceTaskCount);
			}
		}
	}

	void FPhase6Simulation::RecordMajorOperation(
		const FPhase6Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		FPhase6PathMetricRecord* Metric =
			State.PathMetrics.FindByPredicate(
				[&Command](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path == Command.Path;
				});
		if (!Metric)
		{
			return;
		}
		FMajorOperationRecord Operation;
		Operation.Id = {AllocateId()};
		Operation.Path = Command.Path;
		Operation.OperationKind = Command.OperationKind;
		Operation.OwningDomain = TEXT("Phase6");
		Operation.OwningEntityId = Operation.Id.Value;
		Operation.CompletedAtGameMilliseconds = CurrentGameMilliseconds;
		Operation.bCompleted = true;

		bool bFirstEvidence = false;
		auto AddFirstEvidence = [&Metric, &bFirstEvidence](
			const FName EvidenceId)
		{
			if (!Metric->EvidenceIds.Contains(EvidenceId))
			{
				Metric->EvidenceIds.Add(EvidenceId);
				bFirstEvidence = true;
			}
		};
		if (Command.OperationKind == TEXT("Movement.GA"))
		{
			++Metric->CompletedOperations;
		}
		else if (Command.OperationKind == TEXT("Role.GA"))
		{
			Metric->DistinctRolesOrClasses = FMath::Min(
				5,
				Metric->DistinctRolesOrClasses + 1);
		}
		else if (Command.OperationKind == TEXT("Event.GAFlyIn"))
		{
			Metric->bFlyInCompleted = true;
			AddFirstEvidence(TEXT("Event.GAFlyIn"));
		}
		else if (Command.OperationKind == TEXT("Training.Block"))
		{
			++Metric->CompletedOperations;
		}
		else if (Command.OperationKind == TEXT("AircraftRole.Trainer"))
		{
			Metric->DistinctRolesOrClasses = FMath::Min(
				4,
				Metric->DistinctRolesOrClasses + 1);
		}
		else if (Command.OperationKind ==
			TEXT("Event.FlightSchoolOpenDay"))
		{
			Metric->bOpenDayCompleted = true;
			AddFirstEvidence(TEXT("Event.FlightSchoolOpenDay"));
		}
		else if (Command.OperationKind == TEXT("Movement.Charter"))
		{
			++Metric->CompletedOperations;
		}
		else if (Command.OperationKind ==
			TEXT("Movement.Charter.ShortNoticeVip"))
		{
			++Metric->CompletedOperations;
			++Metric->ShortNoticeOrVipMovements;
			Operation.bShortNoticeOrVip = true;
			if (Metric->ShortNoticeOrVipMovements >= 4)
			{
				AddFirstEvidence(TEXT("Movement.ShortNoticeVip.4"));
			}
		}
		else if (Command.OperationKind == TEXT("Shipment.Cargo"))
		{
			++Metric->CompletedOperations;
		}
		else if (Command.OperationKind ==
			TEXT("Movement.Cargo.DedicatedFreighter"))
		{
			++Metric->DedicatedFreighterMovements;
			if (Metric->DedicatedFreighterMovements >= 12)
			{
				AddFirstEvidence(TEXT("Cargo.DedicatedFreighter.12"));
			}
		}
		else if (Command.OperationKind == TEXT("Movement.Cargo.Night"))
		{
			++Metric->NightOperations;
			Operation.bNightOperation = true;
			if (Metric->NightOperations >= 6)
			{
				AddFirstEvidence(TEXT("Cargo.Night.6"));
			}
		}
		else if (Command.OperationKind == TEXT("Flight.Passenger"))
		{
			++Metric->CompletedOperations;
		}
		else if (Command.OperationKind == TEXT("Journey.Passenger"))
		{
			++Metric->CompletedPassengers;
		}
		else if (Command.OperationKind == TEXT("Shared.ResourceDay"))
		{
			++Metric->SharedResourceDays;
			Operation.bSharedResourceUsed = true;
		}

		State.MajorOperations.Add(Operation);
		if (Command.Path != ESpecializationPath::Mixed)
		{
			if (FPhase6PathMetricRecord* Mixed =
				State.PathMetrics.FindByPredicate(
					[](const FPhase6PathMetricRecord& Candidate)
					{
						return Candidate.Path ==
							ESpecializationPath::Mixed;
					}))
			{
				++Mixed->CompletedOperations;
			}
		}
		if (bFirstEvidence)
		{
			Phase1.ApplyExternalEconomyChange(
				TEXT("Phase6.FirstMajorEvidence"),
				0,
				2,
				FString::Printf(
					TEXT("First %s evidence recorded."),
					*Command.OperationKind.ToString()),
				CurrentGameMilliseconds);
		}
		Emit(
			EPhase6EventType::MajorOperationCompleted,
			Command.Id,
			Operation.Id.Value,
			CurrentGameMilliseconds,
			FString::Printf(
				TEXT("%s completed for %s."),
				*Command.OperationKind.ToString(),
				*SpecializationPathDisplayName(Command.Path)));
	}

	void FPhase6Simulation::RefreshPathMetrics(
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State)
	{
		for (FPhase6PathMetricRecord& Metric : State.PathMetrics)
		{
			if (const FPhase5PathEvidenceRecord* Base =
				FindPath(Phase5State, Metric.Path))
			{
				Metric.CompletedOperations = FMath::Max(
					Metric.CompletedOperations,
					Base->CompletedOperations);
				Metric.DistinctRolesOrClasses = FMath::Max(
					Metric.DistinctRolesOrClasses,
					Base->DistinctRolesOrClasses);
				Metric.CompletedPassengers = FMath::Max(
					Metric.CompletedPassengers,
					Base->CompletedPassengers);
				Metric.bPrimaryTenantActive =
					Metric.bPrimaryTenantActive ||
					Base->bPrimaryTenantActive;
				Metric.bSecondaryProviderActive =
					Metric.bSecondaryProviderActive ||
					Base->bSecondaryProviderActive;
			}
			if (Metric.Path == ESpecializationPath::Cargo &&
				Phase5State.CompletedCargoClassCount == 4 &&
				Phase5State.CompletedCargoFlowCount == 3)
			{
				Metric.DistinctRolesOrClasses = FMath::Max(
					Metric.DistinctRolesOrClasses,
					4);
				AddEvidence(Metric, TEXT("Cargo.AllFlows"));
			}
			else if (Metric.Path == ESpecializationPath::Passenger)
			{
				Metric.CompletedOperations = FMath::Max(
					Metric.CompletedOperations,
					Phase4State.CompletedFlightCount);
				Metric.CompletedPassengers = FMath::Max(
					Metric.CompletedPassengers,
					Phase3State.CompletedPassengerCount);
			}
		}

		int32 RegionalCount = 0;
		int32 AdvancedCount = 0;
		for (const FPhase5PathEvidenceRecord& Path : Phase5State.Paths)
		{
			if (Path.Path == ESpecializationPath::Mixed)
			{
				continue;
			}
			const ECapabilityBand Earned =
				Path.EarnedBand == ECapabilityBand::Unavailable
					? Path.Band
					: Path.EarnedBand;
			if (Earned >= ECapabilityBand::Regional)
			{
				++RegionalCount;
			}
			if (Earned >= ECapabilityBand::Advanced)
			{
				++AdvancedCount;
			}
		}
		if (FPhase6PathMetricRecord* Mixed =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path == ESpecializationPath::Mixed;
				}))
		{
			Mixed->RegionalPathCount = RegionalCount;
			Mixed->AdvancedPathCount = AdvancedCount;
			Mixed->bPrimaryTenantActive = RegionalCount >= 3;
			Mixed->bSecondaryProviderActive = AdvancedCount >= 2;
		}

		const auto ProviderActive =
			[&Phase5State](const FName ProviderType)
			{
				return Phase5State.ProviderTenants.ContainsByPredicate(
					[ProviderType](
						const FPhase5ProviderTenantRecord& Tenant)
					{
						return Tenant.ProviderType == ProviderType &&
							Tenant.State ==
								EProviderTenantState::Active;
					});
			};
		if (FPhase6PathMetricRecord* GA =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path ==
						ESpecializationPath::GeneralAviation;
				}))
		{
			if (ProviderActive(TEXT("FBO"))) AddEvidence(*GA, TEXT("Provider.FBO"));
			if (ProviderActive(TEXT("Maintenance"))) AddEvidence(*GA, TEXT("Provider.Maintenance"));
			if (ProviderActive(TEXT("Fuel"))) AddEvidence(*GA, TEXT("Provider.Fuel"));
		}
		if (FPhase6PathMetricRecord* Cargo =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path == ESpecializationPath::Cargo;
				}))
		{
			if (ProviderActive(TEXT("CargoOperator")))
			{
				AddEvidence(*Cargo, TEXT("Tenant.CargoOperator"));
			}
		}
		if (FPhase6PathMetricRecord* Passenger =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path == ESpecializationPath::Passenger;
				}))
		{
			if (Passenger->bPrimaryTenantActive)
			{
				AddEvidence(*Passenger, TEXT("Tenant.PassengerOperator"));
			}
			const int32 Concessions =
				(ProviderActive(TEXT("FoodConcession")) ? 1 : 0) +
				(ProviderActive(TEXT("RetailConcession")) ? 1 : 0);
			if (Concessions >= 2)
			{
				AddEvidence(*Passenger, TEXT("Concession.2"));
			}
		}
		if (FPhase6PathMetricRecord* School =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path ==
						ESpecializationPath::FlightSchool;
				}))
		{
			if (School->bPrimaryTenantActive)
			{
				AddEvidence(*School, TEXT("Tenant.FlightSchool"));
			}
		}
		if (FPhase6PathMetricRecord* Charter =
			State.PathMetrics.FindByPredicate(
				[](const FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path ==
						ESpecializationPath::Charter;
				}))
		{
			if (Charter->bPrimaryTenantActive)
			{
				AddEvidence(*Charter, TEXT("Tenant.Charter"));
			}
		}
	}

	void FPhase6Simulation::RefreshScaleDiagnostics(
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State)
	{
		const FPhase6Fixture& Fixture = GetPhase6Fixture();
		State.ScaleDiagnostics.LogicalAgentCount =
			Phase3State.Passengers.Num() +
			Phase3State.Bags.Num() +
			Phase2State.Teams.Num() +
			Phase2State.Vehicles.Num();
		State.ScaleDiagnostics.VisibleAgentCount = FMath::Min(
			State.ScaleDiagnostics.LogicalAgentCount,
			Fixture.VisibleAgentTarget);
		State.ScaleDiagnostics.OnMapAircraftCount = FMath::Min(
			Phase2State.Aircraft.Num() +
				Phase4State.Flights.Num() +
				State.LargeAircraftOperations.Num(),
			Fixture.OnMapAircraftTarget);
		State.ScaleDiagnostics.VehicleCount = FMath::Min(
			Phase2State.Vehicles.Num(),
			Fixture.VehicleTarget);
		State.ScaleDiagnostics.ActivePresentationProxyCount = FMath::Min(
			State.ScaleDiagnostics.VisibleAgentCount +
				State.ScaleDiagnostics.OnMapAircraftCount +
				State.ScaleDiagnostics.VehicleCount,
			Fixture.VisibleAgentTarget);
		State.ScaleDiagnostics.PendingSimulationBuckets =
			FMath::Max(
				0,
				State.ScaleDiagnostics.LogicalAgentCount -
					Fixture.VisibleAgentTarget) /
			250;
		State.ScaleDiagnostics.bBacklogWarning =
			State.ScaleDiagnostics.LogicalAgentCount >
				Fixture.LogicalAgentTarget;
	}

	FPhase6CapabilitySignals FPhase6Simulation::CreateCapabilitySignals(
		const FPhase1State& Phase1State,
		const FPhase5State& Phase5State) const
	{
		FPhase6CapabilitySignals Signals;
		for (const FPhase6PathMetricRecord& Metric : State.PathMetrics)
		{
			FPhase6MajorPathSignal Signal;
			Signal.Path = Metric.Path;
			Signal.QualifyingOperatingDays =
				Metric.QualifyingOperatingDays;
			Signal.SafetyRating = Metric.SafetyRating;
			Signal.ReliabilityRating = Metric.ReliabilityRating;
			Signal.TenantRelationshipRating =
				Metric.TenantRelationshipRating;
			Signal.CompletedOperations = Metric.CompletedOperations;
			Signal.DistinctRolesOrClasses =
				Metric.DistinctRolesOrClasses;
			Signal.CompletedPassengers = Metric.CompletedPassengers;
			Signal.RegionalPathCount = Metric.RegionalPathCount;
			Signal.AdvancedPathCount = Metric.AdvancedPathCount;
			Signal.SharedResourceDays = Metric.SharedResourceDays;
			Signal.bPrimaryTenantActive = Metric.bPrimaryTenantActive;
			Signal.bSecondaryProviderActive =
				Metric.bSecondaryProviderActive;
			Signal.bSignatureFacilityOperational =
				Metric.bSignatureFacilityOperational;
			Signal.FacilityIds = Metric.FacilityIds;
			Signal.EvidenceIds = Metric.EvidenceIds;
			if (Metric.bCriticalReadinessFailure)
			{
				Signal.OperationalStatus =
					ECapabilityOperationalStatus::Suspended;
			}
			else if (IsIncidentActive(State.SeriousIncident.Lifecycle) &&
				State.SeriousIncident.OwningPath == Metric.Path)
			{
				Signal.OperationalStatus =
					ECapabilityOperationalStatus::Recovering;
			}
			else
			{
				Signal.OperationalStatus =
					ECapabilityOperationalStatus::Healthy;
			}

			FPhase5PathEvidenceRecord Evidence;
			if (const FPhase5PathEvidenceRecord* Base =
				FindPath(Phase5State, Metric.Path))
			{
				Evidence = *Base;
			}
			Evidence.Path = Metric.Path;
			Evidence.AirportPoints = Phase1State.AirportPoints;
			Evidence.QualifyingOperatingDays =
				Metric.QualifyingOperatingDays;
			Evidence.SafetyRating = Metric.SafetyRating;
			Evidence.ReliabilityRating = Metric.ReliabilityRating;
			Evidence.TenantRelationshipRating =
				Metric.TenantRelationshipRating;
			Evidence.CompletedOperations = Metric.CompletedOperations;
			Evidence.DistinctRolesOrClasses =
				Metric.DistinctRolesOrClasses;
			Evidence.CompletedPassengers = Metric.CompletedPassengers;
			Evidence.RegionalPathCount = Metric.RegionalPathCount;
			Evidence.AdvancedPathCount = Metric.AdvancedPathCount;
			Evidence.SharedResourceDays = Metric.SharedResourceDays;
			Evidence.bPrimaryTenantActive =
				Metric.bPrimaryTenantActive;
			Evidence.bSecondaryProviderActive =
				Metric.bSecondaryProviderActive;
			Evidence.bSignatureFacilityOperational =
				Metric.bSignatureFacilityOperational;
			Evidence.FacilityIds = Metric.FacilityIds;
			Evidence.MajorEvidenceIds = Metric.EvidenceIds;
			Signal.bMajorRequirementsMet =
				Signal.OperationalStatus !=
					ECapabilityOperationalStatus::Suspended &&
				MeetsMajorRequirements(Evidence);
			if (Signal.bMajorRequirementsMet)
			{
				++Signals.MajorPathCount;
			}
			Signals.Paths.Add(MoveTemp(Signal));
		}
		return Signals;
	}

	FPhase6QuerySnapshot FPhase6Simulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase5State& Phase5State) const
	{
		FPhase6QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bUnlocked = Phase5State.AdvancedPathCount > 0;
		Query.bInitialized = State.bInitialized;
		Query.bFixtureCompleted = State.bFixtureCompleted;
		Query.OperatingDay = State.CurrentOperatingDay;
		for (const FPhase6RunwayRecord& Runway : State.Runways)
		{
			Query.OperationalRunwayCount += Runway.bOperational ? 1 : 0;
		}
		for (const FMajorFacilityRecord& Facility : State.Facilities)
		{
			Query.OperationalFacilityCount +=
				Facility.State == EMajorFacilityState::Operational ? 1 : 0;
		}
		Query.MajorPathCount = State.MajorPathCount;
		Query.LargeAircraftOperationCount =
			State.LargeAircraftOperations.Num();
		Query.CompletedLargeAircraftCount =
			State.CompletedLargeAircraftCount;
		Query.IncidentLifecycle =
			State.SeriousIncident.Lifecycle;
		Query.ScaleDiagnostics = State.ScaleDiagnostics;
		Query.PrimaryStatus = !State.bInitialized
			? TEXT("Major capability is ready to initialize.")
			: FString::Printf(
				TEXT("%d / 6 Major paths - day %d - %d Credits"),
				Query.MajorPathCount,
				State.CurrentOperatingDay,
				Phase1State.Credits);
		Query.RunwaySummary = FString::Printf(
			TEXT("%d operational runways - %s"),
			Query.OperationalRunwayCount,
			State.Runways.IsEmpty()
				? TEXT("No runway data")
				: *State.Runways[0].Recommendation);
		Query.CapacitySummary = FString::Printf(
			TEXT("%d major facilities operational - %d logical agents"),
			Query.OperationalFacilityCount,
			State.ScaleDiagnostics.LogicalAgentCount);
		const FLargeAircraftOperationRecord* WideBody =
			State.LargeAircraftOperations.IsEmpty()
				? nullptr
				: &State.LargeAircraftOperations[0];
		Query.LargeAircraftSummary = WideBody
			? FString::Printf(
				TEXT("%s - %d / %d services complete"),
				*WideBody->FlightNumber,
				WideBody->CompletedServiceTaskCount,
				WideBody->RequiredServiceTaskCount)
			: TEXT("No wide-body offer.");
		Query.IncidentSummary = FString::Printf(
			TEXT("%s - %s"),
			*SeriousIncidentLifecycleDisplayName(
				State.SeriousIncident.Lifecycle),
			State.SeriousIncident.bOtherOperationsContinue
				? TEXT("safe unaffected operations continue")
				: TEXT("airport-wide response active"));
		Query.ProgressionSummary = FString::Printf(
			TEXT("%d / 6 Major - 100 AP and 14 qualifying days required"),
			Query.MajorPathCount);
		Query.Cause = State.SeriousIncident.RiskFactor;
		Query.Remedy = State.SeriousIncident.Remedy;
		Query.StateChecksum = CalculateChecksum();
		return Query;
	}
}
