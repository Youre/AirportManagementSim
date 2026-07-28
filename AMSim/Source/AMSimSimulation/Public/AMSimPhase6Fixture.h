#pragma once

#include "AMSimPhase6Types.h"

namespace AMSim
{
	struct FPhase6Fixture
	{
		uint64 Seed = 0x504841534536ull;
		int32 MajorAirportPoints = 100;
		int32 MajorOperatingDays = 14;
		int32 MinimumSafetyRating = 80;
		int32 MinimumReliabilityRating = 80;
		int32 MinimumTenantRelationshipRating = 75;
		int64 OperatingDayMilliseconds = 120000;
		int64 FacilityConstructionMilliseconds = 30000;
		int64 LargeAircraftTurnaroundTaskMilliseconds = 8000;
		int64 SeriousIncidentRepairMilliseconds = 60000;
		int32 LargeAircraftPassengerCapacity = 290;
		int32 LargeAircraftRepresentativePassengers = 278;
		int32 LargeAircraftRepresentativeBags = 334;
		int32 LargeAircraftMinimumRunwayLengthMeters = 2800;
		int32 LargeAircraftMinimumRunwayWidthMeters = 45;
		int32 LogicalAgentTarget = 10000;
		int32 VisibleAgentTarget = 2000;
		int32 OnMapAircraftTarget = 150;
		int32 VehicleTarget = 500;
	};

	AMSIMSIMULATION_API const FPhase6Fixture& GetPhase6Fixture();
	AMSIMSIMULATION_API FString RunwayEndLabel(
		int32 MagneticHeadingDegrees,
		ERunwaySide Side);
	AMSIMSIMULATION_API ERunwaySide ReciprocalRunwaySide(ERunwaySide Side);
	AMSIMSIMULATION_API FString RunwayUseDisplayName(ERunwayUse Use);
	AMSIMSIMULATION_API FString MajorFacilityDisplayName(
		EMajorFacilityType Type);
	AMSIMSIMULATION_API FString SeriousIncidentLifecycleDisplayName(
		ESeriousIncidentLifecycle Lifecycle);
	AMSIMSIMULATION_API TArray<FMajorFacilityRecord>
		MakeMajorFacilityDefinitions();
	AMSIMSIMULATION_API bool MeetsMajorRequirements(
		const FPhase5PathEvidenceRecord& Evidence);
	AMSIMSIMULATION_API FPhase5PathEvidenceRecord
		MakeMajorPathEvidenceFixture(ESpecializationPath Path);
}
