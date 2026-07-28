#pragma once

#include "GameFramework/HUD.h"
#include "AMSimHUD.generated.h"

class UAMSimRootScreen;
class UUserWidget;

UCLASS()
class AMSIMUI_API AAMSimHUD final : public AHUD
{
	GENERATED_BODY()

public:
	AAMSimHUD();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UAMSimRootScreen> RootScreenClass;

	UPROPERTY(Transient)
	TObjectPtr<UAMSimRootScreen> RootScreen;

	UPROPERTY()
	TSubclassOf<UUserWidget> DevelopmentGalleryClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> DevelopmentGallery;
};
