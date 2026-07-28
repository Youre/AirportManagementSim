#include "AMSimPhase6Fixture.h"

namespace AMSim
{
	namespace
	{
		bool HasAllNames(
			const TArray<FName>& Values,
			const std::initializer_list<FName> Required)
		{
			for (const FName Name : Required)
			{
				if (!Values.Contains(Name))
				{
					return false;
				}
			}
			return true;
		}

		FMajorFacilityRecord MakeFacility(
			const FName ContentId,
			const FString& DisplayName,
			const EMajorFacilityType Type,
			const ESpecializationPath Path,
			const int32 CapacityUnits,
			const int64 CostCredits,
			const int32 AirportPointReward,
			const bool bPassengerConnected,
			const FString& Requirement)
		{
			FMajorFacilityRecord Facility;
			Facility.ContentId = ContentId;
			Facility.DisplayName = DisplayName;
			Facility.Type = Type;
			Facility.OwningPath = Path;
			Facility.CapacityUnits = CapacityUnits;
			Facility.CostCredits = CostCredits;
			Facility.AirportPointReward = AirportPointReward;
			Facility.bRoadConnected = Type != EMajorFacilityType::ParallelRunway;
			Facility.bAirsideConnected =
				Type != EMajorFacilityType::GroundAccessHub;
			Facility.bPassengerConnected = bPassengerConnected;
			Facility.bStaffed = false;
			Facility.bCookerVisibleDefinition = true;
			Facility.Requirement = Requirement;
			return Facility;
		}
	}

	const FPhase6Fixture& GetPhase6Fixture()
	{
		static const FPhase6Fixture Fixture;
		return Fixture;
	}

	FString RunwayEndLabel(
		const int32 MagneticHeadingDegrees,
		const ERunwaySide Side)
	{
		const int32 Normalized =
			((MagneticHeadingDegrees % 360) + 360) % 360;
		int32 Number = ((Normalized + 5) / 10) % 36;
		if (Number == 0)
		{
			Number = 36;
		}
		FString Suffix;
		switch (Side)
		{
		case ERunwaySide::Left: Suffix = TEXT("L"); break;
		case ERunwaySide::Center: Suffix = TEXT("C"); break;
		case ERunwaySide::Right: Suffix = TEXT("R"); break;
		default: break;
		}
		return FString::Printf(TEXT("%02d%s"), Number, *Suffix);
	}

	ERunwaySide ReciprocalRunwaySide(const ERunwaySide Side)
	{
		switch (Side)
		{
		case ERunwaySide::Left: return ERunwaySide::Right;
		case ERunwaySide::Right: return ERunwaySide::Left;
		default: return Side;
		}
	}

	FString RunwayUseDisplayName(const ERunwayUse Use)
	{
		switch (Use)
		{
		case ERunwayUse::Closed: return TEXT("Closed");
		case ERunwayUse::Available: return TEXT("Available");
		case ERunwayUse::Arrival: return TEXT("Active arrival");
		case ERunwayUse::Departure: return TEXT("Active departure");
		case ERunwayUse::Mixed: return TEXT("Mixed");
		default: return TEXT("Unknown");
		}
	}

	FString MajorFacilityDisplayName(const EMajorFacilityType Type)
	{
		switch (Type)
		{
		case EMajorFacilityType::ParallelRunway: return TEXT("Parallel runway");
		case EMajorFacilityType::LargeStand: return TEXT("Large-aircraft stand");
		case EMajorFacilityType::HangarCampus: return TEXT("GA hangar campus");
		case EMajorFacilityType::InstructionCenter: return TEXT("Instruction center");
		case EMajorFacilityType::ExecutiveFacility: return TEXT("Executive facility");
		case EMajorFacilityType::CargoHub: return TEXT("High-capacity cargo hub");
		case EMajorFacilityType::TerminalConcourse: return TEXT("Terminal concourse");
		case EMajorFacilityType::BaggageHall: return TEXT("High-capacity baggage hall");
		case EMajorFacilityType::ServiceDepot: return TEXT("Major service depot");
		case EMajorFacilityType::EmergencyStation: return TEXT("Emergency response station");
		case EMajorFacilityType::GroundAccessHub: return TEXT("Ground access hub");
		case EMajorFacilityType::OperationsCenter: return TEXT("Airport operations center");
		default: return TEXT("Major facility");
		}
	}

