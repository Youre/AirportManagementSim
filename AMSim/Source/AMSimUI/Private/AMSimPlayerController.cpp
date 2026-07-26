#include "AMSimPlayerController.h"

AAMSimPlayerController::AAMSimPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AAMSimPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ApplyGameplayInputMode();
}

void AAMSimPlayerController::ApplyGameplayInputMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}
