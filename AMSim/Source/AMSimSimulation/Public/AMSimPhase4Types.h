#pragma once

#include "AMSimPhase3Types.h"

namespace AMSim
{
#define AMSIM_DECLARE_PHASE4_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_PHASE4_ID(FConnectionId);

#undef AMSIM_DECLARE_PHASE4_ID

	enum class EPhase4CommandType : uint8
	{
		InitializeRegionalAirport,
		AcceptNextRecurringContract,
		PublishSevenDayTimetable,
		ReassignDisruptedFlight,
		ConfirmHighRiskGateChange,
		HoldAffectedDepartures,
		DivertAffectedArrivals,
		ProtectIncidentArea,
		DeployTowTeam,
		ReviewIncidentCause,
		ApplyRecoveryPlan,
		AcceptTenantRenewal
	};

	enum class EPhase4CommandResult : uint8
	{
		Accepted,
		AcceptedWithWarning,
		RejectedInvalidCommand,
		RejectedPhase3NotReady,
		RejectedInvalidState,
		RejectedMissingReference,
		RejectedScheduleBlocked,
		RejectedLockedHorizon,
		RejectedGateConflict,
		RejectedRemoteBusUnavailable,
		RejectedNotReady
	};

	enum class EPhase4Feasibility : uint8
	{
		Available,
		Advisory,
		HighRisk,
		Blocked
	};

	enum class EPhase4BoardingMode : uint8
	{
		Contact,
		RemoteBus
	};

	enum class EPhase4FlightState : uint8
	{
		Planned,
		Locked,
		Early,
		OnTime,
		Late,
		Holding,
		Diverted,
		AtGate,
		Completed,
		Cancelled
	};

	enum class EPhase4ConnectionState : uint8
	{
		Planned,
		Protected,
		Connected,
		Missed,
		Rebooked,
		Completed
	};

	enum class EPhase4TransferBagState : uint8
	{
		Accepted,
		InboundUnload,
		TransferSort,
		OutboundMakeUp,
		Rebooked,
		Loaded,
		Completed
	};

	enum class EPhase4BorderState : uint8
	{
		NotRequired,
		AwaitingImmigration,
		ImmigrationComplete,
		CustomsComplete
	};

	enum class EPhase4TransportMode : uint8
	{
		RentalCar,
		Rail
	};

	enum class EPhase4WeatherCategory : uint8
	{
		Clear,
		Rain,
		LowVisibility,
		StrongWind
	};

	enum class EPhase4IncidentLifecycle : uint8
	{
		None,
		Warned,
		Alerted,
		ResourcesDispatched,
		AreaProtected,
		Stabilized,
		Investigating,
		Reported,
		Recovered
	};

	enum class EPhase4RenewalState : uint8
	{
		Monitoring,
		Offered,
		Accepted
	};

	enum class EPhase4EventType : uint8
	{
		RegionalAirportInitialized,
		RecurringContractAccepted,
		TimetablePublished,
		FlightActualized,
		GateChangeWarningIssued,
		GateChanged,
		ConnectionUpdated,
		TransferBagUpdated,
		BorderProcessingUpdated,
		WeatherRestrictionIssued,
		IncidentWarningIssued,
		IncidentLifecycleChanged,
		IncidentReportCreated,
		RecoveryApplied,
		TenantRenewalOffered,
		TenantRenewed,
		OperatingDayCompleted,
		WeekCompleted,
		EconomyApplied
	};

	struct FPhase4Command
	{
		FCommandId Id;
		EPhase4CommandType Type =
			EPhase4CommandType::InitializeRegionalAirport;
		FFlightId FlightId;
		FName RequestedGateId;
		bool bConfirmWarning = false;
	};

	struct FPhase4Validation
	{
		bool bValid = false;
		bool bRequiresConfirmation = false;
		EPhase4CommandResult Result =
			EPhase4CommandResult::RejectedInvalidCommand;
		FName ReasonCode;
		FString Cause;
		FString Consequence;
		FString Remedy;
	};

