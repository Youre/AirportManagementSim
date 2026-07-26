#pragma once

#include "AMSimDefinitions.h"
#include "AMSimPhase1Types.h"

namespace AMSim
{
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
		int64 DeliveryAtMilliseconds = 30000;
		int64 BuildingAtMilliseconds = 60000;
		int64 InspectionAtMilliseconds = 90000;
		int64 ReadyToOpenAtMilliseconds = 120000;
		int64 TimetableIncrementMilliseconds = 300000;
		int64 MaximumArrivalDelayMilliseconds = 180000;
		int64 FlightRewardCredits = 600;
		int32 FlightRewardAirportPoints = 5;
		TArray<FDefinition> ContentDefinitions;
	};

	AMSIMSIMULATION_API const FPhase1Fixture& GetPhase1Fixture();
	AMSIMSIMULATION_API FStarterPlanProposal CreateDefaultStarterPlan();
	AMSIMSIMULATION_API FPhase1Validation ValidateStarterPlan(const FStarterPlanProposal& Proposal);
	AMSIMSIMULATION_API FDefinitionValidation ValidatePhase1FixtureDefinitions();
	AMSIMSIMULATION_API int32 CalculateContingencyPercent(int64 StartingCredits, int64 PlanCost);
}
