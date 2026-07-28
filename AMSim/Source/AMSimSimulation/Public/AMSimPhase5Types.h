#pragma once

#include "AMSimPhase4Types.h"

namespace AMSim
{
#define AMSIM_DECLARE_PHASE5_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_PHASE5_ID(FCargoShipmentId);
	AMSIM_DECLARE_PHASE5_ID(FWarehouseZoneId);
	AMSIM_DECLARE_PHASE5_ID(FProviderTenantId);
	AMSIM_DECLARE_PHASE5_ID(FSpecialEventId);
	AMSIM_DECLARE_PHASE5_ID(FRatingContributionId);

#undef AMSIM_DECLARE_PHASE5_ID

	enum class ESpecializationPath : uint8
	{
		GeneralAviation,
		FlightSchool,
		Charter,
		Cargo,
		Passenger,
		Mixed
	};

	enum class ECapabilityBand : uint8
	{
		Unavailable,
		Established,
		Regional,
		Advanced,
		Major
	};

	enum class ECapabilityOperationalStatus : uint8
	{
		Healthy,
		Grace,
		Recovering,
		Suspended
	};

	enum class ECargoClass : uint8
	{
		GeneralFreight,
		MailExpress,
		TemperatureSensitive,
		Oversized
	};

	enum class ECargoFlow : uint8
	{
		Outbound,
		Inbound,
		Transfer
	};

	enum class ECargoState : uint8
	{
		Offered,
		RoadReceipt,
		CheckAndSecure,
		Storage,
		BuildUp,
		AirsideTransfer,
		Loaded,
		InFlight,
		Completed,
		Exception
	};

	enum class EWarehouseZoneType : uint8
	{
		General,
		Express,
		TemperatureControlled,
		OversizedPad
	};

	enum class EProviderTenantState : uint8
	{
		Offered,
		Active,
		Grace,
		Suspended
	};

	enum class ESpecialEventFamily : uint8
	{
		Airshow,
		GAFlyIn,
		FlightSchoolOpenDay,
		CharterVipMovement,
		CargoSurge,
		HolidayPassengerSurge,
		EmergencyExercise,
		NotableVisitingAircraft
	};

	enum class ESpecialEventState : uint8
	{
		Offered,
		Declined,
		Preparing,
		Active,
		PartialSuccess,
		Cleanup,
		Completed,
		Cooldown
	};

	enum class EPhase5CommandType : uint8
	{
		InitializeBreadth,
		AcceptNextCargoContract,
		AdvanceNextCargoShipment,
		AcceptNextProviderTenant,
		TriggerTenantRequirementLoss,
		RecoverTenant,
		AcceptNextEvent,
		DeclineNextEvent,
		AdvanceActiveEvent,
		ReplaceObjective
	};

	enum class EPhase5CommandResult : uint8
	{
		Accepted,
		RejectedInvalidCommand,
		RejectedPhase2NotReady,
		RejectedInvalidState,
		RejectedMissingReference,
		RejectedCapacity,
		RejectedIncompatible,
		RejectedNotReady
	};

	enum class EPhase5EventType : uint8
	{
		BreadthInitialized,
		CargoContractAccepted,
		CargoTaskRequested,
		CargoStateChanged,
		CargoCompleted,
		TenantAccepted,
		TenantGraceStarted,
		TenantRecovered,
		EventAccepted,
		EventDeclined,
		EventStateChanged,
		RatingChanged,
		CapabilityBandChanged,
		AchievementEarned,
		ObjectiveReplaced,
		EconomyApplied
	};

	struct FPhase5Command
	{
		FCommandId Id;
		EPhase5CommandType Type = EPhase5CommandType::InitializeBreadth;
		FCargoShipmentId ShipmentId;
		FProviderTenantId TenantId;
		FSpecialEventId EventId;
		int32 ObjectiveIndex = INDEX_NONE;
	};

	struct FPhase5Validation
	{
		bool bValid = false;
		EPhase5CommandResult Result =
			EPhase5CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Consequence;
		FString Remedy;
	};

	struct FPhase5CargoContractRecord
	{
		FContractId Id;
		FName ContentId;
		FName OperatorId;
		FString DisplayName;
		ECargoClass CargoClass = ECargoClass::GeneralFreight;
		ECargoFlow Flow = ECargoFlow::Outbound;
		FName AircraftContentId;
		int64 RewardCredits = 0;
		bool bAccepted = false;
	};

	struct FPhase5ShipmentRecord
	{
		FCargoShipmentId Id;
		FContractId ContractId;
		ECargoClass CargoClass = ECargoClass::GeneralFreight;
		ECargoFlow Flow = ECargoFlow::Outbound;
		ECargoState State = ECargoState::Offered;
		int32 FlowStep = 0;
		int32 Pieces = 0;
		int64 MassKilograms = 0;
		int64 VolumeLitres = 0;
		int64 DeadlineGameMilliseconds = 0;
		bool bSecurityCleared = false;
		bool bBellyFreight = false;
		FName AircraftContentId;
		FFlightId LinkedFlightId;
		FWarehouseZoneId WarehouseZoneId;
		FServiceTaskId ActiveServiceTaskId;
		FString CurrentLocation;
		FString ExceptionCause;
		FString Remedy;
		bool bRewardRecognized = false;
	};

