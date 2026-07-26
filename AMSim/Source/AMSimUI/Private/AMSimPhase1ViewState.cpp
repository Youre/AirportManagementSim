#include "AMSimPhase1ViewState.h"

namespace AMSim
{
	namespace
	{
		FString ConstructionName(const EConstructionStage Stage)
		{
			switch (Stage)
			{
			case EConstructionStage::None: return TEXT("Not funded");
			case EConstructionStage::Funded: return TEXT("Funded");
			case EConstructionStage::AwaitingDelivery: return TEXT("Awaiting delivery");
			case EConstructionStage::Building: return TEXT("Building");
			case EConstructionStage::Inspection: return TEXT("Safety inspection");
			case EConstructionStage::ReadyToOpen: return TEXT("Ready to open");
			case EConstructionStage::Operational: return TEXT("Operational");
			default: return TEXT("Unknown");
			}
		}

		FString OfferName(const EOfferState State)
		{
			switch (State)
			{
			case EOfferState::Unavailable: return TEXT("No offer");
			case EOfferState::Available: return TEXT("Offer available");
			case EOfferState::Declined: return TEXT("Offer declined");
			case EOfferState::Accepted: return TEXT("Contract accepted");
			case EOfferState::Scheduled: return TEXT("Flight scheduled");
			case EOfferState::Completed: return TEXT("Contract completed");
			default: return TEXT("Unknown");
			}
		}

		FString FlightName(const EFlightState State)
		{
			switch (State)
			{
			case EFlightState::None: return TEXT("No active flight");
			case EFlightState::Scheduled: return TEXT("Scheduled");
			case EFlightState::Inbound: return TEXT("Inbound");
			case EFlightState::Approach: return TEXT("On approach");
			case EFlightState::Landing: return TEXT("Landing");
			case EFlightState::RunwayRoll: return TEXT("Runway roll");
			case EFlightState::TaxiIn: return TEXT("Taxiing to stand");
			case EFlightState::Parked: return TEXT("Parked at Stand A1");
			case EFlightState::Turnaround: return TEXT("Turnaround");
			case EFlightState::Ready: return TEXT("Ready for departure");
			case EFlightState::TaxiOut: return TEXT("Taxiing out");
			case EFlightState::Takeoff: return TEXT("Taking off");
			case EFlightState::Outbound: return TEXT("Outbound");
			case EFlightState::Completed: return TEXT("Flight completed");
			default: return TEXT("Unknown");
			}
		}

		FString ServiceName(const EServiceTaskState State)
		{
			switch (State)
			{
			case EServiceTaskState::Unavailable: return TEXT("--");
			case EServiceTaskState::Waiting: return TEXT("Waiting");
			case EServiceTaskState::Active: return TEXT("Active");
			case EServiceTaskState::Completed: return TEXT("Complete");
			default: return TEXT("Unknown");
			}
		}

		FString RatingName(const FName Component)
		{
			if (Component == TEXT("SafetyReadiness"))
			{
				return TEXT("Safety/readiness");
			}
			if (Component == TEXT("OperationalReliability"))
			{
				return TEXT("Operational reliability");
			}
			return Component.ToString();
		}

		FString FormatGameTime(const int64 GameTimeMilliseconds)
		{
			const int64 TotalMinutes = GameTimeMilliseconds / 60000;
			return FString::Printf(
				TEXT("D%lld %02lld:%02lld"),
				1 + TotalMinutes / (24 * 60),
				(TotalMinutes / 60) % 24,
				TotalMinutes % 60);
		}

		FString CompactObjective(const FString& Objective)
		{
			if (Objective.StartsWith(TEXT("Build a grass runway")))
			{
				return TEXT("Build the starter airfield");
			}
			if (Objective.StartsWith(TEXT("Open the inspected runway")))
			{
				return TEXT("Review first-flight offer");
			}
			if (Objective.StartsWith(TEXT("Schedule the compatible visit")))
			{
				return TEXT("Complete the first turnaround");
			}
			return Objective;
		}
	}

