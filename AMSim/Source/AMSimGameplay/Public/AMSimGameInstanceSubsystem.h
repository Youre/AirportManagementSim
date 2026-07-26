#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AMSimGameInstanceSubsystem.generated.h"

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
	virtual bool IsTickable() const override { return bPhase0SmokeActive; }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual TStatId GetStatId() const override;

	bool IsReady() const { return bReady; }
	const FString& GetSaveRootDirectory() const { return SaveRootDirectory; }

private:
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
};
