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
	bInitializationAttempted = false;
	bSpeechReady = false;
}

void UAMSimRadioSubsystem::Deinitialize()
{
	if (SpeechProvider)
	{
		SpeechProvider->Shutdown();
		SpeechProvider.Reset();
	}
	Deduplicator.Reset();
	bInitializationAttempted = false;
	bSpeechReady = false;
	Super::Deinitialize();
}

void UAMSimRadioSubsystem::InitializeProviderIfNeeded()
{
	if (bInitializationAttempted)
	{
		return;
	}
	bInitializationAttempted = true;
	bSpeechReady =
		SpeechProvider && SpeechProvider->Initialize();
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
	InitializeProviderIfNeeded();
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