	FPhase1ViewState MakePhase1ViewState(
		const FPhase1QuerySnapshot& Query,
		const FPhase1State& State)
	{
		FPhase1ViewState View;
		View.Revision = Query.Revision;
		View.bAirportInitialized = Query.bInitialized;
		View.AirportName = Query.bInitialized ? Query.AirportName.ToUpper() : TEXT("NEW AIRPORT");

		const int64 TotalMinutes = Query.GameTimeMilliseconds / 60000;
		View.Clock = FString::Printf(
			TEXT("DAY %lld  •  %02lld:%02lld  •  CLEAR  •  %s"),
			1 + TotalMinutes / (24 * 60),
			(TotalMinutes / 60) % 24,
			TotalMinutes % 60,
			Query.bPaused ? TEXT("PAUSED") : *FString::Printf(TEXT("%dx"), State.SpeedMultiplier));
		View.Funds = FString::Printf(TEXT("%lld CR  •  %d AP"), Query.Credits, Query.AirportPoints);
		View.Objective = CompactObjective(Query.CurrentObjective);
		View.Status = Query.PrimaryStatus;
		View.Cause = Query.Cause.IsEmpty() ? TEXT("No active constraint.") : Query.Cause;
		View.Remedy = Query.Remedy.IsEmpty() ? TEXT("Choose an available action.") : Query.Remedy;
		View.Project = TEXT("STARTER PLAN\n") + ConstructionName(Query.ConstructionStage);
		View.Offer = OfferName(Query.OfferState) + (State.Offer.bPinned ? TEXT("  /  PINNED") : TEXT(""));
		View.Compatibility = Query.CompatibilitySummary.IsEmpty()
			? TEXT("Build and open the airfield to unlock offers.")
			: Query.CompatibilitySummary;
		View.Flight = FlightName(Query.FlightState);

		if (State.Flight.Id.IsValid())
		{
			const int64 Arrival = State.Flight.ScheduledArrivalGameMilliseconds;
			View.Timetable = FString::Printf(
				TEXT("RIVERBEND 21  •  %s\nSTAND A1  •  PROTECTED %s–%s"),
				*FormatGameTime(Arrival),
				*FormatGameTime(State.Flight.StandOccupancyStartGameMilliseconds),
				*FormatGameTime(State.Flight.StandOccupancyEndGameMilliseconds));
		}
		else
		{
			View.Timetable = TEXT("No flight scheduled.");
		}

		View.Services = FString::Printf(
			TEXT("INSPECTION  •  %s\nFUEL  •  %s"),
			*ServiceName(Query.InspectionState),
			*ServiceName(Query.FuelingState));
		if (State.Transactions.IsEmpty())
		{
			View.Ledger = TEXT("LEDGER  No transactions");
		}
		else
		{
			const FTransactionRecord& Transaction = State.Transactions.Last();
			View.Ledger = FString::Printf(
				TEXT("LEDGER  %s%lld Credits  /  %s"),
				Transaction.AmountCredits >= 0 ? TEXT("+") : TEXT(""),
				Transaction.AmountCredits,
				Transaction.Category == TEXT("FlightCompletion")
					? TEXT("Safe visit reward")
					: *Transaction.Explanation);
		}

		int32 RatingTotal = 0;
		FString RatingBreakdown;
		for (const FRatingContribution& Contribution : State.RatingContributions)
		{
			RatingTotal += Contribution.Value;
			RatingBreakdown += FString::Printf(
				TEXT("%s%s +%d"),
				RatingBreakdown.IsEmpty() ? TEXT("") : TEXT("  |  "),
				*RatingName(Contribution.Component),
				Contribution.Value);
		}
		View.Rating = FString::Printf(
			TEXT("RATING %d  |  VISITS %d%s%s"),
			RatingTotal,
			State.Airframe.VisitCount,
			RatingBreakdown.IsEmpty() ? TEXT("") : TEXT("  |  "),
			*RatingBreakdown);
		View.Caption = Query.LatestCaption.IsEmpty()
			? TEXT("RADIO  •  CAPTIONS ON")
			: TEXT("RADIO  •  ") + Query.LatestCaption;

		View.bCanBuild = Query.bInitialized && Query.ConstructionStage == EConstructionStage::None;
		View.bCanCancelBuild =
			Query.ConstructionStage == EConstructionStage::Funded ||
			Query.ConstructionStage == EConstructionStage::AwaitingDelivery;
		View.bCanOpen = Query.ConstructionStage == EConstructionStage::ReadyToOpen;
		View.bCanClose =
			Query.bAirportOpen &&
			(Query.FlightState == EFlightState::None || Query.FlightState == EFlightState::Completed);
		View.bCanPinOffer = Query.OfferState == EOfferState::Available && !State.Offer.bPinned;
		View.bCanDeclineOffer = Query.OfferState == EOfferState::Available;
		View.bCanAcceptOffer = Query.OfferState == EOfferState::Available;
		View.bCanSchedule = Query.OfferState == EOfferState::Accepted;
		View.bCanRequestRecovery = Query.bInitialized && Query.Credits < 500;
		return View;
	}
}
