#pragma once

#include "AMSimPhase5Types.h"

namespace AMSim
{
#define AMSIM_DECLARE_PHASE6_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_PHASE6_ID(FPhase6RunwayId);
	AMSIM_DECLARE_PHASE6_ID(FMajorFacilityId);
	AMSIM_DECLARE_PHASE6_ID(FLargeAircraftOperationId);
	AMSIM_DECLARE_PHASE6_ID(FMajorOperationId);
	AMSIM_DECLARE_PHASE6_ID(FSeriousIncidentId);
	AMSIM_DECLARE_PHASE6_ID(FRepairProjectId);

#undef AMSIM_DECLARE_PHASE6_ID

	enum class ERunwaySide : uint8
	{
		None,
		Left,
		Center,
		Right
	};

	enum class ERunwayUse : uint8
	{
		Closed,
		Available,
		Arrival,
		Departure,
		Mixed
	};

	enum class EMajorFacilityType : uint8
	{
		ParallelRunway,
		LargeStand,
		HangarCampus,
		InstructionCenter,
		ExecutiveFacility,
		CargoHub,
		TerminalConcourse,
		BaggageHall,
		ServiceDepot,
		EmergencyStation,
		GroundAccessHub,
		OperationsCenter
	};

	enum class EMajorFacilityState : uint8
	{
		Proposed,
		UnderConstruction,
		Operational,
		Closed
	};

	enum class ELargeAircraftOperationState : uint8
	{
		Offered,
		Scheduled,
		Arrived,
		Turnaround,
		DepartureReady,
		Departed,
		Completed,
		Blocked
	};

	enum class ESeriousIncidentLifecycle : uint8
	{
		None,
		WarningIssued,
		RiskAcknowledged,
		Materialized,
		Alerted,
		ResourcesDispatched,
		AreaProtected,
		Stabilized,
		Investigating,
		Repairing,
		Recovered
	};

	enum class EAbstractHumanOutcome : uint8
	{
		NotDetermined,
		NoInjuries,
		InjuriesReported,
		FatalitiesReported
	};

	enum class EPhase6CommandType : uint8
	{
		InitializeMajorCapability,
		ConstructFacility,
		ActivateParallelRunways,
		SetRunwayConfiguration,
		AcceptLargeAircraftOffer,
		StartLargeAircraftTurnaround,
		AdvanceLargeAircraftOperation,
		RecordMajorOperation,
		AdvanceMajorOperatingDay,
		ResolveSharedResourceConflict,
		AcknowledgeSeriousRisk,
		MaterializeSeriousIncident,
		DispatchEmergencyResponse,
		ProtectIncidentArea,
		StabilizeIncident,
		InvestigateIncident,
		BeginIncidentRepair,
		CompleteIncidentRepair,
		ApplyIncidentRecovery,
		ReopenIncidentArea
	};

	enum class EPhase6CommandResult : uint8
	{
		Accepted,
		AcceptedWithWarning,
		RejectedInvalidCommand,
		RejectedPhase5NotReady,
		RejectedInvalidState,
		RejectedMissingReference,
		RejectedCapacity,
		RejectedIncompatible,
		RejectedNotReady,
		RejectedInsufficientFunds,
		RejectedUnsafe
	};

	enum class EPhase6EventType : uint8
	{
		MajorCapabilityInitialized,
		FacilityConstructed,
		RunwayConfigurationChanged,
		ParallelRunwaysActivated,
		LargeAircraftAccepted,
		LargeAircraftStateChanged,
		LargeAircraftTaskRequested,
		MajorOperationCompleted,
		MajorOperatingDayCompleted,
		SharedResourceConflictResolved,
		MajorCapabilityChanged,
		IncidentWarningIssued,
		IncidentRiskAcknowledged,
		IncidentMaterialized,
		IncidentResponseChanged,
		IncidentReportCreated,
		RepairChanged,
		RecoveryApplied,
		AreaReopened,
		EconomyApplied
	};

	struct FPhase6Command
	{
		FCommandId Id;
		EPhase6CommandType Type =
			EPhase6CommandType::InitializeMajorCapability;
		FMajorFacilityId FacilityId;
		FPhase6RunwayId RunwayId;
		FName RunwayEndId;
		ERunwayUse RunwayUse = ERunwayUse::Available;
		FLargeAircraftOperationId LargeAircraftOperationId;
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		FName OperationKind;
		bool bConfirmWarning = false;
	};

	struct FPhase6Validation
	{
		bool bValid = false;
		bool bRequiresConfirmation = false;
		EPhase6CommandResult Result =
			EPhase6CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Consequence;
		FString Remedy;
	};

