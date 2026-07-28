#include "AMSimRootScreen.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimPhase1Fixture.h"
#include "Components/EditableTextBox.h"
#include "Engine/World.h"

void UAMSimRootScreen::CreateAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CreateAirport;
	Command.AirportName = AirportNameEntry
		? AirportNameEntry->GetText().ToString().TrimStartAndEnd()
		: TEXT("Riverbend Field");
	Command.MapId = AMSim::GetPhase1Fixture().MapId;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Airport identity created.") : TEXT("Enter a valid airport name."),
		bAccepted);
}

void UAMSimRootScreen::CommitStarterPlan()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
	Command.Proposal = AMSim::CreateDefaultStarterPlan();
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Starter airfield funded; delivery is underway.") : TEXT("Starter plan was rejected."),
		bAccepted);
}

void UAMSimRootScreen::ToggleConstructionProposal()
{
	if (!ConstructionProposalView)
	{
		return;
	}
	if (ConstructionProposalView->IsProposalOpen())
	{
		ConstructionProposalView->CloseProposal();
	}
	else
	{
		ConstructionProposalView->OpenProposal();
	}
}

void UAMSimRootScreen::CancelStarterPlan()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CancelStarterPlan;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Project cancelled before delivery; 3,400 Credits refunded.")
			: TEXT("The project can only be cancelled before materials arrive."),
		bAccepted);
}

void UAMSimRootScreen::OpenAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::OpenAirport;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Riverbend Field is open.") : TEXT("Opening prerequisites are not complete."),
		bAccepted);
}

void UAMSimRootScreen::CloseAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CloseAirport;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Airfield closed safely.") : TEXT("Complete the active visit before closing."),
		bAccepted);
}

void UAMSimRootScreen::PinOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::PinStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Offer pinned for review.") : TEXT("Only an available offer can be pinned."),
		bAccepted);
}

void UAMSimRootScreen::DeclineOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::DeclineStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Offer declined. Close and reopen for a fresh starter offer.")
			: TEXT("Only an available offer can be declined."),
		bAccepted);
}

void UAMSimRootScreen::AcceptOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::AcceptStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("First-flight contract accepted.") : TEXT("Offer could not be accepted."),
		bAccepted);
}

void UAMSimRootScreen::ScheduleFlight()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::ScheduleStarterFlight;
	Command.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
	Command.ScheduledArrivalGameMilliseconds =
		Subsystem->GetRecommendedStarterArrivalTime();
	const bool bAccepted =
		Subsystem->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Arrival scheduled in the next five-minute slot.")
			: TEXT("Schedule selection was rejected."),
		bAccepted);
}

void UAMSimRootScreen::RequestRecovery()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (Subsystem->GetPhase2Query().bInitialized)
	{
		AMSim::FPhase2Command Phase2Command;
		Phase2Command.Type = AMSim::EPhase2CommandType::RequestRecovery;
		SubmitPhase2Command(
			Phase2Command,
			TEXT("Phase 2 continuity grant recorded in the shared ledger."));
		return;
	}
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::RequestRecovery;
	const bool bAccepted =
		Subsystem->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Recovery assistance granted and recorded.")
			: TEXT("Recovery is not currently eligible."),
		bAccepted);
}
