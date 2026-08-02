#include "AMSimSchedulePickerView.h"

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
	UTextBlock* MakePickerText(
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

	FString FormatGameTime(const int64 Milliseconds)
	{
		const int64 TotalMinutes = Milliseconds / 60000;
		const int64 Day = TotalMinutes / (24 * 60) + 1;
		const int64 Hour = (TotalMinutes / 60) % 24;
		const int64 Minute = TotalMinutes % 60;
		return FString::Printf(
			TEXT("DAY %lld  %02lld:%02lld"),
			Day,
			Hour,
			Minute);
	}
}

TSharedRef<SWidget> UAMSimSchedulePickerView::RebuildWidget()
{
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("SchedulePickerTree"));
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("SchedulePickerRoot"));
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
	PanelSlot->SetAnchors(FAnchors(0.30f, 0.20f, 0.70f, 0.80f));
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
		MakePickerText(
			WidgetTree,
			TEXT("ScheduleTitle"),
			TEXT("CHOOSE ARRIVAL TIME"),
			22,
			AMSim::UITheme::White(),
			true),
		0.0f);
	Add(
		MakePickerText(
			WidgetTree,
			TEXT("ScheduleHint"),
			TEXT("Select one available timetable slot. The stand is reserved automatically."),
			13,
			AMSim::UITheme::Muted()),
		4.0f);
	CurrentTimeText = MakePickerText(
		WidgetTree,
		TEXT("ScheduleCurrentTime"),
		TEXT("CURRENT TIME"),
		12,
		AMSim::UITheme::Amber(),
		true);
	Add(CurrentTimeText, 8.0f);

	for (int32 Index = 0; Index < 4; ++Index)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			FName(*FString::Printf(TEXT("ScheduleOption%d"), Index)));
		Button->SetStyle(AMSim::UITheme::ButtonStyle(
			AMSim::UITheme::EButton::Secondary));
		UTextBlock* Label = MakePickerText(
			WidgetTree,
			FString::Printf(TEXT("ScheduleOption%dLabel"), Index),
			TEXT("AVAILABLE SLOT"),
			14,
			AMSim::UITheme::White(),
			true);
		Label->SetAutoWrapText(false);
		Label->SetJustification(ETextJustify::Center);
		Button->SetContent(Label);
		OptionButtons.Add(Button);
		OptionLabels.Add(Label);
		Add(Button, 5.0f);
	}
	OptionButtons[0]->OnClicked.AddDynamic(this, &UAMSimSchedulePickerView::ChooseOption0);
	OptionButtons[1]->OnClicked.AddDynamic(this, &UAMSimSchedulePickerView::ChooseOption1);
	OptionButtons[2]->OnClicked.AddDynamic(this, &UAMSimSchedulePickerView::ChooseOption2);
	OptionButtons[3]->OnClicked.AddDynamic(this, &UAMSimSchedulePickerView::ChooseOption3);

	UButton* CancelButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("CancelSchedulePicker"));
	CancelButton->SetStyle(AMSim::UITheme::ButtonStyle(
		AMSim::UITheme::EButton::Quiet));
	CancelButton->SetContent(MakePickerText(
		WidgetTree,
		TEXT("CancelSchedulePickerLabel"),
		TEXT("CANCEL"),
		13,
		AMSim::UITheme::White(),
		true));
	CancelButton->OnClicked.AddDynamic(this, &UAMSimSchedulePickerView::Cancel);
	Add(CancelButton, 10.0f);
	RefreshOptions();
	SetVisibility(
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimSchedulePickerProof"))
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	return Root->TakeWidget();
}

void UAMSimSchedulePickerView::OpenPicker(
	const TArray<int64>& InOptions,
	const int64 CurrentGameMilliseconds)
{
	Options = InOptions;
	CurrentTimeMilliseconds = CurrentGameMilliseconds;
	RefreshOptions();
	SetVisibility(ESlateVisibility::Visible);
	AMSim::UIAudio::Play(this, EAMSimUISound::PanelOpen);
}

void UAMSimSchedulePickerView::ClosePicker()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UAMSimSchedulePickerView::IsPickerOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimSchedulePickerView::RefreshOptions()
{
	if (CurrentTimeText)
	{
		CurrentTimeText->SetText(FText::FromString(
			FString::Printf(
				TEXT("CURRENT  %s"),
				*FormatGameTime(CurrentTimeMilliseconds))));
	}
	for (int32 Index = 0; Index < OptionButtons.Num(); ++Index)
	{
		const bool bAvailable = Options.IsValidIndex(Index);
		OptionButtons[Index]->SetIsEnabled(bAvailable);
		OptionLabels[Index]->SetText(FText::FromString(
			bAvailable
				? FString::Printf(
					TEXT("%s  |  STAND A1"),
					*FormatGameTime(Options[Index]))
				: TEXT("NO SLOT")));
	}
}

void UAMSimSchedulePickerView::ChooseOption(const int32 Index)
{
	if (Options.IsValidIndex(Index) &&
		OnScheduleRequested &&
		OnScheduleRequested(Options[Index]))
	{
		ClosePicker();
	}
}

void UAMSimSchedulePickerView::ChooseOption0() { ChooseOption(0); }
void UAMSimSchedulePickerView::ChooseOption1() { ChooseOption(1); }
void UAMSimSchedulePickerView::ChooseOption2() { ChooseOption(2); }
void UAMSimSchedulePickerView::ChooseOption3() { ChooseOption(3); }
void UAMSimSchedulePickerView::Cancel()
{
	AMSim::UIAudio::Play(this, EAMSimUISound::Back);
	ClosePicker();
}