	FString SeriousIncidentLifecycleDisplayName(
		const ESeriousIncidentLifecycle Lifecycle)
	{
		switch (Lifecycle)
		{
		case ESeriousIncidentLifecycle::None: return TEXT("No incident");
		case ESeriousIncidentLifecycle::WarningIssued: return TEXT("Warning issued");
		case ESeriousIncidentLifecycle::RiskAcknowledged: return TEXT("Risk acknowledged");
		case ESeriousIncidentLifecycle::Materialized: return TEXT("Incident detected");
		case ESeriousIncidentLifecycle::Alerted: return TEXT("Response alerted");
		case ESeriousIncidentLifecycle::ResourcesDispatched: return TEXT("Resources dispatched");
		case ESeriousIncidentLifecycle::AreaProtected: return TEXT("Area protected");
		case ESeriousIncidentLifecycle::Stabilized: return TEXT("Stabilized");
		case ESeriousIncidentLifecycle::Investigating: return TEXT("Investigating");
		case ESeriousIncidentLifecycle::Repairing: return TEXT("Repairing");
		case ESeriousIncidentLifecycle::Recovered: return TEXT("Recovered");
		default: return TEXT("Unknown");
		}
	}

	TArray<FMajorFacilityRecord> MakeMajorFacilityDefinitions()
	{
		return {
			MakeFacility(
				TEXT("Facility.Major.Runway.Parallel"),
				TEXT("Parallel Runway 09R/27L"),
				EMajorFacilityType::ParallelRunway,
				ESpecializationPath::Mixed,
				1,
				12000,
				5,
				false,
				TEXT("Paved 2,800 m x 45 m runway with connected taxi and emergency access.")),
			MakeFacility(
				TEXT("Facility.Major.Stand.Large"),
				TEXT("Stand H1"),
				EMajorFacilityType::LargeStand,
				ESpecializationPath::Passenger,
				1,
				6500,
				4,
				true,
				TEXT("Code-E-like clearance, contact boarding, baggage, and service access.")),
			MakeFacility(
				TEXT("Facility.Major.GA.HangarCampus"),
				TEXT("Riverbend Hangar Campus"),
				EMajorFacilityType::HangarCampus,
				ESpecializationPath::GeneralAviation,
				8,
				4800,
				8,
				false,
				TEXT("Eight based-aircraft spaces with FBO, fuel, and maintenance access.")),
			MakeFacility(
				TEXT("Facility.Major.FlightSchool.InstructionCenter"),
				TEXT("Riverbend Training Center"),
				EMajorFacilityType::InstructionCenter,
				ESpecializationPath::FlightSchool,
				4,
				4200,
				8,
				false,
				TEXT("Four trainers, two instructor teams, and protected pattern capacity.")),
			MakeFacility(
				TEXT("Facility.Major.Charter.Executive"),
				TEXT("Riverbend Executive"),
				EMajorFacilityType::ExecutiveFacility,
				ESpecializationPath::Charter,
				2,
				5200,
				8,
				true,
				TEXT("Premium handling and two compatible business-jet stands.")),
			MakeFacility(
				TEXT("Facility.Major.Cargo.Hub"),
				TEXT("Riverbend Cargo Hub"),
				EMajorFacilityType::CargoHub,
				ESpecializationPath::Cargo,
				48,
				8000,
				8,
				false,
				TEXT("Four storage classes, road/airside ports, and night coverage.")),
			MakeFacility(
				TEXT("Facility.Major.Passenger.Concourse"),
				TEXT("Riverbend Concourse B"),
				EMajorFacilityType::TerminalConcourse,
				ESpecializationPath::Passenger,
				600,
				9500,
				8,
				true,
				TEXT("Secure passenger processing and compatible large-aircraft gate path.")),
			MakeFacility(
				TEXT("Facility.Major.Baggage.Hall"),
				TEXT("Baggage Sort Hall B"),
				EMajorFacilityType::BaggageHall,
				ESpecializationPath::Passenger,
				600,
				7000,
				6,
				true,
				TEXT("Screening, sort, make-up, transfer, and reclaim capacity.")),
			MakeFacility(
				TEXT("Facility.Major.Service.Depot"),
				TEXT("Heavy Service Depot"),
				EMajorFacilityType::ServiceDepot,
				ESpecializationPath::Mixed,
				12,
				6000,
				5,
				false,
				TEXT("Wide-body tug, fuel, catering, water, lavatory, baggage, and inspection fleets.")),
			MakeFacility(
				TEXT("Facility.Major.Emergency.Station"),
				TEXT("Airport Rescue Station"),
				EMajorFacilityType::EmergencyStation,
				ESpecializationPath::Mixed,
				4,
				6800,
				5,
				false,
				TEXT("Fire, medical, police, and operations teams with runway access.")),
			MakeFacility(
				TEXT("Facility.Major.Access.Hub"),
				TEXT("Riverbend Transit Hub"),
				EMajorFacilityType::GroundAccessHub,
				ESpecializationPath::Passenger,
				800,
				7200,
				5,
				true,
				TEXT("Two public-access modes with connected curb, parking, bus, or rail capacity.")),
			MakeFacility(
				TEXT("Facility.Major.Operations.Center"),
				TEXT("Airport Operations Center"),
				EMajorFacilityType::OperationsCenter,
				ESpecializationPath::Mixed,
				6,
				5500,
				5,
				false,
				TEXT("Shared-resource policy, timetable, staff, and disruption coordination."))};
	}

