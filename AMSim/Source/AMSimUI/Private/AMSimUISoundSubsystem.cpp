#include "AMSimUISoundSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	struct FUISoundMetadata
	{
		EAMSimUISound Cue;
		const TCHAR* StableId;
		const TCHAR* AssetPath;
		double CooldownSeconds;
	};

	const FUISoundMetadata CueMetadata[] = {
		{EAMSimUISound::HoverSoft, TEXT("ui_hover_soft"), TEXT("/Game/Audio/UI/ui_hover_soft.ui_hover_soft"), 0.08},
		{EAMSimUISound::FocusTick, TEXT("ui_focus_tick"), TEXT("/Game/Audio/UI/ui_focus_tick.ui_focus_tick"), 0.06},
		{EAMSimUISound::ClickPrimary, TEXT("ui_click_primary"), TEXT("/Game/Audio/UI/ui_click_primary.ui_click_primary"), 0.05},
		{EAMSimUISound::ClickSecondary, TEXT("ui_click_secondary"), TEXT("/Game/Audio/UI/ui_click_secondary.ui_click_secondary"), 0.05},
		{EAMSimUISound::TabSwitch, TEXT("ui_tab_switch"), TEXT("/Game/Audio/UI/ui_tab_switch.ui_tab_switch"), 0.08},
		{EAMSimUISound::PanelOpen, TEXT("ui_panel_open"), TEXT("/Game/Audio/UI/ui_panel_open.ui_panel_open"), 0.10},
		{EAMSimUISound::PanelClose, TEXT("ui_panel_close"), TEXT("/Game/Audio/UI/ui_panel_close.ui_panel_close"), 0.10},
		{EAMSimUISound::Back, TEXT("ui_back"), TEXT("/Game/Audio/UI/ui_back.ui_back"), 0.08},
		{EAMSimUISound::BuildStart, TEXT("ui_build_start"), TEXT("/Game/Audio/UI/ui_build_start.ui_build_start"), 0.10},
		{EAMSimUISound::BuildPlace, TEXT("ui_build_place"), TEXT("/Game/Audio/UI/ui_build_place.ui_build_place"), 0.08},
		{EAMSimUISound::BuildSnap, TEXT("ui_build_snap"), TEXT("/Game/Audio/UI/ui_build_snap.ui_build_snap"), 0.14},
		{EAMSimUISound::BuildConfirm, TEXT("ui_build_confirm"), TEXT("/Game/Audio/UI/ui_build_confirm.ui_build_confirm"), 0.20},
		{EAMSimUISound::BuildCancel, TEXT("ui_build_cancel"), TEXT("/Game/Audio/UI/ui_build_cancel.ui_build_cancel"), 0.12},
		{EAMSimUISound::InvalidGeometry, TEXT("ui_invalid_geometry"), TEXT("/Game/Audio/UI/ui_invalid_geometry.ui_invalid_geometry"), 0.25},
		{EAMSimUISound::OfferAvailable, TEXT("ui_offer_available"), TEXT("/Game/Audio/UI/ui_offer_available.ui_offer_available"), 0.35},
		{EAMSimUISound::OfferAccept, TEXT("ui_offer_accept"), TEXT("/Game/Audio/UI/ui_offer_accept.ui_offer_accept"), 0.20},
		{EAMSimUISound::OfferDecline, TEXT("ui_offer_decline"), TEXT("/Game/Audio/UI/ui_offer_decline.ui_offer_decline"), 0.20},
		{EAMSimUISound::ScheduleConfirm, TEXT("ui_schedule_confirm"), TEXT("/Game/Audio/UI/ui_schedule_confirm.ui_schedule_confirm"), 0.20},
		{EAMSimUISound::ScheduleRejected, TEXT("ui_schedule_rejected"), TEXT("/Game/Audio/UI/ui_schedule_rejected.ui_schedule_rejected"), 0.25},
		{EAMSimUISound::Notification, TEXT("ui_notification"), TEXT("/Game/Audio/UI/ui_notification.ui_notification"), 0.30},
		{EAMSimUISound::SaveSuccess, TEXT("ui_save_success"), TEXT("/Game/Audio/UI/ui_save_success.ui_save_success"), 0.25},
		{EAMSimUISound::LoadSuccess, TEXT("ui_load_success"), TEXT("/Game/Audio/UI/ui_load_success.ui_load_success"), 0.25},
		{EAMSimUISound::LoadFailure, TEXT("ui_load_failure"), TEXT("/Game/Audio/UI/ui_load_failure.ui_load_failure"), 0.30},
		{EAMSimUISound::WarningAttention, TEXT("ui_warning_attention"), TEXT("/Game/Audio/UI/ui_warning_attention.ui_warning_attention"), 0.35},
		{EAMSimUISound::ObjectiveComplete, TEXT("ui_objective_complete"), TEXT("/Game/Audio/UI/ui_objective_complete.ui_objective_complete"), 0.40},
		{EAMSimUISound::CapabilityUnlock, TEXT("ui_capability_unlock"), TEXT("/Game/Audio/UI/ui_capability_unlock.ui_capability_unlock"), 0.50},
		{EAMSimUISound::IncidentAlert, TEXT("ui_incident_alert"), TEXT("/Game/Audio/UI/ui_incident_alert.ui_incident_alert"), 0.50},
		{EAMSimUISound::RecoverySuccess, TEXT("ui_recovery_success"), TEXT("/Game/Audio/UI/ui_recovery_success.ui_recovery_success"), 0.40}};

	static_assert(
		UE_ARRAY_COUNT(CueMetadata) == static_cast<uint8>(EAMSimUISound::Count),
		"Every typed UI cue needs one cooker-visible asset reference.");

	const FUISoundMetadata* FindCueMetadata(const EAMSimUISound Cue)
	{
		const int32 Index = static_cast<int32>(Cue);
		return CueMetadata +
			FMath::Clamp(Index, 0, UE_ARRAY_COUNT(CueMetadata) - 1);
	}
}

