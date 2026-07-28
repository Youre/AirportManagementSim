#include "AMSimProgressionViewState.h"

namespace AMSim
{
	namespace ProgressionViewStatePrivate
	{
		FString SpecializationName(
			const EAirportSpecialization Specialization)
		{
			switch (Specialization)
			{
			case EAirportSpecialization::FlightSchool:
				return TEXT("FLIGHT SCHOOL");
			case EAirportSpecialization::Charter:
				return TEXT("CHARTER");
			case EAirportSpecialization::GeneralAviation:
			default:
				return TEXT("GENERAL AVIATION");
			}
		}

		float Ratio(const int32 Current, const int32 Target)
		{
			return Target > 0
				? FMath::Clamp(
					static_cast<float>(Current) /
						static_cast<float>(Target),
					0.0f,
					1.0f)
				: 0.0f;
		}
	}

	using namespace ProgressionViewStatePrivate;

	FProgressionViewState MakeProgressionViewState(
		const FPhase1QuerySnapshot& Phase1,
		const FPhase2QuerySnapshot& Phase2,
		const FPhase2State& Phase2State,
		const FPhase3QuerySnapshot& Phase3,
		const FPhase4QuerySnapshot& Phase4)
	{
		FProgressionViewState View;
		View.Revision =
			Phase1.Revision ^
			(Phase2.Revision << 1) ^
			(Phase3.Revision << 2) ^
			(Phase4.Revision << 3);
		View.Funds = FString::Printf(
			TEXT("%lld CR  |  %d RATING  |  %d AP"),
			Phase1.Credits,
			Phase2.AverageRating,
			Phase1.AirportPoints);
		View.Objectives = {
			FString::Printf(
				TEXT("COMPLETE REGIONAL WEEK\n%d / %d FLIGHTS"),
				Phase4.CompletedFlightCount,
				FMath::Max(Phase4.FlightCount, 21)),
			FString::Printf(
				TEXT("MAINTAIN 90 RATING\n%d / 90"),
				Phase2.AverageRating),
			FString::Printf(
				TEXT("SERVE PASSENGERS\n%d / %d COMPLETE"),
				Phase3.CompletedPassengerCount,
				FMath::Max(Phase3.PassengerCount, 52))};
		View.ObjectiveProgress = {
			Ratio(
				Phase4.CompletedFlightCount,
				FMath::Max(Phase4.FlightCount, 21)),
			Ratio(Phase2.AverageRating, 90),
			Ratio(
				Phase3.CompletedPassengerCount,
				FMath::Max(Phase3.PassengerCount, 52))};

		const FString Phase2Selection =
			SpecializationName(Phase2State.SelectedSpecialization);
		const FString Selected =
			Phase4.bInitialized
				? TEXT("PASSENGER")
				: Phase2Selection;
		const FString RegionalBand =
			Phase4.bInitialized
				? TEXT(
					"ESTABLISHED - DONE\n"
					"REGIONAL - DONE\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED")
				: TEXT(
					"ESTABLISHED - DONE\n"
					"REGIONAL - ACTIVE\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED");
		View.Paths = {
			{
				TEXT("GENERAL AVIATION"),
				TEXT("Club visits / apron / light aircraft"),
				RegionalBand,
				Phase2.bInitialized
					? TEXT("ACTIVE")
					: TEXT("ESTABLISHED"),
				Selected == TEXT("GENERAL AVIATION"),
				false},
			{
				TEXT("FLIGHT SCHOOL"),
				TEXT("Training / instructors / circuit"),
				TEXT(
					"ESTABLISHED - DONE\n"
					"REGIONAL - ACTIVE\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED"),
				Phase2Selection == TEXT("FLIGHT SCHOOL")
					? TEXT("ACTIVE")
					: TEXT("AVAILABLE"),
				Selected == TEXT("FLIGHT SCHOOL"),
				false},
			{
				TEXT("CHARTER"),
				TEXT("Business flights / premium service"),
				TEXT(
					"ESTABLISHED - DONE\n"
					"REGIONAL - ACTIVE\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED"),
				Phase2Selection == TEXT("CHARTER")
					? TEXT("ACTIVE")
					: TEXT("AVAILABLE"),
				Selected == TEXT("CHARTER"),
				false},
			{
				TEXT("CARGO"),
				TEXT("No cargo simulation is implemented yet"),
				TEXT(
					"ESTABLISHED - LOCKED\n"
					"REGIONAL - LOCKED\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED"),
				TEXT("FUTURE LOCKED"),
				false,
				true},
			{
				TEXT("PASSENGER"),
				TEXT("Terminal / bags / connections / border"),
				RegionalBand,
				Phase4.bInitialized
					? TEXT("REGIONAL ACTIVE")
					: Phase3.bTerminalOpen
						? TEXT("ESTABLISHED")
						: TEXT("AVAILABLE"),
				Selected == TEXT("PASSENGER"),
				false},
			{
				TEXT("MIXED AIRPORT"),
				TEXT("Combine implemented operating paths"),
				TEXT(
					"ESTABLISHED - READY\n"
					"REGIONAL - READY\n"
					"ADVANCED - LOCKED\n"
					"MAJOR - LOCKED"),
				TEXT("COMBINATION"),
				Selected == TEXT("MIXED AIRPORT"),
				false}};
		View.SelectedPath = Selected;
		View.Inspector = FString::Printf(
			TEXT(
				"REQUIREMENTS\n"
				"Open terminal; publish timetable; preserve secure and "
				"accessible routes.\n\n"
				"CURRENT EVIDENCE\n"
				"Terminal: %s\n"
				"Regional timetable: %s\n"
				"Security route: %s\n"
				"Accessible route: %s\n\n"
				"SPATIAL FOOTPRINT\n"
				"Terminal hall, secure route, baggage flow, contact gate, "
				"remote stand, and surface transport links.\n\n"
				"BUSINESS EFFECT\n"
				"Passenger throughput, transfers, bags, border readiness, "
				"and regional contract delivery move together.\n\n"
				"OPERATIONAL OUTCOME\n"
				"Passenger, baggage, border, and connection systems "
				"operate as one airport path.\n\n"
				"NEXT CAPABILITY BAND\n"
				"Advanced and Major remain future locked.\n\n"
				"REWARD / OUTCOME\n"
				"Retain this path while combining other unlocked "
				"specializations."),
			Phase3.bTerminalOpen ? TEXT("OPEN") : TEXT("NOT OPEN"),
			Phase4.bTimetablePublished ? TEXT("PUBLISHED") : TEXT("NOT PUBLISHED"),
			Phase3.bSecurityIntegrityValid ? TEXT("VALID") : TEXT("INCOMPLETE"),
			Phase3.bAccessibleRouteValid ? TEXT("VALID") : TEXT("INCOMPLETE"));
		View.CombinedPathExplanation =
			TEXT(
				"PATHS CAN BE COMBINED  |  Capabilities remain available "
				"together; specialization is not an exclusive class and "
				"grants no artificial class bonus.");
		return View;
	}
}
