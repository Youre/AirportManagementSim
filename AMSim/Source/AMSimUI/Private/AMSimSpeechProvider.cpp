#include "AMSimSpeechProvider.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Sound/SoundWaveProcedural.h"
#include "TextToSpeechEngineSubsystem.h"

namespace
{
	const FName RadioChannel(TEXT("AMSim.Radio"));

	class FAMSimLocalSpeechProvider final : public IAMSimSpeechProvider
	{
	public:
		virtual bool Initialize() override
		{
			bAudioOutputDisabled = FParse::Param(FCommandLine::Get(), TEXT("NoSound"));
			if (bAudioOutputDisabled)
			{
				return false;
			}
#if !UE_BUILD_SHIPPING
			if (FParse::Param(FCommandLine::Get(), TEXT("AMSimDisableLocalSpeech")))
			{
				return false;
			}
#endif
			if (!GEngine)
			{
				return false;
			}
			SpeechSubsystem = GEngine->GetEngineSubsystem<UTextToSpeechEngineSubsystem>();
			if (!SpeechSubsystem)
			{
				return false;
			}
			if (!SpeechSubsystem->DoesChannelExist(RadioChannel))
			{
				SpeechSubsystem->AddDefaultChannel(RadioChannel);
			}
			SpeechSubsystem->ActivateChannel(RadioChannel);
			SpeechSubsystem->SetVolumeOnChannel(RadioChannel, 0.72f);
			SpeechSubsystem->SetRateOnChannel(RadioChannel, 0.58f);
			return SpeechSubsystem->DoesChannelExist(RadioChannel) &&
				SpeechSubsystem->IsChannelActive(RadioChannel);
		}

		virtual void Speak(UWorld* World, const FString& Caption) override
		{
			if (bAudioOutputDisabled)
			{
				return;
			}
			if (SpeechSubsystem && SpeechSubsystem->IsChannelActive(RadioChannel))
			{
				SpeechSubsystem->SpeakOnChannel(RadioChannel, Caption);
				return;
			}
			PlayFallbackCue(World);
		}

		virtual void Shutdown() override
		{
			if (SpeechSubsystem && SpeechSubsystem->DoesChannelExist(RadioChannel))
			{
				SpeechSubsystem->StopSpeakingOnChannel(RadioChannel);
				SpeechSubsystem->RemoveChannel(RadioChannel);
			}
			SpeechSubsystem = nullptr;
			FallbackCue.Reset();
		}

		virtual FName GetProviderId() const override
		{
			return SpeechSubsystem ? TEXT("Local.Flite") : TEXT("Local.RadioCue");
		}

	private:
		void PlayFallbackCue(UWorld* World)
		{
			if (!World)
			{
				return;
			}
			if (!FallbackCue.IsValid())
			{
				FallbackCue.Reset(NewObject<USoundWaveProcedural>());
				FallbackCue->SetSampleRate(48000);
				FallbackCue->NumChannels = 1;
				FallbackCue->Duration = 0.12f;
				FallbackCue->SoundGroup = SOUNDGROUP_UI;
			}

			constexpr int32 SampleRate = 48000;
			constexpr int32 SampleCount = SampleRate * 12 / 100;
			TArray<int16> Samples;
			Samples.SetNumUninitialized(SampleCount);
			for (int32 Index = 0; Index < SampleCount; ++Index)
			{
				const double Envelope = 1.0 - static_cast<double>(Index) / SampleCount;
				const double Phase = 2.0 * UE_DOUBLE_PI * 880.0 * Index / SampleRate;
				Samples[Index] = static_cast<int16>(
					FMath::Sin(Phase) * Envelope * 6000.0);
			}
			FallbackCue->QueueAudio(
				reinterpret_cast<const uint8*>(Samples.GetData()),
				Samples.Num() * sizeof(int16));
			UGameplayStatics::PlaySound2D(World, FallbackCue.Get(), 0.45f);
		}

		TObjectPtr<UTextToSpeechEngineSubsystem> SpeechSubsystem;
		TStrongObjectPtr<USoundWaveProcedural> FallbackCue;
		bool bAudioOutputDisabled = false;
	};
}

TUniquePtr<IAMSimSpeechProvider> CreateAMSimLocalSpeechProvider()
{
	return MakeUnique<FAMSimLocalSpeechProvider>();
}
