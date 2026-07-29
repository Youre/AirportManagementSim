#include "AMSimRootScreen.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase2Fixture.h"
#include "AMSimTerminalView.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"

namespace
{
	void SetPhase2Text(UTextBlock* Widget, const FString& Value)
	{
		if (Widget)
		{
			Widget->SetText(FText::FromString(Value));
		}
	}

	void SetActionState(UButton* Button, const bool bVisible, const bool bEnabled)
	{
		if (Button)
		{
			Button->SetVisibility(
				bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			Button->SetIsEnabled(bEnabled);
		}
	}
}

void UAMSimRootScreen::ToggleTerminalPresentation()
{
	if (!TerminalView)
	{
		return;
	}
	if (TerminalView->IsPresentationOpen())
	{
		TerminalView->ClosePresentation();
	}
	else
	{
		TerminalView->ShowPresentation();
	}
	RefreshFromSimulation();
}

void UAMSimRootScreen::CloseTerminalPresentation()
{
	if (TerminalView)
	{
		TerminalView->ClosePresentation();
	}
	RefreshFromSimulation();
}

bool UAMSimRootScreen::SubmitPhase2Command(
	AMSim::FPhase2Command Command,
	const FString& SuccessMessage)
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	const bool bAccepted = Subsystem &&
		Subsystem->SubmitPhase2Command(Command) ==
			AMSim::EPhase2CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? SuccessMessage : TEXT("That living-airport action is not available."),
		bAccepted);
	return bAccepted;
}

void UAMSimRootScreen::InitializePhase2()
{
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::InitializeLivingAirport;
	SubmitPhase2Command(Command, TEXT("Living-airport management is ready."));
}

void UAMSimRootScreen::SelectPhase2Specialization(
	const AMSim::EAirportSpecialization Specialization)
{
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::SelectSpecialization;
	Command.Specialization = Specialization;
	SubmitPhase2Command(
		Command,
		AMSim::SpecializationDisplayName(Specialization) + TEXT(" identity selected."));
}

void UAMSimRootScreen::SelectGeneralAviation()
{
	SelectPhase2Specialization(AMSim::EAirportSpecialization::GeneralAviation);
}

void UAMSimRootScreen::SelectFlightSchool()
{
	SelectPhase2Specialization(AMSim::EAirportSpecialization::FlightSchool);
}

void UAMSimRootScreen::SelectCharter()
{
	SelectPhase2Specialization(AMSim::EAirportSpecialization::Charter);
}

void UAMSimRootScreen::AcceptPhase2Contract()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase2State& State = Subsystem->GetSimulation().GetPhase2State();
	const AMSim::FPhase2ContractRecord* Contract = State.Contracts.FindByPredicate(
		[&State](const AMSim::FPhase2ContractRecord& Candidate)
			{
				return Candidate.Specialization == State.SelectedSpecialization &&
					!Candidate.bAccepted;
			});
	if (!Contract)
	{
		SetInteractionMessage(TEXT("Select an identity with an available contract."), false);
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::AcceptContract;
	Command.ContractId = Contract->Id;
	SubmitPhase2Command(Command, TEXT("Recurring contract added to the timetable."));
}

void UAMSimRootScreen::CancelPhase2Contract()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase2State& State = Subsystem->GetSimulation().GetPhase2State();
	const AMSim::FPhase2ContractRecord* Contract = State.Contracts.FindByPredicate(
		[&State](const AMSim::FPhase2ContractRecord& Candidate)
			{
				return Candidate.Specialization == State.SelectedSpecialization &&
					Candidate.bAccepted;
			});
	if (!Contract)
	{
		SetInteractionMessage(TEXT("No selected recurring agreement can be ended."), false);
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::CancelContract;
	Command.ContractId = Contract->Id;
	SubmitPhase2Command(Command, TEXT("Agreement ended; cancellation cost itemized."));
}

