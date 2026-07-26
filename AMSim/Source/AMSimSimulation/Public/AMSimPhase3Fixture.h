#pragma once

#include "AMSimPhase3Types.h"

namespace AMSim
{
	struct FPhase3Fixture
	{
		uint64 Seed = 0xA17F3026ull;
		int64 TerminalCostCredits = 1200;
		int64 ConstructionStageMilliseconds = 3000;
		int64 JourneyBucketMilliseconds = 5000;
		int64 PassengerFlightRewardCredits = 900;
		int32 DepartingPassengerCount = 28;
		int32 ArrivingPassengerCount = 24;
		int32 DepartingBagCount = 18;
		int32 ArrivingBagCount = 16;
		int32 LogicalScalePassengerCount = 10000;
		int32 VisibleScaleProxyCount = 2000;
		FName Scenario = TEXT("S05.PassengerDepartureArrival");
	};

	AMSIMSIMULATION_API const FPhase3Fixture& GetPhase3Fixture();
	AMSIMSIMULATION_API FString TerminalStageDisplayName(
		ETerminalConstructionStage Stage);
	AMSIMSIMULATION_API FString PassengerJourneyDisplayName(
		EPassengerJourneyState State);
	AMSIMSIMULATION_API FString Phase3FlightDisplayName(EPhase3FlightState State);
	AMSIMSIMULATION_API FPhase3State BuildPhase3LogicalScaleFixture(
		int32 PassengerCount);
}