	struct FPhase4Event
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		EPhase4EventType Type =
			EPhase4EventType::RegionalAirportInitialized;
		FCommandId Cause;
		uint64 SubjectId = 0;
		FString Message;
	};

	struct FPhase4ContractRecord
	{
		FContractId Id;
		FName ContentId;
		FName OperatorId;
		FString OperatorDisplayName;
		FName AircraftId;
		FString AircraftDisplayName;
		int32 FrequencyPerWeek = 0;
		int32 PassengerCapacity = 0;
		int64 RewardPerCompletedFlightCredits = 0;
		bool bInternational = false;
		bool bAccepted = false;
		int32 CompletedFlights = 0;
		int32 ControllableDelayMinutes = 0;
		int32 SatisfactionPercent = 75;
	};

	struct FPhase4GateRecord
	{
		FName GateId;
		FString DisplayName;
		EPhase4BoardingMode BoardingMode = EPhase4BoardingMode::Contact;
		FName SizeClass;
		bool bBusAvailable = false;
		bool bBorderRouteAvailable = false;
		bool bOperational = true;
	};

	struct FPhase4FlightRecord
	{
		FFlightId Id;
		FContractId ContractId;
		FName FlightCode;
		FName OperatorId;
		FString OperatorDisplayName;
		FName AircraftId;
		FString AircraftDisplayName;
		FName OriginRegion;
		FName DestinationRegion;
		int32 DayIndex = 0;
		int32 PlannedArrivalMinute = 0;
		int32 PlannedDepartureMinute = 0;
		int32 ActualArrivalOffsetMinutes = 0;
		int32 ActualDepartureOffsetMinutes = 0;
		FName PlannedGateId;
		FName AssignedGateId;
		EPhase4BoardingMode BoardingMode = EPhase4BoardingMode::Contact;
		EPhase4Feasibility Feasibility = EPhase4Feasibility::Available;
		EPhase4FlightState State = EPhase4FlightState::Planned;
		int32 PassengerCount = 0;
		int32 TransferPassengerCount = 0;
		int32 TransferBagCount = 0;
		bool bInternational = false;
		bool bWeatherRestricted = false;
		bool bGateChanged = false;
		bool bOverrideRecorded = false;
		bool bCompleted = false;
		FString RiskReason;
	};

	struct FPhase4ConnectionRecord
	{
		FConnectionId Id;
		FFlightId InboundFlightId;
		FFlightId OutboundFlightId;
		int32 PassengerCount = 0;
		int32 BagCount = 0;
		int32 MinimumConnectionMinutes = 30;
		int32 AvailableConnectionMinutes = 0;
		EPhase4ConnectionState State = EPhase4ConnectionState::Planned;
		FString Cause;
		FString Remedy;
	};

	struct FPhase4TransferBagRecord
	{
		FBagId Id;
		FConnectionId ConnectionId;
		FFlightId InboundFlightId;
		FFlightId OutboundFlightId;
		EPhase4TransferBagState State = EPhase4TransferBagState::Accepted;
		bool bScreened = true;
		bool bReconciled = false;
	};

	struct FPhase4BorderRecord
	{
		FFlightId FlightId;
		int32 PassengerCount = 0;
		int32 ImmigrationProcessed = 0;
		int32 CustomsProcessed = 0;
		int32 SecondaryCount = 0;
		EPhase4BorderState State = EPhase4BorderState::NotRequired;
		bool bControlledRouteValid = false;
		bool bAccessibleLaneOpen = true;
	};

	struct FPhase4TransportRecord
	{
		EPhase4TransportMode Mode = EPhase4TransportMode::RentalCar;
		FName FacilityId;
		FName TenantId;
		int32 Capacity = 0;
		int32 ServiceFrequencyMinutes = 0;
		int32 PassengerCount = 0;
		bool bOpen = true;
		bool bRouteConnected = true;
	};

	struct FPhase4ForecastRecord
	{
		int32 DayIndex = 0;
		int32 HourOffset = 0;
		EPhase4WeatherCategory Category =
			EPhase4WeatherCategory::Clear;
		int32 WindDirectionDegrees = 0;
		int32 WindSpeedKnots = 0;
		int32 VisibilityMeters = 10000;
		int32 ConfidencePercent = 100;
		FName RunwaySurface;
		FName RecommendedRunway;
	};

	struct FPhase4ApproachLimitRecord
	{
		FName AircraftId;
		int32 MinimumVisibilityMeters = 0;
		int32 MaximumCrosswindKnots = 0;
		FName RequiredApproach;
	};

	struct FPhase4RenewalRecord
	{
		FTenantId TenantId;
		FName OperatorId;
		FString OperatorDisplayName;
		EPhase4RenewalState State = EPhase4RenewalState::Monitoring;
		int32 SatisfactionPercent = 75;
		FString Evidence;
		FString RecoveryOffer;
	};

	struct FPhase4IncidentRecord
	{
		FIncidentId Id;
		EPhase4IncidentLifecycle Lifecycle =
			EPhase4IncidentLifecycle::None;
		FFlightId FlightId;
		FName AffectedRunway;
		uint64 OutcomeSeed = 0;
		FString RiskFactor;
		FString Warning;
		FString Remedy;
		FString PlayerDecision;
		FString CurrentConditions;
		FString Mitigations;
		FString Outcome;
		FString Report;
		FString Prevention;
		bool bWarningShown = false;
		bool bOverrideAcknowledged = false;
		bool bDeparturesHeld = false;
		bool bArrivalsDiverted = false;
		bool bAreaProtected = false;
		bool bTowDispatched = false;
		bool bRunwayClosed = false;
		bool bOtherOperationsContinue = true;
		bool bReportReviewed = false;
		bool bRecoveryApplied = false;
		int64 AlertedAtGameMilliseconds = 0;
		int64 ProtectedAtGameMilliseconds = 0;
	};

	struct FPhase4State
	{
		bool bInitialized = false;
		bool bTimetablePublished = false;
		bool bFixtureCompleted = false;
		bool bRecoverable = true;
		uint64 MasterSeed = 1;
		uint64 RandomStreamState = 1;
		uint64 NextDomainId = 1;
		uint64 NextEventSequence = 1;
		int64 InitializedAtGameMilliseconds = 0;
		int64 PublishedAtGameMilliseconds = 0;
		int64 LastUpdatedGameMilliseconds = 0;
		int32 CurrentOperatingDay = 0;
		uint8 ProcessedDayMask = 0;
		TArray<FPhase4ContractRecord> Contracts;
		TArray<FPhase4GateRecord> Gates;
		TArray<FPhase4FlightRecord> Flights;
		TArray<FPhase4ConnectionRecord> Connections;
		TArray<FPhase4TransferBagRecord> TransferBags;
		TArray<FPhase4BorderRecord> BorderProcesses;
		TArray<FPhase4TransportRecord> Transports;
		TArray<FPhase4ForecastRecord> Forecast;
		TArray<FPhase4ApproachLimitRecord> ApproachLimits;
		TArray<FPhase4RenewalRecord> Renewals;
		FPhase4IncidentRecord Incident;
		TArray<FPhase4Event> Events;
		int32 CompletedFlightCount = 0;
		int32 EarlyFlightCount = 0;
		int32 LateFlightCount = 0;
		int32 GateChangeCount = 0;
		int32 WeatherRestrictedFlightCount = 0;
		int32 ConnectedPassengerCount = 0;
		int32 MissedConnectionPassengerCount = 0;
		int32 RebookedPassengerCount = 0;
		int32 CompletedTransferBagCount = 0;
		int32 InternationalPassengerCount = 0;
		int32 BorderProcessedPassengerCount = 0;
		int32 RentalCarPassengerCount = 0;
		int32 RailPassengerCount = 0;
		int32 RenewalAcceptedCount = 0;
		int64 TotalRegionalRevenueCredits = 0;
	};

	struct FPhase4QuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bTimetablePublished = false;
		bool bFixtureCompleted = false;
		bool bRecoverable = true;
		int32 CurrentOperatingDay = 0;
		int32 AcceptedContractCount = 0;
		int32 ContractCount = 0;
		int32 FlightCount = 0;
		int32 CompletedFlightCount = 0;
		int32 EarlyFlightCount = 0;
		int32 LateFlightCount = 0;
		int32 GateChangeCount = 0;
		int32 WeatherRestrictedFlightCount = 0;
		int32 ConnectedPassengerCount = 0;
		int32 MissedConnectionPassengerCount = 0;
		int32 RebookedPassengerCount = 0;
		int32 TransferBagCount = 0;
		int32 CompletedTransferBagCount = 0;
		int32 InternationalPassengerCount = 0;
		int32 BorderProcessedPassengerCount = 0;
		int32 RentalCarPassengerCount = 0;
		int32 RailPassengerCount = 0;
		int32 RenewalAcceptedCount = 0;
		EPhase4IncidentLifecycle IncidentLifecycle =
			EPhase4IncidentLifecycle::None;
		bool bRunwayClosed = false;
		bool bOtherOperationsContinue = true;
		FString PrimaryStatus;
		FString TimetableSummary;
		FString SelectedFlight;
		FString SelectedFlightDetail;
		FString FeasibilitySummary;
		FString ConnectionSummary;
		FString BorderSummary;
		FString TransportSummary;
		FString WeatherSummary;
		FString ForecastSummary;
		FString IncidentHeadline;
		FString IncidentCause;
		FString IncidentConsequence;
		FString IncidentRemedy;
		FString IncidentLifecycleSummary;
		FString Caption;
		FString RenewalSummary;
		uint64 StateChecksum = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FConnectionId Id)
	{
		return ::GetTypeHash(Id.Value);
	}
}
