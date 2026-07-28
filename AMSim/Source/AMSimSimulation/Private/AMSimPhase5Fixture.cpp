#include "AMSimPhase5Fixture.h"

namespace AMSim
{
	const FPhase5Fixture& GetPhase5Fixture()
	{
		static const FPhase5Fixture Fixture;
		return Fixture;
	}

	FString CargoClassDisplayName(const ECargoClass CargoClass)
	{
		switch (CargoClass)
		{
		case ECargoClass::MailExpress: return TEXT("Mail / express");
		case ECargoClass::TemperatureSensitive:
			return TEXT("Temperature-sensitive");
		case ECargoClass::Oversized: return TEXT("Oversized");
		default: return TEXT("General freight");
		}
	}

	FString CargoFlowDisplayName(const ECargoFlow Flow)
	{
		switch (Flow)
		{
		case ECargoFlow::Inbound: return TEXT("Inbound");
		case ECargoFlow::Transfer: return TEXT("Transfer");
		default: return TEXT("Outbound");
		}
	}

	FString CargoStateDisplayName(const ECargoState State)
	{
		switch (State)
		{
		case ECargoState::Offered: return TEXT("Offered");
		case ECargoState::RoadReceipt: return TEXT("Road receipt");
		case ECargoState::CheckAndSecure: return TEXT("Check and secure");
		case ECargoState::Storage: return TEXT("Storage");
		case ECargoState::BuildUp: return TEXT("Build-up");
		case ECargoState::AirsideTransfer: return TEXT("Airside transfer");
		case ECargoState::Loaded: return TEXT("Loaded");
		case ECargoState::InFlight: return TEXT("In flight");
		case ECargoState::Completed: return TEXT("Complete");
		case ECargoState::Exception: return TEXT("Exception");
		default: return TEXT("Unknown");
		}
	}

	FString SpecializationPathDisplayName(const ESpecializationPath Path)
	{
		switch (Path)
		{
		case ESpecializationPath::FlightSchool: return TEXT("Flight School");
		case ESpecializationPath::Charter: return TEXT("Charter");
		case ESpecializationPath::Cargo: return TEXT("Cargo");
		case ESpecializationPath::Passenger: return TEXT("Passenger");
		case ESpecializationPath::Mixed: return TEXT("Mixed Airport");
		default: return TEXT("General Aviation");
		}
	}

	FString CapabilityBandDisplayName(const ECapabilityBand Band)
	{
		switch (Band)
		{
		case ECapabilityBand::Established: return TEXT("Established");
		case ECapabilityBand::Regional: return TEXT("Regional");
		case ECapabilityBand::Advanced: return TEXT("Advanced");
		case ECapabilityBand::Major: return TEXT("Major");
		default: return TEXT("Unavailable");
		}
	}

	FString SpecialEventDisplayName(const ESpecialEventFamily Family)
	{
		switch (Family)
		{
		case ESpecialEventFamily::GAFlyIn: return TEXT("GA Fly-in");
		case ESpecialEventFamily::FlightSchoolOpenDay:
			return TEXT("Flight-school Open Day");
		case ESpecialEventFamily::CharterVipMovement:
			return TEXT("Charter / VIP Movement");
		case ESpecialEventFamily::CargoSurge: return TEXT("Cargo Surge");
		case ESpecialEventFamily::HolidayPassengerSurge:
			return TEXT("Holiday Passenger Surge");
		case ESpecialEventFamily::EmergencyExercise:
			return TEXT("Emergency Exercise");
		case ESpecialEventFamily::NotableVisitingAircraft:
			return TEXT("Notable Visiting Aircraft");
		default: return TEXT("Airshow");
		}
	}

