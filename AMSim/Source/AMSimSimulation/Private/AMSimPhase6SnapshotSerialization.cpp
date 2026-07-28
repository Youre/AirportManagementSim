#include "AMSimPhase6SnapshotSerialization.h"

#include "Serialization/Archive.h"

namespace AMSim
{
	namespace
	{
		constexpr int32 MaximumSmallRecords = 10000;
		constexpr int32 MaximumHistoryRecords = 100000;

		template <typename EnumType>
		void SerializeEnum(FArchive& Archive, EnumType& Value)
		{
			uint8 Raw = static_cast<uint8>(Value);
			Archive << Raw;
			if (Archive.IsLoading())
			{
				Value = static_cast<EnumType>(Raw);
			}
		}

		void SerializeName(FArchive& Archive, FName& Value)
		{
			FString Text = Value.ToString();
			Archive << Text;
			if (Archive.IsLoading())
			{
				Value = FName(*Text);
			}
		}

		bool SerializeCount(
			FArchive& Archive,
			int32& Count,
			const int32 Maximum)
		{
			Archive << Count;
			if (Archive.IsLoading() && (Count < 0 || Count > Maximum))
			{
				Archive.SetError();
				return false;
			}
			return !Archive.IsError();
		}

		template <typename RecordType, typename Serializer>
		void SerializeRecords(
			FArchive& Archive,
			TArray<RecordType>& Records,
			const int32 Maximum,
			Serializer&& SerializeRecord)
		{
			int32 Count = Records.Num();
			if (!SerializeCount(Archive, Count, Maximum))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Records.SetNum(Count);
			}
			for (RecordType& Record : Records)
			{
				SerializeRecord(Archive, Record);
				if (Archive.IsError())
				{
					return;
				}
			}
		}

