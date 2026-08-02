#include "AMSimRootScreen.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimWorldPresenter.h"
#include "EngineUtils.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimSchedulePickerView.h"
#include "AMSimUISoundSubsystem.h"
#include "Components/EditableTextBox.h"
#include "Components/Widget.h"
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
	if (bAccepted && SaveSlotEntry)
	{
		const FString RequestedSlot =
			SaveSlotEntry->GetText().ToString().TrimStartAndEnd();
		RefreshSaveSlots();
		const int32 RequestedIndex = SaveSlotIds.IndexOfByKey(RequestedSlot);
		if (RequestedIndex != INDEX_NONE)
		{
			SelectedSaveSlotIndex = RequestedIndex;
		}
	}
	SetInteractionMessage(
		bAccepted ? TEXT("Airport identity created.")
			: TEXT("Enter a valid airport name."),
		bAccepted);
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::Notification : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::BuildConfirm : EAMSimUISound::InvalidGeometry);
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

void UAMSimRootScreen::SetConstructionModeChrome(const bool bOpen)
{
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AAMSimWorldPresenter> It(World); It; ++It)
		{
			It->SetConstructionEditorOverlayVisible(bOpen);
		}
	}
	if (BuildModeLeftChrome)
	{
		BuildModeLeftChrome->SetVisibility(
			bOpen || bCompactLayoutActive
				? ESlateVisibility::Collapsed
				: ESlateVisibility::Visible);
	}
	if (BuildModeRightChrome)
	{
		BuildModeRightChrome->SetVisibility(
			bOpen || bCompactLayoutActive
				? ESlateVisibility::Collapsed
				: ESlateVisibility::Visible);
	}
	if (BuildModeFooterChrome)
	{
		BuildModeFooterChrome->SetVisibility(
			bOpen ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::BuildCancel : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::Notification : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::Back : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::Notification : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::OfferDecline : EAMSimUISound::WarningAttention);
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::OfferAccept : EAMSimUISound::WarningAttention);
}

void UAMSimRootScreen::ScheduleFlight()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (!Subsystem || !SchedulePickerView)
	{
		return;
	}
	const int64 FirstSlot =
		Subsystem->GetRecommendedStarterArrivalTime() +
		AMSim::GetPhase1Fixture().TimetableIncrementMilliseconds;
	TArray<int64> Options;
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Options.Add(
			FirstSlot +
			Index * AMSim::GetPhase1Fixture().TimetableIncrementMilliseconds);
	}
	SchedulePickerView->OpenPicker(
		Options,
		Subsystem->GetPhase1Query().GameTimeMilliseconds);
}

bool UAMSimRootScreen::SubmitScheduleAt(
	const int64 ScheduledArrivalGameMilliseconds)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (!Subsystem)
	{
		return false;
	}
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::ScheduleStarterFlight;
	Command.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
	Command.ScheduledArrivalGameMilliseconds = ScheduledArrivalGameMilliseconds;
	const AMSim::EPhase1CommandResult Result =
		Subsystem->SubmitPhase1Command(Command);
	const bool bAccepted = Result == AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted
			? TEXT("Arrival time reserved at Stand A1.")
			: Result == AMSim::EPhase1CommandResult::RejectedInvalidSchedule
				? TEXT("That time is no longer available. Choose a later slot.")
				: TEXT("Accept the offer and open the airport before scheduling."),
		bAccepted);
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::ScheduleConfirm : EAMSimUISound::ScheduleRejected);
	return bAccepted;
}

void UAMSimRootScreen::RequestRecovery()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (Subsystem->GetPhase2Query().bInitialized)
	{
		AMSim::FPhase2Command Phase2Command;
		Phase2Command.Type = AMSim::EPhase2CommandType::RequestRecovery;
		if (SubmitPhase2Command(
			Phase2Command,
			TEXT("Phase 2 continuity grant recorded in the shared ledger.")))
		{
			AMSim::UIAudio::Play(this, EAMSimUISound::RecoverySuccess);
		}
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
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::RecoverySuccess : EAMSimUISound::WarningAttention);
}
