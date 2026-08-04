#include "AMSimPhase1OperationsHubView.h"

#include "AMSimUISoundSubsystem.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	UTextBlock* MakeHubText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(), Name);
		Text->SetText(FText::FromString(Value));
		Text->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Text, Size, Color, bBold, true);
		return Text;
	}

	UButton* MakeHubButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Label,
		const AMSim::UITheme::EButton Style)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		Button->SetStyle(AMSim::UITheme::ButtonStyle(Style));
		Button->SetContent(MakeHubText(
			Tree, *FString::Printf(TEXT("%sLabel"), Name), Label, 12,
			AMSim::UITheme::White(), true));
		return Button;
	}
}

TSharedRef<SWidget> UAMSimPhase1OperationsHubView::RebuildWidget()
{
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("Phase1OperationsHubTree"));
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(), TEXT("Phase1OperationsHubPanel"));
	WidgetTree->RootWidget = Panel;
	AMSim::UITheme::StyleSurface(
		Panel, AMSim::UITheme::ESurface::Panel, FMargin(18.0f), 18.0f, 2.0f);

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>();
	Panel->SetContent(Column);
	const auto Add = [Column](UWidget* Child, const float Top)
	{
		UVerticalBoxSlot* Slot = Column->AddChildToVerticalBox(Child);
		Slot->SetPadding(FMargin(0.0f, Top, 0.0f, 0.0f));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	};
	EyebrowText = MakeHubText(WidgetTree, TEXT("HubEyebrow"), TEXT("OPERATIONS"),
		12, AMSim::UITheme::Cyan(), true);
	TitleText = MakeHubText(WidgetTree, TEXT("HubTitle"), TEXT("FIRST VISIT"),
		24, AMSim::UITheme::White(), true);
	StatusText = MakeHubText(WidgetTree, TEXT("HubStatus"), TEXT("READY"),
		14, AMSim::UITheme::Amber(), true);
	DetailText = MakeHubText(WidgetTree, TEXT("HubDetail"), TEXT(""),
		13, AMSim::UITheme::White());
	TimelineText = MakeHubText(WidgetTree, TEXT("HubTimeline"), TEXT(""),
		11, AMSim::UITheme::CyanSoft(), true);
	FooterText = MakeHubText(WidgetTree, TEXT("HubFooter"), TEXT(""),
		11, AMSim::UITheme::Muted());
	Add(EyebrowText, 0.0f);
	Add(TitleText, 3.0f);
	Add(StatusText, 12.0f);
	Add(DetailText, 5.0f);
	Add(TimelineText, 12.0f);

	ChooseArrivalButton = MakeHubButton(
		WidgetTree, TEXT("HubChooseArrival"), TEXT("CHOOSE ARRIVAL"),
		AMSim::UITheme::EButton::Positive);
	ChooseArrivalButton->OnClicked.AddDynamic(
		this, &UAMSimPhase1OperationsHubView::ChooseArrival);
	WatchButton = MakeHubButton(
		WidgetTree, TEXT("HubWatch"), TEXT("WATCH AT 1x"),
		AMSim::UITheme::EButton::Secondary);
	WatchButton->OnClicked.AddDynamic(this, &UAMSimPhase1OperationsHubView::WatchAtOne);
	AdvanceButton = MakeHubButton(
		WidgetTree, TEXT("HubAdvance"), TEXT("ADVANCE TO ARRIVAL"),
		AMSim::UITheme::EButton::Positive);
	AdvanceButton->OnClicked.AddDynamic(
		this, &UAMSimPhase1OperationsHubView::AdvanceToArrival);
	AdvanceButtonText = Cast<UTextBlock>(AdvanceButton->GetContent());
	Add(ChooseArrivalButton, 13.0f);
	UHorizontalBox* PaceRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	for (UButton* Button : {WatchButton, AdvanceButton})
	{
		UHorizontalBoxSlot* ButtonSlot = PaceRow->AddChildToHorizontalBox(Button);
		ButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ButtonSlot->SetPadding(FMargin(2.0f));
	}
	Add(PaceRow, 8.0f);

	UHorizontalBox* OverlayRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	AirfieldOverlayButton = MakeHubButton(
		WidgetTree, TEXT("HubAirfieldOverlay"), TEXT("AIRFIELD"),
		AMSim::UITheme::EButton::Tool);
	ConnectionsOverlayButton = MakeHubButton(
		WidgetTree, TEXT("HubConnectionsOverlay"), TEXT("CONNECTIONS"),
		AMSim::UITheme::EButton::Tool);
	ActivityOverlayButton = MakeHubButton(
		WidgetTree, TEXT("HubActivityOverlay"), TEXT("ACTIVITY"),
		AMSim::UITheme::EButton::Tool);
	AirfieldOverlayButton->OnClicked.AddDynamic(
		this, &UAMSimPhase1OperationsHubView::SelectAirfieldOverlay);
	ConnectionsOverlayButton->OnClicked.AddDynamic(
		this, &UAMSimPhase1OperationsHubView::SelectConnectionsOverlay);
	ActivityOverlayButton->OnClicked.AddDynamic(
		this, &UAMSimPhase1OperationsHubView::SelectActivityOverlay);
	for (UButton* Button : {
		AirfieldOverlayButton, ConnectionsOverlayButton, ActivityOverlayButton})
	{
		UHorizontalBoxSlot* ButtonSlot = OverlayRow->AddChildToHorizontalBox(Button);
		ButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ButtonSlot->SetPadding(FMargin(2.0f));
	}
	Add(OverlayRow, 9.0f);
	Add(FooterText, 10.0f);
	UButton* CloseButton = MakeHubButton(
		WidgetTree, TEXT("HubClose"), TEXT("CLOSE"),
		AMSim::UITheme::EButton::Quiet);
	CloseButton->OnClicked.AddDynamic(this, &UAMSimPhase1OperationsHubView::Close);
	Add(CloseButton, 10.0f);
	SetVisibility(ESlateVisibility::Collapsed);
	return Panel->TakeWidget();
}