	struct FPhase5WarehouseZoneRecord
	{
		FWarehouseZoneId Id;
		FName ContentId;
		EWarehouseZoneType Type = EWarehouseZoneType::General;
		int64 CapacityLitres = 0;
		int64 OccupiedLitres = 0;
		bool bRoadConnected = true;
		bool bAirsideConnected = true;
		bool bOperational = true;
	};

	struct FPhase5ProviderTenantRecord
	{
		FProviderTenantId Id;
		FName ContentId;
		FString DisplayName;
		FName ProviderType;
		EProviderTenantState State = EProviderTenantState::Offered;
		FName FootprintId;
		int64 OpeningCostCredits = 0;
		int64 RentCreditsPerOperatingDay = 0;
		int32 RevenueSharePercent = 0;
		int32 SatisfactionPercent = 75;
		int32 ContractTermOperatingDays = 0;
		int64 GraceEndsAtGameMilliseconds = 0;
		FName RequiredCapability;
		FString Requirement;
		FString ServiceExpectations;
		FString OperatingPattern;
		FString SatisfactionDrivers;
		FString Cause;
		FString Remedy;
	};

	struct FPhase5RatingContributionRecord
	{
		FRatingContributionId Id;
		FName ComponentId;
		FName SourceGroup;
		int32 Magnitude = 0;
		int64 AppliedAtGameMilliseconds = 0;
		FString Explanation;
	};

	struct FPhase5PathEvidenceRecord
	{
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		ECapabilityBand Band = ECapabilityBand::Unavailable;
		ECapabilityBand EarnedBand = ECapabilityBand::Unavailable;
		ECapabilityBand OperationalBand = ECapabilityBand::Unavailable;
		ECapabilityOperationalStatus OperationalStatus =
			ECapabilityOperationalStatus::Healthy;
		int32 AirportPoints = 0;
		int32 OperatingDays = 0;
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
		bool bSharedResourceDayCompleted = false;
		bool bMajorRequirementsMet = false;
		TArray<FName> FacilityIds;
		TArray<FName> MajorEvidenceIds;
		FString CurrentEvidence;
		FString NextRequirement;
	};

	struct FPhase5ObjectiveRecord
	{
		FObjectiveId Id;
		FName ContentId;
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		FString Text;
		int32 Current = 0;
		int32 Target = 1;
		int32 RewardAirportPoints = 0;
		bool bCompleted = false;
	};

	struct FPhase5AchievementRecord
	{
		FAchievementId Id;
		FName ContentId;
		ESpecializationPath Path = ESpecializationPath::GeneralAviation;
		FString DisplayName;
		bool bEarned = false;
		int64 EarnedAtGameMilliseconds = 0;
	};

	struct FPhase5SpecialEventRecord
	{
		FSpecialEventId Id;
		FName ContentId;
		FString DisplayName;
		ESpecialEventFamily Family = ESpecialEventFamily::Airshow;
		ESpecialEventState State = ESpecialEventState::Offered;
		int64 NoticeEndsAtGameMilliseconds = 0;
		int64 ActiveEndsAtGameMilliseconds = 0;
		int64 CooldownEndsAtGameMilliseconds = 0;
		int32 ExpectedDemand = 0;
		int32 SatisfiedDemand = 0;
		int64 RewardCredits = 0;
		FName IntegrationDomain;
		TArray<uint64> LinkedEntityIds;
		FString Preview;
		FString SuccessSummary;
		bool bRewardRecognized = false;
	};

	struct FPhase5Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase5EventType Type = EPhase5EventType::BreadthInitialized;
		FCommandId Cause;
		uint64 SubjectId = 0;
		FString Message;
	};

	struct FPhase5State
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
		TArray<FPhase5CargoContractRecord> CargoContracts;
		TArray<FPhase5ShipmentRecord> Shipments;
		TArray<FPhase5WarehouseZoneRecord> WarehouseZones;
		TArray<FPhase5ProviderTenantRecord> ProviderTenants;
		TArray<FPhase5RatingContributionRecord> RatingContributions;
		TArray<FPhase5PathEvidenceRecord> Paths;
		TArray<FPhase5ObjectiveRecord> Objectives;
		TArray<FPhase5AchievementRecord> Achievements;
		TArray<FPhase5SpecialEventRecord> SpecialEvents;
		TArray<FPhase5Event> Events;
		int32 CompletedShipmentCount = 0;
		int32 CompletedCargoClassCount = 0;
		int32 CompletedCargoFlowCount = 0;
		int32 AdvancedPathCount = 0;
		int32 MajorPathCount = 0;
		int32 OverallRating = 70;
		int64 TotalCargoRevenueCredits = 0;
		int64 TotalEventRevenueCredits = 0;
	};

	struct FPhase5QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bFixtureCompleted = false;
		int32 OperatingDay = 0;
		int32 AcceptedCargoContractCount = 0;
		int32 ShipmentCount = 0;
		int32 CompletedShipmentCount = 0;
		int32 ActiveShipmentCount = 0;
		int32 WarehouseUtilizationPercent = 0;
		int32 ActiveProviderCount = 0;
		int32 GraceProviderCount = 0;
		int32 AdvancedPathCount = 0;
		int32 MajorPathCount = 0;
		int32 OverallRating = 0;
		int32 ActiveEventCount = 0;
		FString PrimaryStatus;
		FString CargoSummary;
		FString SelectedShipment;
		FString WarehouseSummary;
		FString ProviderSummary;
		FString EventSummary;
		FString ProgressionSummary;
		FString Cause;
		FString Remedy;
		uint64 StateChecksum = 0;
	};
}
