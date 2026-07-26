#include "AMSimRootScreen.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"

TSharedRef<SWidget> UAMSimRootScreen::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Phase0Root"));
		WidgetTree->RootWidget = Root;

		UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Background"));
		Background->SetBrushColor(FLinearColor(0.018f, 0.035f, 0.055f, 1.0f));
		UOverlaySlot* BackgroundSlot = Root->AddChildToOverlay(Background);
		BackgroundSlot->SetHorizontalAlignment(HAlign_Fill);
		BackgroundSlot->SetVerticalAlignment(VAlign_Fill);

		UTextBlock* Status = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Status"));
		Status->SetText(FText::FromString(TEXT("AIRPORT MANAGEMENT SIM\nPHASE 0 — EMPTY AIRPORT READY")));
		Status->SetColorAndOpacity(FSlateColor(FLinearColor(0.72f, 0.9f, 1.0f, 1.0f)));
		Status->SetJustification(ETextJustify::Center);
		UOverlaySlot* StatusSlot = Root->AddChildToOverlay(Status);
		StatusSlot->SetHorizontalAlignment(HAlign_Center);
		StatusSlot->SetVerticalAlignment(VAlign_Center);
	}
	return Super::RebuildWidget();
}
