#include "AMSimSaveLoadView.h"

#include "AMSimUITheme.h"
#include "AMSimUISoundSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace
{
	UTextBlock* MakeLoadText(
		UWidgetTree* Tree,
		const FString& Name,
		const FString& Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			FName(*Name));
		Text->SetText(FText::FromString(Value));
		Text->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Text, Size, Color, bBold, true);
		return Text;
	}
}

TSharedRef<SWidget> UAMSimSaveLoadView::RebuildWidget()
{
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("SaveLoadTree"));
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("SaveLoadRoot"));
	WidgetTree->RootWidget = Root;

	UBorder* Dim = WidgetTree->ConstructWidget<UBorder>();
	Dim->SetBrushColor(FLinearColor(0.01f, 0.02f, 0.03f, 0.72f));
	UCanvasPanelSlot* DimSlot = Root->AddChildToCanvas(Dim);
	DimSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	DimSlot->SetOffsets(FMargin());

	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>();
	AMSim::UITheme::StyleSurface(
		Panel,
		AMSim::UITheme::ESurface::Panel,
		FMargin(22.0f),
		16.0f,
		2.0f);
	UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(Panel);
	PanelSlot->SetAnchors(FAnchors(0.32f, 0.18f, 0.68f, 0.82f));
	PanelSlot->SetOffsets(FMargin());

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>();
	Panel->SetContent(Column);
	auto Add = [Column](UWidget* Child, const float Spacing)
		{
			UVerticalBoxSlot* Slot = Column->AddChildToVerticalBox(Child);
			Slot->SetPadding(FMargin(0.0f, Spacing));
			Slot->SetHorizontalAlignment(HAlign_Fill);
		};
	Add(
		MakeLoadText(
			WidgetTree,
			TEXT("LoadTitle"),
			TEXT("LOAD AIRPORT"),
			22,
			AMSim::UITheme::White(),
			true),
		0.0f);
	Add(
		MakeLoadText(
			WidgetTree,
			TEXT("LoadHint"),
			TEXT("Choose a saved airport. Your current game is unchanged until a slot loads successfully."),
			13,
			AMSim::UITheme::Muted()),
		4.0f);
	EmptyStateText = MakeLoadText(
		WidgetTree,
		TEXT("LoadEmptyState"),
		TEXT("NO SAVED AIRPORTS YET"),
		13,
		AMSim::UITheme::Amber(),
		true);
	Add(EmptyStateText, 8.0f);

	for (int32 Index = 0; Index < 4; ++Index)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			FName(*FString::Printf(TEXT("LoadSlot%d"), Index)));
		Button->SetStyle(AMSim::UITheme::ButtonStyle(
			AMSim::UITheme::EButton::Secondary));
		UTextBlock* Label = MakeLoadText(
			WidgetTree,
			FString::Printf(TEXT("LoadSlot%dLabel"), Index),
			TEXT("EMPTY"),
			13,
			AMSim::UITheme::White(),
			true);
		Label->SetJustification(ETextJustify::Center);
		Button->SetContent(Label);
		SlotButtons.Add(Button);
		SlotLabels.Add(Label);
		Add(Button, 5.0f);
	}
	SlotButtons[0]->OnClicked.AddDynamic(this, &UAMSimSaveLoadView::ChooseSlot0);
	SlotButtons[1]->OnClicked.AddDynamic(this, &UAMSimSaveLoadView::ChooseSlot1);
	SlotButtons[2]->OnClicked.AddDynamic(this, &UAMSimSaveLoadView::ChooseSlot2);
	SlotButtons[3]->OnClicked.AddDynamic(this, &UAMSimSaveLoadView::ChooseSlot3);

	UButton* CancelButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("CancelLoad"));
	CancelButton->SetStyle(AMSim::UITheme::ButtonStyle(
		AMSim::UITheme::EButton::Quiet));
	CancelButton->SetContent(MakeLoadText(
		WidgetTree,
		TEXT("CancelLoadLabel"),
		TEXT("CANCEL"),
		13,
		AMSim::UITheme::White(),
		true));
	CancelButton->OnClicked.AddDynamic(this, &UAMSimSaveLoadView::Cancel);
	Add(CancelButton, 10.0f);
	RefreshSlots();
	SetVisibility(
		FParse::Param(FCommandLine::Get(), TEXT("AMSimLoadMenuProof"))
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	return Root->TakeWidget();
}

void UAMSimSaveLoadView::OpenPicker(
	const TArray<AMSim::FSaveSlotSummary>& InSlots)
{
	Slots = InSlots;
	if (Slots.Num() > 4)
	{
		Slots.SetNum(4);
	}
	RefreshSlots();
	SetVisibility(ESlateVisibility::Visible);
	AMSim::UIAudio::Play(this, EAMSimUISound::PanelOpen);
}

void UAMSimSaveLoadView::ClosePicker()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UAMSimSaveLoadView::IsPickerOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimSaveLoadView::RefreshSlots()
{
	if (EmptyStateText)
	{
		EmptyStateText->SetVisibility(
			Slots.IsEmpty()
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	for (int32 Index = 0; Index < SlotButtons.Num(); ++Index)
	{
		const bool bAvailable = Slots.IsValidIndex(Index);
		SlotButtons[Index]->SetVisibility(
			bAvailable
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
		SlotButtons[Index]->SetIsEnabled(bAvailable);
		if (bAvailable)
		{
			const AMSim::FSaveSlotSummary& Summary = Slots[Index];
			const FString Airport = Summary.Metadata.AirportName.IsEmpty()
				? TEXT("UNNAMED AIRPORT")
				: Summary.Metadata.AirportName.ToUpper();
			SlotLabels[Index]->SetText(FText::FromString(FString::Printf(
				TEXT("%s  |  %s  |  DAY %lld"),
				*Airport,
				*Summary.SlotId.ToUpper(),
				Summary.Metadata.GameTimeMilliseconds /
					(24ll * 60ll * 60ll * 1000ll) + 1)));
		}
	}
}

void UAMSimSaveLoadView::ChooseSlot(const int32 Index)
{
	if (Slots.IsValidIndex(Index) &&
		OnLoadRequested &&
		OnLoadRequested(Slots[Index].SlotId))
	{
		ClosePicker();
	}
}

void UAMSimSaveLoadView::ChooseSlot0() { ChooseSlot(0); }
void UAMSimSaveLoadView::ChooseSlot1() { ChooseSlot(1); }
void UAMSimSaveLoadView::ChooseSlot2() { ChooseSlot(2); }
void UAMSimSaveLoadView::ChooseSlot3() { ChooseSlot(3); }
void UAMSimSaveLoadView::Cancel()
{
	AMSim::UIAudio::Play(this, EAMSimUISound::Back);
	ClosePicker();
}
