#include "AMSimGameInstanceSubsystem.h"
#include "AMSimSaveStore.h"
#include "AMSimSimulation.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"

void UAMSimGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SaveRootDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveSlots"));
	bReady = true;
#if !UE_BUILD_SHIPPING
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase0Smoke")))
	{
		const FString SmokeSlot = FPaths::Combine(SaveRootDirectory, TEXT("Phase0Smoke"));
		AMSim::FSimulation Simulation(0xA17F00D);
		Simulation.Step();
		SmokeChecksum = Simulation.CalculateChecksum();
		const AMSim::FSaveResult SaveResult =
			AMSim::FSaveStore::WriteAsync(SmokeSlot, Simulation.CreateSnapshot()).Get();
		AMSim::FSnapshot Loaded;
		bool bUsedBackup = false;
		AMSim::FSimulation Restored;
		bSmokeSaveLoadPassed =
			SaveResult.bSucceeded &&
			AMSim::FSaveStore::LoadCurrentOrBackup(SmokeSlot, Loaded, bUsedBackup) &&
			!bUsedBackup &&
			Restored.RestoreSnapshot(Loaded) &&
			Restored.CalculateChecksum() == SmokeChecksum;
		bPhase0SmokeActive = true;
	}
#endif
}

void UAMSimGameInstanceSubsystem::Deinitialize()
{
	bPhase0SmokeActive = false;
	bReady = false;
	SaveRootDirectory.Reset();
	Super::Deinitialize();
}

void UAMSimGameInstanceSubsystem::Tick(const float DeltaTime)
{
#if !UE_BUILD_SHIPPING
	FIntPoint ViewportSize = FIntPoint::ZeroValue;
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	}
	if (ViewportSize != FIntPoint(1920, 1080))
	{
		if (!bSmokeResolutionRequested && GEngine)
		{
			GEngine->Exec(nullptr, TEXT("r.SetRes 1920x1080w"));
			bSmokeResolutionRequested = true;
		}
		return;
	}
	SmokeWarmupSeconds += DeltaTime;
	if (!bSmokeScreenshotRequested && SmokeWarmupSeconds >= 0.5)
	{
		const FString ProofDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Phase0"));
		IFileManager::Get().MakeDirectory(*ProofDirectory, true);
		FScreenshotRequest::RequestScreenshot(
			FPaths::Combine(ProofDirectory, TEXT("proof-screen.png")),
			true,
			false);
		bSmokeScreenshotRequested = true;
	}
	if (SmokeWarmupSeconds < 1.0)
	{
		return;
	}

	SmokeElapsedSeconds += DeltaTime;
	const double FrameMilliseconds = static_cast<double>(DeltaTime) * 1000.0;
	SmokeMaximumFrameMilliseconds = FMath::Max(SmokeMaximumFrameMilliseconds, FrameMilliseconds);
	SmokeFrameMilliseconds.Add(FrameMilliseconds);
	++SmokeFrameCount;
	if (SmokeElapsedSeconds < 5.0 || SmokeFrameCount < 120)
	{
		return;
	}

	SmokeFrameMilliseconds.Sort();
	const int32 PercentileIndex = FMath::Clamp(
		FMath::CeilToInt(static_cast<double>(SmokeFrameMilliseconds.Num()) * 0.99) - 1,
		0,
		SmokeFrameMilliseconds.Num() - 1);
	const double Percentile99Milliseconds = SmokeFrameMilliseconds[PercentileIndex];
	const double AverageFps = static_cast<double>(SmokeFrameCount) / SmokeElapsedSeconds;
	const bool bScreenshotCaptured = IFileManager::Get().FileExists(
		*FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Phase0/proof-screen.png")));
	const FString Result = FString::Printf(
		TEXT("{\"schema\":1,\"passed\":%s,\"saveLoadContinuity\":%s,\"screenshotCaptured\":%s,")
		TEXT("\"resolution\":{\"width\":%d,\"height\":%d},")
		TEXT("\"frameCount\":%llu,\"elapsedSeconds\":%.6f,\"averageFps\":%.3f,")
		TEXT("\"p99FrameMilliseconds\":%.3f,\"maximumFrameMilliseconds\":%.3f,")
		TEXT("\"checksum\":\"%llu\"}"),
		bSmokeSaveLoadPassed && AverageFps >= 60.0 &&
			Percentile99Milliseconds <= 16.6 &&
			ViewportSize == FIntPoint(1920, 1080) &&
			bScreenshotCaptured ? TEXT("true") : TEXT("false"),
		bSmokeSaveLoadPassed ? TEXT("true") : TEXT("false"),
		bScreenshotCaptured ? TEXT("true") : TEXT("false"),
		ViewportSize.X,
		ViewportSize.Y,
		SmokeFrameCount,
		SmokeElapsedSeconds,
		AverageFps,
		Percentile99Milliseconds,
		SmokeMaximumFrameMilliseconds,
		SmokeChecksum);
	const FString ResultDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Phase0"));
	IFileManager::Get().MakeDirectory(*ResultDirectory, true);
	FFileHelper::SaveStringToFile(Result, *FPaths::Combine(ResultDirectory, TEXT("smoke-result.json")));
	UE_LOG(LogTemp, Display, TEXT("AMSimPhase0SmokeResult=%s"), *Result);
	bPhase0SmokeActive = false;
	FGenericPlatformMisc::RequestExit(false);
#endif
}

TStatId UAMSimGameInstanceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAMSimGameInstanceSubsystem, STATGROUP_Tickables);
}
