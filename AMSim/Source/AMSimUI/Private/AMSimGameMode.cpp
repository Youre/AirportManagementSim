#include "AMSimGameMode.h"
#include "AMSimCameraPawn.h"
#include "AMSimHUD.h"
#include "AMSimPlayerController.h"
#include "AMSimWorldPresenter.h"
#include "Engine/World.h"

AAMSimGameMode::AAMSimGameMode()
{
	DefaultPawnClass = AAMSimCameraPawn::StaticClass();
	HUDClass = AAMSimHUD::StaticClass();
	PlayerControllerClass = AAMSimPlayerController::StaticClass();
}

void AAMSimGameMode::BeginPlay()
{
	Super::BeginPlay();
	WorldPresenter = GetWorld()->SpawnActor<AAMSimWorldPresenter>();
}
