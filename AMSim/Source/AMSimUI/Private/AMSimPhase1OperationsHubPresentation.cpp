#include "AMSimPhase1OperationsHubPresentation.h"

#include "AMSimPhase1Fixture.h"

namespace AMSim
{
	namespace
	{
		FString ConstructionStatus(const EConstructionStage Stage)
		{
			switch (Stage)
			{
			case EConstructionStage::None: return TEXT("READY TO PLAN");
			case EConstructionStage::Funded: return TEXT("PROJECT FUNDED");
			case EConstructionStage::AwaitingDelivery: return TEXT("CREW EN ROUTE");
			case EConstructionStage::Building: return TEXT("BUILDING SURFACES");
			case EConstructionStage::Inspection: return TEXT("SAFETY INSPECTION");
			case EConstructionStage::ReadyToOpen: return TEXT("READY TO OPEN");
			case EConstructionStage::Operational: return TEXT("STARTER BUILD COMPLETE");
			default: return TEXT("AIRFIELD STATUS");
			}
		}

		FString NextFlightStep(const EFlightState State)
		{
			switch (State)
			{
			case EFlightState::Inbound: return TEXT("NEXT  APPROACH");
			case EFlightState::Approach: return TEXT("NEXT  LANDING");
			case EFlightState::Landing: return TEXT("NEXT  RUNWAY ROLL");
			case EFlightState::RunwayRoll: return TEXT("NEXT  TAXI TO GATE");
			case EFlightState::TaxiIn: return TEXT("NEXT  PARK AT GATE A");
			case EFlightState::Parked: return TEXT("NEXT  TURNAROUND");
			case EFlightState::Turnaround: return TEXT("NEXT  READY TO DEPART");
			case EFlightState::Ready: return TEXT("NEXT  TAXI OUT");
			case EFlightState::TaxiOut: return TEXT("NEXT  TAKEOFF");
			case EFlightState::Takeoff: return TEXT("NEXT  OUTBOUND");
			case EFlightState::Outbound: return TEXT("NEXT  VISIT COMPLETE");
			default: return FString();
			}
		}
	}

	FString FPhase1OperationsHubPresentation::FlightStateDisplayName(
		const EFlightState State)
	{
		switch (State)
		{
		case EFlightState::Scheduled: return TEXT("SCHEDULED");
		case EFlightState::Inbound: return TEXT("INBOUND");
		case EFlightState::Approach: return TEXT("ON APPROACH");
		case EFlightState::Landing: return TEXT("LANDING");
		case EFlightState::RunwayRoll: return TEXT("RUNWAY ROLL");
		case EFlightState::TaxiIn: return TEXT("TAXIING TO GATE");
		case EFlightState::Parked: return TEXT("PARKED AT GATE A");
		case EFlightState::Turnaround: return TEXT("TURNAROUND IN PROGRESS");
		case EFlightState::Ready: return TEXT("READY TO DEPART");
		case EFlightState::TaxiOut: return TEXT("TAXIING OUT");
		case EFlightState::Takeoff: return TEXT("TAKING OFF");
		case EFlightState::Outbound: return TEXT("LEAVING AIRSPACE");
		case EFlightState::Completed: return TEXT("FIRST VISIT COMPLETE");
		default: return TEXT("NO FLIGHT SCHEDULED");
		}
	}

	FString FPhase1OperationsHubPresentation::FormatGameTime(
		const int64 GameTimeMilliseconds)
	{
		const int64 TotalMinutes = FMath::Max<int64>(0, GameTimeMilliseconds / 60000);
		return FString::Printf(
			TEXT("DAY %lld  %02lld:%02lld"),
			TotalMinutes / (24 * 60) + 1,
			(TotalMinutes / 60) % 24,
			TotalMinutes % 60);
	}

