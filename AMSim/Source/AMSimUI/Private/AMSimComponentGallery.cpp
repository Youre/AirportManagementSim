#include "AMSimComponentGallery.h"

#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	UTextBlock* GalleryText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Value,
		const int32 Size,
		const FLinearColor& Color = AMSim::UITheme::White(),
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		Text->SetText(FText::FromString(Value));
		AMSim::UITheme::StyleText(Text, Size, Color, bBold, bBold);
		Text->SetAutoWrapText(true);
		return Text;
	}

	void AddItem(UVerticalBox* Parent, UWidget* Child, const FMargin Padding = FMargin(0.0f, 5.0f))
	{
		UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child);
		Slot->SetPadding(Padding);
	}

	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Kind,
		const FMargin Padding = FMargin(18.0f),
		const float Radius = 16.0f)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
		AMSim::UITheme::StyleSurface(Border, Kind, Padding, Radius, 1.5f);
		return Border;
	}

	UButton* GalleryButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const AMSim::UITheme::EButton Kind,
		const bool bEnabled = true)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		Button->SetStyle(AMSim::UITheme::ButtonStyle(Kind));
		UTextBlock* LabelText = GalleryText(
			Tree,
			*(FString(Name) + TEXT("Label")),
			Label,
			14,
			AMSim::UITheme::White(),
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Button->SetContent(LabelText);
		Button->SetIsEnabled(bEnabled);
		return Button;
	}

	UBorder* StatusChip(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const AMSim::UITheme::ESurface Kind)
	{
		UBorder* Chip = Surface(Tree, Name, Kind, FMargin(11.0f, 7.0f), 10.0f);
		UTextBlock* Text = GalleryText(
			Tree,
			*(FString(Name) + TEXT("Text")),
			Label,
			12,
			AMSim::UITheme::White(),
			true);
		Text->SetJustification(ETextJustify::Center);
		Chip->SetContent(Text);
		return Chip;
	}

	UBorder* GalleryCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Eyebrow,
		const FString& Heading,
		const FString& Body,
		const AMSim::UITheme::ESurface SurfaceKind = AMSim::UITheme::ESurface::Card)
	{
		UBorder* Card = Surface(Tree, Name, SurfaceKind, FMargin(18.0f, 15.0f), 16.0f);
		UVerticalBox* Column = Tree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			FName(*(FString(Name) + TEXT("Column"))));
		Card->SetContent(Column);
		AddItem(
			Column,
			GalleryText(
				Tree,
				*(FString(Name) + TEXT("Eyebrow")),
				Eyebrow,
				11,
				AMSim::UITheme::Cyan(),
				true),
			FMargin());
		AddItem(
			Column,
			GalleryText(
				Tree,
				*(FString(Name) + TEXT("Heading")),
				Heading,
				19,
				AMSim::UITheme::White(),
				true),
			FMargin(0.0f, 4.0f));
		AddItem(
			Column,
			GalleryText(
				Tree,
				*(FString(Name) + TEXT("Body")),
				Body,
				14,
				AMSim::UITheme::Muted()),
			FMargin(0.0f, 3.0f));
		return Card;
	}
}

