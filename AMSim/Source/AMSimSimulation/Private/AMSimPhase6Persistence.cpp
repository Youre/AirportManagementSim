#include "AMSimPhase6Simulation.h"

#include "AMSimPhase6Fixture.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase6IdBase = 1ull << 61;
		constexpr uint64 FnvOffset = 1469598103934665603ull;
		constexpr uint64 FnvPrime = 1099511628211ull;

		void HashBytes(
			uint64& Hash,
			const void* Data,
			const SIZE_T Size)
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
	}
	uint64 FPhase6Simulation::CalculateChecksum() const
	{
		uint64 Hash = FnvOffset;
		HashBytes(Hash, &State.bInitialized, sizeof(State.bInitialized));
		HashBytes(Hash, &State.MasterSeed, sizeof(State.MasterSeed));
		HashBytes(
			Hash,
			&State.RandomStreamState,
			sizeof(State.RandomStreamState));
		HashBytes(
			Hash,
			&State.NextDomainId,
			sizeof(State.NextDomainId));
		HashBytes(
			Hash,
			&State.CurrentOperatingDay,
			sizeof(State.CurrentOperatingDay));
		for (const FPhase6RunwayRecord& Runway : State.Runways)
		{
			HashBytes(Hash, &Runway.Id.Value, sizeof(Runway.Id.Value));
			HashBytes(
				Hash,
				&Runway.MagneticHeadingDegrees,
				sizeof(Runway.MagneticHeadingDegrees));
			HashBytes(
				Hash,
				&Runway.PrimaryUse,
				sizeof(Runway.PrimaryUse));
			HashBytes(
				Hash,
				&Runway.ReciprocalUse,
				sizeof(Runway.ReciprocalUse));
			HashBytes(
				Hash,
				&Runway.bOperational,
				sizeof(Runway.bOperational));
			HashString(Hash, Runway.PrimaryEndId.ToString());
			HashString(Hash, Runway.ReciprocalEndId.ToString());
		}
		for (const FMajorFacilityRecord& Facility : State.Facilities)
		{
			HashBytes(Hash, &Facility.Id.Value, sizeof(Facility.Id.Value));
			HashBytes(Hash, &Facility.State, sizeof(Facility.State));
			HashString(Hash, Facility.ContentId.ToString());
		}
		for (const FLargeAircraftOperationRecord& Operation :
			State.LargeAircraftOperations)
		{
			HashBytes(
				Hash,
				&Operation.Id.Value,
				sizeof(Operation.Id.Value));
			HashBytes(Hash, &Operation.State, sizeof(Operation.State));
			HashBytes(
				Hash,
				&Operation.CompletedServiceTaskCount,
				sizeof(Operation.CompletedServiceTaskCount));
			for (const FServiceTaskId TaskId : Operation.ServiceTaskIds)
			{
				HashBytes(Hash, &TaskId.Value, sizeof(TaskId.Value));
			}
		}
		for (const FPhase6PathMetricRecord& Metric : State.PathMetrics)
		{
			HashBytes(Hash, &Metric.Path, sizeof(Metric.Path));
			HashBytes(
				Hash,
				&Metric.QualifyingOperatingDays,
				sizeof(Metric.QualifyingOperatingDays));
			HashBytes(
				Hash,
				&Metric.CompletedOperations,
				sizeof(Metric.CompletedOperations));
			for (const FName EvidenceId : Metric.EvidenceIds)
			{
				HashString(Hash, EvidenceId.ToString());
			}
		}
		HashBytes(
			Hash,
			&State.SeriousIncident.Lifecycle,
			sizeof(State.SeriousIncident.Lifecycle));
		HashBytes(
			Hash,
			&State.SeriousIncident.OutcomeSeed,
			sizeof(State.SeriousIncident.OutcomeSeed));
		HashBytes(
			Hash,
			&State.SeriousIncident.HumanOutcome,
			sizeof(State.SeriousIncident.HumanOutcome));
		HashBytes(
			Hash,
			&State.RepairProject.ProgressPercent,
			sizeof(State.RepairProject.ProgressPercent));
		for (const FPhase6Event& Event : State.Events)
		{
			HashBytes(Hash, &Event.Sequence, sizeof(Event.Sequence));
			HashBytes(Hash, &Event.Type, sizeof(Event.Type));
			HashBytes(Hash, &Event.SubjectId, sizeof(Event.SubjectId));
			HashString(Hash, Event.Message);
		}
		return Hash;
	}

	bool FPhase6Simulation::RestoreState(
		const FPhase6State& Candidate,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		const FPhase5State& Phase5State)
	{
		(void)Phase1State;
		(void)Phase3State;
		(void)Phase4State;
		(void)Phase5State;
		if (Candidate.MasterSeed == 0 ||
			Candidate.NextDomainId < Phase6IdBase ||
			Candidate.NextEventSequence == 0)
		{
			return false;
		}
		if (!Candidate.bInitialized)
		{
			State = Candidate;
			PendingCommands.Reset();
			return true;
		}
		if (Candidate.Runways.Num() < 2 ||
			Candidate.PathMetrics.Num() != 6 ||
			Candidate.ScaleDiagnostics.LogicalAgentCount >
				GetPhase6Fixture().LogicalAgentTarget ||
			Candidate.ScaleDiagnostics.VisibleAgentCount >
				GetPhase6Fixture().VisibleAgentTarget ||
			Candidate.ScaleDiagnostics.OnMapAircraftCount >
				GetPhase6Fixture().OnMapAircraftTarget ||
			Candidate.ScaleDiagnostics.VehicleCount >
				GetPhase6Fixture().VehicleTarget)
		{
			return false;
		}
		TSet<uint64> Ids;
		auto AddId = [&Ids, &Candidate](const uint64 Id)
		{
			return Id >= Phase6IdBase &&
				Id < Candidate.NextDomainId &&
				!Ids.Contains(Id) &&
				(Ids.Add(Id), true);
		};
		for (const FPhase6RunwayRecord& Runway : Candidate.Runways)
		{
			if (!AddId(Runway.Id.Value) ||
				Runway.MagneticHeadingDegrees < 0 ||
				Runway.MagneticHeadingDegrees >= 360 ||
				Runway.DeclaredLengthMeters <= 0 ||
				Runway.WidthMeters <= 0 ||
				Runway.PrimaryEndId != FName(*RunwayEndLabel(
					Runway.MagneticHeadingDegrees,
					Runway.Side)) ||
				Runway.ReciprocalEndId != FName(*RunwayEndLabel(
					(Runway.MagneticHeadingDegrees + 180) % 360,
					ReciprocalRunwaySide(Runway.Side))))
			{
				return false;
			}
		}
		for (const FMajorFacilityRecord& Facility : Candidate.Facilities)
		{
			if (!AddId(Facility.Id.Value) ||
				Facility.ContentId.IsNone() ||
				Facility.CapacityUnits <= 0 ||
				Facility.CostCredits < 0 ||
				!Facility.bCookerVisibleDefinition)
			{
				return false;
			}
		}
		for (const FLargeAircraftOperationRecord& Operation :
			Candidate.LargeAircraftOperations)
		{
			if (!AddId(Operation.Id.Value) ||
				Operation.AircraftContentId.IsNone() ||
				Operation.PassengerCount < 0 ||
				Operation.PassengerCount > Operation.PassengerCapacity ||
				Operation.LoadedBagCount > Operation.AcceptedBagCount ||
				Operation.CompletedServiceTaskCount >
					Operation.RequiredServiceTaskCount)
			{
				return false;
			}
			for (const FServiceTaskId TaskId : Operation.ServiceTaskIds)
			{
				if (!Phase2State.ServiceTasks.ContainsByPredicate(
					[TaskId](const FPhase2ServiceTaskRecord& Task)
					{
						return Task.Id == TaskId &&
							Task.OwnerDomain == TEXT("Phase6");
					}))
				{
					return false;
				}
			}
		}
		for (const FMajorOperationRecord& Operation :
			Candidate.MajorOperations)
		{
			if (!AddId(Operation.Id.Value) ||
				Operation.OperationKind.IsNone() ||
				!Operation.bCompleted)
			{
				return false;
			}
		}
		TSet<uint8> Paths;
		for (const FPhase6PathMetricRecord& Metric :
			Candidate.PathMetrics)
		{
			const uint8 Path = static_cast<uint8>(Metric.Path);
			if (Path >= 6 ||
				Paths.Contains(Path) ||
				Metric.QualifyingOperatingDays < 0 ||
				Metric.CompletedOperations < 0 ||
				Metric.TenantRelationshipRating < 0 ||
				Metric.TenantRelationshipRating > 100)
			{
				return false;
			}
			Paths.Add(Path);
		}
		if (!AddId(Candidate.SeriousIncident.Id.Value) ||
			!AddId(Candidate.RepairProject.Id.Value) ||
			Candidate.RepairProject.IncidentId !=
				Candidate.SeriousIncident.Id)
		{
			return false;
		}
		if (Candidate.SeriousIncident.bAircraftLost &&
			(!Candidate.SeriousIncident.bWarningShown ||
				!Candidate.SeriousIncident.bRiskAcknowledged ||
				Candidate.SeriousIncident.RiskFactor.IsEmpty() ||
				Candidate.SeriousIncident.Warning.IsEmpty()))
		{
			return false;
		}
		if (Candidate.SeriousIncident.Lifecycle ==
				ESeriousIncidentLifecycle::Recovered &&
			(!Candidate.RepairProject.bCompleted ||
				!Candidate.SeriousIncident.bRecoveryFundingApplied ||
				!Candidate.SeriousIncident.bAreaReopened))
		{
			return false;
		}
		uint64 PreviousSequence = 0;
		for (const FPhase6Event& Event : Candidate.Events)
		{
			if (Event.Sequence <= PreviousSequence ||
				Event.Sequence >= Candidate.NextEventSequence)
			{
				return false;
			}
			PreviousSequence = Event.Sequence;
		}
		State = Candidate;
		PendingCommands.Reset();
		return true;
	}

	void FPhase6Simulation::Emit(
		const EPhase6EventType Type,
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

	uint64 FPhase6Simulation::AllocateId()
	{
		return State.NextDomainId++;
	}
}
