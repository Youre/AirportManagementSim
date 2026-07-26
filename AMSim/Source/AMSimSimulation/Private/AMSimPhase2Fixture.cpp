#include "AMSimPhase2Fixture.h"

namespace AMSim
{
	const FPhase2Fixture& GetPhase2Fixture()
	{
		static const FPhase2Fixture Fixture;
		return Fixture;
	}

	FString SpecializationDisplayName(const EAirportSpecialization Specialization)
	{
		switch (Specialization)
		{
		case EAirportSpecialization::GeneralAviation: return TEXT("General aviation");
		case EAirportSpecialization::FlightSchool: return TEXT("Flight school");
		case EAirportSpecialization::Charter: return TEXT("Charter");
		default: return TEXT("Not selected");
		}
	}

	FString WeatherCategoryDisplayName(const EWeatherCategory Category)
	{
		switch (Category)
		{
		case EWeatherCategory::Wet: return TEXT("Wet runway");
		case EWeatherCategory::LowCloud: return TEXT("Low cloud");
		case EWeatherCategory::ColdWet: return TEXT("Cold and wet");
		case EWeatherCategory::StrongCrosswind: return TEXT("Strong crosswind");
		default: return TEXT("Visual conditions");
		}
	}

	FName ContractContentIdFor(const EAirportSpecialization Specialization)
	{
		switch (Specialization)
		{
		case EAirportSpecialization::GeneralAviation:
			return TEXT("Contract.GA.CircuitAndTouring");
		case EAirportSpecialization::FlightSchool:
			return TEXT("Contract.FlightSchool.RecurringTraining");
		case EAirportSpecialization::Charter:
			return TEXT("Contract.Charter.ExecutiveShuttle");
		default:
			return NAME_None;
		}
	}

	int32 Phase2FlightsPerOperatingDay(const EAirportSpecialization Specialization)
	{
		switch (Specialization)
		{
		case EAirportSpecialization::FlightSchool: return 3;
		case EAirportSpecialization::Charter: return 1;
		case EAirportSpecialization::GeneralAviation: return 2;
		default: return 0;
		}
	}
}
