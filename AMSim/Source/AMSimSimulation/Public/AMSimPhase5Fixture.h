#pragma once

#include "AMSimPhase5Types.h"

namespace AMSim
{
	struct FPhase5Fixture
	{
		uint64 Seed = 0x504841534535ull;
		int32 AdvancedAirportPoints = 50;
		int32 AdvancedOperatingDays = 7;
		int32 MinimumSafetyRating = 70;
		int32 MinimumReliabilityRating = 70;
		int32 GAOperations = 12;
		int32 FlightSchoolOperations = 12;
		int32 CharterOperations = 8;
		int32 CargoShipments = 12;
		int32 PassengerFlights = 8;
		int32 PassengerJourneys = 100;
		int64 OperatingDayMilliseconds = 120000;
		int64 TenantGraceMilliseconds = 180000;
		int64 EventPreparationMilliseconds = 30000;
		int64 EventActiveMilliseconds = 60000;
		int64 EventCooldownMilliseconds = 240000;
	};

	AMSIMSIMULATION_API const FPhase5Fixture& GetPhase5Fixture();
	AMSIMSIMULATION_API FString CargoClassDisplayName(ECargoClass CargoClass);
	AMSIMSIMULATION_API FString CargoFlowDisplayName(ECargoFlow Flow);
	AMSIMSIMULATION_API FString CargoStateDisplayName(ECargoState State);
	AMSIMSIMULATION_API FString SpecializationPathDisplayName(
		ESpecializationPath Path);
	AMSIMSIMULATION_API FString CapabilityBandDisplayName(ECapabilityBand Band);
	AMSIMSIMULATION_API FString SpecialEventDisplayName(
		ESpecialEventFamily Family);
	AMSIMSIMULATION_API bool MeetsAdvancedRequirements(
		const FPhase5PathEvidenceRecord& Evidence);
	AMSIMSIMULATION_API FPhase5PathEvidenceRecord
		MakeAdvancedPathEvidenceFixture(ESpecializationPath Path);
}
