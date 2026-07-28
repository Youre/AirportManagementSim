#include "AMSimPhase6ViewState.h"

#include "AMSimPhase5Fixture.h"
#include "AMSimPhase6Fixture.h"

namespace AMSim
{
	namespace
	{
		FString FacilityStateName(const EMajorFacilityState State)
		{
			switch (State)
			{
			case EMajorFacilityState::UnderConstruction: return TEXT("BUILDING");
			case EMajorFacilityState::Operational: return TEXT("READY");
			case EMajorFacilityState::Closed: return TEXT("CLOSED");
			default: return TEXT("PROPOSAL");
			}
		}

		FString AircraftStateName(
			const ELargeAircraftOperationState State)
		{
			switch (State)
			{
			case ELargeAircraftOperationState::Scheduled: return TEXT("SCHEDULED");
			case ELargeAircraftOperationState::Arrived: return TEXT("AT STAND");
			case ELargeAircraftOperationState::Turnaround: return TEXT("TURNAROUND");
			case ELargeAircraftOperationState::DepartureReady: return TEXT("READY");
			case ELargeAircraftOperationState::Departed: return TEXT("DEPARTED");
			case ELargeAircraftOperationState::Completed: return TEXT("COMPLETE");
			case ELargeAircraftOperationState::Blocked: return TEXT("BLOCKED");
			default: return TEXT("OFFER");
			}
		}

		FString MajorEvidenceAction(
			const FPhase6PathMetricRecord& Metric)
		{
			switch (Metric.Path)
			{
			case ESpecializationPath::GeneralAviation:
				if (Metric.DistinctRolesOrClasses < 5) return TEXT("ADD GA ROLE");
				if (!Metric.bFlyInCompleted) return TEXT("COMPLETE FLY-IN");
				return TEXT("COMPLETE GA MOVEMENT");
			case ESpecializationPath::FlightSchool:
				if (Metric.DistinctRolesOrClasses < 4) return TEXT("ADD TRAINER ROLE");
				if (!Metric.bOpenDayCompleted) return TEXT("COMPLETE OPEN DAY");
				return TEXT("COMPLETE TRAINING BLOCK");
			case ESpecializationPath::Charter:
				if (Metric.ShortNoticeOrVipMovements < 4) return TEXT("COMPLETE VIP MOVEMENT");
				return TEXT("COMPLETE CHARTER MOVEMENT");
			case ESpecializationPath::Cargo:
				if (Metric.DedicatedFreighterMovements < 12) return TEXT("COMPLETE FREIGHTER MOVE");
				if (Metric.NightOperations < 6) return TEXT("COMPLETE NIGHT MOVE");
				return TEXT("COMPLETE CARGO SHIPMENT");
			case ESpecializationPath::Passenger:
				if (Metric.CompletedPassengers < 500) return TEXT("COMPLETE PASSENGER JOURNEY");
				return TEXT("COMPLETE PASSENGER FLIGHT");
			case ESpecializationPath::Mixed:
				return TEXT("COMPLETE SHARED-RESOURCE DAY");
			default:
				return TEXT("COMPLETE OPERATION");
			}
		}
	}