void UAMSimRootScreen::ReschedulePhase2Flight()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase2FlightRecord* Flight =
		Subsystem->GetSimulation().GetPhase2State().Flights.FindByPredicate(
			[](const AMSim::FPhase2FlightRecord& Candidate)
				{
					return Candidate.State == AMSim::EPhase2FlightState::Scheduled;
				});
	if (!Flight)
	{
		SetInteractionMessage(TEXT("No future flight is available to reschedule."), false);
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::RescheduleFlight;
	Command.FlightId = Flight->Id;
	Command.RequestedGameTimeMilliseconds =
		Flight->ScheduledArrivalGameMilliseconds + 15000;
	SubmitPhase2Command(Command, TEXT("Flight and protected stand buffer moved."));
}

void UAMSimRootScreen::DispatchPhase2Service()
{
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::DispatchNextService;
	SubmitPhase2Command(Command, TEXT("Next compatible service dispatched."));
}

void UAMSimRootScreen::TogglePhase2Runway()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::SetActiveRunway;
	Command.RunwayDirection =
		Subsystem->GetSimulation().GetPhase2State().ActiveRunwayDirection == TEXT("24")
			? FName(TEXT("06"))
			: FName(TEXT("24"));
	SubmitPhase2Command(Command, TEXT("Active runway updated with a published reason."));
}

void UAMSimRootScreen::TowPhase2Aircraft()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase2FlightRecord* Flight =
		Subsystem->GetSimulation().GetPhase2State().Flights.FindByPredicate(
			[](const AMSim::FPhase2FlightRecord& Candidate)
				{
					return Candidate.State == AMSim::EPhase2FlightState::TaxiIn ||
						Candidate.State == AMSim::EPhase2FlightState::Turnaround ||
						Candidate.State == AMSim::EPhase2FlightState::ReadyToDepart;
				});
	if (!Flight)
	{
		SetInteractionMessage(TEXT("No aircraft currently needs an eligible tow."), false);
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::RequestTow;
	Command.FlightId = Flight->Id;
	SubmitPhase2Command(Command, TEXT("Conflict-free tug route reserved."));
}

void UAMSimRootScreen::RotatePhase2TeamZone()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem || Subsystem->GetSimulation().GetPhase2State().Teams.IsEmpty())
	{
		return;
	}
	const AMSim::FPhase2StaffTeamRecord& Team =
		Subsystem->GetSimulation().GetPhase2State().Teams[0];
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::AssignTeamZone;
	Command.TeamId = Team.Id;
	Command.ZoneId = Team.ZoneId == TEXT("Zone.PatternAndRunway")
		? FName(TEXT("Zone.Airside"))
		: FName(TEXT("Zone.PatternAndRunway"));
	SubmitPhase2Command(Command, TEXT("Flight-operations coverage zone updated."));
}

void UAMSimRootScreen::PurchasePhase2Parcel()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem || Subsystem->GetSimulation().GetPhase2State().Parcels.IsEmpty())
	{
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::PurchaseParcel;
	Command.ParcelId = Subsystem->GetSimulation().GetPhase2State().Parcels[0].Id;
	SubmitPhase2Command(Command, TEXT("East meadow added to airport-owned land."));
}

void UAMSimRootScreen::StartPhase2Expansion()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem || Subsystem->GetSimulation().GetPhase2State().Parcels.IsEmpty())
	{
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::StartExpansion;
	Command.ParcelId = Subsystem->GetSimulation().GetPhase2State().Parcels[0].Id;
	SubmitPhase2Command(Command, TEXT("East-apron project funded with closure guidance."));
}

void UAMSimRootScreen::RespondPhase2Incident()
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	AMSim::FPhase2Command Command;
	Command.Type = AMSim::EPhase2CommandType::RespondToIncident;
	Command.IncidentId = Subsystem->GetSimulation().GetPhase2State().Incident.Id;
	SubmitPhase2Command(Command, TEXT("Emergency-readiness team dispatched."));
}

