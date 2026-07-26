#pragma once

#include "AMSimPhase2Types.h"

namespace AMSim
{
	struct FPhase2Fixture
	{
		uint64 Seed = 0xA17F2026ull;
		int64 OperatingDayMilliseconds = 600000;
		int64 FlightSpacingMilliseconds = 30000;
		int64 FlightCycleMilliseconds = 120000;
		int64 DailyStaffAndLeaseCostCredits = 180;
		int64 ParcelPurchaseCostCredits = 750;
		int64 ExpansionCostCredits = 900;
		int64 RecoveryGrantCredits = 1200;
		int32 RequiredOperatingDays = 14;
		FName ScenarioGeneralAviation = TEXT("S02.SustainedGA");
		FName ScenarioFlightSchool = TEXT("S03.FlightSchool");
		FName ScenarioCharter = TEXT("S04.Charter");
	};

	AMSIMSIMULATION_API const FPhase2Fixture& GetPhase2Fixture();
	AMSIMSIMULATION_API FString SpecializationDisplayName(EAirportSpecialization Specialization);
	AMSIMSIMULATION_API FString WeatherCategoryDisplayName(EWeatherCategory Category);
	AMSIMSIMULATION_API FName ContractContentIdFor(EAirportSpecialization Specialization);
	AMSIMSIMULATION_API int32 Phase2FlightsPerOperatingDay(
		EAirportSpecialization Specialization);
}
