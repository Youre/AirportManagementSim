#include "AMSimGameInstanceSubsystem.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimSaveStore.h"
#include "AMSimSimulation.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "HAL/PlatformTime.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/DateTime.h"
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
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase1Smoke")))
	{
		const FString ProofDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Phase1"));
		IFileManager::Get().MakeDirectory(*ProofDirectory, true);
		for (const TCHAR* ProofName : {
			TEXT("va01-new-airport.png"),
			TEXT("va02-construction.png"),
			TEXT("va04-offer.png"),
			TEXT("va05-turnaround.png"),
			TEXT("phase1-complete.png")})
		{
			IFileManager::Get().Delete(*FPaths::Combine(ProofDirectory, ProofName), false, true);
		}
		bPhase1SmokeActive = true;
	}
#endif
}

void UAMSimGameInstanceSubsystem::Deinitialize()
{
	bPhase0SmokeActive = false;
	bPhase1SmokeActive = false;
	bReady = false;
	SaveRootDirectory.Reset();
	Super::Deinitialize();
}

void UAMSimGameInstanceSubsystem::Tick(const float DeltaTime)
{
#if !UE_BUILD_SHIPPING
	if (bPhase1SmokeActive)
	{
		TickPhase1Smoke(DeltaTime);
		return;
	}

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

void UAMSimGameInstanceSubsystem::TickPhase1Smoke(const float DeltaTime)
{
#if !UE_BUILD_SHIPPING
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	UAMSimAirportSimulationSubsystem* SimulationSubsystem =
		World ? World->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!SimulationSubsystem)
	{
		return;
	}

	FIntPoint ViewportSize = FIntPoint::ZeroValue;
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	}
	if (ViewportSize != FIntPoint(1920, 1080))
	{
		if (!bPhase1SmokeResolutionRequested && GEngine)
		{
			GEngine->Exec(nullptr, TEXT("r.SetRes 1920x1080w"));
			bPhase1SmokeResolutionRequested = true;
		}
		return;
	}

	Phase1SmokeTimeoutSeconds += FMath::Min(static_cast<double>(DeltaTime), 0.25);
	if (Phase1SmokeStage == 10)
	{
		Phase1SmokeElapsedSeconds += DeltaTime;
		const double FrameMilliseconds = static_cast<double>(DeltaTime) * 1000.0;
		Phase1SmokeMaximumFrameMilliseconds =
			FMath::Max(Phase1SmokeMaximumFrameMilliseconds, FrameMilliseconds);
		Phase1SmokeFrameMilliseconds.Add(FrameMilliseconds);
		++Phase1SmokeFrameCount;
	}
	Phase1MaximumBacklogSteps =
		FMath::Max(Phase1MaximumBacklogSteps, SimulationSubsystem->GetBacklogSteps());

	const FString ProofDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Phase1"));
	auto RequestProof = [this, &ProofDirectory](const TCHAR* FileName)
	{
		Phase1PendingScreenshot = FPaths::Combine(ProofDirectory, FileName);
		FScreenshotRequest::RequestScreenshot(Phase1PendingScreenshot, true, false);
	};
	auto ProofReady = [this]()
	{
		if (Phase1PendingScreenshot.IsEmpty() ||
			!IFileManager::Get().FileExists(*Phase1PendingScreenshot))
		{
			return false;
		}
		Phase1PendingScreenshot.Reset();
		return true;
	};
	auto Submit = [this, SimulationSubsystem](AMSim::FPhase1Command Command)
	{
		const bool bAccepted =
			SimulationSubsystem->SubmitPhase1Command(MoveTemp(Command)) ==
			AMSim::EPhase1CommandResult::Accepted;
		bPhase1SmokeJourneyPassed &= bAccepted;
		return bAccepted;
	};
	auto SetSpeed = [&Submit](const int32 Multiplier)
	{
		AMSim::FPhase1Command Command;
		Command.Type = AMSim::EPhase1CommandType::SetSpeed;
		Command.SpeedMultiplier = Multiplier;
		return Submit(MoveTemp(Command));
	};
	auto Pause = [&Submit]()
	{
		AMSim::FPhase1Command Command;
		Command.Type = AMSim::EPhase1CommandType::SetPaused;
		Command.bPaused = true;
		return Submit(MoveTemp(Command));
	};
	auto Finalize = [this, SimulationSubsystem, ViewportSize, &ProofDirectory](const bool bTimedOut)
	{
		const AMSim::FPhase1QuerySnapshot Query = SimulationSubsystem->GetPhase1Query();
		const AMSim::FPhase1State& State = SimulationSubsystem->GetSimulation().GetPhase1State();
		Phase1SmokeFrameMilliseconds.Sort();
		const int32 PercentileIndex = FMath::Clamp(
			FMath::CeilToInt(static_cast<double>(Phase1SmokeFrameMilliseconds.Num()) * 0.99) - 1,
			0,
			Phase1SmokeFrameMilliseconds.Num() - 1);
		const double Percentile99Milliseconds =
			Phase1SmokeFrameMilliseconds.IsEmpty()
				? 0.0
				: Phase1SmokeFrameMilliseconds[PercentileIndex];
		const double AverageFps =
			Phase1SmokeElapsedSeconds > 0.0
				? static_cast<double>(Phase1SmokeFrameCount) / Phase1SmokeElapsedSeconds
				: 0.0;
		const uint64 ResidentMemoryMiB =
			FPlatformMemory::GetStats().UsedPhysical / (1024ull * 1024ull);
		const bool bAllProofsCaptured =
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va01-new-airport.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va02-construction.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va04-offer.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va05-turnaround.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("phase1-complete.png")));
		const bool bStatePassed =
			Query.FlightState == AMSim::EFlightState::Completed &&
			Query.OfferState == AMSim::EOfferState::Completed &&
			Query.Credits == 2200 &&
			Query.AirportPoints == 5 &&
			State.Airframe.VisitCount == 1 &&
			State.Flight.bRewardRecognized &&
			State.PhraseIntents.Num() >= 5;
		const bool bPassed =
			!bTimedOut &&
			bPhase1SmokeJourneyPassed &&
			bPhase1SmokeSaveLoadPassed &&
			bStatePassed &&
			bAllProofsCaptured &&
			ViewportSize == FIntPoint(1920, 1080) &&
			AverageFps >= 60.0 &&
			Percentile99Milliseconds <= 16.6 &&
			Phase1MaximumBacklogSteps == 0 &&
			Phase1SnapshotCaptureMilliseconds <= 50.0 &&
			Phase1SaveWriteMilliseconds <= 2000.0 &&
			ResidentMemoryMiB < 4096;
		const FString Result = FString::Printf(
			TEXT("{\"schema\":1,\"scenario\":\"S01.StarterGrassAirfield\",")
			TEXT("\"passed\":%s,\"timedOut\":%s,\"journeyPassed\":%s,")
			TEXT("\"saveLoadContinuity\":%s,\"stateAssertions\":%s,\"screenshotsCaptured\":%s,")
			TEXT("\"resolution\":{\"width\":%d,\"height\":%d},")
			TEXT("\"frameCount\":%llu,\"elapsedSeconds\":%.6f,\"averageFps\":%.3f,")
			TEXT("\"p99FrameMilliseconds\":%.3f,\"maximumFrameMilliseconds\":%.3f,")
			TEXT("\"snapshotCaptureMilliseconds\":%.3f,\"saveWriteMilliseconds\":%.3f,")
			TEXT("\"maximum8xBacklogSteps\":%d,\"residentMemoryMiB\":%llu,")
			TEXT("\"credits\":%lld,\"airportPoints\":%d,\"phraseIntentCount\":%d,")
			TEXT("\"checksum\":\"%llu\"}"),
			bPassed ? TEXT("true") : TEXT("false"),
			bTimedOut ? TEXT("true") : TEXT("false"),
			bPhase1SmokeJourneyPassed ? TEXT("true") : TEXT("false"),
			bPhase1SmokeSaveLoadPassed ? TEXT("true") : TEXT("false"),
			bStatePassed ? TEXT("true") : TEXT("false"),
			bAllProofsCaptured ? TEXT("true") : TEXT("false"),
			ViewportSize.X,
			ViewportSize.Y,
			Phase1SmokeFrameCount,
			Phase1SmokeElapsedSeconds,
			AverageFps,
			Percentile99Milliseconds,
			Phase1SmokeMaximumFrameMilliseconds,
			Phase1SnapshotCaptureMilliseconds,
			Phase1SaveWriteMilliseconds,
			Phase1MaximumBacklogSteps,
			ResidentMemoryMiB,
			Query.Credits,
			Query.AirportPoints,
			State.PhraseIntents.Num(),
			Phase1SmokeChecksum);
		FFileHelper::SaveStringToFile(
			Result,
			*FPaths::Combine(ProofDirectory, TEXT("smoke-result.json")));
		UE_LOG(LogTemp, Display, TEXT("AMSimPhase1SmokeResult=%s"), *Result);
		bPhase1SmokeActive = false;
		FGenericPlatformMisc::RequestExit(false);
	};

	if (Phase1SmokeTimeoutSeconds > 120.0)
	{
		Finalize(true);
		return;
	}

	const AMSim::FPhase1QuerySnapshot Query = SimulationSubsystem->GetPhase1Query();
	switch (Phase1SmokeStage)
	{
	case 0:
		Phase1SmokeWarmupSeconds += FMath::Min(static_cast<double>(DeltaTime), 0.1);
		if (Phase1SmokeWarmupSeconds >= 0.5)
		{
			AMSim::FPhase1Command Create;
			Create.Type = AMSim::EPhase1CommandType::CreateAirport;
			Create.AirportName = TEXT("Riverbend Field");
			Create.MapId = AMSim::GetPhase1Fixture().MapId;
			Submit(MoveTemp(Create));
			RequestProof(TEXT("va01-new-airport.png"));
			Phase1SmokeStage = 1;
		}
		break;
	case 1:
		if (ProofReady())
		{
			AMSim::FPhase1Command Build;
			Build.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
			Build.Proposal = AMSim::CreateDefaultStarterPlan();
			Submit(MoveTemp(Build));
			SetSpeed(1);
			Phase1SmokeStage = 2;
		}
		break;
	case 2:
		if (Query.ConstructionStage == AMSim::EConstructionStage::Building)
		{
			Pause();
			RequestProof(TEXT("va02-construction.png"));
			Phase1SmokeStage = 3;
		}
		break;
	case 3:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 4;
		}
		break;
	case 4:
		if (Query.ConstructionStage == AMSim::EConstructionStage::ReadyToOpen)
		{
			AMSim::FPhase1Command Open;
			Open.Type = AMSim::EPhase1CommandType::OpenAirport;
			Submit(MoveTemp(Open));
			Pause();
			RequestProof(TEXT("va04-offer.png"));
			Phase1SmokeStage = 5;
		}
		break;
	case 5:
		if (ProofReady())
		{
			AMSim::FPhase1Command Accept;
			Accept.Type = AMSim::EPhase1CommandType::AcceptStarterOffer;
			Submit(MoveTemp(Accept));
			AMSim::FPhase1Command Schedule;
			Schedule.Type = AMSim::EPhase1CommandType::ScheduleStarterFlight;
			Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
			Schedule.ScheduledArrivalGameMilliseconds =
				SimulationSubsystem->GetRecommendedStarterArrivalTime();
			Submit(MoveTemp(Schedule));
			SetSpeed(1);
			Phase1SmokeStage = 6;
		}
		break;
	case 6:
		if (Query.FlightState == AMSim::EFlightState::Turnaround)
		{
			Pause();
			RequestProof(TEXT("va05-turnaround.png"));
			Phase1SmokeStage = 7;
		}
		break;
	case 7:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 8;
		}
		break;
	case 8:
		if (Query.FlightState == AMSim::EFlightState::Completed)
		{
			Pause();
			const double SnapshotStartSeconds = FPlatformTime::Seconds();
			AMSim::FSnapshot Snapshot = SimulationSubsystem->CreateSnapshot();
			Phase1SnapshotCaptureMilliseconds =
				(FPlatformTime::Seconds() - SnapshotStartSeconds) * 1000.0;
			Phase1SmokeChecksum = SimulationSubsystem->GetSimulation().CalculateChecksum();
			AMSim::FSaveMetadata Metadata;
			Metadata.PlayerLabel = TEXT("S01 rendered smoke");
			Metadata.AirportName = Query.AirportName;
			const double SaveStartSeconds = FPlatformTime::Seconds();
			const AMSim::FSaveResult SaveResult = SaveSnapshotAsync(
				TEXT("Phase1Smoke"),
				MoveTemp(Snapshot),
				MoveTemp(Metadata)).Get();
			Phase1SaveWriteMilliseconds =
				(FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
			AMSim::FSnapshot Loaded;
			bool bUsedBackup = false;
			bPhase1SmokeSaveLoadPassed =
				SaveResult.bSucceeded &&
				LoadSnapshot(TEXT("Phase1Smoke"), Loaded, bUsedBackup) &&
				!bUsedBackup &&
				SimulationSubsystem->RestoreSnapshot(Loaded) &&
				SimulationSubsystem->GetSimulation().CalculateChecksum() == Phase1SmokeChecksum;
			RequestProof(TEXT("phase1-complete.png"));
			Phase1SmokeStage = 9;
		}
		break;
	case 9:
		if (ProofReady())
		{
			Phase1SmokeElapsedSeconds = 0.0;
			Phase1SmokeMaximumFrameMilliseconds = 0.0;
			Phase1SmokeFrameCount = 0;
			Phase1SmokeFrameMilliseconds.Reset();
			Phase1SmokeStage = 10;
		}
		break;
	case 10:
		if (Phase1SmokeElapsedSeconds >= 5.0 && Phase1SmokeFrameCount >= 120)
		{
			Finalize(false);
		}
		break;
	default:
		Finalize(true);
		break;
	}
