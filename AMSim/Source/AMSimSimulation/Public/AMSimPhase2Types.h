#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
#define AMSIM_DECLARE_PHASE2_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_PHASE2_ID(FVehicleId);
	AMSIM_DECLARE_PHASE2_ID(FTenantId);
	AMSIM_DECLARE_PHASE2_ID(FParcelId);
	AMSIM_DECLARE_PHASE2_ID(FServiceTaskId);
	AMSIM_DECLARE_PHASE2_ID(FIncidentId);
	AMSIM_DECLARE_PHASE2_ID(FAchievementId);

#undef AMSIM_DECLARE_PHASE2_ID

	enum class EAirportSpecialization : uint8
	{
		Unselected,
		GeneralAviation,
		FlightSchool,
		Charter
	};

	enum class EPhase2CommandType : uint8
	{
		InitializeLivingAirport,
		SelectSpecialization,
		AcceptContract,
		CancelContract,
		RescheduleFlight,
		SetActiveRunway,
		AssignTeamZone,
		DispatchNextService,
		RequestTow,
		PurchaseParcel,
		StartExpansion,
		RespondToIncident,
		RequestRecovery
	};

	enum class EPhase2CommandResult : uint8
	{
		Accepted,
		RejectedInvalidCommand,
		RejectedPhase1NotReady,
		RejectedInvalidState,
		RejectedMissingReference,
		RejectedIncompatible,
		RejectedConflict,
		RejectedInsufficientCredits,
		RejectedNotReady
	};

	enum class EPhase2FlightState : uint8
	{
		Scheduled,
		Inbound,
		Approach,
		Landing,
		TaxiIn,
		Turnaround,
		ReadyToDepart,
		TaxiOut,
		Outbound,
		Completed,
		Cancelled
	};

	enum class EPhase2ServiceState : uint8
	{
		Blocked,
		Queued,
		Dispatched,
		InProgress,
		Completed
	};

	enum class EPhase2VehicleState : uint8
	{
		AtDepot,
		Dispatched,
		Working,
		Returning
	};

	enum class EWeatherCategory : uint8
	{
		Visual,
		Wet,
		LowCloud,
		ColdWet,
		StrongCrosswind
	};

	enum class EIncidentType : uint8
	{
		None,
		DisabledAircraft,
		FuelSpill,
		WeatherClosure
	};

	enum class EIncidentState : uint8
	{
		None,
		Reported,
		ResponseDispatched,
		Contained,
		Cleanup,
		Resolved
	};

	enum class EExpansionStage : uint8
	{
		None,
		Funded,
		Delivery,
		Building,
		Inspection,
		Operational
	};

	enum class EPhase2EventType : uint8
	{
		LivingAirportInitialized,
		SpecializationSelected,
		ContractAccepted,
		ContractCancelled,
		FlightCreated,
		FlightRescheduled,
		FlightStateChanged,
		ServiceStateChanged,
		VehicleStateChanged,
		TeamAssigned,
		RunwayChanged,
		TowCompleted,
		ParcelPurchased,
		ExpansionStageChanged,
		WeatherChanged,
		IncidentStateChanged,
		RatingChanged,
		AchievementEarned,
		EconomyApplied,
		RecoveryGranted
	};

	struct FPhase2Command
	{
		FCommandId Id;
		EPhase2CommandType Type = EPhase2CommandType::InitializeLivingAirport;
		EAirportSpecialization Specialization = EAirportSpecialization::Unselected;
		FContractId ContractId;
		FStaffTeamId TeamId;
		FName ZoneId;
		FName RunwayDirection;
		FFlightId FlightId;
		FParcelId ParcelId;
		FIncidentId IncidentId;
		int64 RequestedGameTimeMilliseconds = 0;
	};

	struct FPhase2Validation
	{
		bool bValid = false;
		EPhase2CommandResult Result = EPhase2CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Remedy;
		int64 QuotedCredits = 0;
	};

	struct FPhase2Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase2EventType Type = EPhase2EventType::LivingAirportInitialized;
		FCommandId Cause;
		uint64 SubjectId = 0;
		FString Message;
	};

	struct FPhase2AircraftRecord
	{
		FAircraftInstanceId Id;
		FName AircraftContentId;
		FName RoleId;
		FName OperatorContentId;
		FString TailNumber;
		int32 VisitCount = 0;
		int32 ServiceCount = 0;
		int64 LastVisitGameMilliseconds = 0;
		FString HistorySummary;
	};

	struct FPhase2ContractRecord
	{
		FContractId Id;
		FTenantId TenantId;
		EAirportSpecialization Specialization = EAirportSpecialization::Unselected;
		FName ContractContentId;
		FName AircraftRoleId;
		int32 FlightsPerOperatingDay = 1;
		int64 RewardPerFlightCredits = 0;
		int64 CancellationCostCredits = 0;
		bool bAccepted = false;
		FString CompatibilitySummary;
	};

	struct FPhase2FlightRecord
	{
		FFlightId Id;
		FContractId ContractId;
		FAircraftInstanceId AircraftId;
		EPhase2FlightState State = EPhase2FlightState::Scheduled;
		int32 OperatingDay = 0;
		int64 ScheduledArrivalGameMilliseconds = 0;
		int64 StateChangedAtGameMilliseconds = 0;
		int64 StandStartGameMilliseconds = 0;
		int64 StandEndGameMilliseconds = 0;
		FName AssignedRunwayDirection;
		FName AssignedStandId;
		bool bRequiresDeicing = false;
		bool bRewardRecognized = false;
		FString Blocker;
	};

	struct FPhase2MovementReservation
	{
		FName BlockId;
		FFlightId FlightId;
		int64 StartGameMilliseconds = 0;
		int64 EndGameMilliseconds = 0;
		bool bTow = false;
	};

	struct FPhase2ServiceTaskRecord
	{
		FServiceTaskId Id;
		FFlightId FlightId;
		FName ServiceId;
		FName OwnerDomain;
		uint64 OwnerId = 0;
		FName OperationId;
		int32 Quantity = 1;
		EPhase2ServiceState State = EPhase2ServiceState::Queued;
		FServiceTaskId PrerequisiteTaskId;
		FVehicleId AssignedVehicleId;
		FStaffTeamId AssignedTeamId;
		int64 StateChangedAtGameMilliseconds = 0;
		int64 DurationMilliseconds = 0;
		FString Blocker;
	};

	struct FPhase2VehicleRecord
	{
		FVehicleId Id;
		FName VehicleContentId;
		FName CapabilityId;
		FName HomeDepotId;
		EPhase2VehicleState State = EPhase2VehicleState::AtDepot;
		FServiceTaskId AssignedTaskId;
		int64 StateChangedAtGameMilliseconds = 0;
		int32 CompletedTaskCount = 0;
	};

	struct FPhase2StaffTeamRecord
	{
		FStaffTeamId Id;
		FName RoleId;
		FName ZoneId;
		int32 TeamSize = 0;
		int32 WorkloadPercent = 0;
		int32 MoralePercent = 100;
		bool bOnShift = true;
		FServiceTaskId AssignedTaskId;
	};

	struct FPhase2TenantRecord
	{
		FTenantId Id;
		FName TenantContentId;
		EAirportSpecialization Specialization = EAirportSpecialization::Unselected;
		FString DisplayName;
		FString Requirements;
		int32 EvidencePoints = 0;
		bool bActive = false;
	};

	struct FPhase2ParcelRecord
	{
		FParcelId Id;
		FName ParcelContentId;
		int64 PurchaseCostCredits = 0;
		bool bOwned = false;
	};

	struct FPhase2ExpansionProjectRecord
	{
		FConstructionProjectId Id;
		FParcelId ParcelId;
		EExpansionStage Stage = EExpansionStage::None;
		int64 FundedAtGameMilliseconds = 0;
		int64 StageChangedAtGameMilliseconds = 0;
		int64 QuotedCostCredits = 0;
		bool bDeliveryArrived = false;
		bool bInspectionPassed = false;
		FString ClosureSummary;
	};

	struct FPhase2WeatherRecord
	{
		int32 OperatingDay = 0;
		EWeatherCategory Category = EWeatherCategory::Visual;
		int32 WindDirectionDegrees = 240;
		int32 WindSpeedKnots = 5;
		int32 VisibilityMeters = 10000;
		int32 TemperatureCelsius = 16;
		FString OperationalSummary;
	};

	struct FPhase2IncidentRecord
	{
		FIncidentId Id;
		EIncidentType Type = EIncidentType::None;
		EIncidentState State = EIncidentState::None;
		int64 ReportedAtGameMilliseconds = 0;
		int64 StateChangedAtGameMilliseconds = 0;
		FStaffTeamId ResponseTeamId;
		FString Cause;
		FString Remedy;
		int64 CostCredits = 0;
	};

	struct FPhase2RatingRecord
	{
		FName ComponentId;
		int32 Value = 50;
		FString RecentCause;
		FString Remedy;
	};

	struct FPhase2AchievementRecord
	{
		FAchievementId Id;
		FName AchievementContentId;
		FString DisplayName;
		bool bEarned = false;
		int64 EarnedAtGameMilliseconds = 0;
	};

	struct FPhase2State
	{
		bool bInitialized = false;
		uint64 MasterSeed = 1;
		uint64 RandomStreamState = 1;
		uint64 NextDomainId = 1;
		uint64 NextEventSequence = 1;
		int64 InitializedAtGameMilliseconds = 0;
		int64 LastUpdatedGameMilliseconds = 0;
		int32 CurrentOperatingDay = 0;
		int32 LastEconomyOperatingDay = -1;
		EAirportSpecialization SelectedSpecialization = EAirportSpecialization::Unselected;
		FName ActiveRunwayDirection = TEXT("24");
		FString ActiveRunwayReason;
		bool bApproachAidReady = true;
		bool bAutomaticDispatch = true;
		TArray<FPhase2AircraftRecord> Aircraft;
		TArray<FPhase2ContractRecord> Contracts;
		TArray<FPhase2FlightRecord> Flights;
		TArray<FPhase2MovementReservation> Reservations;
		TArray<FPhase2ServiceTaskRecord> ServiceTasks;
		TArray<FPhase2VehicleRecord> Vehicles;
		TArray<FPhase2StaffTeamRecord> Teams;
		TArray<FPhase2TenantRecord> Tenants;
		TArray<FPhase2ParcelRecord> Parcels;
		FPhase2ExpansionProjectRecord Expansion;
		FPhase2WeatherRecord CurrentWeather;
		TArray<FPhase2WeatherRecord> Forecast;
		FPhase2IncidentRecord Incident;
		TArray<FPhase2RatingRecord> Ratings;
		TArray<FPhase2AchievementRecord> Achievements;
		TArray<FPhase2Event> Events;
		int32 CompletedFlightCount = 0;
		int32 RecoveryGrantCount = 0;
		int64 TotalPhase2RevenueCredits = 0;
		int64 TotalPhase2CostCredits = 0;
	};

	struct FPhase2QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bInitialized = false;
		int32 OperatingDay = 0;
		EAirportSpecialization Specialization = EAirportSpecialization::Unselected;
		int32 ActiveAircraftCount = 0;
		int32 ScheduledFlightCount = 0;
		int32 CompletedFlightCount = 0;
		int32 BusyVehicleCount = 0;
		int32 BusyTeamCount = 0;
		int32 AverageWorkloadPercent = 0;
		int32 AverageRating = 0;
		FName ActiveRunwayDirection;
		EWeatherCategory WeatherCategory = EWeatherCategory::Visual;
		EIncidentState IncidentState = EIncidentState::None;
		EExpansionStage ExpansionStage = EExpansionStage::None;
		FString PrimaryStatus;
		FString OperationsSummary;
		FString WeatherSummary;
		FString StaffSummary;
		FString TenantSummary;
		FString EconomySummary;
		FString Cause;
		FString Remedy;
		uint64 StateChecksum = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FVehicleId Id)
	{
		return ::GetTypeHash(Id.Value);
	}

	FORCEINLINE uint32 GetTypeHash(const FServiceTaskId Id)
	{
		return ::GetTypeHash(Id.Value);
	}
}