	struct FPhase6RunwayRecord
	{
		FPhase6RunwayId Id;
		FName ContentId;
		FString DisplayName;
		int32 MagneticHeadingDegrees = 0;
		int32 DeclaredLengthMeters = 0;
		int32 WidthMeters = 0;
		int32 ParallelOrder = 0;
		ERunwaySide Side = ERunwaySide::None;
		FName PrimaryEndId;
		FName ReciprocalEndId;
		ERunwayUse PrimaryUse = ERunwayUse::Available;
		ERunwayUse ReciprocalUse = ERunwayUse::Available;
		bool bPaved = false;
		bool bOperational = false;
		bool bPrimaryClosed = false;
		bool bReciprocalClosed = false;
		bool bPrimaryOccupied = false;
		bool bReciprocalOccupied = false;
		bool bApproachAidOperational = false;
		FString Recommendation;
		FString ClosureReason;
	};

	struct FMajorFacilityRecord
	{
		FMajorFacilityId Id;
		FName ContentId;
		FString DisplayName;
		EMajorFacilityType Type = EMajorFacilityType::ParallelRunway;
		EMajorFacilityState State = EMajorFacilityState::Proposed;
		ESpecializationPath OwningPath = ESpecializationPath::Mixed;
		int32 CapacityUnits = 0;
		int64 CostCredits = 0;
		int32 AirportPointReward = 0;
		bool bRoadConnected = false;
		bool bAirsideConnected = false;
		bool bPassengerConnected = false;
		bool bStaffed = false;
		bool bCookerVisibleDefinition = false;
		int64 FundedAtGameMilliseconds = 0;
		int64 CompletesAtGameMilliseconds = 0;
		FString Requirement;
		FString Cause;
		FString Remedy;
	};

	struct FLargeAircraftOperationRecord
	{
		FLargeAircraftOperationId Id;
		FName ContentId;
		FName AircraftContentId;
		FName OperatorId;
		FString OperatorDisplayName;
		FString FlightNumber;
		ELargeAircraftOperationState State =
			ELargeAircraftOperationState::Offered;
		FPhase6RunwayId RunwayId;
		FMajorFacilityId StandFacilityId;
		int32 PassengerCapacity = 290;
		int32 PassengerCount = 0;
		int32 AcceptedBagCount = 0;
		int32 LoadedBagCount = 0;
		int32 CompletedServiceTaskCount = 0;
		int32 RequiredServiceTaskCount = 0;
		int64 ScheduledArrivalGameMilliseconds = 0;
		int64 ScheduledDepartureGameMilliseconds = 0;
		TArray<FServiceTaskId> ServiceTaskIds;
		bool bPassengerReconciled = false;
		bool bBagReconciled = false;
		bool bRewardRecognized = false;
		FString Blocker;
	};

	struct FMajorOperationRecord
	{
		FMajorOperationId Id;
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		FName OperationKind;
		FName OwningDomain;
		uint64 OwningEntityId = 0;
		int64 CompletedAtGameMilliseconds = 0;
		bool bCompleted = false;
		bool bNightOperation = false;
		bool bShortNoticeOrVip = false;
		bool bSharedResourceUsed = false;
	};

	struct FPhase6PathMetricRecord
	{
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		int32 QualifyingOperatingDays = 0;
		int32 SafetyRating = 80;
		int32 ReliabilityRating = 80;
		int32 TenantRelationshipRating = 75;
		int32 CompletedOperations = 0;
		int32 DistinctRolesOrClasses = 0;
		int32 CompletedPassengers = 0;
		int32 DedicatedFreighterMovements = 0;
		int32 NightOperations = 0;
		int32 ShortNoticeOrVipMovements = 0;
		int32 SharedResourceDays = 0;
		int32 RegionalPathCount = 0;
		int32 AdvancedPathCount = 0;
		bool bPrimaryTenantActive = false;
		bool bSecondaryProviderActive = false;
		bool bSignatureFacilityOperational = false;
		bool bFlyInCompleted = false;
		bool bOpenDayCompleted = false;
		bool bWideBodyTurnaroundCompleted = false;
		bool bSharedConflictRecovered = false;
		bool bCriticalReadinessFailure = false;
		TArray<FName> FacilityIds;
		TArray<FName> EvidenceIds;
	};

