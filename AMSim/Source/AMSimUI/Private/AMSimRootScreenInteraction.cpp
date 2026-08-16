#include "AMSimRootScreen.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimCameraPawn.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimReleaseGuideView.h"
#include "AMSimSaveLoadView.h"
#include "AMSimSchedulePickerView.h"
#include "AMSimTerminalView.h"
#include "AMSimUISoundSubsystem.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

void UAMSimRootScreen::ShowReleaseGuide()
{
	bOpenReleaseGuideWhenReady = true;
	if (ReleaseGuideView)
	{
		ReleaseGuideView->OpenGuide();
	}
}

void UAMSimRootScreen::ToggleReleaseGuide()
{
	if (!ReleaseGuideView)
	{
		return;
	}
	bOpenReleaseGuideWhenReady = !ReleaseGuideView->IsGuideOpen();
	if (bOpenReleaseGuideWhenReady)
	{
		ReleaseGuideView->OpenGuide();
	}
	else
	{
		ReleaseGuideView->CloseGuide();
	}
}

FReply UAMSimRootScreen::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	const FKey Button = InMouseEvent.GetEffectingButton();
	const bool bSupportedPanButton =
		Button == EKeys::MiddleMouseButton ||
		Button == EKeys::RightMouseButton;
	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(
		InMouseEvent.GetScreenSpacePosition());
	const FVector2D Size = InGeometry.GetLocalSize();
	const bool bOverWorld =
		Size.X > 0.0f &&
		Size.Y > 0.0f &&
		LocalPosition.X / Size.X >= 0.11f &&
		LocalPosition.X / Size.X <= 0.83f &&
		LocalPosition.Y / Size.Y >= 0.12f &&
		LocalPosition.Y / Size.Y <= 0.88f;
	if (!bSupportedPanButton ||
		!bOverWorld ||
		(ReleaseGuideView && ReleaseGuideView->IsGuideOpen()) ||
		(SchedulePickerView && SchedulePickerView->IsPickerOpen()) ||
		(SaveLoadView && SaveLoadView->IsPickerOpen()) ||
		(TerminalView && TerminalView->IsPresentationOpen()))
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	bWorldPanning = true;
	bRightMousePanning = Button == EKeys::RightMouseButton;
	return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
}

FReply UAMSimRootScreen::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	const FKey Button = InMouseEvent.GetEffectingButton();
	const bool bReleasedPanButton =
		(bRightMousePanning && Button == EKeys::RightMouseButton) ||
		(!bRightMousePanning && Button == EKeys::MiddleMouseButton);
	if (bWorldPanning && bReleasedPanButton)
	{
		bWorldPanning = false;
		bRightMousePanning = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UAMSimRootScreen::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!bWorldPanning)
	{
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}
	AAMSimCameraPawn* CameraPawn = GetOwningPlayer()
		? Cast<AAMSimCameraPawn>(GetOwningPlayer()->GetPawn())
		: nullptr;
	if (CameraPawn)
	{
		CameraPawn->PanByScreenDelta(InMouseEvent.GetCursorDelta());
	}
	return FReply::Handled();
}

FReply UAMSimRootScreen::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape &&
		TerminalView && TerminalView->IsPresentationOpen())
	{
		CloseTerminalPresentation();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UAMSimRootScreen::PauseSimulation()
{
	bReturnToOneAtInbound = false;
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	if (Subsystem->GetPhase1Query().bPaused)
	{
		SubmitSpeed(1);
	}
	else
	{
		AMSim::FPhase1Command Command;
		Command.Type = AMSim::EPhase1CommandType::SetPaused;
		Command.bPaused = true;
		const bool bAccepted =
			Subsystem->SubmitPhase1Command(Command) ==
			AMSim::EPhase1CommandResult::Accepted;
		SetInteractionMessage(
			bAccepted ? TEXT("Simulation paused.") : TEXT("Pause is not available."),
			bAccepted);
	}
}

void UAMSimRootScreen::SetSpeedOne()
{
	bReturnToOneAtInbound = false;
	SubmitSpeed(1);
}

void UAMSimRootScreen::SetSpeedTwo()
{
	bReturnToOneAtInbound = false;
	SubmitSpeed(2);
}

void UAMSimRootScreen::SetSpeedFour()
{
	bReturnToOneAtInbound = false;
	SubmitSpeed(4);
}

void UAMSimRootScreen::SetSpeedEight()
{
	bReturnToOneAtInbound = false;
	SubmitSpeed(8);
}

void UAMSimRootScreen::SubmitSpeed(const int32 Multiplier)
{
	UAMSimAirportSimulationSubsystem* Subsystem = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	if (!Subsystem)
	{
		return;
	}
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::SetSpeed;
	Command.SpeedMultiplier = Multiplier;
	Command.bPaused = false;
	const bool bAccepted =
		Subsystem->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted
			? FString::Printf(TEXT("Simulation running at %dx."), Multiplier)
			: TEXT("Speed control is not available."),
		bAccepted);
}