	FPhase1OperationsHubState FPhase1OperationsHubPresentation::Derive(
		const EPhase1OperationsPage Page,
		const FPhase1QuerySnapshot& Query,
		const FPhase1State& State,
		const int32 OverlayMode)
	{
		FPhase1OperationsHubState View;
		if (Page == EPhase1OperationsPage::Airfield)
		{
			View.Eyebrow = TEXT("AIRFIELD  /  BUILD");
			View.Title = ConstructionStatus(Query.ConstructionStage);
			const int32 SegmentCount = State.Project.Proposal.TaxiwaySegments.IsEmpty()
				? (State.Project.Proposal.TaxiStart == State.Project.Proposal.TaxiEnd ? 0 : 1)
				: State.Project.Proposal.TaxiwaySegments.Num();
			if (Query.ConstructionStage == EConstructionStage::None)
			{
				View.Status = TEXT("Draw a runway and a connected taxiway network.");
				View.Detail = TEXT("Runway, taxiway, and optional road placement remain editable until funding.");
			}
			else if (Query.ConstructionStage < EConstructionStage::ReadyToOpen)
			{
				View.Status = TEXT("4 workers dispatch and build automatically.");
				View.Detail = TEXT("Use the time controls to observe the project. The funded layout is now committed.");
			}
			else
			{
				View.Status = FString::Printf(
					TEXT("RUNWAY READY  /  %d TAXI SEGMENT%s  /  GATE A CONNECTED"),
					SegmentCount,
					SegmentCount == 1 ? TEXT("") : TEXT("S"));
				View.Detail = Query.FlightState == EFlightState::Completed
					? TEXT("Starter construction is complete. Continue into Living Airport for expansion and recurring operations.")
					: TEXT("The starter layout is committed. Complete the first visit to unlock further construction and expansion.");
			}
			View.Footer = TEXT("CONNECTIONS overlay shows the operational network.");
			return View;
		}

		if (Page == EPhase1OperationsPage::Overlays)
		{
			View.Eyebrow = TEXT("MAP  /  OVERLAYS");
			View.Title = OverlayMode == 1
				? TEXT("CONNECTIONS")
				: OverlayMode == 2 ? TEXT("ACTIVITY") : TEXT("AIRFIELD");
			View.Status = OverlayMode == 1
				? TEXT("Cyan movement surfaces connect runway to gate; amber marks road access.")
				: OverlayMode == 2
					? TEXT("Infrastructure is muted so aircraft, crews, and active service markers stand out.")
					: TEXT("Standard operational colors and textures.");
			View.Detail = TEXT("Choose an overlay. Labels and geometry remain available without relying on color alone.");
			View.Footer = TEXT("AIRFIELD  /  CONNECTIONS  /  ACTIVITY");
			View.bShowOverlayModes = true;
			return View;
		}

		View.Eyebrow = TEXT("OPERATIONS  /  FIRST VISIT");
		View.Title = FlightStateDisplayName(Query.FlightState);
		View.Timeline = TEXT("ARRIVE  >  TAXI  >  GATE  >  SERVICE  >  DEPART");
		if (!Query.bAirportOpen)
		{
			View.Status = Query.ConstructionStage == EConstructionStage::ReadyToOpen
				? TEXT("OPEN THE AIRPORT")
				: TEXT("FINISH THE STARTER AIRFIELD");
			View.Detail = TEXT("The first GA offer becomes schedulable after the airport is open.");
		}
		else if (Query.OfferState == EOfferState::Available ||
			Query.OfferState == EOfferState::Declined)
		{
			View.Status = TEXT("ACCEPT THE RIVERBEND 21 OFFER");
			View.Detail = TEXT("Review the offer card, then accept it to choose an exact arrival time.");
		}
		else if (Query.OfferState == EOfferState::Accepted)
		{
			View.Status = TEXT("READY TO CHOOSE AN ARRIVAL");
			View.Detail = TEXT("Select one of four valid timetable slots. No aircraft steering is required.");
			View.bShowChooseArrival = true;
		}
		else if (Query.FlightState == EFlightState::Scheduled)
		{
			const int64 VisibleAt = State.Flight.ScheduledArrivalGameMilliseconds +
				GetPhase1Fixture().FlightInboundOffsetMilliseconds;
			const int64 RemainingGameMilliseconds = FMath::Max<int64>(
				0, VisibleAt - Query.GameTimeMilliseconds);
			const int32 Speed = FMath::Max(1, State.SpeedMultiplier);
			const int64 RemainingRealSeconds = FMath::CeilToInt64(
				static_cast<double>(RemainingGameMilliseconds) / (60000.0 * Speed));
			View.Status = Query.bPaused
				? FString::Printf(
					TEXT("ARRIVAL %s  /  PAUSED"),
					*FormatGameTime(State.Flight.ScheduledArrivalGameMilliseconds))
				: FString::Printf(
					TEXT("ARRIVAL %s  /  IN AIRSPACE IN %lld SEC AT %dx"),
					*FormatGameTime(State.Flight.ScheduledArrivalGameMilliseconds),
					RemainingRealSeconds,
					Speed);
			View.Detail = Query.bPaused
				? TEXT("Paused. Watch at normal speed or advance to the first visible inbound moment.")
				: TEXT("The visit runs automatically through landing, taxi, service, and departure.");
			View.bShowWatch = true;
			View.bShowAdvance = RemainingGameMilliseconds > 0;
		}
		else if (Query.FlightState >= EFlightState::Inbound &&
			Query.FlightState < EFlightState::Completed)
		{
			View.Status = NextFlightStep(Query.FlightState);
			View.Detail = TEXT("Automatic operation — observe the aircraft and service crew. Use 2x or 4x if desired.");
			View.bShowWatch = true;
			View.bShowAdvance = true;
			View.AdvanceLabel = TEXT("CONTINUE AT 4x");
		}
		else if (Query.FlightState == EFlightState::Completed)
		{
			View.Status = TEXT("REWARD POSTED  /  LIVING AIRPORT UNLOCKED");
			View.Detail = TEXT("No more first-visit actions are required. Select START LIVING AIRPORT in the right-hand card.");
			View.Timeline = TEXT("ARRIVAL COMPLETE  /  TURNAROUND COMPLETE  /  DEPARTURE COMPLETE");
		}
		else
		{
			View.Status = TEXT("FIRST VISIT NOT YET AVAILABLE");
			View.Detail = TEXT("Open the airport and accept its first compatible GA offer.");
		}
		View.Footer = TEXT("AUTOMATIC: aircraft movement and ground service do not require clicks.");
		return View;
	}
}
