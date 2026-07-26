#include "AMSimComponentGallery.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"

namespace
{
	const FLinearColor GalleryBackground(0.006f, 0.014f, 0.021f, 1.0f);
	const FLinearColor GalleryPanel(0.012f, 0.028f, 0.038f, 1.0f);
	const FLinearColor GalleryPanelLight(0.024f, 0.064f, 0.078f, 1.0f);
	const FLinearColor GalleryCyan(0.27f, 0.82f, 0.92f, 1.0f);
	const FLinearColor GalleryAmber(0.96f, 0.68f, 0.22f, 1.0f);
	const FLinearColor GalleryWhite(0.91f, 0.95f, 0.94f, 1.0f);
	const FLinearColor GalleryMuted(0.57f, 0.68f, 0.69f, 1.0f);
	const FLinearColor GalleryDanger(0.76f, 0.16f, 0.13f, 1.0f);

	UTextBlock* GalleryText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Value,
		const int32 Size,
		const FLinearColor& Color = GalleryWhite)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		Text->SetText(FText::FromString(Value));
		Text->SetColorAndOpacity(FSlateColor(Color));
		Text->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Size));
		Text->SetAutoWrapText(true);
		return Text;
	}

	void AddGalleryItem(UVerticalBox* Parent, UWidget* Child, const float Padding = 5.0f)
	{
		UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child);
		Slot->SetPadding(FMargin(0.0f, Padding));
	}

	UBorder* GalleryCard(UWidgetTree* Tree, const TCHAR* Name, UVerticalBox*& Content)
	{
		UBorder* Card = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
		Card->SetBrushColor(GalleryPanel);
		Card->SetPadding(FMargin(20.0f));
		Content = Tree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			FName(*(FString(Name) + TEXT("Content"))));
		Card->SetContent(Content);
		return Card;
	}

	UButton* GalleryButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const FLinearColor& Normal,
		const bool bEnabled = true)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(Normal);
		Style.Hovered.TintColor = FSlateColor(GalleryCyan);
		Style.Pressed.TintColor = FSlateColor(GalleryAmber);
		Style.Disabled.TintColor = FSlateColor(FLinearColor(0.008f, 0.016f, 0.020f, 0.55f));
		Button->SetStyle(Style);
		Button->SetContent(GalleryText(
			Tree,
			*(FString(Name) + TEXT("Label")),
			Label,
			16));
		Button->SetIsEnabled(bEnabled);
		return Button;
	}
}

TSharedRef<SWidget> UAMSimComponentGallery::RebuildWidget()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	UBorder* Root = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("GalleryRoot"));
	Root->SetBrushColor(GalleryBackground);
	Root->SetPadding(FMargin(28.0f));
	WidgetTree->RootWidget = Root;
	UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("GalleryScroll"));
	Root->SetContent(Scroll);
	UVerticalBox* Page = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("GalleryPage"));
	Scroll->AddChild(Page);

	AddGalleryItem(Page, GalleryText(
		WidgetTree,
		TEXT("GalleryTitle"),
		TEXT("PHASE 1.5 COMPONENT GALLERY"),
		30,
		GalleryCyan));
	AddGalleryItem(Page, GalleryText(
		WidgetTree,
		TEXT("GallerySubtitle"),
		TEXT("Production states, interaction variants, color-independent labels, and long-text stress cases."),
		16,
		GalleryMuted));

	UVerticalBox* Buttons = nullptr;
	AddGalleryItem(Page, GalleryCard(WidgetTree, TEXT("ButtonsCard"), Buttons), 14.0f);
	AddGalleryItem(Buttons, GalleryText(WidgetTree, TEXT("ButtonsHeader"), TEXT("BUTTONS"), 14, GalleryAmber));
	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("ButtonRow"));
	AddGalleryItem(Buttons, ButtonRow);
	for (UButton* Button : {
		GalleryButton(WidgetTree, TEXT("Primary"), TEXT("PRIMARY ACTION"), FLinearColor(0.025f, 0.20f, 0.24f, 1.0f)),
		GalleryButton(WidgetTree, TEXT("Secondary"), TEXT("SECONDARY"), GalleryPanelLight),
		GalleryButton(WidgetTree, TEXT("Destructive"), TEXT("CANCEL PROJECT"), GalleryDanger),
		GalleryButton(WidgetTree, TEXT("IconTool"), TEXT("[+] TOOL"), GalleryPanelLight),
		GalleryButton(WidgetTree, TEXT("Disabled"), TEXT("DISABLED"), GalleryPanelLight, false)})
	{
		ButtonRow->AddChildToHorizontalBox(Button)->SetPadding(FMargin(4.0f));
	}

	UVerticalBox* Status = nullptr;
	AddGalleryItem(Page, GalleryCard(WidgetTree, TEXT("StatusCard"), Status), 14.0f);
	AddGalleryItem(Status, GalleryText(WidgetTree, TEXT("StatusHeader"), TEXT("STATUS CHIPS"), 14, GalleryAmber));
	AddGalleryItem(Status, GalleryText(
		WidgetTree,
		TEXT("StatusValues"),
		TEXT("[VALID] Connected package    [IN PROGRESS] Inspection    [BLOCKED] Stand occupied    [READY] Departure"),
		18,
		GalleryWhite));
	AddGalleryItem(Status, GalleryText(
		WidgetTree,
		TEXT("StatusNote"),
		TEXT("Every state is encoded by label and symbol as well as color."),
		14,
		GalleryMuted));

	UVerticalBox* Cards = nullptr;
	AddGalleryItem(Page, GalleryCard(WidgetTree, TEXT("CardsCard"), Cards), 14.0f);
	AddGalleryItem(Cards, GalleryText(WidgetTree, TEXT("CardsHeader"), TEXT("CONTEXT CARDS"), 14, GalleryAmber));
	AddGalleryItem(Cards, GalleryText(
		WidgetTree,
		TEXT("ObjectiveCard"),
		TEXT("ACTIVE OBJECTIVE\nBuild a grass runway, taxi connection, stand, access road, and operations hut."),
		19));
	AddGalleryItem(Cards, GalleryText(
		WidgetTree,
		TEXT("OfferCard"),
		TEXT("FIRST-FLIGHT OFFER\nRiverbend 21  |  Light piston trainer  |  600 Credits + 5 AP"),
		18,
		GalleryCyan));
	AddGalleryItem(Cards, GalleryText(
		WidgetTree,
		TEXT("LongTextCard"),
		TEXT("LONG-TEXT STRESS\nThis representative localization string deliberately expands well beyond the concise English source so wrapping, vertical growth, and scanning rhythm remain reviewable at every supported scale."),
		16,
		GalleryMuted));

	return Super::RebuildWidget();
}
