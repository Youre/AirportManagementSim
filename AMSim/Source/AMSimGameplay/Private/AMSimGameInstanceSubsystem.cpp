#include "AMSimGameInstanceSubsystem.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase2Fixture.h"
#include "AMSimPhase4Fixture.h"
#include "AMSimSaveStore.h"
#include "AMSimSimulation.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"

namespace
{
#if !UE_BUILD_SHIPPING
	struct FPhase1ScalabilityValues
	{
		int32 ViewDistance = -1;
		int32 AntiAliasing = -1;
		int32 Shadow = -1;
		int32 GlobalIllumination = -1;
		int32 Reflection = -1;
		int32 PostProcess = -1;
		int32 Texture = -1;
		int32 Effects = -1;
		int32 Foliage = -1;
		int32 Shading = -1;
	};

	void SetScalabilityValue(const TCHAR* Name, const int32 Value)
	{
		if (IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			Variable->Set(Value, ECVF_SetByCommandline);
		}
	}

	int32 GetScalabilityValue(const TCHAR* Name)
	{
		const IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(Name);
		return Variable ? Variable->GetInt() : -1;
	}

	void ApplyPhase1ReferenceProfile()
	{
		SetScalabilityValue(TEXT("sg.ViewDistanceQuality"), 0);
		SetScalabilityValue(TEXT("sg.AntiAliasingQuality"), 2);
		SetScalabilityValue(TEXT("sg.ShadowQuality"), 0);
		SetScalabilityValue(TEXT("sg.GlobalIlluminationQuality"), 0);
		SetScalabilityValue(TEXT("sg.ReflectionQuality"), 0);
		SetScalabilityValue(TEXT("sg.PostProcessQuality"), 0);
		SetScalabilityValue(TEXT("sg.TextureQuality"), 2);
		SetScalabilityValue(TEXT("sg.EffectsQuality"), 1);
		SetScalabilityValue(TEXT("sg.FoliageQuality"), 0);
		SetScalabilityValue(TEXT("sg.ShadingQuality"), 1);
	}

	FPhase1ScalabilityValues GetPhase1ScalabilityValues()
	{
		return {
			GetScalabilityValue(TEXT("sg.ViewDistanceQuality")),
			GetScalabilityValue(TEXT("sg.AntiAliasingQuality")),
			GetScalabilityValue(TEXT("sg.ShadowQuality")),
			GetScalabilityValue(TEXT("sg.GlobalIlluminationQuality")),
			GetScalabilityValue(TEXT("sg.ReflectionQuality")),
			GetScalabilityValue(TEXT("sg.PostProcessQuality")),
			GetScalabilityValue(TEXT("sg.TextureQuality")),
			GetScalabilityValue(TEXT("sg.EffectsQuality")),
			GetScalabilityValue(TEXT("sg.FoliageQuality")),
			GetScalabilityValue(TEXT("sg.ShadingQuality"))};
	}

	bool IsPhase1ReferenceProfile(const FPhase1ScalabilityValues& Values)
	{
		return Values.ViewDistance == 0 &&
			Values.AntiAliasing == 2 &&
			Values.Shadow == 0 &&
			Values.GlobalIllumination == 0 &&
			Values.Reflection == 0 &&
			Values.PostProcess == 0 &&
			Values.Texture == 2 &&
			Values.Effects == 1 &&
			Values.Foliage == 0 &&
			Values.Shading == 1;
	}

