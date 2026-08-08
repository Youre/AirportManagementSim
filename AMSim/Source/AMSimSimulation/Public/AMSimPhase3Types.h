#pragma once

#include "AMSimTerminalLayoutTypes.h"

namespace AMSim
{
#define AMSIM_DECLARE_PHASE3_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_PHASE3_ID(FPassengerId);
	AMSIM_DECLARE_PHASE3_ID(FPassengerPartyId);
	AMSIM_DECLARE_PHASE3_ID(FBagId);
	AMSIM_DECLARE_PHASE3_ID(FTerminalRouteId);
	AMSIM_DECLARE_PHASE3_ID(FCheckpointId);

#undef AMSIM_DECLARE_PHASE3_ID

	enum class EPhase3CommandType : uint8
	{
		InitializePassengerAirport,
		FundTerminal,
		ConnectNextNetwork,
		OpenTerminal,
		SchedulePassengerService,
		ToggleSecurityLane,
		RequestPassengerAssistance,
		ResolveBaggageException,
		PlaceTerminalFloor,
		PlaceTerminalWall,
		PlaceTerminalDoor,
		PlaceTerminalObject,
		RotateTerminalObject,
		DemolishTerminalElement,
		UndoTerminalEdit
	};

	enum class EPhase3CommandResult : uint8
	{
		Accepted,
		RejectedInvalidCommand,
		RejectedPhase2NotReady,
		RejectedInvalidState,
		RejectedMissingReference,
		RejectedInsufficientCredits,
		RejectedDisconnected,
		RejectedSecurityBypass,
		RejectedNoAccessibleRoute,
		RejectedNotReady,
		RejectedOccupied,
		RejectedInvalidTopology,
		RejectedRouteLoss,
		RejectedNothingToUndo
	};

	enum class ETerminalConstructionStage : uint8
	{
		None,
		Funded,
		Building,
		Inspection,
		ShellReady,
		Operational
	};

	enum class ETerminalZone : uint8
	{
		Outside,
		Landside,
		SterileDepartures,
		SterileArrivals,
		RestrictedBaggage
	};

	enum class ETerminalRouteKind : uint8
	{
		LandsideRoad,
		EntranceToCheckIn,
		SecurityControlled,
		SterileGate,
		ArrivalsToCurb,
		BaggageOutbound,
		BaggageArrival
	};

	enum class EPassengerDirection : uint8
	{
		Departing,
		Arriving
	};

	enum class EPassengerJourneyState : uint8
	{
		ApproachingAirport,
		LandsideEntry,
		CheckInBagDrop,
		SecurityQueue,
		Screening,
		GateArea,
		Boarding,
		OnAircraft,
		ArrivalsCorridor,
		BaggageReclaim,
		GroundTransport,
		Completed,
		WaitingForRoute
	};

	enum class ESecurityClearanceState : uint8
	{
		NotRequired,
		AwaitingScreening,
		Cleared,
		SecondaryCleared
	};

	enum class EBagJourneyState : uint8
	{
		Accepted,
		Conveyor,
		Screened,
		Sorted,
		MakeUp,
		OnAircraft,
		ArrivalInfeed,
		Reclaim,
		Collected,
		Exception
	};

	enum class ELandsideMode : uint8
	{
		PrivateCar,
		Taxi,
		PublicBus
	};

	enum class EPhase3FlightState : uint8
	{
		Unscheduled,
		Scheduled,
		AtGate,
		Boarding,
		ReadyToDepart,
		Outbound,
		Completed
	};

	enum class EPhase3EventType : uint8
	{
		PassengerAirportInitialized,
		TerminalStageChanged,
		NetworkConnected,
		TerminalOpened,
		PassengerServiceScheduled,
		PassengerStateChanged,
		BagStateChanged,
		SecurityLaneChanged,
		PassengerAssistanceAssigned,
		ReconciliationPassed,
		PassengerFlightCompleted,
		EconomyApplied,
		TerminalEditCommitted,
		TerminalEditUndone,
		TerminalConstructionStageChanged
	};