void UAMSimRootScreen::SaveGame()
{
	UAMSimAirportSimulationSubsystem* Simulation = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	UAMSimGameInstanceSubsystem* GameInstance = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
		: nullptr;
	if (!Simulation || !GameInstance)
	{
		return;
	}
	const FString SlotId = GetSelectedSaveSlotId();
	AMSim::FSaveMetadata Metadata;
	Metadata.PlayerLabel = SlotId;
	Metadata.AirportName = Simulation->GetPhase1Query().AirportName;
	Metadata.GameTimeMilliseconds =
		Simulation->GetPhase1Query().GameTimeMilliseconds;
	const AMSim::FSaveResult Result =
		GameInstance->SaveSnapshotAsync(
			SlotId,
			Simulation->CreateSnapshot(),
			MoveTemp(Metadata)).Get();
	SetInteractionMessage(
		Result.bSucceeded
			? FString::Printf(TEXT("Saved to slot %s."), *SlotId)
			: FString::Printf(TEXT("Save failed: %s"), *Result.Error),
		Result.bSucceeded);
	AMSim::UIAudio::Play(
		this,
		Result.bSucceeded
			? EAMSimUISound::SaveSuccess
			: EAMSimUISound::LoadFailure);
	if (Result.bSucceeded)
	{
		RefreshSaveSlots();
	}
}

void UAMSimRootScreen::LoadGame()
{
	UAMSimGameInstanceSubsystem* GameInstance = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
		: nullptr;
	if (!GameInstance || !SaveLoadView)
	{
		return;
	}
	SaveLoadView->OpenPicker(GameInstance->ListSaveSlots());
}

bool UAMSimRootScreen::LoadSlotById(const FString& SlotId)
{
	UAMSimAirportSimulationSubsystem* Simulation = GetWorld()
		? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
		: nullptr;
	UAMSimGameInstanceSubsystem* GameInstance = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
		: nullptr;
	if (!Simulation || !GameInstance)
	{
		return false;
	}
	AMSim::FSnapshot Snapshot;
	bool bUsedBackup = false;
	const bool bLoaded =
		GameInstance->LoadSnapshot(SlotId, Snapshot, bUsedBackup) &&
		Simulation->RestoreSnapshot(Snapshot);
	SetInteractionMessage(
		bLoaded
			? bUsedBackup
				? FString::Printf(
					TEXT("Loaded backup from slot %s."),
					*SlotId)
				: FString::Printf(TEXT("Loaded slot %s."), *SlotId)
			: FString::Printf(TEXT("No valid save exists in slot %s."), *SlotId),
		bLoaded);
	AMSim::UIAudio::Play(
		this,
		bLoaded ? EAMSimUISound::LoadSuccess : EAMSimUISound::LoadFailure);
	if (bLoaded)
	{
		// Save data restores simulation state. It must not preserve or infer a
		// full-screen UI destination, so return to the stable airport overview.
		if (TerminalView)
		{
			TerminalView->ClosePresentation();
		}
		RefreshSaveSlots();
		const int32 LoadedIndex = SaveSlotIds.IndexOfByKey(SlotId);
		if (LoadedIndex != INDEX_NONE)
		{
			SelectedSaveSlotIndex = LoadedIndex;
		}
	}
	return bLoaded;
}

void UAMSimRootScreen::RefreshSaveSlots()
{
	const FString PreviousSelection = GetSelectedSaveSlotId();
	SaveSlotIds.Reset();
	UAMSimGameInstanceSubsystem* GameInstance = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
		: nullptr;
	if (GameInstance)
	{
		for (const AMSim::FSaveSlotSummary& Summary :
			GameInstance->ListSaveSlots())
		{
			SaveSlotIds.AddUnique(Summary.SlotId);
		}
	}
	const FString EntrySlot = SaveSlotEntry
		? SaveSlotEntry->GetText().ToString().TrimStartAndEnd()
		: FString();
	if (!EntrySlot.IsEmpty())
	{
		SaveSlotIds.AddUnique(EntrySlot);
	}
	SaveSlotIds.AddUnique(TEXT("Phase1Auto"));
	SaveSlotIds.AddUnique(TEXT("Riverbend1"));
	SaveSlotIds.AddUnique(TEXT("Riverbend2"));
	SaveSlotIds.AddUnique(TEXT("Riverbend3"));
	SelectedSaveSlotIndex = FMath::Max(
		0,
		SaveSlotIds.IndexOfByKey(PreviousSelection));
}

