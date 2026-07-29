#pragma once

#include "AMSimSpeechProvider.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMSimRadioSubsystem.generated.h"

class AMSIMUI_API FAMSimCaptionDeduplicator
{
public:
	bool Accept(FName Channel, const FString& Caption);
	void Reset();

private:
	TMap<FName, FString> LastCaptionByChannel;
};

UCLASS()
class AMSIMUI_API UAMSimRadioSubsystem final
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(
		FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool PresentCaption(
		UWorld* World,
		FName Channel,
		const FString& Caption);
	bool IsSpeechReady() const { return bSpeechReady; }
	FName GetProviderId() const;

private:
	TUniquePtr<IAMSimSpeechProvider> SpeechProvider;
	FAMSimCaptionDeduplicator Deduplicator;
	bool bSpeechReady = false;
};