void UAMSimRootScreen::RefreshPhase2Presentation(
	const AMSim::FPhase2QuerySnapshot& Query,
	const AMSim::FPhase2State& State,
	const AMSim::FPhase1State& Phase1State)
{
	CurrentPhase2ViewState = AMSim::MakePhase2ViewState(Query, State, Phase1State);
	const bool bShowPanel = CurrentPhase2ViewState.bUnlocked || Query.bInitialized;
	if (Phase2Panel)
	{
		Phase2Panel->SetVisibility(
			bShowPanel ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	SetPhase2Text(Phase2StatusText, CurrentPhase2ViewState.Status);
	SetPhase2Text(Phase2OperationsText, CurrentPhase2ViewState.Operations);
	SetPhase2Text(Phase2WeatherText, CurrentPhase2ViewState.Weather);
	SetPhase2Text(Phase2StaffText, CurrentPhase2ViewState.Staff);
	SetPhase2Text(Phase2TenantText, CurrentPhase2ViewState.Tenant);
	SetPhase2Text(Phase2EconomyText, CurrentPhase2ViewState.Economy);

	SetActionState(
		InitializePhase2Button,
		bShowPanel && !Query.bInitialized,
		CurrentPhase2ViewState.bUnlocked);
	SetActionState(SelectGAButton, Query.bInitialized, CurrentPhase2ViewState.bCanSelectIdentity);
	SetActionState(
		SelectSchoolButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanSelectIdentity);
	SetActionState(
		SelectCharterButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanSelectIdentity);
	SetActionState(
		AcceptPhase2ContractButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanAcceptContract);
	SetActionState(
		CancelPhase2ContractButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanCancelContract);
	SetActionState(
		ReschedulePhase2FlightButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanRescheduleFlight);
	SetActionState(
		DispatchPhase2ServiceButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanDispatchService);
	SetActionState(TogglePhase2RunwayButton, Query.bInitialized, Query.bInitialized);
	SetActionState(TowPhase2AircraftButton, Query.bInitialized, CurrentPhase2ViewState.bCanTow);
	SetActionState(RotatePhase2TeamZoneButton, Query.bInitialized, Query.bInitialized);
	SetActionState(
		PurchasePhase2ParcelButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanPurchaseParcel);
	SetActionState(
		StartPhase2ExpansionButton,
		Query.bInitialized,
		CurrentPhase2ViewState.bCanStartExpansion);
	SetActionState(
		RespondPhase2IncidentButton,
		CurrentPhase2ViewState.bCanRespondToIncident,
		CurrentPhase2ViewState.bCanRespondToIncident);

	if (Phase1ActivityCard)
	{
		Phase1ActivityCard->SetVisibility(
			Query.bInitialized ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	if (Phase1FlightCard)
	{
		Phase1FlightCard->SetVisibility(
			Query.bInitialized ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	if (Query.bInitialized)
	{
		SetPhase2Text(
			ClockText,
			FString::Printf(
				TEXT("OPERATING DAY %d · %s · %s"),
				Query.OperatingDay + 1,
				*AMSim::WeatherCategoryDisplayName(Query.WeatherCategory).ToUpper(),
				Phase1State.bPaused
					? TEXT("PAUSED")
					: *FString::Printf(TEXT("%dx"), Phase1State.SpeedMultiplier)));
		SetPhase2Text(StatusText, Query.PrimaryStatus);
		SetPhase2Text(CauseText, CurrentPhase2ViewState.Cause);
		SetPhase2Text(
			RemedyText,
			TEXT("NEXT: ") + CurrentPhase2ViewState.Remedy);
		SetPhase2Text(
			ObjectiveText,
			FString::Printf(
				TEXT("Operate safely through Day %d"),
				AMSim::GetPhase2Fixture().RequiredOperatingDays));
		if (CauseText)
		{
			CauseText->SetVisibility(
				CurrentPhase2ViewState.Cause.IsEmpty()
					? ESlateVisibility::Collapsed
					: ESlateVisibility::Visible);
		}
		if (RemedyText)
		{
			RemedyText->SetVisibility(
				CurrentPhase2ViewState.Remedy.IsEmpty()
					? ESlateVisibility::Collapsed
					: ESlateVisibility::Visible);
		}
		if (BuildButton)
		{
			BuildButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (CloseButton)
		{
			CloseButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
