#include "AMSimTerminalView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimRegionalOperationsView.h"
#include "AMSimUITheme.h"
#include "AMSimWorldPresenter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine/Texture2D.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/ConstructorHelpers.h"

namespace AMSimTerminalViewPrivate
{
	using namespace AMSim::UITheme;

	UTextBlock* MakeText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Copy,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			Name);
		Text->SetText(FText::FromString(Copy));
		Text->SetAutoWrapText(true);
		Text->SetLineHeightPercentage(0.92f);
		StyleText(Text, Size, Color, bBold, true);
		return Text;
	}

	UBorder* MakeSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const ESurface Surface,
		const FMargin& Padding = FMargin(12.0f),
		const float Radius = 14.0f)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			Name);
		StyleSurface(Border, Surface, Padding, Radius, 1.4f);
		return Border;
	}

	UButton* MakeButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const EButton Kind,
		const int32 Size)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			Name);
		Button->SetStyle(ButtonStyle(Kind, 11.0f));
		UTextBlock* Text = MakeText(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			Size,
			White(),
			true);
		Text->SetJustification(ETextJustify::Center);
		Button->SetContent(Text);
		return Button;
	}

	UCanvasPanelSlot* AddAnchored(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors& Anchors,
		const FMargin& Offsets,
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(Offsets);
		Slot->SetZOrder(ZOrder);
		return Slot;
	}

	void AddVertical(
		UVerticalBox* Box,
		UWidget* Widget,
		const float Bottom = 8.0f,
		const bool bFill = false)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, Bottom));
		Slot->SetSize(FSlateChildSize(
			bFill ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic));
	}

	UTextBlock* AddSectionTitle(
		UWidgetTree* Tree,
		UVerticalBox* Box,
		const TCHAR* Name,
		const FString& Label,
		const int32 Size)
	{
		UTextBlock* Text = MakeText(Tree, Name, Label, Size, Cyan(), true);
		AddVertical(Box, Text, 7.0f);
		return Text;
	}

	UBorder* MakeMetricCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		TObjectPtr<UTextBlock>& OutText,
		const FString& Initial,
		const int32 Size,
		const ESurface Surface = ESurface::Card)
	{
		UBorder* Card = MakeSurface(Tree, Name, Surface, FMargin(11.0f), 12.0f);
		OutText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sText"), Name),
			Initial,
			Size,
			White());
		Card->SetContent(OutText);
		return Card;
	}

	UTextBlock* AddLabel(
		UWidgetTree* Tree,
		UCanvasPanel* Canvas,
		const TCHAR* Name,
		const FString& Copy,
		const FVector2D& Position,
		const FVector2D& Size,
		const int32 FontSize)
	{
		UBorder* Label = MakeSurface(
			Tree,
			Name,
			ESurface::Chip,
			FMargin(8.0f, 5.0f),
			9.0f);
		UTextBlock* Text = MakeText(
			Tree,
			*FString::Printf(TEXT("%sText"), Name),
			Copy.ToUpper(),
			FontSize,
			White(),
			true);
		Text->SetJustification(ETextJustify::Center);
		Label->SetContent(Text);
		AddAnchored(
			Canvas,
			Label,
			FAnchors(Position.X, Position.Y),
			FMargin(-Size.X * 0.5f, -Size.Y * 0.5f, Size.X, Size.Y),
			3);
		return Text;
	}
}

using namespace AMSimTerminalViewPrivate;

UAMSimTerminalView::UAMSimTerminalView(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> PassengerFamily(
		TEXT("/Game/Phase45/Presentation/Textures/Operations/T_PassengerFamily.T_PassengerFamily"));
	PassengerFamilyTexture = PassengerFamily.Object;
}