TSharedRef<SWidget> UAMSimComponentGallery::RebuildWidget()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	UBorder* Root = Surface(
		WidgetTree,
		TEXT("GalleryRoot"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(28.0f),
		0.0f);
	WidgetTree->RootWidget = Root;
	UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(
		UScrollBox::StaticClass(),
		TEXT("GalleryScroll"));
	Root->SetContent(Scroll);
	UVerticalBox* Page = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("GalleryPage"));
	Scroll->AddChild(Page);

	AddItem(
		Page,
		GalleryText(
			WidgetTree,
			TEXT("GalleryTitle"),
			TEXT("RIVERBEND UI KIT"),
			30,
			AMSim::UITheme::White(),
			true),
		FMargin());
	AddItem(
		Page,
		GalleryText(
			WidgetTree,
			TEXT("GallerySubtitle"),
			TEXT("Rounded production primitives for every airport-management screen."),
			15,
			AMSim::UITheme::Muted()),
		FMargin(0.0f, 4.0f, 0.0f, 14.0f));

	UBorder* ButtonSection = Surface(
		WidgetTree,
		TEXT("ButtonSection"),
		AMSim::UITheme::ESurface::Panel);
	UVerticalBox* ButtonColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ButtonColumn"));
	ButtonSection->SetContent(ButtonColumn);
	AddItem(
		ButtonColumn,
		GalleryText(
			WidgetTree,
			TEXT("ButtonHeader"),
			TEXT("BUTTONS"),
			12,
			AMSim::UITheme::Amber(),
			true),
		FMargin());
	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("ButtonRow"));
	AddItem(ButtonColumn, ButtonRow, FMargin(0.0f, 7.0f));
	for (UButton* Button : {
		GalleryButton(WidgetTree, TEXT("Primary"), TEXT("SCHEDULE FLIGHT"), AMSim::UITheme::EButton::Primary),
		GalleryButton(WidgetTree, TEXT("Positive"), TEXT("CREATE AIRPORT"), AMSim::UITheme::EButton::Positive),
		GalleryButton(WidgetTree, TEXT("Secondary"), TEXT("PIN"), AMSim::UITheme::EButton::Secondary),
		GalleryButton(WidgetTree, TEXT("Danger"), TEXT("CANCEL"), AMSim::UITheme::EButton::Destructive),
		GalleryButton(WidgetTree, TEXT("Disabled"), TEXT("LOCKED"), AMSim::UITheme::EButton::Tool, false)})
	{
		UHorizontalBoxSlot* RowSlot = ButtonRow->AddChildToHorizontalBox(Button);
		RowSlot->SetPadding(FMargin(5.0f));
		RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}
	AddItem(Page, ButtonSection, FMargin(0.0f, 7.0f));

	UBorder* StatusSection = Surface(
		WidgetTree,
		TEXT("StatusSection"),
		AMSim::UITheme::ESurface::Panel);
	UVerticalBox* StatusColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("StatusColumn"));
	StatusSection->SetContent(StatusColumn);
	AddItem(
		StatusColumn,
		GalleryText(
			WidgetTree,
			TEXT("StatusHeader"),
			TEXT("STATUS CHIPS"),
			12,
			AMSim::UITheme::Amber(),
			true),
		FMargin());
	UHorizontalBox* StatusRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("StatusRow"));
	AddItem(StatusColumn, StatusRow, FMargin(0.0f, 7.0f));
	for (UBorder* Chip : {
		StatusChip(WidgetTree, TEXT("ReadyChip"), TEXT("READY"), AMSim::UITheme::ESurface::Positive),
		StatusChip(WidgetTree, TEXT("ProgressChip"), TEXT("IN PROGRESS"), AMSim::UITheme::ESurface::Chip),
		StatusChip(WidgetTree, TEXT("WarningChip"), TEXT("CHECK REQUIRED"), AMSim::UITheme::ESurface::Warning),
		StatusChip(WidgetTree, TEXT("BlockedChip"), TEXT("BLOCKED"), AMSim::UITheme::ESurface::Danger)})
	{
		UHorizontalBoxSlot* RowSlot = StatusRow->AddChildToHorizontalBox(Chip);
		RowSlot->SetPadding(FMargin(5.0f));
	}
	AddItem(Page, StatusSection, FMargin(0.0f, 7.0f));

	UBorder* CardSection = Surface(
		WidgetTree,
		TEXT("CardSection"),
		AMSim::UITheme::ESurface::Panel);
	UVerticalBox* CardColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("CardColumn"));
	CardSection->SetContent(CardColumn);
	AddItem(
		CardColumn,
		GalleryText(
			WidgetTree,
			TEXT("CardsHeader"),
			TEXT("CARDS"),
			12,
			AMSim::UITheme::Amber(),
			true),
		FMargin());
	UHorizontalBox* CardRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("CardRow"));
	AddItem(CardColumn, CardRow, FMargin(0.0f, 7.0f));
	for (UBorder* Card : {
		GalleryCard(
			WidgetTree,
			TEXT("ObjectiveCard"),
			TEXT("ACTIVE OBJECTIVE"),
			TEXT("Open the airfield"),
			TEXT("Complete the safety inspection, then open Runway 09/27.")),
		GalleryCard(
			WidgetTree,
			TEXT("OfferCard"),
			TEXT("FIRST-FLIGHT OFFER"),
			TEXT("Riverbend 21"),
			TEXT("Light trainer  •  Stand A1  •  600 CR + 5 AP"),
			AMSim::UITheme::ESurface::RaisedCard),
		GalleryCard(
			WidgetTree,
			TEXT("LongTextCard"),
			TEXT("TEXT EXPANSION"),
			TEXT("Representative long heading"),
			TEXT("This localization stress case expands safely without replacing the concise production hierarchy."))})
	{
		UHorizontalBoxSlot* RowSlot = CardRow->AddChildToHorizontalBox(Card);
		RowSlot->SetPadding(FMargin(5.0f));
		RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}
	AddItem(Page, CardSection, FMargin(0.0f, 7.0f));

	return Super::RebuildWidget();
}