	FPhase6ViewState MakePhase6ViewState(
		const FPhase6QuerySnapshot& Query,
		const FPhase6State& State,
		const ESpecializationPath SelectedPath)
	{
		FPhase6ViewState View;
		View.Revision = Query.Revision;
		View.Status = Query.PrimaryStatus;
		View.RunwaySummary = Query.RunwaySummary;
		View.CapacitySummary = Query.CapacitySummary;
		View.LargeAircraftSummary = Query.LargeAircraftSummary;
		View.IncidentSummary = Query.IncidentSummary;
		View.ProgressionSummary = Query.ProgressionSummary;
		View.Cause = Query.Cause;
		View.Remedy = Query.Remedy;
		View.bCanInitialize = Query.bUnlocked && !Query.bInitialized;
		if (!Query.bInitialized)
		{
			View.SelectedPathHeadline = TEXT("MAJOR CAPABILITY");
			View.SelectedPathEvidence =
				TEXT("Initialize Major planning to review six independent paths.");
			View.NextEvidenceLabel = TEXT("OPEN PHASE 6");
			return View;
		}

		for (const FPhase6RunwayRecord& Runway : State.Runways)
		{
			View.RunwayCards.Add(FString::Printf(
				TEXT("%s\n%s / %s  ·  %d M × %d M\n%s"),
				*Runway.DisplayName.ToUpper(),
				*RunwayUseDisplayName(Runway.PrimaryUse).ToUpper(),
				*RunwayUseDisplayName(Runway.ReciprocalUse).ToUpper(),
				Runway.DeclaredLengthMeters,
				Runway.WidthMeters,
				Runway.bOperational ? TEXT("OPERATIONAL") : TEXT("PROJECT")));
		}
		for (const FMajorFacilityRecord& Facility : State.Facilities)
		{
			View.FacilityCards.Add(FString::Printf(
				TEXT("%s\n%s  ·  %d CAP  ·  %lld CR"),
				*Facility.DisplayName.ToUpper(),
				*FacilityStateName(Facility.State),
				Facility.CapacityUnits,
				Facility.CostCredits));
			View.bCanConstruct |=
				Facility.State == EMajorFacilityState::Proposed;
		}
		View.bCanActivateParallel =
			State.Facilities.ContainsByPredicate(
				[](const FMajorFacilityRecord& Facility)
				{
					return Facility.Type ==
							EMajorFacilityType::ParallelRunway &&
						Facility.State ==
							EMajorFacilityState::Operational;
				}) &&
			State.Runways.ContainsByPredicate(
				[](const FPhase6RunwayRecord& Runway)
				{
					return !Runway.bOperational;
				});
		for (const FLargeAircraftOperationRecord& Operation :
			State.LargeAircraftOperations)
		{
			View.AircraftCards.Add(FString::Printf(
				TEXT("%s  ·  %s\n%s\n%d / %d PAX  ·  %d BAGS\n%d / %d SERVICES"),
				*Operation.FlightNumber,
				*Operation.OperatorDisplayName.ToUpper(),
				*AircraftStateName(Operation.State),
				Operation.PassengerCount,
				Operation.PassengerCapacity,
				Operation.AcceptedBagCount,
				Operation.CompletedServiceTaskCount,
				Operation.RequiredServiceTaskCount));
			View.bCanAcceptLargeAircraft |=
				Operation.State ==
					ELargeAircraftOperationState::Offered;
			View.bCanStartTurnaround |=
				Operation.State ==
					ELargeAircraftOperationState::Arrived;
			View.bCanAdvanceLargeAircraft |=
				Operation.State ==
					ELargeAircraftOperationState::Turnaround ||
				Operation.State ==
					ELargeAircraftOperationState::DepartureReady ||
				Operation.State ==
					ELargeAircraftOperationState::Departed;
		}
		View.IncidentCards = {
			FString::Printf(
				TEXT("LIFECYCLE\n%s"),
				*SeriousIncidentLifecycleDisplayName(
					State.SeriousIncident.Lifecycle).ToUpper()),
			FString::Printf(
				TEXT("HUMAN OUTCOME\n%s"),
				State.SeriousIncident.HumanOutcome ==
						EAbstractHumanOutcome::NotDetermined
					? TEXT("NOT DETERMINED")
					: State.SeriousIncident.HumanOutcome ==
							EAbstractHumanOutcome::NoInjuries
						? TEXT("NO INJURIES")
						: TEXT("INJURIES REPORTED")),
			FString::Printf(
				TEXT("CONTINUITY\n%s"),
				State.SeriousIncident.bOtherOperationsContinue
					? TEXT("SAFE OPERATIONS CONTINUE")
					: TEXT("AFFECTED CAPACITY CLOSED")),
			FString::Printf(
				TEXT("REPAIR\n%d%%  ·  %s"),
				State.RepairProject.ProgressPercent,
				State.RepairProject.bFunded
					? TEXT("FUNDED")
					: TEXT("NOT FUNDED"))};
		View.bCanAdvanceIncident =
			State.SeriousIncident.Lifecycle !=
				ESeriousIncidentLifecycle::Recovered;

		const FPhase6PathMetricRecord* Selected =
			State.PathMetrics.FindByPredicate(
				[SelectedPath](const FPhase6PathMetricRecord& Metric)
				{
					return Metric.Path == SelectedPath;
				});
		for (const FPhase6PathMetricRecord& Metric : State.PathMetrics)
		{
			View.CapabilityCards.Add(FString::Printf(
				TEXT("%s\n%d DAYS  ·  %d OPS  ·  %d EVIDENCE"),
				*SpecializationPathDisplayName(Metric.Path).ToUpper(),
				Metric.QualifyingOperatingDays,
				Metric.CompletedOperations,
				Metric.EvidenceIds.Num()));
		}
		if (Selected)
		{
			View.SelectedPathHeadline =
				SpecializationPathDisplayName(Selected->Path).ToUpper();
			View.SelectedPathEvidence = FString::Printf(
				TEXT("%d / 14 DAYS  ·  %d OPS\n%d ROLES/CLASSES  ·  %d PAX\n%d SHARED DAYS"),
				Selected->QualifyingOperatingDays,
				Selected->CompletedOperations,
				Selected->DistinctRolesOrClasses,
				Selected->CompletedPassengers,
				Selected->SharedResourceDays);
			View.NextEvidenceLabel = MajorEvidenceAction(*Selected);
		}
		View.bCanAdvanceDay = true;
		View.bCanResolveConflict =
			State.Facilities.ContainsByPredicate(
				[](const FMajorFacilityRecord& Facility)
				{
					return Facility.Type ==
							EMajorFacilityType::OperationsCenter &&
						Facility.State ==
							EMajorFacilityState::Operational;
				});
		return View;
	}
}
