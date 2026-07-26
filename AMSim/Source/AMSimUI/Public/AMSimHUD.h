#pragma once

#include "GameFramework/HUD.h"
#include "AMSimHUD.generated.h"

class UAMSimRootScreen;

UCLASS()
class AMSIMUI_API AAMSimHUD final : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UAMSimRootScreen> RootScreen;
};