	bool MeetsAdvancedRequirements(const FPhase5PathEvidenceRecord& Evidence)
	{
		const FPhase5Fixture& Fixture = GetPhase5Fixture();
		if (Evidence.AirportPoints < Fixture.AdvancedAirportPoints ||
			Evidence.OperatingDays < Fixture.AdvancedOperatingDays ||
			Evidence.SafetyRating < Fixture.MinimumSafetyRating ||
			Evidence.ReliabilityRating < Fixture.MinimumReliabilityRating ||
			!Evidence.bPrimaryTenantActive ||
			!Evidence.bSignatureFacilityOperational)
		{
			return false;
		}

		switch (Evidence.Path)
		{
		case ESpecializationPath::GeneralAviation:
			return Evidence.bSecondaryProviderActive &&
				Evidence.DistinctRolesOrClasses >= 3 &&
				Evidence.CompletedOperations >= Fixture.GAOperations;
		case ESpecializationPath::FlightSchool:
			return Evidence.bSecondaryProviderActive &&
				Evidence.DistinctRolesOrClasses >= 2 &&
				Evidence.CompletedOperations >= Fixture.FlightSchoolOperations;
		case ESpecializationPath::Charter:
			return Evidence.bSecondaryProviderActive &&
				Evidence.CompletedOperations >= Fixture.CharterOperations;
		case ESpecializationPath::Cargo:
			return Evidence.DistinctRolesOrClasses >= 4 &&
				Evidence.CompletedOperations >= Fixture.CargoShipments;
		case ESpecializationPath::Passenger:
			return Evidence.bSecondaryProviderActive &&
				Evidence.CompletedOperations >= Fixture.PassengerFlights &&
				Evidence.CompletedPassengers >= Fixture.PassengerJourneys;
		case ESpecializationPath::Mixed:
			return Evidence.RegionalPathCount >= 2 &&
				Evidence.bSharedResourceDayCompleted;
		default:
			return false;
		}
	}

	FPhase5PathEvidenceRecord MakeAdvancedPathEvidenceFixture(
		const ESpecializationPath Path)
	{
		const FPhase5Fixture& Fixture = GetPhase5Fixture();
		FPhase5PathEvidenceRecord Evidence;
		Evidence.Path = Path;
		Evidence.Band = ECapabilityBand::Advanced;
		Evidence.AirportPoints = Fixture.AdvancedAirportPoints;
		Evidence.OperatingDays = Fixture.AdvancedOperatingDays;
		Evidence.SafetyRating = Fixture.MinimumSafetyRating;
		Evidence.ReliabilityRating = Fixture.MinimumReliabilityRating;
		Evidence.bPrimaryTenantActive = true;
		Evidence.bSecondaryProviderActive = true;
		Evidence.bSignatureFacilityOperational = true;
		switch (Path)
		{
		case ESpecializationPath::GeneralAviation:
			Evidence.CompletedOperations = Fixture.GAOperations;
			Evidence.DistinctRolesOrClasses = 3;
			Evidence.FacilityIds = {
				TEXT("Facility.FBO.Advanced"),
				TEXT("Facility.Maintenance.Light"),
				TEXT("Facility.Apron.GA")};
			break;
		case ESpecializationPath::FlightSchool:
			Evidence.CompletedOperations = Fixture.FlightSchoolOperations;
			Evidence.DistinctRolesOrClasses = 2;
			Evidence.FacilityIds = {
				TEXT("Facility.FlightSchool"),
				TEXT("Facility.TrainingApron"),
				TEXT("Facility.BriefingRoom")};
			break;
		case ESpecializationPath::Charter:
			Evidence.CompletedOperations = Fixture.CharterOperations;
			Evidence.FacilityIds = {
				TEXT("Facility.CharterLounge"),
				TEXT("Facility.Stand.Business"),
				TEXT("Facility.Fuel.Premium")};
			break;
		case ESpecializationPath::Cargo:
			Evidence.CompletedOperations = Fixture.CargoShipments;
			Evidence.DistinctRolesOrClasses = 4;
			Evidence.FacilityIds = {
				TEXT("Facility.Cargo.Warehouse"),
				TEXT("Facility.Cargo.OversizedPad"),
				TEXT("Network.Cargo.RoadAirside")};
			break;
		case ESpecializationPath::Passenger:
			Evidence.CompletedOperations = Fixture.PassengerFlights;
			Evidence.CompletedPassengers = Fixture.PassengerJourneys;
			Evidence.FacilityIds = {
				TEXT("Facility.Terminal.Domestic"),
				TEXT("Facility.Security.Checkpoint"),
				TEXT("Facility.Baggage.MakeUp"),
				TEXT("Facility.Concession.Food")};
			break;
		case ESpecializationPath::Mixed:
			Evidence.RegionalPathCount = 2;
			Evidence.bSharedResourceDayCompleted = true;
			Evidence.FacilityIds = {
				TEXT("Facility.Shared.Stand"),
				TEXT("StaffTeam.Shared.GroundService")};
			break;
		}
		Evidence.CurrentEvidence = TEXT("Advanced fixture requirements satisfied.");
		Evidence.NextRequirement = TEXT("Major remains future locked.");
		return Evidence;
	}
}