	struct FPhase3Command
	{
		FCommandId Id;
		EPhase3CommandType Type = EPhase3CommandType::InitializePassengerAirport;
		FPassengerId PassengerId;
		FBagId BagId;
		FTerminalElementId TerminalElementId;
		FTerminalCellCoord StartCell;
		FTerminalCellCoord EndCell;
		ETerminalFloorKind FloorKind = ETerminalFloorKind::Public;
		ETerminalEdgeKind EdgeKind = ETerminalEdgeKind::InteriorWall;
		ETerminalObjectKind ObjectKind = ETerminalObjectKind::SeatGroup2;
		int32 QuarterTurns = 0;
	};

	struct FPhase3Validation
	{
		bool bValid = false;
		EPhase3CommandResult Result = EPhase3CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Remedy;
		int64 QuotedCredits = 0;
	};

	struct FPhase3Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase3EventType Type = EPhase3EventType::PassengerAirportInitialized;
		FCommandId Cause;
		uint64 SubjectId = 0;
		FString Message;
	};

	struct FTerminalRoomRecord
	{
		FFacilityId Id;
		FName DefinitionId;
		FString DisplayName;
		ETerminalZone Zone = ETerminalZone::Landside;
		int32 Capacity = 0;
		bool bBuilt = false;
		bool bOpen = false;
		bool bAccessible = true;
	};

	struct FTerminalRouteRecord
	{
		FTerminalRouteId Id;
		ETerminalRouteKind Kind = ETerminalRouteKind::LandsideRoad;
		FName DefinitionId;
		ETerminalZone FromZone = ETerminalZone::Outside;
		ETerminalZone ToZone = ETerminalZone::Landside;
		bool bConnected = false;
		bool bControlledTransition = false;
		bool bAccessible = true;
		bool bIntroducesSecurityBypass = false;
		int32 Capacity = 0;
	};

	struct FSecurityCheckpointRecord
	{
		FCheckpointId Id;
		FName DefinitionId;
		FStaffTeamId TeamId;
		bool bOpen = true;
		bool bAccessibleLaneOpen = true;
		int32 ThroughputPerBucket = 12;
		int32 QueueCapacity = 48;
		int32 ProcessedCount = 0;
		int32 SecondaryCount = 0;
	};

	struct FPassengerPartyRecord
	{
		FPassengerPartyId Id;
		FString DisplayName;
		TArray<FPassengerId> Members;
		ELandsideMode ArrivalMode = ELandsideMode::PrivateCar;
		bool bRequiresAccessibleRoute = false;
		bool bAssistanceAssigned = false;
		int32 TimeConfidencePercent = 100;
		FString CompactNeeds;
	};

	struct FPassengerRecord
	{
		FPassengerId Id;
		FPassengerPartyId PartyId;
		FFlightId FlightId;
		FString DisplayName;
		FName AgeBand;
		EPassengerDirection Direction = EPassengerDirection::Departing;
		EPassengerJourneyState JourneyState =
			EPassengerJourneyState::ApproachingAirport;
		ESecurityClearanceState SecurityState =
			ESecurityClearanceState::AwaitingScreening;
		ELandsideMode LandsideMode = ELandsideMode::PrivateCar;
		int32 BagCount = 0;
		int32 PatiencePercent = 100;
		int32 TimeConfidencePercent = 100;
		bool bRequiresAccessibleRoute = false;
		bool bUsedAccessibleRoute = false;
		int64 StateChangedAtGameMilliseconds = 0;
		FString Blocker;
	};

	struct FBagRecord
	{
		FBagId Id;
		FPassengerId PassengerId;
		FFlightId FlightId;
		EPassengerDirection Direction = EPassengerDirection::Departing;
		EBagJourneyState JourneyState = EBagJourneyState::Accepted;
		bool bScreened = false;
		bool bReconciled = false;
		int64 StateChangedAtGameMilliseconds = 0;
		FString ExceptionReason;
	};

	struct FLandsideAccessRecord
	{
		ELandsideMode Mode = ELandsideMode::PrivateCar;
		FName FacilityId;
		int32 Capacity = 0;
		int32 ArrivedPassengerCount = 0;
		int32 DepartedPassengerCount = 0;
		bool bRouteConnected = false;
		bool bOpen = true;
	};

	struct FPhase3StaffTeamRecord
	{
		FStaffTeamId Id;
		FName RoleId;
		FName ZoneId;
		int32 TeamSize = 0;
		int32 WorkloadPercent = 0;
		bool bOnShift = true;
	};

	struct FPassengerTenantRecord
	{
		FTenantId Id;
		FName TenantContentId;
		FString DisplayName;
		FString Requirements;
		bool bActive = false;
		int32 SatisfactionPercent = 70;
	};

	struct FPassengerFlightRecord
	{
		FFlightId Id;
		FName FlightCode;
		FName OperatorContentId;
		FName AircraftContentId;
		FName GateId;
		EPhase3FlightState State = EPhase3FlightState::Unscheduled;
		int64 ScheduledAtGameMilliseconds = 0;
		int64 StateChangedAtGameMilliseconds = 0;
		int32 DepartingPassengerCount = 0;
		int32 ArrivingPassengerCount = 0;
		int32 AcceptedDepartureBagCount = 0;
		int32 ArrivalBagCount = 0;
		bool bPassengerReconciled = false;
		bool bBagReconciled = false;
		bool bRewardRecognized = false;
		FString Blocker;
	};

	struct FPhase3State
	{
		bool bInitialized = false;
		uint64 MasterSeed = 1;
		uint64 RandomStreamState = 1;
		uint64 NextDomainId = 1;
		uint64 NextEventSequence = 1;
		int64 InitializedAtGameMilliseconds = 0;
		int64 LastUpdatedGameMilliseconds = 0;
		ETerminalConstructionStage TerminalStage =
			ETerminalConstructionStage::None;
		int64 TerminalStageChangedAtGameMilliseconds = 0;
		int64 TerminalCostCredits = 0;
		bool bTerminalOpen = false;
		bool bSecurityIntegrityValid = false;
		bool bAccessibleRouteValid = false;
		TArray<FTerminalRoomRecord> Rooms;
		TArray<FTerminalRouteRecord> Routes;
		FSecurityCheckpointRecord Checkpoint;
		TArray<FPassengerPartyRecord> Parties;
		TArray<FPassengerRecord> Passengers;
		TArray<FBagRecord> Bags;
		TArray<FLandsideAccessRecord> Landside;
		TArray<FPhase3StaffTeamRecord> Teams;
		FPassengerTenantRecord Tenant;
		FPassengerFlightRecord Flight;
		FTerminalLayoutState TerminalLayout;
		TArray<FPhase3Event> Events;
		int32 CompletedPassengerCount = 0;
		int32 CompletedBagCount = 0;
		int32 ReconciliationPassCount = 0;
		int64 TotalPassengerRevenueCredits = 0;
	};

	struct FPhase3QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bTerminalOpen = false;
		ETerminalConstructionStage TerminalStage =
			ETerminalConstructionStage::None;
		EPhase3FlightState FlightState = EPhase3FlightState::Unscheduled;
		int32 RequiredConnectionCount = 0;
		int32 ConnectedCount = 0;
		int32 PassengerCount = 0;
		int32 DepartingPassengerCount = 0;
		int32 ArrivingPassengerCount = 0;
		int32 CompletedPassengerCount = 0;
		int32 BagCount = 0;
		int32 CompletedBagCount = 0;
		int32 SecurityQueueCount = 0;
		int32 BoardedCount = 0;
		int32 ReclaimCount = 0;
		int32 LandsideExitCount = 0;
		int32 VisiblePassengerTarget = 0;
		bool bSecurityIntegrityValid = false;
		bool bAccessibleRouteValid = false;
		bool bPassengerReconciled = false;
		bool bBagReconciled = false;
		FString PrimaryStatus;
		FString Cause;
		FString Remedy;
		FString FlightCode;
		FString FlightStatus;
		FString FeaturedPartyName;
		int32 FeaturedPartyMembers = 0;
		FString FeaturedStep;
		FString FeaturedNeeds;
		FString FeaturedRoute;
		int32 FeaturedTimeConfidencePercent = 0;
		bool bFeaturedAccessible = false;
		FString BaggageSummary;
		FString LandsideSummary;
		FTerminalLayoutQuerySnapshot TerminalLayout;
		uint64 StateChecksum = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FPassengerId Id)
	{
		return ::GetTypeHash(Id.Value);
	}

	FORCEINLINE uint32 GetTypeHash(const FBagId Id)
	{
		return ::GetTypeHash(Id.Value);
	}
}
