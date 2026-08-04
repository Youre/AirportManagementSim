#include "AMSimRootScreen.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimExpandingToolButton.h"
#include "AMSimWorldPresenter.h"
#include "EngineUtils.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1OperationsHubView.h"
#include "AMSimPhase1StaffView.h"
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
	const UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (Subsystem &&
		Subsystem->GetPhase1Query().ConstructionStage != AMSim::EConstructionStage::None)
	{
		OpenPhase1OperationsPage(AMSim::EPhase1OperationsPage::Airfield);
	}
	else if (ConstructionProposalView->IsProposalOpen())
	{
		ConstructionProposalView->CloseProposal();
	}
	else
	{
		ConstructionProposalView->OpenProposal();
	}
}

void UAMSimRootScreen::BindPhase1UtilityNavigation()
{
	ScheduleNavigationButton = NavigationButtons.IsValidIndex(1)
		? NavigationButtons[1] : nullptr;
	StaffButton = NavigationButtons.IsValidIndex(2) ? NavigationButtons[2] : nullptr;
	OverlayButton = NavigationButtons.IsValidIndex(3) ? NavigationButtons[3] : nullptr;
	if (ScheduleNavigationButton)
	{
		ScheduleNavigationButton->OnActivated.BindUObject(
			this, &UAMSimRootScreen::ToggleSchedulePanel);
	}
	if (StaffButton)
	{
		StaffButton->OnActivated.BindUObject(this, &UAMSimRootScreen::ToggleStaffPanel);
	}
	if (OverlayButton)
	{
		OverlayButton->OnActivated.BindUObject(this, &UAMSimRootScreen::ToggleOverlayPanel);
	}
}

void UAMSimRootScreen::ToggleSchedulePanel()
{
	OpenPhase1OperationsPage(AMSim::EPhase1OperationsPage::Schedule);
}

void UAMSimRootScreen::ToggleOverlayPanel()
{
	OpenPhase1OperationsPage(AMSim::EPhase1OperationsPage::Overlays);
}

void UAMSimRootScreen::OpenPhase1OperationsPage(
	const AMSim::EPhase1OperationsPage Page)
{
	if (!Phase1OperationsHubView)
	{
		return;
	}
	if (ConstructionProposalView && ConstructionProposalView->IsProposalOpen())
	{
		ConstructionProposalView->CloseProposal();
	}
	if (StaffView)
	{
		StaffView->ClosePanel();
	}
	const bool bClose = Phase1OperationsHubView->IsPanelOpen() &&
		Phase1OperationsHubView->GetPage() == Page;
	if (bClose)
	{
		Phase1OperationsHubView->ClosePanel();
		return;
	}
	Phase1OperationsHubView->OpenPage(Page);
	if (UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>())
	{
		Phase1OperationsHubView->RefreshFromSnapshot(
			Subsystem->GetPhase1Query(),
			Subsystem->GetSimulation().GetPhase1State(),
			Phase1OverlayMode);
	}
}

void UAMSimRootScreen::RefreshPhase1OperationsHub(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	if (!Phase1OperationsHubView)
	{
		return;
	}
	if (!Phase1OperationsHubView->OnChooseArrival.IsBound())
	{
		Phase1OperationsHubView->OnChooseArrival.BindUObject(
			this, &UAMSimRootScreen::ScheduleFlight);
		Phase1OperationsHubView->OnWatchAtOne.BindUObject(
			this, &UAMSimRootScreen::WatchFirstVisitAtOne);
		Phase1OperationsHubView->OnAdvanceToArrival.BindUObject(
			this, &UAMSimRootScreen::AdvanceToFirstVisit);
		Phase1OperationsHubView->OnOverlaySelected.BindUObject(
			this, &UAMSimRootScreen::SelectPhase1Overlay);
	}
	if (bReturnToOneAtInbound &&
		Query.FlightState >= AMSim::EFlightState::Inbound &&
		Query.FlightState < AMSim::EFlightState::Completed)
	{
		bReturnToOneAtInbound = false;
		SubmitSpeed(1);
		SetInteractionMessage(
			TEXT("Riverbend 21 is inbound. Returned to 1x for the visible arrival."),
			true);
	}
	Phase1OperationsHubView->RefreshFromSnapshot(
		Query, State, Phase1OverlayMode);
}