FString UAMSimRootScreen::GetSelectedSaveSlotId() const
{
	return SaveSlotIds.IsValidIndex(SelectedSaveSlotIndex)
		? SaveSlotIds[SelectedSaveSlotIndex]
		: TEXT("Phase1Auto");
}

void UAMSimRootScreen::ToggleWindowMode()
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return;
	}
	const bool bCurrentlyWindowed =
		Settings->GetFullscreenMode() == EWindowMode::Windowed;
	if (bCurrentlyWindowed)
	{
		Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		SetButtonLabel(WindowModeButton, TEXT("WINDOWED"));
		SetInteractionMessage(TEXT("Borderless fullscreen enabled."), true);
	}
	else
	{
		Settings->SetFullscreenMode(EWindowMode::Windowed);
		Settings->SetScreenResolution(FIntPoint(1600, 900));
		SetButtonLabel(WindowModeButton, TEXT("BORDERLESS"));
		SetInteractionMessage(TEXT("Windowed mode enabled at 1600 x 900."), true);
	}
	Settings->ApplySettings(false);
	Settings->SaveSettings();
}

void UAMSimRootScreen::OpenRegionalPresentation()
{
	if (TerminalView)
	{
		TerminalView->ShowRegionalOperations();
	}
}

void UAMSimRootScreen::OpenAdvancedPresentation()
{
	if (TerminalView)
	{
		TerminalView->ShowAdvancedOperations();
	}
}

void UAMSimRootScreen::OpenMajorPresentation()
{
	if (TerminalView)
	{
		TerminalView->ShowMajorOperations();
	}
}

void UAMSimRootScreen::RefreshDestinationButtons(
	const AMSim::FPhase1QuerySnapshot& Phase1Query,
	const AMSim::FPhase3QuerySnapshot& Phase3Query,
	const AMSim::FPhase4QuerySnapshot& Phase4Query,
	const AMSim::FPhase5QuerySnapshot& Phase5Query,
	const AMSim::FPhase6QuerySnapshot& Phase6Query)
{
	const auto ApplyDestination =
		[](UButton* Button,
			const bool bUnlocked,
			const FString& Label,
			const FString& Requirement)
		{
			if (!Button)
			{
				return;
			}
			Button->SetVisibility(
				bUnlocked
					? ESlateVisibility::Visible
					: ESlateVisibility::Collapsed);
			Button->SetIsEnabled(bUnlocked);
			SetButtonLabel(
				Button,
				bUnlocked ? Label : Label + TEXT(" (LOCKED)"));
			Button->SetToolTipText(FText::FromString(
				bUnlocked
					? FString::Printf(TEXT("Open %s."), *Label)
					: Requirement));
		};
	ApplyDestination(
		TerminalNavigationButton,
		Phase1Query.bInitialized || Phase3Query.bUnlocked || Phase3Query.bInitialized,
		TEXT("TERMINAL"),
		TEXT("Create the airport to open the starter terminal."));
	ApplyDestination(
		RegionalNavigationButton,
		Phase4Query.bUnlocked || Phase4Query.bInitialized,
		TEXT("REGIONAL"),
		TEXT("Unlock regional operations to open this destination."));
	ApplyDestination(
		AdvancedNavigationButton,
		Phase5Query.bUnlocked || Phase5Query.bInitialized,
		TEXT("ADVANCED"),
		TEXT("Reach Advanced capability to open this destination."));
	ApplyDestination(
		MajorNavigationButton,
		Phase6Query.bUnlocked || Phase6Query.bInitialized,
		TEXT("MAJOR"),
		TEXT("Reach Major capability to open this destination."));
}

void UAMSimRootScreen::SetButtonLabel(
	UButton* Button,
	const FString& Label)
{
	if (Button)
	{
		if (UTextBlock* Text = Cast<UTextBlock>(Button->GetContent()))
		{
			Text->SetText(FText::FromString(Label));
		}
	}
}
