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

void FAMSimSpeechQueue::Enqueue(const FString& Caption)
{
	if (!Caption.TrimStartAndEnd().IsEmpty())
	{
		PendingCaptions.Add(Caption);
	}
}

bool FAMSimSpeechQueue::TryBeginNext(
	const bool bProviderSpeaking,
	FString& OutCaption)
{
	if (bProviderSpeaking || PendingCaptions.IsEmpty())
	{
		return false;
	}
	OutCaption = MoveTemp(PendingCaptions[0]);
	PendingCaptions.RemoveAt(0);
	return true;
}

void FAMSimSpeechQueue::Reset()
{
	PendingCaptions.Reset();
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
	SpeechQueue.Reset();
	PresentationWorld.Reset();
	bInitializationAttempted = false;
	bSpeechReady = false;
	Super::Deinitialize();
}

void UAMSimRadioSubsystem::Tick(const float DeltaTime)
{
	PumpSpeech();
}

bool UAMSimRadioSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject) && SpeechQueue.Num() > 0;
}

TStatId UAMSimRadioSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(
		UAMSimRadioSubsystem,
		STATGROUP_Tickables);
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
	PresentationWorld = World;
	SpeechQueue.Enqueue(Caption);
	PumpSpeech();
	return true;
}

void UAMSimRadioSubsystem::PumpSpeech()
{
	if (!SpeechProvider)
	{
		SpeechQueue.Reset();
		return;
	}
	FString Caption;
	if (SpeechQueue.TryBeginNext(SpeechProvider->IsSpeaking(), Caption))
	{
		SpeechProvider->Speak(PresentationWorld.Get(), Caption);
	}
}

FName UAMSimRadioSubsystem::GetProviderId() const
{
	return SpeechProvider
		? SpeechProvider->GetProviderId()
		: FName(TEXT("Local.RadioCue"));
}
