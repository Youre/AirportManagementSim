#pragma once

#include "GameFramework/HUD.h"
#include "TimerManager.h"
#include "AMSimHUD.generated.h"

class UAMSimRootScreen;
class UUserWidget;

UCLASS()
class AMSIMUI_API AAMSimHUD final : public AHUD
{
	GENERATED_BODY()

public:
	AAMSimHUD();
	void ApplyInterfaceScale(float Scale);

protected:
	virtual void BeginPlay() override;

private:
	void CreateRootScreen(bool bOpenReleaseGuide);
	void TickDevelopmentGalleryProof();

	UPROPERTY()
	TSubclassOf<UAMSimRootScreen> RootScreenClass;

	UPROPERTY(Transient)
	TObjectPtr<UAMSimRootScreen> RootScreen;

	UPROPERTY()
	TSubclassOf<UUserWidget> DevelopmentGalleryClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> DevelopmentGallery;

	FTimerHandle DevelopmentGalleryProofTimer;
	int32 DevelopmentGalleryProofStage = 0;
};
