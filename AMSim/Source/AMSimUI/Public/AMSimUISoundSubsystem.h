#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMSimUISoundSubsystem.generated.h"

class USoundBase;

UENUM()
enum class EAMSimUISound : uint8
{
	HoverSoft,
	FocusTick,
	ClickPrimary,
	ClickSecondary,
	TabSwitch,
	PanelOpen,
	PanelClose,
	Back,
	BuildStart,
	BuildPlace,
	BuildSnap,
	BuildConfirm,
	BuildCancel,
	InvalidGeometry,
	OfferAvailable,
	OfferAccept,
	OfferDecline,
	ScheduleConfirm,
	ScheduleRejected,
	Notification,
	SaveSuccess,
	LoadSuccess,
	LoadFailure,
	WarningAttention,
	ObjectiveComplete,
	CapabilityUnlock,
	IncidentAlert,
	RecoverySuccess,
	Count UMETA(Hidden)
};

class AMSIMUI_API FAMSimUISoundCooldown
{
public:
	bool Accept(EAMSimUISound Cue, double NowSeconds, double CooldownSeconds);
	void Reset();

private:
	TMap<EAMSimUISound, double> LastPlayedSeconds;
};

UCLASS()
class AMSIMUI_API UAMSimUISoundSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAMSimUISoundSubsystem();

	void Play(EAMSimUISound Cue);
	USoundBase* GetCue(EAMSimUISound Cue) const;
	bool HasCompleteCueSet(TArray<FName>* MissingCueIds = nullptr) const;

	static USoundBase* GetDefaultCue(EAMSimUISound Cue);
	static const TCHAR* GetStableCueId(EAMSimUISound Cue);
	static TArray<FName> RequiredCueIds();
	static bool IsPlaybackDisabled();

private:
	UPROPERTY()
	TArray<TObjectPtr<USoundBase>> Cues;

	FAMSimUISoundCooldown Cooldown;
};

namespace AMSim::UIAudio
{
	AMSIMUI_API void Play(const UObject* WorldContext, EAMSimUISound Cue);
}