void UAMSimPhase1OperationsHubView::OpenPage(
	const AMSim::EPhase1OperationsPage Page)
{
	CurrentPage = Page;
	SetVisibility(ESlateVisibility::Visible);
	AMSim::UIAudio::Play(this, EAMSimUISound::PanelOpen);
}

void UAMSimPhase1OperationsHubView::ClosePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UAMSimPhase1OperationsHubView::IsPanelOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimPhase1OperationsHubView::RefreshFromSnapshot(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State,
	const int32 OverlayMode)
{
	const AMSim::FPhase1OperationsHubState View =
		AMSim::FPhase1OperationsHubPresentation::Derive(
			CurrentPage, Query, State, OverlayMode);
	const auto SetText = [](UTextBlock* Text, const FString& Value)
	{
		if (Text) Text->SetText(FText::FromString(Value));
	};
	SetText(EyebrowText, View.Eyebrow);
	SetText(TitleText, View.Title);
	SetText(StatusText, View.Status);
	SetText(DetailText, View.Detail);
	SetText(TimelineText, View.Timeline);
	SetText(FooterText, View.Footer);
	SetText(AdvanceButtonText, View.AdvanceLabel);
	if (ChooseArrivalButton) ChooseArrivalButton->SetVisibility(
		View.bShowChooseArrival ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (WatchButton) WatchButton->SetVisibility(
		View.bShowWatch ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (AdvanceButton) AdvanceButton->SetVisibility(
		View.bShowAdvance ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	for (UButton* Button : {
		AirfieldOverlayButton, ConnectionsOverlayButton, ActivityOverlayButton})
	{
		if (Button) Button->SetVisibility(
			View.bShowOverlayModes ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UAMSimPhase1OperationsHubView::ChooseArrival()
{
	OnChooseArrival.ExecuteIfBound();
}

void UAMSimPhase1OperationsHubView::WatchAtOne()
{
	OnWatchAtOne.ExecuteIfBound();
}

void UAMSimPhase1OperationsHubView::AdvanceToArrival()
{
	OnAdvanceToArrival.ExecuteIfBound();
}

void UAMSimPhase1OperationsHubView::SelectAirfieldOverlay()
{
	OnOverlaySelected.ExecuteIfBound(0);
}

void UAMSimPhase1OperationsHubView::SelectConnectionsOverlay()
{
	OnOverlaySelected.ExecuteIfBound(1);
}

void UAMSimPhase1OperationsHubView::SelectActivityOverlay()
{
	OnOverlaySelected.ExecuteIfBound(2);
}

void UAMSimPhase1OperationsHubView::Close()
{
	AMSim::UIAudio::Play(this, EAMSimUISound::Back);
	ClosePanel();
}
