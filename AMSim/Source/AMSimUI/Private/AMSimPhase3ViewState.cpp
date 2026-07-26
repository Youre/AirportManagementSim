#include "AMSimPhase3ViewState.h"

#include "Algo/Count.h"

namespace AMSim
{
	namespace
	{
		float JourneyProgress(const FString& Step)
		{
			static const TArray<FString> OrderedSteps = {
				TEXT("Approaching airport"),
				TEXT("Landside entry"),
				TEXT("Check-in & bag drop"),
				TEXT("Security queue"),
				TEXT("Screening"),
				TEXT("Gate area"),
				TEXT("Boarding"),
				TEXT("On aircraft"),
				TEXT("Completed")};
			const int32 Index = OrderedSteps.IndexOfByKey(Step);
			return Index == INDEX_NONE
				? 0.12f
				: FMath::Clamp(
					static_cast<float>(Index + 1) /
						static_cast<float>(OrderedSteps.Num()),
					0.08f,
					1.0f);
		}
	}

	FPhase3ViewState MakePhase3ViewState(
		const FPhase3QuerySnapshot& Query,
		const FPhase3State& State)
	{
		FPhase3ViewState View;
		View.Revision = Query.Revision;
		View.Status = Query.PrimaryStatus;
		View.Flight = Query.FlightCode.IsEmpty()
			? TEXT("FIRST PASSENGER SERVICE")
			: FString::Printf(
				TEXT("%s  ·  %s  ·  GATE A1"),
				*Query.FlightCode,
				*Query.FlightStatus.ToUpper());
		View.PassengerCounts = FString::Printf(
			TEXT("%d DEPARTING  ·  %d ARRIVING  ·  %d COMPLETE"),
			Query.DepartingPassengerCount,
			Query.ArrivingPassengerCount,
			Query.CompletedPassengerCount);
		View.Security = FString::Printf(
			TEXT("%s  ·  %d IN FLOW"),
			Query.bSecurityIntegrityValid ? TEXT("CONTROLLED") : TEXT("ROUTE CHECK"),
			Query.SecurityQueueCount);
		View.FeaturedParty = Query.FeaturedPartyName.IsEmpty()
			? TEXT("MAYA'S PARTY  ·  4 TRAVELLERS")
			: FString::Printf(
				TEXT("%s  ·  %d TRAVELLERS"),
				*Query.FeaturedPartyName.ToUpper(),
				Query.FeaturedPartyMembers);
		View.FeaturedStep = Query.FeaturedStep.IsEmpty()
			? TEXT("Waiting for the first passenger service")
			: Query.FeaturedStep;
		View.FeaturedNeeds = Query.FeaturedNeeds.IsEmpty()
			? TEXT("No active passenger needs")
			: Query.FeaturedNeeds;
		View.FeaturedRoute = Query.FeaturedRoute.IsEmpty()
			? TEXT("Curb → Check-in → Security → Gate A1")
			: Query.FeaturedRoute;
		View.Baggage = Query.BaggageSummary.IsEmpty()
			? TEXT("Baggage system ready")
			: Query.BaggageSummary;
		View.Landside = Query.LandsideSummary.IsEmpty()
			? TEXT("Car · taxi · bus")
			: Query.LandsideSummary;
		View.Construction = Query.bInitialized
			? FString::Printf(
				TEXT("%d/%d ROUTES CONNECTED"),
				Query.ConnectedCount,
				Query.RequiredConnectionCount)
			: TEXT("DOMESTIC TERMINAL NOT STARTED");
		View.FeaturedTimeConfidencePercent =
			Query.FeaturedTimeConfidencePercent > 0
				? Query.FeaturedTimeConfidencePercent
				: 86;
		View.FeaturedStepProgress = JourneyProgress(Query.FeaturedStep);
		View.bUnlocked = Query.bUnlocked;
		View.bInitialized = Query.bInitialized;
		View.bShowTerminal =
			Query.bInitialized &&
			Query.TerminalStage >= ETerminalConstructionStage::ShellReady;
		View.bCanInitialize = Query.bUnlocked && !Query.bInitialized;
		View.bCanFund =
			Query.bInitialized &&
			Query.TerminalStage == ETerminalConstructionStage::None;
		View.bCanConnect =
			Query.TerminalStage == ETerminalConstructionStage::ShellReady &&
			Query.ConnectedCount < Query.RequiredConnectionCount;
		View.bCanOpen =
			Query.TerminalStage == ETerminalConstructionStage::ShellReady &&
			Query.ConnectedCount == Query.RequiredConnectionCount &&
			!Query.bTerminalOpen;
		View.bCanSchedule =
			Query.bTerminalOpen &&
			Query.FlightState == EPhase3FlightState::Unscheduled;
		View.bCanToggleSecurity =
			Query.bInitialized &&
			State.Checkpoint.Id.IsValid();
		View.bCanRequestAssistance =
			State.Parties.ContainsByPredicate(
				[](const FPassengerPartyRecord& Party)
					{
						return Party.bRequiresAccessibleRoute &&
							!Party.bAssistanceAssigned;
					});
		View.bCanResolveBaggage =
			State.Bags.ContainsByPredicate(
				[](const FBagRecord& Bag)
					{
						return Bag.JourneyState == EBagJourneyState::Exception;
					});
		return View;
	}
}