#endif
}

TStatId UAMSimGameInstanceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAMSimGameInstanceSubsystem, STATGROUP_Tickables);
}

TFuture<AMSim::FSaveResult> UAMSimGameInstanceSubsystem::SaveSnapshotAsync(
	const FString& SlotId,
	AMSim::FSnapshot Snapshot,
	AMSim::FSaveMetadata Metadata)
{
	FString SanitizedSlotId;
	if (!SanitizeSlotId(SlotId, SanitizedSlotId))
	{
		TPromise<AMSim::FSaveResult> Promise;
		Promise.SetValue({false, TEXT("Slot id must contain 1-48 letters, numbers, hyphens, or underscores.")});
		return Promise.GetFuture();
	}

	const int64 NowUnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
	Metadata.SlotId = SanitizedSlotId;
	Metadata.MapId = TEXT("Map.TemperateStarter");
	Metadata.SaveSchema = AMSim::SnapshotSchemaVersion;
	Metadata.ContentManifestHash = TEXT("phase1-internal-v1");
	Metadata.GameTimeMilliseconds = Snapshot.GameTimeMilliseconds;
	Metadata.LastPlayedUnixSeconds = NowUnixSeconds;
	if (Metadata.CreatedUnixSeconds <= 0)
	{
		Metadata.CreatedUnixSeconds = NowUnixSeconds;
	}

	return AMSim::FSaveStore::WriteAsync(
		FPaths::Combine(SaveRootDirectory, SanitizedSlotId),
		MoveTemp(Snapshot),
		MoveTemp(Metadata));
}

bool UAMSimGameInstanceSubsystem::LoadSnapshot(
	const FString& SlotId,
	AMSim::FSnapshot& Snapshot,
	bool& bUsedBackup) const
{
	FString SanitizedSlotId;
	if (!SanitizeSlotId(SlotId, SanitizedSlotId))
	{
		return false;
	}
	return AMSim::FSaveStore::LoadCurrentOrBackup(
		FPaths::Combine(SaveRootDirectory, SanitizedSlotId),
		Snapshot,
		bUsedBackup);
}

bool UAMSimGameInstanceSubsystem::SanitizeSlotId(
	const FString& SlotId,
	FString& SanitizedSlotId)
{
	if (SlotId.IsEmpty() || SlotId.Len() > 48)
	{
		return false;
	}
	for (const TCHAR Character : SlotId)
	{
		if (!FChar::IsAlnum(Character) && Character != TEXT('-') && Character != TEXT('_'))
		{
			return false;
		}
	}
	SanitizedSlotId = SlotId;
	return true;
}
