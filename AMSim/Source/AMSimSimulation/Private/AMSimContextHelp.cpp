#include "AMSimContextHelp.h"

namespace AMSim
{
	const TArray<FContextHelpDefinition>& GetPhase1ContextHelpCatalog()
	{
		static const TArray<FContextHelpDefinition> Catalog = {
			{
				TEXT("Help.Start.NameAirport"),
				TEXT("FIRST AIRPORT"),
				TEXT("Name your airport"),
				TEXT("Riverbend Plains is a forgiving temperate region. "
					"Choose a short airport name; each save keeps its own "
					"money, visitors, achievements, and progress."),
				TEXT("Enter an airport name and create the save.")
			},
			{
				TEXT("Help.Start.Build"),
				TEXT("STARTER PLAN"),
				TEXT("Build one connected airfield"),
				TEXT("The starter plan joins a grass runway, Taxiway A, "
					"Stand A1, road access, and an operations hut. Cyan "
					"proposal marks show where each part will go."),
				TEXT("Review the cost and commit the valid proposal.")
			},
			{
				TEXT("Help.Start.Open"),
				TEXT("READINESS"),
				TEXT("Open after inspection"),
				TEXT("Construction is complete, but the airport remains "
					"closed until you open it. Opening makes the runway, "
					"taxiway, and stand available to compatible traffic."),
				TEXT("Select Open Airport when every facility is ready.")
			},
			{
				TEXT("Help.Start.Offer"),
				TEXT("COMPATIBILITY"),
				TEXT("Check why the aircraft fits"),
				TEXT("Compatibility compares the aircraft with runway "
					"surface and length, taxi access, stand size, and "
					"required services. A green result still names the evidence."),
				TEXT("Review the evidence, then accept or decline the offer.")
			},
			{
				TEXT("Help.Start.Schedule"),
				TEXT("TIMETABLE"),
				TEXT("Choose an exact slot"),
				TEXT("A slot reserves the runway and Stand A1 around the "
					"visit. The timetable uses exact five-minute times so "
					"future flights can share the airport safely."),
				TEXT("Choose the recommended slot and assign Stand A1.")
			},
			{
				TEXT("Help.Start.Turnaround"),
				TEXT("AUTOMATIC DISPATCH"),
				TEXT("Watch the visit explain itself"),
				TEXT("The aircraft lands, taxis, parks, receives inspection "
					"and fuel, then departs automatically. Captions and the "
					"task cards show what is happening and why."),
				TEXT("Resume time and follow the aircraft through departure.")
			}
		};
		return Catalog;
	}

	const FContextHelpDefinition* FindPhase1ContextHelp(const FName HelpId)
	{
		return GetPhase1ContextHelpCatalog().FindByPredicate(
			[HelpId](const FContextHelpDefinition& Definition)
			{
				return Definition.Id == HelpId;
			});
	}

	const FContextHelpDefinition* SelectPhase1ContextHelp(
		const FPhase1State& State)
	{
		FName Candidate;
		if (!State.bInitialized)
		{
			Candidate = TEXT("Help.Start.NameAirport");
		}
		else if (State.Project.Stage == EConstructionStage::None)
		{
			Candidate = TEXT("Help.Start.Build");
		}
		else if (State.Project.Stage == EConstructionStage::ReadyToOpen)
		{
			Candidate = TEXT("Help.Start.Open");
		}
		else if (State.Offer.State == EOfferState::Available)
		{
			Candidate = TEXT("Help.Start.Offer");
		}
		else if (State.Offer.State == EOfferState::Accepted)
		{
			Candidate = TEXT("Help.Start.Schedule");
		}
		else if (State.Flight.State != EFlightState::None &&
			State.Flight.State != EFlightState::Completed)
		{
			Candidate = TEXT("Help.Start.Turnaround");
		}
		if (Candidate.IsNone() ||
			State.AcknowledgedContextHelp.Contains(Candidate))
		{
			return nullptr;
		}
		return FindPhase1ContextHelp(Candidate);
	}
}
