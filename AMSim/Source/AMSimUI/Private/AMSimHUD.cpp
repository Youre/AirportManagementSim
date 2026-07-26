#include "AMSimHUD.h"
#include "AMSimRootScreen.h"

void AAMSimHUD::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* Controller = GetOwningPlayerController())
	{
		RootScreen = CreateWidget<UAMSimRootScreen>(Controller, UAMSimRootScreen::StaticClass());
		RootScreen->AddToViewport();
		RootScreen->ActivateWidget();
	}
}
