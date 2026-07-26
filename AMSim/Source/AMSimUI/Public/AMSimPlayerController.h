#pragma once

#include "GameFramework/PlayerController.h"
#include "AMSimPlayerController.generated.h"

UCLASS()
class AMSIMUI_API AAMSimPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	AAMSimPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	void ApplyGameplayInputMode();
};
