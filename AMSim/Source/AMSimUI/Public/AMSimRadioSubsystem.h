#pragma once

#include "AMSimSpeechProvider.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AMSimRadioSubsystem.generated.h"

class AMSIMUI_API FAMSimCaptionDeduplicator
{
public:
	bool Accept(FName Channel, const FString& Caption);
	void Reset();

private:
	TMap<FName, FString> LastCaptionByChannel;
};

class AMSIMUI_API FAMSimSpeechQueue
{
public:
	void Enqueue(const FString& Caption);
	bool TryBeginNext(bool bProviderSpeaking, FString& OutCaption);
	void Reset();
	int32 Num() const { return PendingCaptions.Num(); }

private:
	TArray<FString> PendingCaptions;
};

UCLASS()
class AMSIMUI_API UAMSimRadioSubsystem final
	: public UGameInstanceSubsystem,
	  public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(
		FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	bool PresentCaption(
		UWorld* World,
		FName Channel,
		const FString& Caption);
	bool IsSpeechReady() const { return bSpeechReady; }
	FName GetProviderId() const;
	int32 GetPendingSpeechCount() const { return SpeechQueue.Num(); }

private:
	void InitializeProviderIfNeeded();
	void PumpSpeech();

	TUniquePtr<IAMSimSpeechProvider> SpeechProvider;
	FAMSimCaptionDeduplicator Deduplicator;
	FAMSimSpeechQueue SpeechQueue;
	TWeakObjectPtr<UWorld> PresentationWorld;
	bool bInitializationAttempted = false;
	bool bSpeechReady = false;
};