		void SerializeNames(FArchive& Archive, TArray<FName>& Names)
		{
			int32 Count = Names.Num();
			if (!SerializeCount(Archive, Count, MaximumSmallRecords))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Names.SetNum(Count);
			}
			for (FName& Name : Names)
			{
				SerializeName(Archive, Name);
			}
		}

		void SerializeTaskIds(
			FArchive& Archive,
			TArray<FServiceTaskId>& Ids)
		{
			int32 Count = Ids.Num();
			if (!SerializeCount(Archive, Count, MaximumHistoryRecords))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Ids.SetNum(Count);
			}
			for (FServiceTaskId& Id : Ids)
			{
				Archive << Id.Value;
			}
		}

		void SerializeRunway(
			FArchive& Archive,
			FPhase6RunwayRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			Archive << Record.DisplayName;
			Archive << Record.MagneticHeadingDegrees;
			Archive << Record.DeclaredLengthMeters;
			Archive << Record.WidthMeters;
			Archive << Record.ParallelOrder;
			SerializeEnum(Archive, Record.Side);
			SerializeName(Archive, Record.PrimaryEndId);
			SerializeName(Archive, Record.ReciprocalEndId);
			SerializeEnum(Archive, Record.PrimaryUse);
			SerializeEnum(Archive, Record.ReciprocalUse);
			Archive << Record.bPaved;
			Archive << Record.bOperational;
			Archive << Record.bPrimaryClosed;
			Archive << Record.bReciprocalClosed;
			Archive << Record.bPrimaryOccupied;
			Archive << Record.bReciprocalOccupied;
			Archive << Record.bApproachAidOperational;
			Archive << Record.Recommendation;
			Archive << Record.ClosureReason;
		}

		void SerializeFacility(
			FArchive& Archive,
			FMajorFacilityRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			Archive << Record.DisplayName;
			SerializeEnum(Archive, Record.Type);
			SerializeEnum(Archive, Record.State);
			SerializeEnum(Archive, Record.OwningPath);
			Archive << Record.CapacityUnits;
			Archive << Record.CostCredits;
			Archive << Record.AirportPointReward;
			Archive << Record.bRoadConnected;
			Archive << Record.bAirsideConnected;
			Archive << Record.bPassengerConnected;
			Archive << Record.bStaffed;
			Archive << Record.bCookerVisibleDefinition;
			Archive << Record.FundedAtGameMilliseconds;
			Archive << Record.CompletesAtGameMilliseconds;
			Archive << Record.Requirement;
			Archive << Record.Cause;
			Archive << Record.Remedy;
		}

		void SerializeLargeAircraft(
			FArchive& Archive,
			FLargeAircraftOperationRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			SerializeName(Archive, Record.AircraftContentId);
			SerializeName(Archive, Record.OperatorId);
			Archive << Record.OperatorDisplayName;
			Archive << Record.FlightNumber;
			SerializeEnum(Archive, Record.State);
			Archive << Record.RunwayId.Value;
			Archive << Record.StandFacilityId.Value;
			Archive << Record.PassengerCapacity;
			Archive << Record.PassengerCount;
			Archive << Record.AcceptedBagCount;
			Archive << Record.LoadedBagCount;
			Archive << Record.CompletedServiceTaskCount;
			Archive << Record.RequiredServiceTaskCount;
			Archive << Record.ScheduledArrivalGameMilliseconds;
			Archive << Record.ScheduledDepartureGameMilliseconds;
			SerializeTaskIds(Archive, Record.ServiceTaskIds);
			Archive << Record.bPassengerReconciled;
			Archive << Record.bBagReconciled;
			Archive << Record.bRewardRecognized;
			Archive << Record.Blocker;
		}

		void SerializeMajorOperation(
			FArchive& Archive,
			FMajorOperationRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeEnum(Archive, Record.Path);
			SerializeName(Archive, Record.OperationKind);
			SerializeName(Archive, Record.OwningDomain);
			Archive << Record.OwningEntityId;
			Archive << Record.CompletedAtGameMilliseconds;
			Archive << Record.bCompleted;
			Archive << Record.bNightOperation;
			Archive << Record.bShortNoticeOrVip;
			Archive << Record.bSharedResourceUsed;
		}

		void SerializePathMetric(
			FArchive& Archive,
			FPhase6PathMetricRecord& Record)
		{
			SerializeEnum(Archive, Record.Path);
			Archive << Record.QualifyingOperatingDays;
			Archive << Record.SafetyRating;
			Archive << Record.ReliabilityRating;
			Archive << Record.TenantRelationshipRating;
			Archive << Record.CompletedOperations;
			Archive << Record.DistinctRolesOrClasses;
			Archive << Record.CompletedPassengers;
			Archive << Record.DedicatedFreighterMovements;
			Archive << Record.NightOperations;
			Archive << Record.ShortNoticeOrVipMovements;
			Archive << Record.SharedResourceDays;
			Archive << Record.RegionalPathCount;
			Archive << Record.AdvancedPathCount;
			Archive << Record.bPrimaryTenantActive;
			Archive << Record.bSecondaryProviderActive;
			Archive << Record.bSignatureFacilityOperational;
			Archive << Record.bFlyInCompleted;
			Archive << Record.bOpenDayCompleted;
			Archive << Record.bWideBodyTurnaroundCompleted;
			Archive << Record.bSharedConflictRecovered;
			Archive << Record.bCriticalReadinessFailure;
			SerializeNames(Archive, Record.FacilityIds);
			SerializeNames(Archive, Record.EvidenceIds);
		}

		void SerializeIncident(
			FArchive& Archive,
			FSeriousIncidentRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeEnum(Archive, Record.Lifecycle);
			SerializeEnum(Archive, Record.OwningPath);
			Archive << Record.LargeAircraftOperationId.Value;
			Archive << Record.AffectedRunwayId.Value;
			Archive << Record.OutcomeSeed;
			SerializeEnum(Archive, Record.HumanOutcome);
			Archive << Record.RiskFactor;
			Archive << Record.Warning;
			Archive << Record.Remedy;
			Archive << Record.AcknowledgedDecision;
			Archive << Record.ResponseSummary;
			Archive << Record.Report;
			Archive << Record.Prevention;
			Archive << Record.bWarningShown;
			Archive << Record.bRiskAcknowledged;
			Archive << Record.bAircraftLost;
			Archive << Record.bAreaProtected;
			Archive << Record.bOtherOperationsContinue;
			Archive << Record.bRecoveryFundingApplied;
			Archive << Record.bAreaReopened;
			Archive << Record.WarningIssuedAtGameMilliseconds;
			Archive << Record.MaterializedAtGameMilliseconds;
			Archive << Record.RecoveredAtGameMilliseconds;
		}

		void SerializeRepair(
			FArchive& Archive,
			FRepairProjectRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.IncidentId.Value;
			SerializeName(Archive, Record.ContentId);
			Archive << Record.CostCredits;
			Archive << Record.ProgressPercent;
			Archive << Record.bFunded;
			Archive << Record.bCompleted;
			Archive << Record.StartedAtGameMilliseconds;
			Archive << Record.CompletesAtGameMilliseconds;
		}

		void SerializeScale(
			FArchive& Archive,
			FPhase6ScaleDiagnostics& Scale)
		{
			Archive << Scale.LogicalAgentCount;
			Archive << Scale.VisibleAgentCount;
			Archive << Scale.OnMapAircraftCount;
			Archive << Scale.VehicleCount;
			Archive << Scale.ActivePresentationProxyCount;
			Archive << Scale.PendingSimulationBuckets;
			Archive << Scale.bBacklogWarning;
		}

		void SerializeEvent(FArchive& Archive, FPhase6Event& Record)
		{
			Archive << Record.Sequence;
			Archive << Record.GameTimeMilliseconds;
			SerializeEnum(Archive, Record.Type);
			Archive << Record.Cause.Value;
			Archive << Record.SubjectId;
			Archive << Record.Message;
		}
	}

	void SerializePhase6State(FArchive& Archive, FPhase6State& State)
	{
		Archive << State.bInitialized;
		Archive << State.bFixtureCompleted;
		Archive << State.MasterSeed;
		Archive << State.RandomStreamState;
		Archive << State.NextDomainId;
		Archive << State.NextEventSequence;
		Archive << State.InitializedAtGameMilliseconds;
		Archive << State.LastUpdatedGameMilliseconds;
		Archive << State.CurrentOperatingDay;
		SerializeRecords(
			Archive,
			State.Runways,
			MaximumSmallRecords,
			SerializeRunway);
		SerializeRecords(
			Archive,
			State.Facilities,
			MaximumSmallRecords,
			SerializeFacility);
		SerializeRecords(
			Archive,
			State.LargeAircraftOperations,
			MaximumHistoryRecords,
			SerializeLargeAircraft);
		SerializeRecords(
			Archive,
			State.MajorOperations,
			MaximumHistoryRecords,
			SerializeMajorOperation);
		SerializeRecords(
			Archive,
			State.PathMetrics,
			MaximumSmallRecords,
			SerializePathMetric);
		SerializeIncident(Archive, State.SeriousIncident);
		SerializeRepair(Archive, State.RepairProject);
		SerializeScale(Archive, State.ScaleDiagnostics);
		SerializeRecords(
			Archive,
			State.Events,
			MaximumHistoryRecords,
			SerializeEvent);
		Archive << State.MajorPathCount;
		Archive << State.CompletedLargeAircraftCount;
		Archive << State.RecoveredSeriousIncidentCount;
		Archive << State.TotalMajorRevenueCredits;
		Archive << State.TotalRecoverySupportCredits;
	}
}
