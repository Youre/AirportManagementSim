#pragma once

#include "AMSimPhase4Types.h"

namespace AMSim
{
	struct FPhase4Fixture
	{
		uint64 Seed = 0x524547494F4E414Cull;
		int32 TimetableDays = 7;
		int32 FlightsPerDay = 3;
		int32 LockedHorizonMinutes = 30;
		int32 TimetableIncrementMinutes = 5;
		int64 OperatingDayMilliseconds = 120000;
		int32 ConnectingPassengerCount = 24;
		int32 MissedConnectionPassengerCount = 6;
		int32 TransferBagCount = 32;
		int32 InternationalPassengerCount = 96;
		int32 RentalCarPassengerCount = 42;
		int32 RailPassengerCount = 58;
		int64 CompletedFlightRewardCredits = 180;
		int64 IncidentExpenseCredits = 900;
		int64 RecoveryGrantCredits = 1200;
	};

	AMSIMSIMULATION_API const FPhase4Fixture& GetPhase4Fixture();
	AMSIMSIMULATION_API FString Phase4FlightStateDisplayName(
		EPhase4FlightState State);
	AMSIMSIMULATION_API FString Phase4IncidentDisplayName(
		EPhase4IncidentLifecycle State);
}
