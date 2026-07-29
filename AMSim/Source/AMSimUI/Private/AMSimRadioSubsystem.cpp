#include "AMSimRadioSubsystem.h"

#include "AMSimSpeechProvider.h"

bool FAMSimCaptionDeduplicator::Accept(
	const FName Channel,
	const FString& Caption)
{
	if (Channel.IsNone() || Caption.TrimStartAndEnd().IsEmpty())
	{
		return false;
	}
	if (const FString* Previous =
		LastCaptionByChannel.Find(Channel))
	{
		if (*Previous == Caption)
		{
			return false;
		}
	}
	LastCaptionByChannel.Add(Channel, Caption);
	return true;
}

void FAMSimCaptionDeduplicator::Reset()
{
	LastCaptionByChannel.Reset();
}

void UAMSimRadioSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SpeechProvider = CreateAMSimLocalSpeechProvider();
	bSpeechReady =
		SpeechProvider && SpeechProvider->Initialize();
}

void UAMSimRadioSubsystem::Deinitialize()
{
	if (SpeechProvider)
	{
		SpeechProvider->Shutdown();
		SpeechProvider.Reset();
	}
	Deduplicator.Reset();
	bSpeechReady = false;
	Super::Deinitialize();
}

bool UAMSimRadioSubsystem::PresentCaption(
	UWorld* World,
	const FName Channel,
	const FString& Caption)
{
	if (!Deduplicator.Accept(Channel, Caption))
	{
		return false;
	}
	if (SpeechProvider)
	{
		SpeechProvider->Speak(World, Caption);
	}
	return true;
}

FName UAMSimRadioSubsystem::GetProviderId() const
{
	return SpeechProvider
		? SpeechProvider->GetProviderId()
		: FName(TEXT("Local.RadioCue"));
}
