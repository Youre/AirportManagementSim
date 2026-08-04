#pragma once

#include "AMSimDefinitions.h"
#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FRunwayDesignation
	{
		double HeadingDegrees = 0.0;
		int32 PrimaryNumber = 36;
		int32 ReciprocalNumber = 18;
	};

	struct FTaxiwayNetworkValidation
	{
		bool bHasRunwayConnection = false;
		bool bHasGateConnection = false;
		bool bAllSegmentsConnected = false;
		int32 ConnectedGateIndex = INDEX_NONE;
		int32 FirstDisconnectedSegment = INDEX_NONE;
		int32 ConnectedSegmentCount = 0;
	};

	struct FPhase1Fixture
	{
		uint64 Seed = 11684030530412666515ull;
		FName ScenarioId = TEXT("S01.StarterGrassAirfield");
		FName MapId = TEXT("Map.TemperateStarter");
		FName AircraftId = TEXT("Aircraft.LightPiston.Starter");
		FName OperatorId = TEXT("Operator.RiverbendFlyingClub");
		int64 StartingCredits = 5000;
		int64 StarterPlanCost = 3400;
		int64 MaximumStarterPlanCost = 3500;
		int32 MinimumContingencyPercent = 30;
		int64 DeliveryAtMilliseconds = 1800000;
		int64 BuildingAtMilliseconds = 3600000;
		int64 InspectionAtMilliseconds = 10800000;
		int64 ReadyToOpenAtMilliseconds = 12600000;
		int32 ServiceRoadConstructionBonusPercent = 20;
		int32 ConstructionWorkerCount = 4;
		int64 TimetableIncrementMilliseconds = 300000;
		int64 MaximumArrivalDelayMilliseconds = 3600000;
		int64 FlightInboundOffsetMilliseconds = -180000;
		int64 FlightApproachOffsetMilliseconds = -60000;
		int64 FlightLandingOffsetMilliseconds = 0;
		int64 FlightRunwayRollOffsetMilliseconds = 60000;
		int64 FlightTaxiInOffsetMilliseconds = 120000;
		int64 FlightParkedOffsetMilliseconds = 300000;
		int64 FlightTurnaroundOffsetMilliseconds = 360000;
		int64 FlightReadyOffsetMilliseconds = 2760000;
		int64 FlightTaxiOutOffsetMilliseconds = 3060000;
		int64 FlightTakeoffOffsetMilliseconds = 3240000;
		int64 FlightOutboundOffsetMilliseconds = 3300000;
		int64 FlightCompletedOffsetMilliseconds = 3480000;
		int64 FlightRewardCredits = 600;
		int32 FlightRewardAirportPoints = 5;
		TArray<FDefinition> ContentDefinitions;
	};

	AMSIMSIMULATION_API const FPhase1Fixture& GetPhase1Fixture();
	AMSIMSIMULATION_API FStarterPlanProposal CreateDefaultStarterPlan();
	AMSIMSIMULATION_API FRunwayDesignation CalculateRunwayDesignation(
		const FPhase1Point& Start,
		const FPhase1Point& End);
	AMSIMSIMULATION_API int64 GetPhase1FlightStateOffsetMilliseconds(
		EFlightState State);
	AMSIMSIMULATION_API int64 GetPhase1FlightStateEndOffsetMilliseconds(
		EFlightState State);
	AMSIMSIMULATION_API FPhase1Point ClosestPointOnSegment(
		const FPhase1Point& Point,
		const FPhase1Point& SegmentStart,
		const FPhase1Point& SegmentEnd);
	AMSIMSIMULATION_API bool IsPointNearSegment(
		const FPhase1Point& Point,
		const FPhase1Point& SegmentStart,
		const FPhase1Point& SegmentEnd,
		int64 ToleranceCentimeters);
	AMSIMSIMULATION_API bool DoSegmentsIntersect(
		const FPhase1Point& FirstStart,
		const FPhase1Point& FirstEnd,
		const FPhase1Point& SecondStart,
		const FPhase1Point& SecondEnd);
	AMSIMSIMULATION_API bool TryFindSegmentIntersection(
		const FPhase1Point& FirstStart,
		const FPhase1Point& FirstEnd,
		const FPhase1Point& SecondStart,
		const FPhase1Point& SecondEnd,
		FPhase1Point& OutIntersection);
	AMSIMSIMULATION_API bool IsTaxiwayConnectedToRunway(
		const FStarterPlanProposal& Proposal,
		int64 ToleranceCentimeters = 500);
	AMSIMSIMULATION_API const TArray<FPhase1Point>& GetStarterGatePoints();
	AMSIMSIMULATION_API FPhase1Point GetStarterTerminalCenter();
	AMSIMSIMULATION_API TArray<FTaxiwaySegment> GetTaxiwaySegments(
		const FStarterPlanProposal& Proposal);
	AMSIMSIMULATION_API FTaxiwayNetworkValidation ValidateTaxiwayNetwork(
		const FStarterPlanProposal& Proposal,
		int64 ToleranceCentimeters = 500);
	AMSIMSIMULATION_API FStarterPlanProposal NormalizeStarterPlanConnections(
		const FStarterPlanProposal& Proposal);
	AMSIMSIMULATION_API bool HasConstructionRoadBenefit(
		const FStarterPlanProposal& Proposal);
	AMSIMSIMULATION_API FPhase1Validation ValidateStarterPlan(const FStarterPlanProposal& Proposal);
	AMSIMSIMULATION_API FDefinitionValidation ValidatePhase1FixtureDefinitions();
	AMSIMSIMULATION_API int32 CalculateContingencyPercent(int64 StartingCredits, int64 PlanCost);
}