	double CalculatePercentile(const TArray<double>& SortedValues, const double Percentile)
	{
		if (SortedValues.IsEmpty())
		{
			return 0.0;
		}
		const int32 Index = FMath::Clamp(
			FMath::CeilToInt(static_cast<double>(SortedValues.Num()) * Percentile) - 1,
			0,
			SortedValues.Num() - 1);
		return SortedValues[Index];
	}
#endif
}

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
	bPhase4SmokeRequested =
		FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase4Smoke"));
	bPhase45ConstructionProofRequested =
		FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase45ConstructionProof"));
	bPhase45OperationalEvidenceRequested =
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimPhase45OperationalEvidence"));
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase1Smoke")) ||
		bPhase4SmokeRequested)
	{
		double RequestedPerformanceSeconds = Phase1PerformanceTargetSeconds;
		if (FParse::Value(
			FCommandLine::Get(),
			TEXT("AMSimPhase1PerformanceSeconds="),
			RequestedPerformanceSeconds))
		{
			Phase1PerformanceTargetSeconds =
				FMath::Clamp(RequestedPerformanceSeconds, 5.0, 14400.0);
		}
		bPhase1ReferenceProfileRequested =
			FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase1ReferenceProfile"));
		if (bPhase1ReferenceProfileRequested)
		{
			ApplyPhase1ReferenceProfile();
		}
		const FString ProofDirectory = FPaths::Combine(
			FPaths::ProjectSavedDir(),
			bPhase4SmokeRequested ? TEXT("Phase4") : TEXT("Phase1"));
		IFileManager::Get().MakeDirectory(*ProofDirectory, true);
		for (const TCHAR* ProofName : {
			TEXT("va01-new-airport.png"),
			TEXT("va02-construction-proposal.png"),
			TEXT("va02-construction.png"),
			TEXT("va04-offer.png"),
			TEXT("va05-turnaround.png"),
			TEXT("phase1-complete.png"),
			TEXT("va06-weather.png"),
			TEXT("va06-incident.png"),
			TEXT("va07-progression.png"),
			TEXT("phase2-complete.png"),
			TEXT("va03-terminal-passenger-flow.png"),
			TEXT("phase3-complete.png"),
			TEXT("va04-regional-timetable.png"),
			TEXT("va04-locked-horizon.png"),
			TEXT("va06-regional-incident.png"),
			TEXT("va02-affected-closure.png"),
			TEXT("phase4-complete.png"),
			TEXT("smoke-result.json")})
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
		Phase1SimulationWorkMilliseconds.Add(
			SimulationSubsystem->GetLastSimulationWorkMilliseconds());
		Phase1MaximumMemoryMiB = FMath::Max(
			Phase1MaximumMemoryMiB,
			FPlatformMemory::GetStats().UsedPhysical / (1024ull * 1024ull));
		++Phase1SmokeFrameCount;
	}
	Phase1MaximumBacklogSteps =
		FMath::Max(Phase1MaximumBacklogSteps, SimulationSubsystem->GetBacklogSteps());

	const FString ProofDirectory = FPaths::Combine(
		FPaths::ProjectSavedDir(),
		bPhase4SmokeRequested ? TEXT("Phase4") : TEXT("Phase1"));
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
	auto SubmitPhase2 = [this, SimulationSubsystem](AMSim::FPhase2Command Command)
	{
		const bool bAccepted =
			SimulationSubsystem->SubmitPhase2Command(MoveTemp(Command)) ==
			AMSim::EPhase2CommandResult::Accepted;
		bPhase1SmokeJourneyPassed &= bAccepted;
		return bAccepted;
	};
	auto SubmitPhase3 = [this, SimulationSubsystem](AMSim::FPhase3Command Command)
	{
		const bool bAccepted =
			SimulationSubsystem->SubmitPhase3Command(MoveTemp(Command)) ==
			AMSim::EPhase3CommandResult::Accepted;
		bPhase1SmokeJourneyPassed &= bAccepted;
		return bAccepted;
	};
	auto SubmitPhase4 = [this, SimulationSubsystem](AMSim::FPhase4Command Command)
	{
		const bool bAccepted =
			SimulationSubsystem->SubmitPhase4Command(MoveTemp(Command)) ==
			AMSim::EPhase4CommandResult::Accepted;
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
		const AMSim::FPhase2QuerySnapshot Phase2Query =
			SimulationSubsystem->GetPhase2Query();
		const AMSim::FPhase2State& Phase2State =
			SimulationSubsystem->GetSimulation().GetPhase2State();
		const AMSim::FPhase3QuerySnapshot Phase3Query =
			SimulationSubsystem->GetPhase3Query();
		const AMSim::FPhase3State& Phase3State =
			SimulationSubsystem->GetSimulation().GetPhase3State();
		const AMSim::FPhase4QuerySnapshot Phase4Query =
			SimulationSubsystem->GetPhase4Query();
		const AMSim::FPhase4State& Phase4State =
			SimulationSubsystem->GetSimulation().GetPhase4State();
		Phase1SmokeFrameMilliseconds.Sort();
		Phase1SimulationWorkMilliseconds.Sort();
		const double Percentile99Milliseconds =
			CalculatePercentile(Phase1SmokeFrameMilliseconds, 0.99);
		const double SimulationMedianMilliseconds =
			CalculatePercentile(Phase1SimulationWorkMilliseconds, 0.5);
		const double SimulationPercentile99Milliseconds =
			CalculatePercentile(Phase1SimulationWorkMilliseconds, 0.99);
		const double AverageFps =
			Phase1SmokeElapsedSeconds > 0.0
				? static_cast<double>(Phase1SmokeFrameCount) / Phase1SmokeElapsedSeconds
				: 0.0;
		const uint64 ResidentMemoryMiB =
			FPlatformMemory::GetStats().UsedPhysical / (1024ull * 1024ull);
		Phase1MaximumMemoryMiB = FMath::Max(Phase1MaximumMemoryMiB, ResidentMemoryMiB);
		const int64 MemoryGrowthMiB =
			static_cast<int64>(ResidentMemoryMiB) -
			static_cast<int64>(Phase1StartingMemoryMiB);
		const FPhase1ScalabilityValues Scalability = GetPhase1ScalabilityValues();
		const bool bReferenceProfilePassed =
			!bPhase1ReferenceProfileRequested || IsPhase1ReferenceProfile(Scalability);
		const bool bAllProofsCaptured =
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va01-new-airport.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va02-construction.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va04-offer.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va05-turnaround.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("phase1-complete.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va06-weather.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va06-incident.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va07-progression.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("phase2-complete.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va03-terminal-passenger-flow.png"))) &&
			IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("phase3-complete.png"))) &&
			(!bPhase45OperationalEvidenceRequested ||
			 IFileManager::Get().FileExists(*FPaths::Combine(
				 ProofDirectory,
				 TEXT("va02-affected-closure.png")))) &&
			(!bPhase4SmokeRequested ||
				(IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va04-regional-timetable.png"))) &&
				 (!bPhase45OperationalEvidenceRequested ||
				  IFileManager::Get().FileExists(*FPaths::Combine(
					  ProofDirectory,
					  TEXT("va04-locked-horizon.png")))) &&
				 IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("va06-regional-incident.png"))) &&
				 IFileManager::Get().FileExists(*FPaths::Combine(ProofDirectory, TEXT("phase4-complete.png")))));
		const bool bStatePassed =
			Query.FlightState == AMSim::EFlightState::Completed &&
			Query.OfferState == AMSim::EOfferState::Completed &&
			Query.Credits >= 0 &&
			Query.AirportPoints > 5 &&
			State.Airframe.VisitCount == 1 &&
			State.Flight.bRewardRecognized &&
			State.PhraseIntents.Num() >= 5 &&
			Phase2Query.bInitialized &&
			Phase2Query.OperatingDay >= 8 &&
			Phase2Query.CompletedFlightCount >= 20 &&
			Phase2State.SelectedSpecialization ==
				AMSim::EAirportSpecialization::FlightSchool &&
			Phase2State.Expansion.Stage == AMSim::EExpansionStage::Operational &&
			Phase2State.Incident.State == AMSim::EIncidentState::Resolved &&
			Phase3Query.bTerminalOpen &&
			Phase3Query.FlightState == AMSim::EPhase3FlightState::Completed &&
			Phase3Query.CompletedPassengerCount == Phase3Query.PassengerCount &&
			Phase3Query.CompletedBagCount == Phase3Query.BagCount &&
			Phase3State.Flight.bPassengerReconciled &&
			Phase3State.Flight.bBagReconciled &&
			(!bPhase4SmokeRequested ||
				(Phase4Query.bFixtureCompleted &&
				 Phase4Query.bRecoverable &&
				 Phase4Query.CompletedFlightCount == 21 &&
				 Phase4Query.CompletedTransferBagCount ==
					Phase4Query.TransferBagCount &&
				 Phase4Query.BorderProcessedPassengerCount ==
					Phase4Query.InternationalPassengerCount &&
				 Phase4Query.RebookedPassengerCount ==
					Phase4Query.MissedConnectionPassengerCount &&
				 Phase4Query.IncidentLifecycle ==
					AMSim::EPhase4IncidentLifecycle::Recovered &&
				 Phase4State.Incident.Outcome == TEXT("No injuries") &&
				 Phase4State.Incident.bReportReviewed &&
				 Phase4Query.RenewalAcceptedCount == 1));
		const bool bPassed =
			!bTimedOut &&
			bPhase1SmokeJourneyPassed &&
			bPhase1SmokeSaveLoadPassed &&
			bStatePassed &&
			bAllProofsCaptured &&
			ViewportSize == FIntPoint(1920, 1080) &&
			AverageFps >= 60.0 &&
			Percentile99Milliseconds <= 16.6 &&
			SimulationMedianMilliseconds <= 4.0 &&
			SimulationPercentile99Milliseconds <= 8.0 &&
			Phase1MaximumBacklogSteps == 0 &&
			Phase1SnapshotCaptureMilliseconds <= 50.0 &&
			Phase1SaveWriteMilliseconds <= 2000.0 &&
			Phase1MaximumMemoryMiB < 4096 &&
			bReferenceProfilePassed;
		const FString Result = FString::Printf(
			TEXT("{\"schema\":%d,\"scenario\":\"%s\",")
			TEXT("\"passed\":%s,\"timedOut\":%s,\"journeyPassed\":%s,")
			TEXT("\"saveLoadContinuity\":%s,\"stateAssertions\":%s,\"screenshotsCaptured\":%s,")
			TEXT("\"resolution\":{\"width\":%d,\"height\":%d},")
			TEXT("\"frameCount\":%llu,\"elapsedSeconds\":%.6f,\"averageFps\":%.3f,")
			TEXT("\"p99FrameMilliseconds\":%.3f,\"maximumFrameMilliseconds\":%.3f,")
			TEXT("\"simulationMedianMilliseconds\":%.3f,\"simulationP99Milliseconds\":%.3f,")
			TEXT("\"simulationSampleCount\":%d,\"performanceTargetSeconds\":%.3f,")
			TEXT("\"snapshotCaptureMilliseconds\":%.3f,\"saveWriteMilliseconds\":%.3f,")
			TEXT("\"maximum8xBacklogSteps\":%d,\"residentMemoryMiB\":%llu,")
			TEXT("\"startingMemoryMiB\":%llu,\"maximumMemoryMiB\":%llu,\"memoryGrowthMiB\":%lld,")
			TEXT("\"referenceProfileRequested\":%s,\"referenceProfileApplied\":%s,")
			TEXT("\"scalability\":{\"viewDistance\":%d,\"antiAliasing\":%d,\"shadow\":%d,")
			TEXT("\"globalIllumination\":%d,\"reflection\":%d,\"postProcess\":%d,")
			TEXT("\"texture\":%d,\"effects\":%d,\"foliage\":%d,\"shading\":%d},")
			TEXT("\"credits\":%lld,\"airportPoints\":%d,\"phraseIntentCount\":%d,")
			TEXT("\"phase2OperatingDay\":%d,\"phase2CompletedFlights\":%d,")
			TEXT("\"phase2IncidentState\":%d,\"phase2ExpansionStage\":%d,")
			TEXT("\"phase3FlightState\":%d,\"phase3Passengers\":%d,")
			TEXT("\"phase3CompletedPassengers\":%d,\"phase3Bags\":%d,")
			TEXT("\"phase3CompletedBags\":%d,\"phase3ReconciliationPasses\":%d,")
			TEXT("\"phase4OperatingDay\":%d,\"phase4CompletedFlights\":%d,")
			TEXT("\"phase4TransferBags\":%d,\"phase4CompletedTransferBags\":%d,")
			TEXT("\"phase4InternationalPassengers\":%d,\"phase4BorderProcessedPassengers\":%d,")
			TEXT("\"phase4MissedConnections\":%d,\"phase4RebookedPassengers\":%d,")
			TEXT("\"phase4IncidentLifecycle\":%d,\"phase4Renewals\":%d,")
			TEXT("\"phase4FixtureCompleted\":%s,\"phase4Recoverable\":%s,")
			TEXT("\"checksum\":\"%llu\"}"),
			bPhase4SmokeRequested ? 4 : 3,
			bPhase4SmokeRequested
				? TEXT("S06-S10.IntegratedPhase4RegionalWeek")
				: TEXT("S01-S11.IntegratedPhase3Smoke"),
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
			SimulationMedianMilliseconds,
			SimulationPercentile99Milliseconds,
			Phase1SimulationWorkMilliseconds.Num(),
			Phase1PerformanceTargetSeconds,
			Phase1SnapshotCaptureMilliseconds,
			Phase1SaveWriteMilliseconds,
			Phase1MaximumBacklogSteps,
			ResidentMemoryMiB,
			Phase1StartingMemoryMiB,
			Phase1MaximumMemoryMiB,
			MemoryGrowthMiB,
			bPhase1ReferenceProfileRequested ? TEXT("true") : TEXT("false"),
			bReferenceProfilePassed ? TEXT("true") : TEXT("false"),
			Scalability.ViewDistance,
			Scalability.AntiAliasing,
			Scalability.Shadow,
			Scalability.GlobalIllumination,
			Scalability.Reflection,
			Scalability.PostProcess,
			Scalability.Texture,
			Scalability.Effects,
			Scalability.Foliage,
			Scalability.Shading,
			Query.Credits,
			Query.AirportPoints,
			State.PhraseIntents.Num(),
			Phase2Query.OperatingDay,
			Phase2Query.CompletedFlightCount,
			static_cast<int32>(Phase2State.Incident.State),
			static_cast<int32>(Phase2State.Expansion.Stage),
			static_cast<int32>(Phase3Query.FlightState),
			Phase3Query.PassengerCount,
			Phase3Query.CompletedPassengerCount,
			Phase3Query.BagCount,
			Phase3Query.CompletedBagCount,
			Phase3State.ReconciliationPassCount,
			Phase4Query.CurrentOperatingDay,
			Phase4Query.CompletedFlightCount,
			Phase4Query.TransferBagCount,
			Phase4Query.CompletedTransferBagCount,
			Phase4Query.InternationalPassengerCount,
			Phase4Query.BorderProcessedPassengerCount,
			Phase4Query.MissedConnectionPassengerCount,
			Phase4Query.RebookedPassengerCount,
			static_cast<int32>(Phase4Query.IncidentLifecycle),
			Phase4Query.RenewalAcceptedCount,
			Phase4Query.bFixtureCompleted ? TEXT("true") : TEXT("false"),
			Phase4Query.bRecoverable ? TEXT("true") : TEXT("false"),
			Phase1SmokeChecksum);
		FFileHelper::SaveStringToFile(
			Result,
			*FPaths::Combine(ProofDirectory, TEXT("smoke-result.json")));
		UE_LOG(LogTemp, Display, TEXT("AMSimPhase1SmokeResult=%s"), *Result);
		bPhase1SmokeActive = false;
		FGenericPlatformMisc::RequestExit(false);
	};

	if (Phase1SmokeTimeoutSeconds > Phase1PerformanceTargetSeconds + 120.0)
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
			RequestProof(TEXT("va01-new-airport.png"));
			Phase1SmokeStage = 1;
		}
		break;
	case 1:
		if (ProofReady())
		{
			AMSim::FPhase1Command Create;
			Create.Type = AMSim::EPhase1CommandType::CreateAirport;
			Create.AirportName = TEXT("Riverbend Field");
			Create.MapId = AMSim::GetPhase1Fixture().MapId;
			Submit(MoveTemp(Create));
			if (bPhase45ConstructionProofRequested)
			{
				Phase1SmokeStage = 40;
			}
			else
			{
				AMSim::FPhase1Command Build;
				Build.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
				Build.Proposal = AMSim::CreateDefaultStarterPlan();
				Submit(MoveTemp(Build));
				SetSpeed(1);
				Phase1SmokeStage = 2;
			}
		}
		break;
	case 40:
		++Phase1ProposalSettleFrames;
		if (Phase1ProposalSettleFrames >= 3)
		{
			RequestProof(TEXT("va02-construction-proposal.png"));
			Phase1SmokeStage = 41;
		}
		break;
	case 41:
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
			++Phase1TurnaroundSettleFrames;
			if (Phase1TurnaroundSettleFrames >= 2)
			{
				RequestProof(TEXT("va05-turnaround.png"));
				Phase1SmokeStage = 7;
			}
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
			SetSpeed(1);
			Phase1SmokeElapsedSeconds = 0.0;
			Phase1SmokeMaximumFrameMilliseconds = 0.0;
			Phase1SmokeFrameCount = 0;
			Phase1SmokeFrameMilliseconds.Reset();
			Phase1SimulationWorkMilliseconds.Reset();
			Phase1StartingMemoryMiB =
				FPlatformMemory::GetStats().UsedPhysical / (1024ull * 1024ull);
			Phase1MaximumMemoryMiB = Phase1StartingMemoryMiB;
			Phase1SmokeStage = 10;
		}
		break;
	case 10:
		if (Phase1SmokeElapsedSeconds >= Phase1PerformanceTargetSeconds &&
			Phase1SmokeFrameCount >= 120)
		{
			Pause();
			AMSim::FPhase2Command Initialize;
			Initialize.Type = AMSim::EPhase2CommandType::InitializeLivingAirport;
			SubmitPhase2(MoveTemp(Initialize));
			AMSim::FPhase2Command Select;
			Select.Type = AMSim::EPhase2CommandType::SelectSpecialization;
			Select.Specialization = AMSim::EAirportSpecialization::FlightSchool;
			SubmitPhase2(MoveTemp(Select));
			const AMSim::FPhase2State& Phase2State =
				SimulationSubsystem->GetSimulation().GetPhase2State();
			const AMSim::FPhase2ContractRecord* Contract =
				Phase2State.Contracts.FindByPredicate(
					[](const AMSim::FPhase2ContractRecord& Candidate)
						{
							return Candidate.Specialization ==
								AMSim::EAirportSpecialization::FlightSchool;
						});
			if (Contract)
			{
				AMSim::FPhase2Command Accept;
				Accept.Type = AMSim::EPhase2CommandType::AcceptContract;
				Accept.ContractId = Contract->Id;
				SubmitPhase2(MoveTemp(Accept));
			}
			else
			{
				bPhase1SmokeJourneyPassed = false;
			}
			if (!Phase2State.Parcels.IsEmpty())
			{
				const AMSim::FParcelId ParcelId = Phase2State.Parcels[0].Id;
				AMSim::FPhase2Command Purchase;
				Purchase.Type = AMSim::EPhase2CommandType::PurchaseParcel;
				Purchase.ParcelId = ParcelId;
				SubmitPhase2(MoveTemp(Purchase));
				AMSim::FPhase2Command Expand;
				Expand.Type = AMSim::EPhase2CommandType::StartExpansion;
				Expand.ParcelId = ParcelId;
				SubmitPhase2(MoveTemp(Expand));
			}
			RequestProof(TEXT("va07-progression.png"));
			Phase1SmokeStage = 11;
		}
		break;
	case 11:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage =
				bPhase45OperationalEvidenceRequested ? 42 : 12;
		}
		break;
	case 42:
		if (SimulationSubsystem->GetPhase2Query().ExpansionStage ==
			AMSim::EExpansionStage::Building)
		{
			Pause();
			RequestProof(TEXT("va02-affected-closure.png"));
			Phase1SmokeStage = 43;
		}
		break;
	case 43:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 12;
		}
		break;
	case 12:
		if (SimulationSubsystem->GetPhase2Query().OperatingDay >= 4 &&
			SimulationSubsystem->GetPhase2Query().WeatherCategory ==
				AMSim::EWeatherCategory::ColdWet)
		{
			Pause();
			RequestProof(TEXT("va06-weather.png"));
			Phase1SmokeStage = 13;
		}
		break;
	case 13:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 14;
		}
		break;
	case 14:
		if (SimulationSubsystem->GetPhase2Query().IncidentState ==
			AMSim::EIncidentState::Reported)
		{
			Pause();
			RequestProof(TEXT("va06-incident.png"));
			Phase1SmokeStage = 15;
		}
		break;
	case 15:
		if (ProofReady())
		{
			AMSim::FPhase2Command Respond;
			Respond.Type = AMSim::EPhase2CommandType::RespondToIncident;
			Respond.IncidentId =
				SimulationSubsystem->GetSimulation().GetPhase2State().Incident.Id;
			SubmitPhase2(MoveTemp(Respond));
			Phase1SmokeChecksum =
				SimulationSubsystem->GetSimulation().CalculateChecksum();
			AMSim::FSnapshot IncidentSnapshot = SimulationSubsystem->CreateSnapshot();
			AMSim::FSaveMetadata Metadata;
			Metadata.PlayerLabel = TEXT("Phase 2 incident smoke");
			Metadata.AirportName = Query.AirportName;
			const AMSim::FSaveResult SaveResult = SaveSnapshotAsync(
				TEXT("Phase2Smoke"),
				MoveTemp(IncidentSnapshot),
				MoveTemp(Metadata)).Get();
			AMSim::FSnapshot Loaded;
			bool bUsedBackup = false;
			bPhase1SmokeSaveLoadPassed &=
				SaveResult.bSucceeded &&
				LoadSnapshot(TEXT("Phase2Smoke"), Loaded, bUsedBackup) &&
				!bUsedBackup &&
				SimulationSubsystem->RestoreSnapshot(Loaded) &&
				SimulationSubsystem->GetSimulation().CalculateChecksum() ==
					Phase1SmokeChecksum;
			SetSpeed(8);
			Phase1SmokeStage = 16;
		}
		break;
	case 16:
		if (SimulationSubsystem->GetPhase2Query().OperatingDay >= 8 &&
			SimulationSubsystem->GetPhase2Query().IncidentState ==
				AMSim::EIncidentState::Resolved)
		{
			Pause();
			RequestProof(TEXT("phase2-complete.png"));
			Phase1SmokeStage = 17;
		}
		break;
	case 17:
		if (ProofReady())
		{
			AMSim::FPhase3Command Initialize;
			Initialize.Type =
				AMSim::EPhase3CommandType::InitializePassengerAirport;
			SubmitPhase3(MoveTemp(Initialize));
			AMSim::FPhase3Command Fund;
			Fund.Type = AMSim::EPhase3CommandType::FundTerminal;
			SubmitPhase3(MoveTemp(Fund));
			SetSpeed(8);
			Phase1SmokeStage = 18;
		}
		break;
	case 18:
		if (SimulationSubsystem->GetPhase3Query().TerminalStage ==
			AMSim::ETerminalConstructionStage::ShellReady)
		{
			Pause();
			const int32 RouteCount =
				SimulationSubsystem->GetSimulation().GetPhase3State().Routes.Num();
			for (int32 Index = 0; Index < RouteCount; ++Index)
			{
				AMSim::FPhase3Command Connect;
				Connect.Type = AMSim::EPhase3CommandType::ConnectNextNetwork;
				SubmitPhase3(MoveTemp(Connect));
			}
			AMSim::FPhase3Command Open;
			Open.Type = AMSim::EPhase3CommandType::OpenTerminal;
			SubmitPhase3(MoveTemp(Open));
			AMSim::FPhase3Command Schedule;
			Schedule.Type =
				AMSim::EPhase3CommandType::SchedulePassengerService;
			SubmitPhase3(MoveTemp(Schedule));
			const AMSim::FPassengerRecord* Accessible =
				SimulationSubsystem->GetSimulation().GetPhase3State().
					Passengers.FindByPredicate(
						[](const AMSim::FPassengerRecord& Passenger)
							{
								return Passenger.bRequiresAccessibleRoute;
							});
			if (Accessible)
			{
				AMSim::FPhase3Command Assist;
				Assist.Type =
					AMSim::EPhase3CommandType::RequestPassengerAssistance;
				Assist.PassengerId = Accessible->Id;
				SubmitPhase3(MoveTemp(Assist));
			}
			else
			{
				bPhase1SmokeJourneyPassed = false;
			}
			SetSpeed(8);
			Phase1SmokeStage = 19;
		}
		break;
	case 19:
		if (SimulationSubsystem->GetPhase3Query().SecurityQueueCount > 0)
		{
			Pause();
			Phase1SmokeChecksum =
				SimulationSubsystem->GetSimulation().CalculateChecksum();
			AMSim::FSnapshot Snapshot =
				SimulationSubsystem->CreateSnapshot();
			AMSim::FSaveMetadata Metadata;
			Metadata.PlayerLabel = TEXT("Phase 3 security-flow smoke");
			Metadata.AirportName = Query.AirportName;
			const AMSim::FSaveResult SaveResult = SaveSnapshotAsync(
				TEXT("Phase3Smoke"),
				MoveTemp(Snapshot),
				MoveTemp(Metadata)).Get();
			AMSim::FSnapshot Loaded;
			bool bUsedBackup = false;
			bPhase1SmokeSaveLoadPassed &=
				SaveResult.bSucceeded &&
				LoadSnapshot(TEXT("Phase3Smoke"), Loaded, bUsedBackup) &&
				!bUsedBackup &&
				SimulationSubsystem->RestoreSnapshot(Loaded) &&
				SimulationSubsystem->GetSimulation().CalculateChecksum() ==
					Phase1SmokeChecksum;
			RequestProof(TEXT("va03-terminal-passenger-flow.png"));
			Phase1SmokeStage = 20;
		}
		break;
	case 20:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 21;
		}
		break;
	case 21:
		if (SimulationSubsystem->GetPhase3Query().FlightState ==
			AMSim::EPhase3FlightState::Completed)
		{
			Pause();
			RequestProof(TEXT("phase3-complete.png"));
			Phase1SmokeStage = 22;
		}
		break;
	case 22:
		if (ProofReady())
		{
			if (!bPhase4SmokeRequested)
			{
				Phase1SmokeChecksum =
					SimulationSubsystem->GetSimulation().CalculateChecksum();
				Finalize(false);
				break;
			}
			AMSim::FPhase4Command Initialize;
			Initialize.Type =
				AMSim::EPhase4CommandType::InitializeRegionalAirport;
			SubmitPhase4(MoveTemp(Initialize));
			for (int32 Index = 0; Index < 3; ++Index)
			{
				AMSim::FPhase4Command Accept;
				Accept.Type =
					AMSim::EPhase4CommandType::AcceptNextRecurringContract;
				SubmitPhase4(MoveTemp(Accept));
			}
			AMSim::FPhase4Command Publish;
			Publish.Type =
				AMSim::EPhase4CommandType::PublishSevenDayTimetable;
			SubmitPhase4(MoveTemp(Publish));
			Pause();
			RequestProof(TEXT("va04-regional-timetable.png"));
			Phase1SmokeStage = 23;
		}
		break;
	case 23:
		if (ProofReady())
		{
			const AMSim::FPhase4FlightRecord* Disrupted =
				SimulationSubsystem->GetSimulation().GetPhase4State().
					Flights.FindByPredicate(
						[](const AMSim::FPhase4FlightRecord& Flight)
							{
								return Flight.DayIndex == 3 &&
									Flight.bWeatherRestricted;
							});
			if (!Disrupted)
			{
				bPhase1SmokeJourneyPassed = false;
				Finalize(true);
				break;
			}
			AMSim::FPhase4Command Review;
			Review.Type =
				AMSim::EPhase4CommandType::ReassignDisruptedFlight;
			Review.FlightId = Disrupted->Id;
			Review.RequestedGateId = TEXT("R1");
			const AMSim::EPhase4CommandResult ReviewResult =
				SimulationSubsystem->SubmitPhase4Command(Review);
			bPhase1SmokeJourneyPassed &=
				ReviewResult ==
					AMSim::EPhase4CommandResult::AcceptedWithWarning;
			AMSim::FPhase4Command Confirm;
			Confirm.Type =
				AMSim::EPhase4CommandType::ConfirmHighRiskGateChange;
			Confirm.FlightId = Disrupted->Id;
			Confirm.RequestedGateId = TEXT("R1");
			Confirm.bConfirmWarning = true;
			SubmitPhase4(MoveTemp(Confirm));
			if (bPhase45OperationalEvidenceRequested)
			{
				Phase4IncidentSettleFrames = 0;
				Phase1SmokeStage = 44;
			}
			else
			{
				SetSpeed(8);
				Phase1SmokeStage = 24;
			}
		}
		break;
	case 44:
		++Phase4IncidentSettleFrames;
		if (Phase4IncidentSettleFrames >= 2)
		{
			RequestProof(TEXT("va04-locked-horizon.png"));
			Phase1SmokeStage = 45;
		}
		break;
	case 45:
		if (ProofReady())
		{
			SetSpeed(8);
			Phase1SmokeStage = 24;
		}
		break;
	case 24:
		if (SimulationSubsystem->GetPhase4Query().IncidentLifecycle ==
			AMSim::EPhase4IncidentLifecycle::Alerted)
		{
			Pause();
			for (const AMSim::EPhase4CommandType Type : {
				AMSim::EPhase4CommandType::HoldAffectedDepartures,
				AMSim::EPhase4CommandType::DivertAffectedArrivals,
				AMSim::EPhase4CommandType::DeployTowTeam})
			{
				AMSim::FPhase4Command Response;
				Response.Type = Type;
				SubmitPhase4(MoveTemp(Response));
			}
			Phase4IncidentSettleFrames = 0;
			Phase1SmokeStage = 25;
		}
		break;
	case 25:
		++Phase4IncidentSettleFrames;
		if (Phase4IncidentSettleFrames >= 2)
		{
			AMSim::FPhase4Command Protect;
			Protect.Type =
				AMSim::EPhase4CommandType::ProtectIncidentArea;
			SubmitPhase4(MoveTemp(Protect));
			Phase4IncidentSettleFrames = 0;
			Phase1SmokeStage = 26;
		}
		break;
	case 26:
		++Phase4IncidentSettleFrames;
		if (Phase4IncidentSettleFrames >= 2)
		{
			RequestProof(TEXT("va06-regional-incident.png"));
			Phase1SmokeStage = 27;
		}
		break;
	case 27:
		if (ProofReady())
		{
			for (const AMSim::EPhase4CommandType Type : {
				AMSim::EPhase4CommandType::ReviewIncidentCause,
				AMSim::EPhase4CommandType::ApplyRecoveryPlan})
			{
				AMSim::FPhase4Command Response;
				Response.Type = Type;
				SubmitPhase4(MoveTemp(Response));
			}
			Phase1SmokeChecksum =
				SimulationSubsystem->GetSimulation().CalculateChecksum();
			AMSim::FSnapshot Snapshot =
				SimulationSubsystem->CreateSnapshot();
			AMSim::FSaveMetadata Metadata;
			Metadata.PlayerLabel = TEXT("Phase 4 recovered incident");
			Metadata.AirportName = Query.AirportName;
			const AMSim::FSaveResult SaveResult = SaveSnapshotAsync(
				TEXT("Phase4Smoke"),
				MoveTemp(Snapshot),
				MoveTemp(Metadata)).Get();
			AMSim::FSnapshot Loaded;
			bool bUsedBackup = false;
			bPhase1SmokeSaveLoadPassed &=
				SaveResult.bSucceeded &&
				LoadSnapshot(TEXT("Phase4Smoke"), Loaded, bUsedBackup) &&
				!bUsedBackup &&
				SimulationSubsystem->RestoreSnapshot(Loaded) &&
				SimulationSubsystem->GetSimulation().CalculateChecksum() ==
					Phase1SmokeChecksum;
			SetSpeed(8);
			Phase1SmokeStage = 28;
		}
		break;
	case 28:
		if (SimulationSubsystem->GetPhase4Query().CurrentOperatingDay == 7)
		{
			Pause();
			AMSim::FPhase4Command Renew;
			Renew.Type =
				AMSim::EPhase4CommandType::AcceptTenantRenewal;
			SubmitPhase4(MoveTemp(Renew));
			Phase4CompleteSettleFrames = 0;
			Phase1SmokeStage = 29;
		}
		break;
	case 29:
		++Phase4CompleteSettleFrames;
		if (Phase4CompleteSettleFrames >= 2)
		{
			RequestProof(TEXT("phase4-complete.png"));
			Phase1SmokeStage = 30;
		}
		break;
	case 30:
		if (ProofReady())
		{
			Phase1SmokeChecksum =
				SimulationSubsystem->GetSimulation().CalculateChecksum();
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