	struct FSeriousIncidentRecord
	{
		FSeriousIncidentId Id;
		ESeriousIncidentLifecycle Lifecycle =
			ESeriousIncidentLifecycle::None;
		ESpecializationPath OwningPath =
			ESpecializationPath::GeneralAviation;
		FLargeAircraftOperationId LargeAircraftOperationId;
		FPhase6RunwayId AffectedRunwayId;
		uint64 OutcomeSeed = 0;
		EAbstractHumanOutcome HumanOutcome =
			EAbstractHumanOutcome::NotDetermined;
		FString RiskFactor;
		FString Warning;
		FString Remedy;
		FString AcknowledgedDecision;
		FString ResponseSummary;
		FString Report;
		FString Prevention;
		bool bWarningShown = false;
		bool bRiskAcknowledged = false;
		bool bAircraftLost = false;
		bool bAreaProtected = false;
		bool bOtherOperationsContinue = true;
		bool bRecoveryFundingApplied = false;
		bool bAreaReopened = false;
		int64 WarningIssuedAtGameMilliseconds = 0;
		int64 MaterializedAtGameMilliseconds = 0;
		int64 RecoveredAtGameMilliseconds = 0;
	};

	struct FRepairProjectRecord
	{
		FRepairProjectId Id;
		FSeriousIncidentId IncidentId;
		FName ContentId;
		int64 CostCredits = 0;
		int32 ProgressPercent = 0;
		bool bFunded = false;
		bool bCompleted = false;
		int64 StartedAtGameMilliseconds = 0;
		int64 CompletesAtGameMilliseconds = 0;
	};

	struct FPhase6Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase6EventType Type =
			EPhase6EventType::MajorCapabilityInitialized;
		FCommandId Cause;
		uint64 SubjectId = 0;
		FString Message;
	};

	struct FPhase6MajorPathSignal
	{
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		int32 QualifyingOperatingDays = 0;
		int32 SafetyRating = 0;
		int32 ReliabilityRating = 0;
		int32 TenantRelationshipRating = 0;
		int32 CompletedOperations = 0;
		int32 DistinctRolesOrClasses = 0;
		int32 CompletedPassengers = 0;
		int32 RegionalPathCount = 0;
		int32 AdvancedPathCount = 0;
		int32 SharedResourceDays = 0;
		bool bPrimaryTenantActive = false;
		bool bSecondaryProviderActive = false;
		bool bSignatureFacilityOperational = false;
		bool bMajorRequirementsMet = false;
		ECapabilityOperationalStatus OperationalStatus =
			ECapabilityOperationalStatus::Healthy;
		TArray<FName> FacilityIds;
		TArray<FName> EvidenceIds;
	};

	struct FPhase6CapabilitySignals
	{
		TArray<FPhase6MajorPathSignal> Paths;
		int32 MajorPathCount = 0;
	};

	struct FPhase6ScaleDiagnostics
	{
		int32 LogicalAgentCount = 0;
		int32 VisibleAgentCount = 0;
		int32 OnMapAircraftCount = 0;
		int32 VehicleCount = 0;
		int32 ActivePresentationProxyCount = 0;
		int32 PendingSimulationBuckets = 0;
		bool bBacklogWarning = false;
	};

	struct FPhase6State
	{
		bool bInitialized = false;
		bool bFixtureCompleted = false;
		uint64 MasterSeed = 1;
		uint64 RandomStreamState = 1;
		uint64 NextDomainId = 1;
		uint64 NextEventSequence = 1;
		int64 InitializedAtGameMilliseconds = 0;
		int64 LastUpdatedGameMilliseconds = 0;
		int32 CurrentOperatingDay = 0;
		TArray<FPhase6RunwayRecord> Runways;
		TArray<FMajorFacilityRecord> Facilities;
		TArray<FLargeAircraftOperationRecord> LargeAircraftOperations;
		TArray<FMajorOperationRecord> MajorOperations;
		TArray<FPhase6PathMetricRecord> PathMetrics;
		FSeriousIncidentRecord SeriousIncident;
		FRepairProjectRecord RepairProject;
		FPhase6ScaleDiagnostics ScaleDiagnostics;
		TArray<FPhase6Event> Events;
		int32 MajorPathCount = 0;
		int32 CompletedLargeAircraftCount = 0;
		int32 RecoveredSeriousIncidentCount = 0;
		int64 TotalMajorRevenueCredits = 0;
		int64 TotalRecoverySupportCredits = 0;
	};

	struct FPhase6QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bFixtureCompleted = false;
		int32 OperatingDay = 0;
		int32 OperationalRunwayCount = 0;
		int32 OperationalFacilityCount = 0;
		int32 MajorPathCount = 0;
		int32 LargeAircraftOperationCount = 0;
		int32 CompletedLargeAircraftCount = 0;
		ESeriousIncidentLifecycle IncidentLifecycle =
			ESeriousIncidentLifecycle::None;
		FPhase6ScaleDiagnostics ScaleDiagnostics;
		FString PrimaryStatus;
		FString RunwaySummary;
		FString CapacitySummary;
		FString LargeAircraftSummary;
		FString IncidentSummary;
		FString ProgressionSummary;
		FString Cause;
		FString Remedy;
		uint64 StateChecksum = 0;
	};
}
