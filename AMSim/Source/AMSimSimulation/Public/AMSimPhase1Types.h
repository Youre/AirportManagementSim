#pragma once

#include "AMSimSimulationTypes.h"

namespace AMSim
{
#define AMSIM_DECLARE_STRONG_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_STRONG_ID(FAirportId);
	AMSIM_DECLARE_STRONG_ID(FFacilityId);
	AMSIM_DECLARE_STRONG_ID(FNetworkNodeId);
	AMSIM_DECLARE_STRONG_ID(FNetworkEdgeId);
	AMSIM_DECLARE_STRONG_ID(FConstructionProjectId);
	AMSIM_DECLARE_STRONG_ID(FStaffTeamId);
	AMSIM_DECLARE_STRONG_ID(FAircraftInstanceId);
	AMSIM_DECLARE_STRONG_ID(FContractId);
	AMSIM_DECLARE_STRONG_ID(FFlightId);
	AMSIM_DECLARE_STRONG_ID(FTransactionId);
	AMSIM_DECLARE_STRONG_ID(FObjectiveId);
	AMSIM_DECLARE_STRONG_ID(FPhraseIntentId);

#undef AMSIM_DECLARE_STRONG_ID

	enum class EPhase1CommandType : uint8
	{
		CreateAirport,
		CommitStarterPlan,
		CancelStarterPlan,
		OpenAirport,
		CloseAirport,
		PinStarterOffer,
		DeclineStarterOffer,
		AcceptStarterOffer,
		ScheduleStarterFlight,
		RequestRecovery,
		SetPaused,
		SetSpeed,
		AcknowledgeContextHelp
	};

	enum class EPhase1CommandResult : uint8
	{
		Accepted,
		RejectedInvalidCommand,
		RejectedInvalidState,
		RejectedInvalidName,
		RejectedInvalidGeometry,
		RejectedOutsideOwnedLand,
		RejectedDisconnected,
		RejectedInsufficientCredits,
		RejectedNotReady,
		RejectedIncompatible,
		RejectedInvalidSchedule
	};

	enum class EConstructionStage : uint8
	{
		None,
		Funded,
		AwaitingDelivery,
		Building,
		Inspection,
		ReadyToOpen,
		Operational
	};

	enum class EFacilityType : uint8
	{
		GrassRunway,
		GrassTaxiway,
		GAStand,
		AccessConnection,
		OperationsHut,
		MarkingsAndWindsock
	};

	enum class EOfferState : uint8
	{
		Unavailable,
		Available,
		Declined,
		Accepted,
		Scheduled,
		Completed
	};

	enum class EFlightState : uint8
	{
		None,
		Scheduled,
		Inbound,
		Approach,
		Landing,
		RunwayRoll,
		TaxiIn,
		Parked,
		Turnaround,
		Ready,
		TaxiOut,
		Takeoff,
		Outbound,
		Completed
	};

	enum class EServiceTaskState : uint8
	{
		Unavailable,
		Waiting,
		Active,
		Completed
	};

	enum class EPhase1EventType : uint8
	{
		AirportCreated,
		ProjectCommitted,
		ProjectCancelled,
		ProjectStageChanged,
		FacilityOpened,
		FacilityClosed,
		OfferAvailable,
		OfferPinned,
		OfferDeclined,
		OfferAccepted,
		FlightScheduled,
		FlightStateChanged,
		ServiceStateChanged,
		TransactionRecorded,
		CapabilityAwarded,
		RecoveryGranted,
		PhraseQueued,
		ContextHelpAcknowledged
	};

	struct FPhase1Point
	{
		int64 X = 0;
		int64 Y = 0;
		auto operator<=>(const FPhase1Point&) const = default;
	};

	struct FTaxiwaySegment
	{
		FPhase1Point Start;
		FPhase1Point End;
		auto operator<=>(const FTaxiwaySegment&) const = default;
	};

	struct FStarterPlanProposal
	{
		FPhase1Point RunwayStart;
		FPhase1Point RunwayEnd;
		int32 RunwayWidthCentimeters = 2000;
		FPhase1Point TaxiStart;
		FPhase1Point TaxiEnd;
		FPhase1Point StandCenter;
		FPhase1Point AccessStart;
		FPhase1Point AccessEnd;
		FPhase1Point OperationsHutCenter;
		TArray<FTaxiwaySegment> TaxiwaySegments;
	};

	struct FPhase1Validation
	{
		bool bValid = false;
		EPhase1CommandResult Result = EPhase1CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Remedy;
		int64 QuotedCost = 0;
	};

	struct FPhase1Command
	{
		FCommandId Id;
		EPhase1CommandType Type = EPhase1CommandType::CreateAirport;
		FString AirportName;
		FName MapId;
		FStarterPlanProposal Proposal;
		int64 ScheduledArrivalGameMilliseconds = 0;
		FName RequestedStandDefinitionId;
		int32 SpeedMultiplier = 0;
		bool bPaused = true;
		FName ContextHelpId;
	};

