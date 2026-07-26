#pragma once

#include "GameFramework/GameModeBase.h"
#include "AMSimGameMode.generated.h"

UCLASS()
class AMSIMUI_API AAMSimGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAMSimGameMode();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<class AAMSimWorldPresenter> WorldPresenter;
};