TSharedRef<SWidget> UAMSimTerminalView::RebuildWidget()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	const UUserInterfaceSettings* Settings = GetDefault<UUserInterfaceSettings>();
	const float Scale = Settings
		? Settings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	const bool bCompact = Scale >= 1.75f;
	const int32 BodySize = bCompact ? 12 : 14;
	const int32 SmallSize = bCompact ? 10 : 12;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("TerminalCanvas"));
	WidgetTree->RootWidget = Canvas;
	TerminalCanvas = Canvas;
	TerminalChrome = MakeSurface(
		WidgetTree,
		TEXT("TerminalChrome"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(0.0f),
		0.0f);
	TerminalChrome->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.32f));
	AddAnchored(
		Canvas,
		TerminalChrome,
		FAnchors(0.0f, 0.0f, 1.0f, 1.0f),
		FMargin(0.0f),
		0);

	UBorder* TopBar = MakeSurface(
		WidgetTree,
		TEXT("TerminalTopBar"),
		AMSim::UITheme::ESurface::Chrome,
		bCompact ? FMargin(12.0f, 7.0f) : FMargin(20.0f, 10.0f),
		16.0f);
	UHorizontalBox* TopRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("TerminalTopRow"));
	TopBar->SetContent(TopRow);
	UTextBlock* Brand = MakeText(
		WidgetTree,
		TEXT("TerminalBrand"),
		bCompact ? TEXT("RIVERBEND  ·  TERMINAL") :
			TEXT("RIVERBEND AIRPORT\nDOMESTIC TERMINAL"),
		bCompact ? 15 : 18,
		AMSim::UITheme::White(),
		true);
	UHorizontalBoxSlot* BrandSlot = TopRow->AddChildToHorizontalBox(Brand);
	BrandSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BrandSlot->SetVerticalAlignment(VAlign_Center);
	UButton* ReturnButton = MakeButton(
		WidgetTree,
		TEXT("TerminalReturn"),
		TEXT("BACK"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	ReturnButton->OnClicked.AddDynamic(
		this,
		&UAMSimTerminalView::ReturnToAirport);
	TopRow->AddChildToHorizontalBox(
		ReturnButton)->SetPadding(FMargin(6.0f));
	FundsText = MakeText(
		WidgetTree,
		TEXT("TerminalFunds"),
		TEXT("0 CR"),
		bCompact ? 13 : 16,
		AMSim::UITheme::Amber(),
		true);
	TopRow->AddChildToHorizontalBox(FundsText)->SetPadding(FMargin(12.0f));
	UTextBlock* Weather = MakeText(
		WidgetTree,
		TEXT("TerminalWeather"),
		TEXT("FAIR  ·  8 KT"),
		SmallSize,
		AMSim::UITheme::Muted(),
		true);
	TopRow->AddChildToHorizontalBox(Weather)->SetPadding(FMargin(12.0f));
	AdvancedOperationsButton = MakeButton(
		WidgetTree,
		TEXT("TerminalAdvancedOperations"),
		bCompact ? TEXT("ADV") : TEXT("ADVANCED"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	AdvancedOperationsButton->OnClicked.AddDynamic(
		this,
		&UAMSimTerminalView::OpenAdvancedOperations);
	AdvancedOperationsButton->SetVisibility(
		ESlateVisibility::Collapsed);
	TopRow->AddChildToHorizontalBox(
		AdvancedOperationsButton)->SetPadding(FMargin(6.0f));
	MajorOperationsButton = MakeButton(
		WidgetTree,
		TEXT("TerminalMajorOperations"),
		TEXT("MAJOR"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	MajorOperationsButton->OnClicked.AddDynamic(
		this,
		&UAMSimTerminalView::OpenMajorOperations);
	MajorOperationsButton->SetVisibility(
		ESlateVisibility::Collapsed);
	TopRow->AddChildToHorizontalBox(
		MajorOperationsButton)->SetPadding(FMargin(6.0f));
	ClockText = MakeText(
		WidgetTree,
		TEXT("TerminalClock"),
		TEXT("DAY 1  ·  08:00  ·  8x"),
		bCompact ? 12 : 15,
		AMSim::UITheme::Cyan(),
		true);
	TopRow->AddChildToHorizontalBox(ClockText)->SetPadding(FMargin(12.0f));
	AddAnchored(
		Canvas,
		TopBar,
		FAnchors(0.008f, 0.012f, 0.992f, bCompact ? 0.083f : 0.09f),
		FMargin(0.0f),
		10);

	UBorder* LeftRail = MakeSurface(
		WidgetTree,
		TEXT("TerminalToolsRail"),
		AMSim::UITheme::ESurface::Panel,
		bCompact ? FMargin(11.0f) : FMargin(14.0f),
		17.0f);
	UVerticalBox* Tools = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("TerminalTools"));
	UScrollBox* ToolsDrawer = WidgetTree->ConstructWidget<UScrollBox>(
		UScrollBox::StaticClass(),
		TEXT("TerminalToolsDrawer"));
	ToolsDrawer->SetAnimateWheelScrolling(true);
	ToolsDrawer->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	ToolsDrawer->AddChild(Tools);
	LeftRail->SetContent(ToolsDrawer);
	AddSectionTitle(
		WidgetTree,
		Tools,
		TEXT("TerminalToolsTitle"),
		TEXT("TERMINAL TOOLS"),
		bCompact ? 13 : 16);
	StatusText = MakeText(
		WidgetTree,
		TEXT("TerminalStatus"),
		TEXT("Passenger terminal available"),
		BodySize,
		AMSim::UITheme::White(),
		true);
	AddVertical(Tools, StatusText, 9.0f);
	ConstructionText = MakeText(
		WidgetTree,
		TEXT("TerminalConstruction"),
		TEXT("DOMESTIC TERMINAL NOT STARTED"),
		SmallSize,
		AMSim::UITheme::Muted(),
		true);
	AddVertical(Tools, ConstructionText, 12.0f);

	InitializeButton = MakeButton(
		WidgetTree, TEXT("InitializeTerminal"), TEXT("OPEN TERMINAL PLANNING"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	InitializeButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::InitializePassengerAirport);
	AddVertical(Tools, InitializeButton, 7.0f);
	FundButton = MakeButton(
		WidgetTree, TEXT("FundTerminal"), TEXT("FUND TERMINAL SHELL"),
		AMSim::UITheme::EButton::Positive, SmallSize);
	FundButton->OnClicked.AddDynamic(this, &UAMSimTerminalView::FundTerminal);
	AddVertical(Tools, FundButton, 7.0f);
	ConnectButton = MakeButton(
		WidgetTree, TEXT("ConnectTerminal"), TEXT("CONNECT NEXT FLOW"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	ConnectButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::ConnectNextNetwork);
	AddVertical(Tools, ConnectButton, 7.0f);
	OpenButton = MakeButton(
		WidgetTree, TEXT("OpenTerminal"), TEXT("OPEN TERMINAL"),
		AMSim::UITheme::EButton::Positive, SmallSize);
	OpenButton->OnClicked.AddDynamic(this, &UAMSimTerminalView::OpenTerminal);
	AddVertical(Tools, OpenButton, 7.0f);
	ScheduleButton = MakeButton(
		WidgetTree, TEXT("SchedulePassengerService"),
		TEXT("SCHEDULE RB 304"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	ScheduleButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::SchedulePassengerService);
	AddVertical(Tools, ScheduleButton, 12.0f);

	AddSectionTitle(
		WidgetTree,
		Tools,
		TEXT("FlowToolsTitle"),
		TEXT("OPERATIONS"),
		bCompact ? 12 : 14);
	SecurityButton = MakeButton(
		WidgetTree, TEXT("SecurityLane"), TEXT("TOGGLE SECURITY LANE"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	SecurityButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::ToggleSecurityLane);
	AddVertical(Tools, SecurityButton, 7.0f);
	AssistanceButton = MakeButton(
		WidgetTree, TEXT("PassengerAssistance"), TEXT("ASSIGN ROUTE ASSISTANCE"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	AssistanceButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::RequestPassengerAssistance);
	AddVertical(Tools, AssistanceButton, 7.0f);
	BaggageButton = MakeButton(
		WidgetTree, TEXT("BaggageRecovery"), TEXT("RESOLVE BAG EXCEPTION"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	BaggageButton->OnClicked.AddDynamic(
		this, &UAMSimTerminalView::ResolveBaggageException);
	AddVertical(Tools, BaggageButton, 9.0f);
	UButton* OverlayButton = MakeButton(
		WidgetTree, TEXT("OverlayMode"), TEXT("CYCLE FLOW OVERLAY"),
		AMSim::UITheme::EButton::Secondary, SmallSize);
	OverlayButton->OnClicked.AddDynamic(this, &UAMSimTerminalView::CycleOverlay);
	AddVertical(Tools, OverlayButton, 5.0f);
	OverlayText = MakeText(
		WidgetTree,
		TEXT("OverlayModeText"),
		TEXT("ALL FLOWS"),
		SmallSize,
		AMSim::UITheme::Cyan(),
		true);
	AddVertical(Tools, OverlayText, 5.0f);
	InteractionText = MakeText(
		WidgetTree,
		TEXT("TerminalInteraction"),
		TEXT("Terminal controls ready."),
		SmallSize,
		AMSim::UITheme::Muted());
	AddVertical(Tools, InteractionText, 0.0f, true);
	AddAnchored(
		Canvas,
		LeftRail,
		bCompact
			? FAnchors(0.01f, 0.745f, 0.605f, 0.985f)
			: FAnchors(0.008f, 0.105f, 0.176f, 0.89f),
		FMargin(0.0f),
		9);

	UBorder* PartyRail = MakeSurface(
		WidgetTree,
		TEXT("PassengerPartyRail"),
		AMSim::UITheme::ESurface::Panel,
		bCompact ? FMargin(11.0f) : FMargin(15.0f),
		17.0f);
	UVerticalBox* Party = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("PassengerParty"));
	UScrollBox* PartyDrawer = WidgetTree->ConstructWidget<UScrollBox>(
		UScrollBox::StaticClass(),
		TEXT("PassengerPartyDrawer"));
	PartyDrawer->SetAnimateWheelScrolling(true);
	PartyDrawer->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	PartyDrawer->AddChild(Party);
	PartyRail->SetContent(PartyDrawer);
	AddSectionTitle(
		WidgetTree,
		Party,
		TEXT("PassengerFlowTitle"),
		TEXT("PASSENGER FLOW"),
		bCompact ? 13 : 17);
	UBorder* PartyIdentity = MakeMetricCard(
		WidgetTree, TEXT("PartyIdentity"), PartyText,
		TEXT("MAYA'S PARTY  ·  4 TRAVELLERS"), BodySize,
		AMSim::UITheme::ESurface::RaisedCard);
	UHorizontalBox* PartyIdentityRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("PartyIdentityRow"));
	if (PassengerFamilyTexture)
	{
		USizeBox* PortraitSize = WidgetTree->ConstructWidget<USizeBox>();
		PortraitSize->SetWidthOverride(bCompact ? 52.0f : 68.0f);
		PortraitSize->SetHeightOverride(bCompact ? 52.0f : 68.0f);
		UImage* Portrait = WidgetTree->ConstructWidget<UImage>();
		Portrait->SetBrushFromTexture(PassengerFamilyTexture, true);
		PortraitSize->SetContent(Portrait);
		UHorizontalBoxSlot* PortraitSlot =
			PartyIdentityRow->AddChildToHorizontalBox(PortraitSize);
		PortraitSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		PortraitSlot->SetVerticalAlignment(VAlign_Center);
	}
	UHorizontalBoxSlot* PartyIdentitySlot =
		PartyIdentityRow->AddChildToHorizontalBox(PartyIdentity);
	PartyIdentitySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AddVertical(Party, PartyIdentityRow, 6.0f);
	UHorizontalBox* PartyMembers = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("PartyMembers"));
	for (const FString& Initial : {
		FString(TEXT("M")), FString(TEXT("J")),
		FString(TEXT("E")), FString(TEXT("R"))})
	{
		UBorder* Avatar = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("Avatar%s"), *Initial),
			AMSim::UITheme::ESurface::Chip,
			FMargin(8.0f, 5.0f),
			20.0f);
		UTextBlock* InitialText = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("Avatar%sText"), *Initial),
			Initial,
			SmallSize,
			AMSim::UITheme::Cyan(),
			true);
		InitialText->SetJustification(ETextJustify::Center);
		Avatar->SetContent(InitialText);
		UHorizontalBoxSlot* MemberSlot =
			PartyMembers->AddChildToHorizontalBox(Avatar);
		MemberSlot->SetPadding(FMargin(0.0f, 0.0f, 7.0f, 0.0f));
	}
	AddVertical(Party, PartyMembers, 8.0f);
	UBorder* FlightCard = MakeMetricCard(
		WidgetTree, TEXT("PassengerFlight"), FlightText,
		TEXT("RB 304  ·  ON TIME  ·  GATE A1"), SmallSize);
	AddVertical(Party, FlightCard, 7.0f);
	UBorder* CountsCard = MakeMetricCard(
		WidgetTree, TEXT("PassengerCounts"), PassengerCountsText,
		TEXT("28 DEPARTING  ·  24 ARRIVING"), SmallSize);
	AddVertical(Party, CountsCard, 7.0f);
	UBorder* SecurityCard = MakeMetricCard(
		WidgetTree, TEXT("PassengerSecurity"), SecurityText,
		TEXT("CONTROLLED  ·  0 IN FLOW"), SmallSize,
		AMSim::UITheme::ESurface::Positive);
	AddVertical(Party, SecurityCard, 11.0f);

	AddSectionTitle(
		WidgetTree,
		Party,
		TEXT("CurrentStepTitle"),
		TEXT("CURRENT STEP"),
		SmallSize);
	CurrentStepText = MakeText(
		WidgetTree,
		TEXT("CurrentStep"),
		TEXT("Security queue"),
		BodySize,
		AMSim::UITheme::White(),
		true);
	AddVertical(Party, CurrentStepText, 4.0f);
	StepProgress = WidgetTree->ConstructWidget<UProgressBar>(
		UProgressBar::StaticClass(),
		TEXT("PassengerStepProgress"));
	StepProgress->SetFillColorAndOpacity(AMSim::UITheme::Cyan());
	AddVertical(Party, StepProgress, 10.0f);

	UBorder* NeedsCard = MakeMetricCard(
		WidgetTree, TEXT("PassengerNeeds"), NeedsText,
		TEXT("Accessible route · child · checked bags"), SmallSize,
		AMSim::UITheme::ESurface::Warning);
	AddVertical(Party, NeedsCard, 10.0f);
	AddSectionTitle(
		WidgetTree,
		Party,
		TEXT("RouteTitle"),
		TEXT("ROUTE"),
		SmallSize);
	RouteText = MakeText(
		WidgetTree,
		TEXT("PassengerRoute"),
		TEXT("Curb → Check-in → Accessible security → Gate A1"),
		SmallSize,
		AMSim::UITheme::White(),
		true);
	AddVertical(Party, RouteText, 10.0f);

	ConfidenceText = MakeText(
		WidgetTree,
		TEXT("PassengerConfidence"),
		TEXT("TIME CONFIDENCE  ·  86%"),
		SmallSize,
		AMSim::UITheme::Green(),
		true);
	AddVertical(Party, ConfidenceText, 4.0f);
	ConfidenceProgress = WidgetTree->ConstructWidget<UProgressBar>(
		UProgressBar::StaticClass(),
		TEXT("PassengerConfidenceProgress"));
	ConfidenceProgress->SetFillColorAndOpacity(AMSim::UITheme::Green());
	AddVertical(Party, ConfidenceProgress, 11.0f);
	BaggageText = MakeText(
		WidgetTree,
		TEXT("PassengerBaggage"),
		TEXT("Baggage system ready"),
		SmallSize,
		AMSim::UITheme::Muted());
	AddVertical(Party, BaggageText, 7.0f);
	LandsideText = MakeText(
		WidgetTree,
		TEXT("PassengerLandside"),
		TEXT("Car · taxi · bus"),
		SmallSize,
		AMSim::UITheme::Muted());
	AddVertical(Party, LandsideText, 0.0f);
	AddAnchored(
		Canvas,
		PartyRail,
		bCompact
			? FAnchors(0.615f, 0.105f, 0.992f, 0.985f)
			: FAnchors(0.802f, 0.105f, 0.992f, 0.89f),
		FMargin(0.0f),
		9);

	TerminalLabels = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("TerminalWorldLabels"));
	TerminalLabels->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	const float WorldLabelScale = bCompact ? 0.62f : 1.0f;
	const int32 WorldLabelFont = bCompact ? 8 : 11;
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("GateA1Label"),
		TEXT("Gate A1"), FVector2D(0.31f, 0.18f),
		FVector2D(118.0f, 32.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("GateA2Label"),
		TEXT("Gate A2"), FVector2D(0.61f, 0.18f),
		FVector2D(118.0f, 32.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("LoungeLabel"),
		bCompact ? TEXT("Departures") : TEXT("Departure lounge"),
		FVector2D(0.46f, 0.31f),
		FVector2D(176.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("CheckInLabel"),
		bCompact ? TEXT("Check-in") : TEXT("Check-in & bag drop"),
		FVector2D(0.26f, 0.52f),
		FVector2D(188.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("SecurityLabel"),
		bCompact ? TEXT("Controlled") : TEXT("Controlled door"),
		FVector2D(0.49f, 0.50f),
		FVector2D(166.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("BagMakeupLabel"),
		bCompact ? TEXT("Bag make-up") : TEXT("Baggage make-up"),
		FVector2D(0.70f, 0.44f),
		FVector2D(164.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	BaggageExceptionLabelText = AddLabel(
		WidgetTree,
		TerminalLabels,
		TEXT("BagExceptionLabel"),
		bCompact ? TEXT("Exception") : TEXT("Exception · clear"),
		FVector2D(0.73f, 0.55f),
		FVector2D(172.0f, 34.0f) * WorldLabelScale,
		WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("ArrivalsLabel"),
		bCompact ? TEXT("Arrivals") : TEXT("Arrivals corridor"),
		FVector2D(0.65f, 0.63f),
		FVector2D(168.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("ReclaimLabel"),
		bCompact ? TEXT("Reclaim") : TEXT("Baggage reclaim"),
		FVector2D(0.43f, 0.69f),
		FVector2D(162.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("EntranceLabel"),
		bCompact ? TEXT("Entrance") : TEXT("Entrance & curb"),
		FVector2D(0.26f, 0.82f),
		FVector2D(154.0f, 34.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("ParkingLabel"),
		TEXT("Parking"), FVector2D(0.55f, 0.88f),
		FVector2D(122.0f, 32.0f) * WorldLabelScale, WorldLabelFont);
	AddLabel(
		WidgetTree, TerminalLabels, TEXT("BusStopLabel"),
		TEXT("Bus stop"), FVector2D(0.76f, 0.86f),
		FVector2D(122.0f, 32.0f) * WorldLabelScale, WorldLabelFont);
	AddAnchored(
		Canvas,
		TerminalLabels,
		bCompact
			? FAnchors(0.01f, 0.10f, 0.605f, 0.65f)
			: FAnchors(0.185f, 0.105f, 0.793f, 0.89f),
		FMargin(0.0f),
		4);

	UBorder* Legend = MakeSurface(
		WidgetTree,
		TEXT("TerminalLegend"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(12.0f, 7.0f),
		14.0f);
	UHorizontalBox* LegendRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("TerminalLegendRow"));
	Legend->SetContent(LegendRow);
	const TArray<FString> LegendItems = bCompact
		? TArray<FString>{FString(TEXT("P/S/Q/A"))}
		: TArray<FString>{
			FString(TEXT("GREEN  PASSENGER FLOW")),
			FString(TEXT("CYAN  SECURE AREA")),
			FString(TEXT("AMBER  CONGESTION")),
			FString(TEXT("DASHED  ACCESSIBLE ROUTE"))};
	for (const FString& Item : LegendItems)
	{
		UTextBlock* Text = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("Legend%d"), LegendRow->GetChildrenCount()),
			Item,
			SmallSize,
			Item.StartsWith(TEXT("AMBER"))
				? AMSim::UITheme::Amber()
				: Item.StartsWith(TEXT("GREEN"))
					? AMSim::UITheme::Green()
					: AMSim::UITheme::Cyan(),
			true);
		Text->SetAutoWrapText(false);
		UHorizontalBoxSlot* LegendSlot = LegendRow->AddChildToHorizontalBox(Text);
		LegendSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		LegendSlot->SetHorizontalAlignment(HAlign_Center);
	}
	for (const TPair<FString, TFunction<void(UButton*)>>& Control :
		{
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("PAUSE"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddDynamic(
							this, &UAMSimTerminalView::PauseSimulation);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("1x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddDynamic(
							this, &UAMSimTerminalView::SetSpeedOne);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("2x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddDynamic(
							this, &UAMSimTerminalView::SetSpeedTwo);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("4x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddDynamic(
							this, &UAMSimTerminalView::SetSpeedFour);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("8x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddDynamic(
							this, &UAMSimTerminalView::SetSpeedEight);
					})
		})
	{
		UButton* TimeButton = MakeButton(
			WidgetTree,
			*FString::Printf(TEXT("Terminal%s"), *Control.Key),
			Control.Key,
			AMSim::UITheme::EButton::Quiet,
			SmallSize);
		Control.Value(TimeButton);
		UHorizontalBoxSlot* TimeSlot =
			LegendRow->AddChildToHorizontalBox(TimeButton);
		TimeSlot->SetPadding(FMargin(3.0f, 0.0f));
		TimeSlot->SetVerticalAlignment(VAlign_Center);
	}
	UButton* SaveButton = MakeButton(
		WidgetTree,
		TEXT("TerminalSave"),
		TEXT("SAVE"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	SaveButton->OnClicked.AddDynamic(this, &UAMSimTerminalView::SaveGame);
	LegendRow->AddChildToHorizontalBox(SaveButton)->SetPadding(FMargin(6.0f, 0.0f, 3.0f, 0.0f));
	UButton* LoadButton = MakeButton(
		WidgetTree,
		TEXT("TerminalLoad"),
		TEXT("LOAD"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	LoadButton->OnClicked.AddDynamic(this, &UAMSimTerminalView::LoadGame);
	LegendRow->AddChildToHorizontalBox(LoadButton)->SetPadding(FMargin(3.0f, 0.0f));
	AddAnchored(
		Canvas,
		Legend,
		bCompact
			? FAnchors(0.01f, 0.66f, 0.605f, 0.735f)
			: FAnchors(0.008f, 0.895f, 0.992f, 0.985f),
		FMargin(0.0f),
		9);

	PlanningCard = MakeSurface(
		WidgetTree,
		TEXT("TerminalPlanningCard"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(20.0f),
		20.0f);
	UVerticalBox* Planning = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("TerminalPlanningContent"));
	PlanningCard->SetContent(Planning);
	AddSectionTitle(
		WidgetTree,
		Planning,
		TEXT("TerminalPlanningTitle"),
		TEXT("PHASE 3  ·  PASSENGER AIRPORT"),
		bCompact ? 15 : 20);
	UTextBlock* PlanningCopy = MakeText(
		WidgetTree,
		TEXT("TerminalPlanningCopy"),
		TEXT("Build one domestic terminal, connect every passenger and baggage flow, then follow Maya's party from curb to Gate A1."),
		BodySize,
		AMSim::UITheme::White());
	AddVertical(Planning, PlanningCopy, 0.0f);
	AddAnchored(
		Canvas,
		PlanningCard,
		bCompact
			? FAnchors(0.05f, 0.24f, 0.56f, 0.49f)
			: FAnchors(0.30f, 0.34f, 0.70f, 0.61f),
		FMargin(0.0f),
		8);

	RegionalOperationsView =
		WidgetTree->ConstructWidget<UAMSimRegionalOperationsView>(
			UAMSimRegionalOperationsView::StaticClass(),
			TEXT("RegionalOperationsView"));
	RegionalOperationsView->OnReturnRequested.BindUObject(
		this,
		&UAMSimTerminalView::ReturnToAirport);
	AddAnchored(
		Canvas,
		RegionalOperationsView,
		FAnchors(0.0f, 0.0f, 1.0f, 1.0f),
		FMargin(0.0f),
		100);

	SetVisibility(ESlateVisibility::Collapsed);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimTerminalView::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

bool UAMSimTerminalView::Submit(
	AMSim::FPhase3Command Command,
	const FString& SuccessMessage)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	const AMSim::EPhase3CommandResult Result =
		Subsystem->SubmitPhase3Command(Command);
	const bool bAccepted = Result == AMSim::EPhase3CommandResult::Accepted;
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bAccepted ? SuccessMessage : TEXT("Action blocked. Check the highlighted status.")));
		InteractionText->SetColorAndOpacity(FSlateColor(
			bAccepted ? AMSim::UITheme::Cyan() : AMSim::UITheme::Coral()));
	}
	return bAccepted;
}

void UAMSimTerminalView::InitializePassengerAirport()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::InitializePassengerAirport;
	Submit(Command, TEXT("Passenger-airport planning opened."));
}

void UAMSimTerminalView::FundTerminal()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::FundTerminal;
	Submit(Command, TEXT("Domestic terminal shell funded."));
}

void UAMSimTerminalView::ConnectNextNetwork()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::ConnectNextNetwork;
	Submit(Command, TEXT("Next terminal flow connected."));
}

void UAMSimTerminalView::OpenTerminal()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::OpenTerminal;
	Submit(Command, TEXT("Domestic terminal opened."));
}

void UAMSimTerminalView::SchedulePassengerService()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::SchedulePassengerService;
	Submit(Command, TEXT("RB 304 scheduled at Gate A1."));
}

void UAMSimTerminalView::ToggleSecurityLane()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::ToggleSecurityLane;
	Submit(Command, TEXT("Security lane state changed."));
}

void UAMSimTerminalView::RequestPassengerAssistance()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase3State& State =
		Subsystem->GetSimulation().GetPhase3State();
	const AMSim::FPassengerRecord* Passenger = State.Passengers.FindByPredicate(
		[](const AMSim::FPassengerRecord& Candidate)
			{
				return Candidate.bRequiresAccessibleRoute;
			});
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::RequestPassengerAssistance;
	Command.PassengerId = Passenger ? Passenger->Id : AMSim::FPassengerId{};
	Submit(Command, TEXT("Route assistance assigned to Maya's party."));
}

void UAMSimTerminalView::ResolveBaggageException()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase3State& State =
		Subsystem->GetSimulation().GetPhase3State();
	const AMSim::FBagRecord* Bag = State.Bags.FindByPredicate(
		[](const AMSim::FBagRecord& Candidate)
			{
				return Candidate.JourneyState == AMSim::EBagJourneyState::Exception;
			});
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::ResolveBaggageException;
	Command.BagId = Bag ? Bag->Id : AMSim::FBagId{};
	Submit(Command, TEXT("Baggage exception returned to the correct flow."));
}

void UAMSimTerminalView::CycleOverlay()
{
	OverlayMode = (OverlayMode + 1) % 5;
	static const TArray<FString> Labels = {
		TEXT("ALL FLOWS"),
		TEXT("DEPARTURES"),
		TEXT("ARRIVALS"),
		TEXT("BAGGAGE"),
		TEXT("ACCESSIBLE ROUTE")};
	if (OverlayText)
	{
		OverlayText->SetText(FText::FromString(Labels[OverlayMode]));
	}
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetPhase3OverlayMode(OverlayMode);
		break;
	}
}

void UAMSimTerminalView::SubmitSpeed(
	const int32 Multiplier,
	const bool bPaused)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}
	AMSim::FPhase1Command Command;
	Command.Type = bPaused
		? AMSim::EPhase1CommandType::SetPaused
		: AMSim::EPhase1CommandType::SetSpeed;
	Command.bPaused = bPaused;
	Command.SpeedMultiplier = Multiplier;
	const bool bAccepted =
		Subsystem->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bAccepted
				? bPaused
					? TEXT("Simulation paused.")
					: FString::Printf(TEXT("Simulation speed set to %dx."), Multiplier)
				: TEXT("Time control action was blocked.")));
	}
}