void UAMSimRootScreen::WatchFirstVisitAtOne()
{
	bReturnToOneAtInbound = false;
	SubmitSpeed(1);
	SetInteractionMessage(
		TEXT("Watching the autonomous first visit at 1x."), true);
}

void UAMSimRootScreen::AdvanceToFirstVisit()
{
	const UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (Subsystem &&
		Subsystem->GetPhase1Query().FlightState == AMSim::EFlightState::Scheduled)
	{
		SubmitSpeed(8);
		bReturnToOneAtInbound = true;
		SetInteractionMessage(
			TEXT("Advancing at 8x; the game will return to 1x when the aircraft enters airspace."),
			true);
		return;
	}
	bReturnToOneAtInbound = false;
	SubmitSpeed(4);
	SetInteractionMessage(TEXT("Continuing the automatic visit at 4x."), true);
}

void UAMSimRootScreen::SelectPhase1Overlay(const int32 Mode)
{
	Phase1OverlayMode = FMath::Clamp(Mode, 0, 2);
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AAMSimWorldPresenter> It(World); It; ++It)
		{
			It->SetPhase1OverlayMode(Phase1OverlayMode);
			break;
		}
	}
	SetInteractionMessage(
		Phase1OverlayMode == 1
			? TEXT("Connections overlay: movement network cyan, road access amber.")
			: Phase1OverlayMode == 2
				? TEXT("Activity overlay: infrastructure muted, active operations emphasized.")
				: TEXT("Standard airfield view restored."),
		true);
	if (UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>())
	{
		Phase1OperationsHubView->RefreshFromSnapshot(
			Subsystem->GetPhase1Query(),
			Subsystem->GetSimulation().GetPhase1State(),
			Phase1OverlayMode);
	}
}

void UAMSimRootScreen::ToggleStaffPanel()
{
	if (StaffView)
	{
		if (Phase1OperationsHubView)
		{
			Phase1OperationsHubView->ClosePanel();
		}
		StaffView->TogglePanel();
	}
}

void UAMSimRootScreen::SetConstructionModeChrome(const bool bOpen)
{
	if (bOpen && StaffView)
	{
		StaffView->ClosePanel();
	}
	if (bOpen && Phase1OperationsHubView)
	{
		Phase1OperationsHubView->ClosePanel();
	}
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
			bOpen
				? ESlateVisibility::Collapsed
				: ESlateVisibility::SelfHitTestInvisible);
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
		Subsystem->GetRecommendedStarterArrivalTime();
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
	if (bAccepted)
	{
		AMSim::FPhase1Command PauseCommand;
		PauseCommand.Type = AMSim::EPhase1CommandType::SetPaused;
		PauseCommand.bPaused = true;
		Subsystem->SubmitPhase1Command(PauseCommand);
		bReturnToOneAtInbound = false;
	}
	SetInteractionMessage(
		bAccepted
			? TEXT("Arrival reserved and paused. Choose WATCH AT 1x or ADVANCE TO ARRIVAL in Schedule.")
			: Result == AMSim::EPhase1CommandResult::RejectedInvalidSchedule
				? TEXT("That time is no longer available. Choose a later slot.")
				: TEXT("Accept the offer and open the airport before scheduling."),
		bAccepted);
	AMSim::UIAudio::Play(
		this,
		bAccepted ? EAMSimUISound::ScheduleConfirm : EAMSimUISound::ScheduleRejected);
	if (bAccepted && Phase1OperationsHubView)
	{
		Phase1OperationsHubView->OpenPage(AMSim::EPhase1OperationsPage::Schedule);
	}
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