	struct FPhase1Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase1EventType Type = EPhase1EventType::AirportCreated;
		FCommandId Cause;
		FString Message;
	};

	struct FFacilityRecord
	{
		FFacilityId Id;
		FName DefinitionId;
		EFacilityType Type = EFacilityType::GrassRunway;
		bool bBuilt = false;
		bool bOpen = false;
		FString Status;
	};

	struct FConstructionProjectRecord
	{
		FConstructionProjectId Id;
		EConstructionStage Stage = EConstructionStage::None;
		FStarterPlanProposal Proposal;
		int64 QuotedCost = 0;
		int64 FundedAtGameMilliseconds = 0;
		int64 StageChangedAtGameMilliseconds = 0;
		bool bDeliveryArrived = false;
		bool bInspectionPassed = false;
	};

	struct FStaffTeamRecord
	{
		FStaffTeamId Id;
		FName RoleId;
		int32 TeamSize = 0;
		bool bAvailable = false;
		FString CurrentTask;
	};

	struct FOfferRecord
	{
		FContractId ContractId;
		EOfferState State = EOfferState::Unavailable;
		FName AircraftContentId;
		FName OperatorContentId;
		FString CompatibilitySummary;
		int64 RewardCredits = 0;
		bool bPinned = false;
	};

	struct FAirframeRecord
	{
		FAircraftInstanceId Id;
		FName AircraftContentId;
		FName OperatorContentId;
		FString TailNumber;
		int32 VisitCount = 0;
		FString HistorySummary;
	};

	struct FFlightRecord
	{
		FFlightId Id;
		FContractId ContractId;
		FAircraftInstanceId AirframeId;
		EFlightState State = EFlightState::None;
		int64 ScheduledArrivalGameMilliseconds = 0;
		int64 StandOccupancyStartGameMilliseconds = 0;
		int64 StandOccupancyEndGameMilliseconds = 0;
		int64 StateChangedAtGameMilliseconds = 0;
		FFacilityId AssignedRunway;
		FFacilityId AssignedStand;
		EServiceTaskState Inspection = EServiceTaskState::Unavailable;
		EServiceTaskState Fueling = EServiceTaskState::Unavailable;
		FString Blocker;
		bool bRewardRecognized = false;
	};

	struct FMovementReservation
	{
		FName BlockId;
		FAircraftInstanceId Owner;
		int64 StartGameMilliseconds = 0;
		int64 EndGameMilliseconds = 0;
	};

	struct FTransactionRecord
	{
		FTransactionId Id;
		int64 GameTimeMilliseconds = 0;
		FName Category;
		int64 AmountCredits = 0;
		FString Explanation;
	};

	struct FRatingContribution
	{
		FName Component;
		int32 Value = 0;
		FString Reason;
	};

	struct FObjectiveRecord
	{
		FObjectiveId Id;
		FName ObjectiveId;
		FString Text;
		bool bCompleted = false;
	};

	struct FPhraseIntentRecord
	{
		FPhraseIntentId Id;
		int64 GameTimeMilliseconds = 0;
		FName PhraseId;
		FString Speaker;
		FString CallSign;
		FString Caption;
		int32 Priority = 0;
		FString DeduplicationKey;
	};

	struct FPhase1State
	{
		bool bInitialized = false;
		bool bPaused = true;
		bool bAirportOpen = false;
		FAirportId AirportId;
		FName MapId;
		FString AirportName;
		uint64 MasterSeed = 1;
		uint64 RandomStreamState = 1;
		int32 SpeedMultiplier = 0;
		int64 Credits = 5000;
		int32 AirportPoints = 0;
		int64 LastUpdatedGameMilliseconds = 0;
		uint64 NextDomainId = 1;
		uint64 NextPhase1EventSequence = 1;
		FConstructionProjectRecord Project;
		TArray<FFacilityRecord> Facilities;
		TArray<FStaffTeamRecord> Teams;
		FOfferRecord Offer;
		FAirframeRecord Airframe;
		FFlightRecord Flight;
		TArray<FMovementReservation> Reservations;
		TArray<FTransactionRecord> Transactions;
		TArray<FRatingContribution> RatingContributions;
		TArray<FObjectiveRecord> Objectives;
		TArray<FPhraseIntentRecord> PhraseIntents;
		TArray<FPhase1Event> Events;
		TArray<FName> AcknowledgedContextHelp;
		int32 RecoveryGrantCount = 0;
	};

	struct FPhase1QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bInitialized = false;
		bool bPaused = true;
		bool bAirportOpen = false;
		FString AirportName;
		FString MapDisplayName;
		int64 Credits = 0;
		int32 AirportPoints = 0;
		EConstructionStage ConstructionStage = EConstructionStage::None;
		EOfferState OfferState = EOfferState::Unavailable;
		EFlightState FlightState = EFlightState::None;
		EServiceTaskState InspectionState = EServiceTaskState::Unavailable;
		EServiceTaskState FuelingState = EServiceTaskState::Unavailable;
		FString PrimaryStatus;
		FString Cause;
		FString Remedy;
		FString CurrentObjective;
		FString LatestCaption;
		FString CompatibilitySummary;
		uint64 StateChecksum = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FFacilityId Id)
	{
		return ::GetTypeHash(Id.Value);
	}

	FORCEINLINE uint32 GetTypeHash(const FAircraftInstanceId Id)
	{
		return ::GetTypeHash(Id.Value);
	}
}