void UAMSimTerminalView::PauseSimulation()
{
	SubmitSpeed(1, true);
}

void UAMSimTerminalView::SetSpeedOne()
{
	SubmitSpeed(1);
}

void UAMSimTerminalView::SetSpeedTwo()
{
	SubmitSpeed(2);
}

void UAMSimTerminalView::SetSpeedFour()
{
	SubmitSpeed(4);
}

void UAMSimTerminalView::SetSpeedEight()
{
	SubmitSpeed(8);
}

void UAMSimTerminalView::SaveGame()
{
	UAMSimAirportSimulationSubsystem* SimulationSubsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	UAMSimGameInstanceSubsystem* GameSubsystem =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
			: nullptr;
	if (!SimulationSubsystem || !GameSubsystem)
	{
		return;
	}
	AMSim::FSaveMetadata Metadata;
	Metadata.PlayerLabel = TEXT("Phase 3 terminal");
	Metadata.AirportName =
		SimulationSubsystem->GetPhase1Query().AirportName;
	const AMSim::FSaveResult Result = GameSubsystem->SaveSnapshotAsync(
		TEXT("Phase3Auto"),
		SimulationSubsystem->CreateSnapshot(),
		MoveTemp(Metadata)).Get();
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			Result.bSucceeded
				? TEXT("Terminal save completed.")
				: FString::Printf(TEXT("Save failed: %s"), *Result.Error)));
		InteractionText->SetColorAndOpacity(FSlateColor(
			Result.bSucceeded
				? AMSim::UITheme::Cyan()
				: AMSim::UITheme::Coral()));
	}
}

