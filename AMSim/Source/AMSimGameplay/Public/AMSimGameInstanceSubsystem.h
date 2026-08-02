#pragma once

#include "AMSimSaveStore.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AMSimGameInstanceSubsystem.generated.h"

namespace AMSim
{
	struct FSaveSlotSummary
	{
		FString SlotId;
		FSaveMetadata Metadata;
	};
}

UCLASS()
class AMSIMGAMEPLAY_API UAMSimGameInstanceSubsystem final
	: public UGameInstanceSubsystem
	, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bPhase0SmokeActive || bPhase1SmokeActive; }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual TStatId GetStatId() const override;

	bool IsReady() const { return bReady; }
	const FString& GetSaveRootDirectory() const { return SaveRootDirectory; }
	TFuture<AMSim::FSaveResult> SaveSnapshotAsync(
		const FString& SlotId,
		AMSim::FSnapshot Snapshot,
		AMSim::FSaveMetadata Metadata = {});
	bool LoadSnapshot(
		const FString& SlotId,
		AMSim::FSnapshot& Snapshot,
		bool& bUsedBackup) const;
	TArray<AMSim::FSaveSlotSummary> ListSaveSlots() const;

private:
	static bool SanitizeSlotId(const FString& SlotId, FString& SanitizedSlotId);
	void TickPhase1Smoke(float DeltaTime);

	bool bReady = false;
	bool bPhase0SmokeActive = false;
	bool bSmokeSaveLoadPassed = false;
	bool bSmokeResolutionRequested = false;
	bool bSmokeScreenshotRequested = false;
	FString SaveRootDirectory;
	double SmokeWarmupSeconds = 0.0;
	double SmokeElapsedSeconds = 0.0;
	double SmokeMaximumFrameMilliseconds = 0.0;
	uint64 SmokeFrameCount = 0;
	uint64 SmokeChecksum = 0;
	TArray<double> SmokeFrameMilliseconds;

	bool bPhase1SmokeActive = false;
	bool bPhase4SmokeRequested = false;
	bool bPhase45ConstructionProofRequested = false;
	bool bPhase45OperationalEvidenceRequested = false;
	bool bPhase1SmokeJourneyPassed = true;
	bool bPhase1SmokeSaveLoadPassed = false;
	bool bPhase1SmokeResolutionRequested = false;
	int32 Phase1SmokeStage = 0;
	double Phase1SmokeWarmupSeconds = 0.0;
	double Phase1SmokeTimeoutSeconds = 0.0;
	double Phase1SmokeElapsedSeconds = 0.0;
	double Phase1PerformanceTargetSeconds = 5.0;
	double Phase1SmokeMaximumFrameMilliseconds = 0.0;
	double Phase1SnapshotCaptureMilliseconds = 0.0;
	double Phase1SaveWriteMilliseconds = 0.0;
	uint64 Phase1SmokeFrameCount = 0;
	uint64 Phase1SmokeChecksum = 0;
	uint64 Phase1StartingMemoryMiB = 0;
	uint64 Phase1MaximumMemoryMiB = 0;
	int32 Phase1MaximumBacklogSteps = 0;
	int32 Phase1TurnaroundSettleFrames = 0;
	int32 Phase1ProposalSettleFrames = 0;
	int32 Phase4IncidentSettleFrames = 0;
	int32 Phase4CompleteSettleFrames = 0;
	bool bPhase1ReferenceProfileRequested = false;
	FString Phase1PendingScreenshot;
	TArray<double> Phase1SmokeFrameMilliseconds;
	TArray<double> Phase1SimulationWorkMilliseconds;
};
