#include "AMSimHUD.h"
#include "AMSimRootScreen.h"
#include "Blueprint/UserWidget.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/ConstructorHelpers.h"

AAMSimHUD::AAMSimHUD()
{
	static ConstructorHelpers::FClassFinder<UAMSimRootScreen> ProductionScreen(
		TEXT("/Game/UI/Screens/WBP_AMSimRootScreen"));
	if (ProductionScreen.Succeeded())
	{
		RootScreenClass = ProductionScreen.Class;
	}
#if !UE_BUILD_SHIPPING
	else
	{
		ensureMsgf(
			false,
			TEXT("Production root screen /Game/UI/Screens/WBP_AMSimRootScreen is missing; using the native audit fallback."));
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> Gallery(
		TEXT("/Game/Developer/Phase15/WBP_AMSimComponentGallery"));
	if (Gallery.Succeeded())
	{
		DevelopmentGalleryClass = Gallery.Class;
	}
#endif
}

void AAMSimHUD::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* Controller = GetOwningPlayerController())
	{
#if !UE_BUILD_SHIPPING
		if (FParse::Param(FCommandLine::Get(), TEXT("AMSimComponentGallery")))
		{
			DevelopmentGallery = DevelopmentGalleryClass
				? CreateWidget<UUserWidget>(
					Controller,
					DevelopmentGalleryClass)
				: nullptr;
			if (DevelopmentGallery)
			{
				DevelopmentGallery->AddToViewport();
			}
			return;
		}
#endif
		TSubclassOf<UAMSimRootScreen> ScreenClass = RootScreenClass;
		if (!ScreenClass)
		{
			ScreenClass = UAMSimRootScreen::StaticClass();
		}
		RootScreen = CreateWidget<UAMSimRootScreen>(Controller, ScreenClass);
		RootScreen->AddToViewport();
		RootScreen->ActivateWidget();
	}
}