void UAMSimTerminalView::LoadGame()
{
	UAMSimAirportSimulationSubsystem* SimulationSubsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	UAMSimGameInstanceSubsystem* GameSubsystem =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
			: nullptr;
	if (!SimulationSubsystem || !GameSubsystem)
	{
		return;
	}
	AMSim::FSnapshot Snapshot;
	bool bUsedBackup = false;
	const bool bLoaded =
		GameSubsystem->LoadSnapshot(TEXT("Phase3Auto"), Snapshot, bUsedBackup) &&
		SimulationSubsystem->RestoreSnapshot(Snapshot);
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bLoaded
				? bUsedBackup
					? TEXT("Backup terminal save restored.")
					: TEXT("Terminal save restored.")
				: TEXT("No valid terminal save was found.")));
		InteractionText->SetColorAndOpacity(FSlateColor(
			bLoaded ? AMSim::UITheme::Cyan() : AMSim::UITheme::Coral()));
	}
}

void UAMSimTerminalView::OpenAdvancedOperations()
{
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowAdvancedOperations();
		RegionalOperationsView->SetVisibility(
			ESlateVisibility::SelfHitTestInvisible);
	}
}

void UAMSimTerminalView::OpenMajorOperations()
{
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowMajorOperations();
		RegionalOperationsView->SetVisibility(
			ESlateVisibility::SelfHitTestInvisible);
	}
}