	bool MeetsMajorRequirements(const FPhase5PathEvidenceRecord& Evidence)
	{
		const FPhase6Fixture& Fixture = GetPhase6Fixture();
		if (Evidence.AirportPoints < Fixture.MajorAirportPoints ||
			Evidence.QualifyingOperatingDays < Fixture.MajorOperatingDays ||
			Evidence.SafetyRating < Fixture.MinimumSafetyRating ||
			Evidence.ReliabilityRating < Fixture.MinimumReliabilityRating ||
			Evidence.TenantRelationshipRating <
				Fixture.MinimumTenantRelationshipRating ||
			!Evidence.bPrimaryTenantActive ||
			!Evidence.bSignatureFacilityOperational)
		{
			return false;
		}

		switch (Evidence.Path)
		{
		case ESpecializationPath::GeneralAviation:
			return Evidence.CompletedOperations >= 36 &&
				Evidence.DistinctRolesOrClasses >= 5 &&
				HasAllNames(
					Evidence.FacilityIds,
					{TEXT("Facility.Major.GA.HangarCampus")}) &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("Provider.FBO"),
						TEXT("Provider.Maintenance"),
						TEXT("Provider.Fuel"),
						TEXT("Event.GAFlyIn")});
		case ESpecializationPath::FlightSchool:
			return Evidence.CompletedOperations >= 36 &&
				Evidence.DistinctRolesOrClasses >= 4 &&
				HasAllNames(
					Evidence.FacilityIds,
					{TEXT("Facility.Major.FlightSchool.InstructionCenter")}) &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("Tenant.FlightSchool"),
						TEXT("Team.Instructor.2"),
						TEXT("Event.FlightSchoolOpenDay")});
		case ESpecializationPath::Charter:
			return Evidence.CompletedOperations >= 24 &&
				HasAllNames(
					Evidence.FacilityIds,
					{TEXT("Facility.Major.Charter.Executive")}) &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("Tenant.Charter"),
						TEXT("Stand.BusinessJet.1"),
						TEXT("Stand.BusinessJet.2"),
						TEXT("Service.PremiumHandling"),
						TEXT("Movement.ShortNoticeVip.4")});
		case ESpecializationPath::Cargo:
			return Evidence.CompletedOperations >= 36 &&
				Evidence.DistinctRolesOrClasses >= 4 &&
				HasAllNames(
					Evidence.FacilityIds,
					{TEXT("Facility.Major.Cargo.Hub")}) &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("Tenant.CargoOperator"),
						TEXT("Cargo.DedicatedFreighter.12"),
						TEXT("Cargo.Night.6"),
						TEXT("Cargo.AllFlows")});
		case ESpecializationPath::Passenger:
			return Evidence.CompletedOperations >= 24 &&
				Evidence.CompletedPassengers >= 500 &&
				HasAllNames(
					Evidence.FacilityIds,
					{TEXT("Facility.Major.Passenger.Concourse"),
						TEXT("Facility.Major.Baggage.Hall"),
						TEXT("Facility.Major.Access.Hub")}) &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("Tenant.PassengerOperator"),
						TEXT("Concession.2"),
						TEXT("Aircraft.Boeing787-9.Turnaround"),
						TEXT("Transport.Public.2")});
		case ESpecializationPath::Mixed:
			return Evidence.CompletedOperations >= 60 &&
				Evidence.RegionalPathCount >= 3 &&
				Evidence.AdvancedPathCount >= 2 &&
				Evidence.SharedResourceDays >= 5 &&
				HasAllNames(
					Evidence.MajorEvidenceIds,
					{TEXT("SharedConflict.Recovered")});
		default:
			return false;
		}
	}

	FPhase5PathEvidenceRecord MakeMajorPathEvidenceFixture(
		const ESpecializationPath Path)
	{
		const FPhase6Fixture& Fixture = GetPhase6Fixture();
		FPhase5PathEvidenceRecord Evidence;
		Evidence.Path = Path;
		Evidence.Band = ECapabilityBand::Major;
		Evidence.EarnedBand = ECapabilityBand::Major;
		Evidence.OperationalBand = ECapabilityBand::Major;
		Evidence.OperationalStatus = ECapabilityOperationalStatus::Healthy;
		Evidence.AirportPoints = Fixture.MajorAirportPoints;
		Evidence.OperatingDays = Fixture.MajorOperatingDays;
		Evidence.QualifyingOperatingDays = Fixture.MajorOperatingDays;
		Evidence.SafetyRating = Fixture.MinimumSafetyRating;
		Evidence.ReliabilityRating = Fixture.MinimumReliabilityRating;
		Evidence.TenantRelationshipRating =
			Fixture.MinimumTenantRelationshipRating;
		Evidence.bPrimaryTenantActive = true;
		Evidence.bSecondaryProviderActive = true;
		Evidence.bSignatureFacilityOperational = true;
		Evidence.bMajorRequirementsMet = true;

		switch (Path)
		{
		case ESpecializationPath::GeneralAviation:
			Evidence.CompletedOperations = 36;
			Evidence.DistinctRolesOrClasses = 5;
			Evidence.FacilityIds = {TEXT("Facility.Major.GA.HangarCampus")};
			Evidence.MajorEvidenceIds = {
				TEXT("Provider.FBO"),
				TEXT("Provider.Maintenance"),
				TEXT("Provider.Fuel"),
				TEXT("Event.GAFlyIn")};
			break;
		case ESpecializationPath::FlightSchool:
			Evidence.CompletedOperations = 36;
			Evidence.DistinctRolesOrClasses = 4;
			Evidence.FacilityIds = {
				TEXT("Facility.Major.FlightSchool.InstructionCenter")};
			Evidence.MajorEvidenceIds = {
				TEXT("Tenant.FlightSchool"),
				TEXT("Team.Instructor.2"),
				TEXT("Event.FlightSchoolOpenDay")};
			break;
		case ESpecializationPath::Charter:
			Evidence.CompletedOperations = 24;
			Evidence.FacilityIds = {
				TEXT("Facility.Major.Charter.Executive")};
			Evidence.MajorEvidenceIds = {
				TEXT("Tenant.Charter"),
				TEXT("Stand.BusinessJet.1"),
				TEXT("Stand.BusinessJet.2"),
				TEXT("Service.PremiumHandling"),
				TEXT("Movement.ShortNoticeVip.4")};
			break;
		case ESpecializationPath::Cargo:
			Evidence.CompletedOperations = 36;
			Evidence.DistinctRolesOrClasses = 4;
			Evidence.FacilityIds = {TEXT("Facility.Major.Cargo.Hub")};
			Evidence.MajorEvidenceIds = {
				TEXT("Tenant.CargoOperator"),
				TEXT("Cargo.DedicatedFreighter.12"),
				TEXT("Cargo.Night.6"),
				TEXT("Cargo.AllFlows")};
			break;
		case ESpecializationPath::Passenger:
			Evidence.CompletedOperations = 24;
			Evidence.CompletedPassengers = 500;
			Evidence.FacilityIds = {
				TEXT("Facility.Major.Passenger.Concourse"),
				TEXT("Facility.Major.Baggage.Hall"),
				TEXT("Facility.Major.Access.Hub")};
			Evidence.MajorEvidenceIds = {
				TEXT("Tenant.PassengerOperator"),
				TEXT("Concession.2"),
				TEXT("Aircraft.Boeing787-9.Turnaround"),
				TEXT("Transport.Public.2")};
			break;
		case ESpecializationPath::Mixed:
			Evidence.CompletedOperations = 60;
			Evidence.RegionalPathCount = 3;
			Evidence.AdvancedPathCount = 2;
			Evidence.SharedResourceDays = 5;
			Evidence.FacilityIds = {
				TEXT("Facility.Major.Operations.Center")};
			Evidence.MajorEvidenceIds = {
				TEXT("SharedConflict.Recovered")};
			break;
		}
		Evidence.bMajorRequirementsMet = MeetsMajorRequirements(Evidence);
		Evidence.CurrentEvidence = TEXT("Major evidence complete.");
		Evidence.NextRequirement = TEXT("Major earned; continue operating.");
		return Evidence;
	}
}