bool FAMSimUISoundCooldown::Accept(
	const EAMSimUISound Cue,
	const double NowSeconds,
	const double CooldownSeconds)
{
	if (const double* LastPlayed = LastPlayedSeconds.Find(Cue))
	{
		if (NowSeconds - *LastPlayed < CooldownSeconds)
		{
			return false;
		}
	}
	LastPlayedSeconds.Add(Cue, NowSeconds);
	return true;
}

void FAMSimUISoundCooldown::Reset()
{
	LastPlayedSeconds.Reset();
}

UAMSimUISoundSubsystem::UAMSimUISoundSubsystem()
{
	Cues.SetNum(static_cast<int32>(EAMSimUISound::Count));
	for (const FUISoundMetadata& Metadata : CueMetadata)
	{
		ConstructorHelpers::FObjectFinder<USoundBase> CueFinder(
			Metadata.AssetPath);
		if (CueFinder.Succeeded())
		{
			Cues[static_cast<int32>(Metadata.Cue)] = CueFinder.Object;
		}
	}
}

void UAMSimUISoundSubsystem::Play(const EAMSimUISound Cue)
{
	USoundBase* Sound = GetCue(Cue);
	UWorld* World = GetWorld();
	const FUISoundMetadata* Metadata = FindCueMetadata(Cue);
	if (!Sound || !World || IsPlaybackDisabled() ||
		!Cooldown.Accept(Cue, FPlatformTime::Seconds(), Metadata->CooldownSeconds))
	{
		return;
	}
	UGameplayStatics::PlaySound2D(World, Sound);
}

USoundBase* UAMSimUISoundSubsystem::GetCue(const EAMSimUISound Cue) const
{
	const int32 Index = static_cast<int32>(Cue);
	return Cues.IsValidIndex(Index) ? Cues[Index] : nullptr;
}

bool UAMSimUISoundSubsystem::HasCompleteCueSet(
	TArray<FName>* MissingCueIds) const
{
	bool bComplete = Cues.Num() == static_cast<int32>(EAMSimUISound::Count);
	for (const FUISoundMetadata& Metadata : CueMetadata)
	{
		if (!GetCue(Metadata.Cue))
		{
			bComplete = false;
			if (MissingCueIds)
			{
				MissingCueIds->Add(Metadata.StableId);
			}
		}
	}
	return bComplete;
}

USoundBase* UAMSimUISoundSubsystem::GetDefaultCue(const EAMSimUISound Cue)
{
	return GetDefault<UAMSimUISoundSubsystem>()->GetCue(Cue);
}

const TCHAR* UAMSimUISoundSubsystem::GetStableCueId(const EAMSimUISound Cue)
{
	return FindCueMetadata(Cue)->StableId;
}

TArray<FName> UAMSimUISoundSubsystem::RequiredCueIds()
{
	TArray<FName> Result;
	Result.Reserve(UE_ARRAY_COUNT(CueMetadata));
	for (const FUISoundMetadata& Metadata : CueMetadata)
	{
		Result.Add(Metadata.StableId);
	}
	return Result;
}

bool UAMSimUISoundSubsystem::IsPlaybackDisabled()
{
	return FParse::Param(FCommandLine::Get(), TEXT("NoSound"));
}

namespace AMSim::UIAudio
{
	void Play(const UObject* WorldContext, const EAMSimUISound Cue)
	{
		const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		if (UAMSimUISoundSubsystem* Audio = GameInstance
			? GameInstance->GetSubsystem<UAMSimUISoundSubsystem>()
			: nullptr)
		{
			Audio->Play(Cue);
		}
	}
}