void UAMSimTerminalView::ShowRegionalOperations()
{
	ShowPresentation();
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowRegionalOperations();
	}
}

void UAMSimTerminalView::ShowAdvancedOperations()
{
	ShowPresentation();
	OpenAdvancedOperations();
}

void UAMSimTerminalView::ShowMajorOperations()
{
	ShowPresentation();
	OpenMajorOperations();
}

void UAMSimTerminalView::ShowPresentation()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	RefreshFromSimulation();
}

void UAMSimTerminalView::ClosePresentation()
{
	bPresentationOpen = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAMSimTerminalView::ReturnToAirport()
{
	ClosePresentation();
	OnReturnRequested.ExecuteIfBound();
}

void UAMSimTerminalView::RefreshFromSimulation()
{
	if (!bPresentationOpen)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase2State& Phase2State =
		Subsystem->GetSimulation().GetPhase2State();
	const bool bShowPhase2ClosureProof =
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimPhase45OperationalEvidence")) &&
		Phase2State.Expansion.Stage != AMSim::EExpansionStage::None &&
		Phase2State.Expansion.Stage != AMSim::EExpansionStage::Operational;
	if (bShowPhase2ClosureProof)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const AMSim::FPhase3QuerySnapshot Query = Subsystem->GetPhase3Query();
	const AMSim::FPhase3State& State =
		Subsystem->GetSimulation().GetPhase3State();
	const AMSim::FPhase4QuerySnapshot Phase4Query =
		Subsystem->GetPhase4Query();
	const AMSim::FPhase5QuerySnapshot Phase5Query =
		Subsystem->GetPhase5Query();
	const AMSim::FPhase6QuerySnapshot Phase6Query =
		Subsystem->GetPhase6Query();
	if (AdvancedOperationsButton)
	{
		AdvancedOperationsButton->SetVisibility(
			Phase5Query.bUnlocked || Phase5Query.bInitialized
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (MajorOperationsButton)
	{
		MajorOperationsButton->SetVisibility(
			Phase6Query.bUnlocked || Phase6Query.bInitialized
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (RegionalOperationsView)
	{
		RegionalOperationsView->RefreshFromSimulation();
	}
	if (RegionalOperationsView &&
		(RegionalOperationsView->IsAdvancedOperationsOpen() ||
			RegionalOperationsView->IsMajorOperationsOpen()))
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RegionalOperationsView->SetVisibility(
			ESlateVisibility::SelfHitTestInvisible);
		return;
	}
	if (Phase4Query.bUnlocked || Phase4Query.bInitialized)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (TerminalCanvas)
		{
			for (int32 Index = 0;
				Index < TerminalCanvas->GetChildrenCount();
				++Index)
			{
				UWidget* Child = TerminalCanvas->GetChildAt(Index);
				Child->SetVisibility(
					Child == RegionalOperationsView
						? ESlateVisibility::SelfHitTestInvisible
						: ESlateVisibility::Collapsed);
			}
		}
		return;
	}
	if (!Query.bUnlocked && !Query.bInitialized)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->SetVisibility(
			ESlateVisibility::Collapsed);
	}
	UpdateWorldPresentation(Query, State);
	if (ViewState.Revision == Query.Revision)
	{
		return;
	}
	ViewState = AMSim::MakePhase3ViewState(Query, State);
	StatusText->SetText(FText::FromString(ViewState.Status));
	ConstructionText->SetText(FText::FromString(ViewState.Construction));
	FlightText->SetText(FText::FromString(ViewState.Flight));
	PassengerCountsText->SetText(FText::FromString(ViewState.PassengerCounts));
	SecurityText->SetText(FText::FromString(ViewState.Security));
	PartyText->SetText(FText::FromString(ViewState.FeaturedParty));
	CurrentStepText->SetText(FText::FromString(ViewState.FeaturedStep));
	NeedsText->SetText(FText::FromString(ViewState.FeaturedNeeds));
	RouteText->SetText(FText::FromString(ViewState.FeaturedRoute));
	BaggageText->SetText(FText::FromString(ViewState.Baggage));
	if (BaggageExceptionLabelText)
	{
		BaggageExceptionLabelText->SetText(FText::FromString(
			ViewState.bCanResolveBaggage
				? TEXT("EXCEPTION · ACTIVE")
				: TEXT("EXCEPTION · CLEAR")));
		BaggageExceptionLabelText->SetColorAndOpacity(
			FSlateColor(
				ViewState.bCanResolveBaggage
					? AMSim::UITheme::Coral()
					: AMSim::UITheme::White()));
	}
	LandsideText->SetText(FText::FromString(ViewState.Landside));
	ConfidenceText->SetText(FText::FromString(FString::Printf(
		TEXT("TIME CONFIDENCE  ·  %d%%"),
		ViewState.FeaturedTimeConfidencePercent)));
	StepProgress->SetPercent(ViewState.FeaturedStepProgress);
	ConfidenceProgress->SetPercent(
		static_cast<float>(ViewState.FeaturedTimeConfidencePercent) / 100.0f);

	const AMSim::FPhase1State& Phase1 =
		Subsystem->GetSimulation().GetPhase1State();
	const AMSim::FPhase2QuerySnapshot Phase2Query =
		Subsystem->GetPhase2Query();
	FundsText->SetText(FText::FromString(FString::Printf(
		TEXT("%lld CR  ·  %d RATING"),
		Phase1.Credits,
		Phase2Query.AverageRating)));
	const int64 TotalMinutes = Query.GameTimeMilliseconds / 60000;
	ClockText->SetText(FText::FromString(FString::Printf(
		TEXT("DAY %lld  ·  %02lld:%02lld  ·  8x"),
		(TotalMinutes / (24 * 60)) + 1,
		(TotalMinutes / 60) % 24,
		TotalMinutes % 60)));
	PlanningCard->SetVisibility(
		ViewState.bShowTerminal
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible);
	TerminalLabels->SetVisibility(
		ViewState.bShowTerminal
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	InitializeButton->SetIsEnabled(ViewState.bCanInitialize);
	InitializeButton->SetVisibility(
		ViewState.bCanInitialize
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	FundButton->SetIsEnabled(ViewState.bCanFund);
	FundButton->SetVisibility(
		ViewState.bCanFund
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	ConnectButton->SetIsEnabled(ViewState.bCanConnect);
	ConnectButton->SetVisibility(
		ViewState.bCanConnect
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	OpenButton->SetIsEnabled(ViewState.bCanOpen);
	OpenButton->SetVisibility(
		ViewState.bCanOpen
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	ScheduleButton->SetIsEnabled(ViewState.bCanSchedule);
	ScheduleButton->SetVisibility(
		ViewState.bCanSchedule
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	SecurityButton->SetIsEnabled(ViewState.bCanToggleSecurity);
	AssistanceButton->SetIsEnabled(ViewState.bCanRequestAssistance);
	BaggageButton->SetIsEnabled(ViewState.bCanResolveBaggage);
	ForceLayoutPrepass();
}

void UAMSimTerminalView::UpdateWorldPresentation(
	const AMSim::FPhase3QuerySnapshot& Query,
	const AMSim::FPhase3State& State)
{
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->ApplyPhase3Snapshot(Query, State);
		It->SetPhase3OverlayMode(OverlayMode);
		break;
	}
}
