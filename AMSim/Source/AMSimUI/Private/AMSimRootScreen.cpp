#include "AMSimRootScreen.h"

#include "AMSimAircraftPresentation.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "AMSimWorldPresenter.h"
#include "Styling/CoreStyle.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FLinearColor Background(0.025f, 0.047f, 0.063f, 1.0f);
	const FLinearColor Panel(0.012f, 0.028f, 0.038f, 0.96f);
	const FLinearColor PanelLight(0.024f, 0.064f, 0.078f, 1.0f);
	const FLinearColor Grass(0.145f, 0.255f, 0.165f, 1.0f);
	const FLinearColor GrassBuilt(0.39f, 0.58f, 0.31f, 1.0f);
	const FLinearColor Amber(0.96f, 0.68f, 0.22f, 1.0f);
	const FLinearColor Cyan(0.27f, 0.82f, 0.92f, 1.0f);
	const FLinearColor White(0.91f, 0.95f, 0.94f, 1.0f);
	const FLinearColor Muted(0.57f, 0.68f, 0.69f, 1.0f);
	const FLinearColor ErrorColor(0.97f, 0.39f, 0.32f, 1.0f);

	UTextBlock* MakeText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Text,
		const int32 Size,
		const FLinearColor Color = White)
	{
		UTextBlock* Widget = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			FName(Name));
		Widget->SetText(FText::FromString(Text));
		Widget->SetColorAndOpacity(FSlateColor(Color));
		Widget->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Size));
		Widget->SetAutoWrapText(true);
		return Widget;
	}

	UButton* MakeButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const bool bAccent = false)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
		FButtonStyle Style = Button->GetStyle();
		const FLinearColor Normal =
			bAccent ? FLinearColor(0.025f, 0.20f, 0.24f, 1.0f) : PanelLight;
		const FLinearColor Hovered =
			bAccent ? FLinearColor(0.04f, 0.34f, 0.39f, 1.0f) : FLinearColor(0.04f, 0.12f, 0.14f, 1.0f);
		Style.Normal.TintColor = FSlateColor(Normal);
		Style.Hovered.TintColor = FSlateColor(Hovered);
		Style.Pressed.TintColor = FSlateColor(Amber);
		Style.Disabled.TintColor = FSlateColor(FLinearColor(0.008f, 0.016f, 0.020f, 0.55f));
		Button->SetStyle(Style);
		Button->SetBackgroundColor(FLinearColor::White);
		Button->SetContent(MakeText(Tree, *FString::Printf(TEXT("%sLabel"), Name), Label, 16));
		return Button;
	}

	UEditableTextBox* MakeEntry(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Value,
		const FString& Hint)
	{
		UEditableTextBox* Entry = Tree->ConstructWidget<UEditableTextBox>(
			UEditableTextBox::StaticClass(),
			FName(Name));
		Entry->SetText(FText::FromString(Value));
		Entry->SetHintText(FText::FromString(Hint));
		FEditableTextBoxStyle Style = Entry->GetWidgetStyle();
		Style.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 17));
		Style.BackgroundColor = PanelLight;
		Style.Padding = FMargin(10.0f, 8.0f);
		Entry->SetWidgetStyle(Style);
		Entry->SetForegroundColor(White);
		Entry->SetMinDesiredWidth(220.0f);
		return Entry;
	}

	void AddVertical(UVerticalBox* Parent, UWidget* Child, const float Padding = 5.0f)
	{
		UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child);
		Slot->SetPadding(FMargin(0.0f, Padding));
	}

	UBorder* MakePanel(UWidgetTree* Tree, const TCHAR* Name)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		Border->SetBrushColor(Panel);
		Border->SetPadding(FMargin(20.0f));
		Border->SetClipping(EWidgetClipping::ClipToBounds);
		return Border;
	}

	void PlaceCanvas(
		UCanvasPanel* Canvas,
		UWidget* Child,
		const FAnchors Anchors,
		const FMargin Offsets = FMargin())
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Child);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(Offsets);
	}

	FString ConstructionName(const AMSim::EConstructionStage Stage)
	{
		switch (Stage)
		{
		case AMSim::EConstructionStage::None: return TEXT("Not funded");
		case AMSim::EConstructionStage::Funded: return TEXT("Funded");
		case AMSim::EConstructionStage::AwaitingDelivery: return TEXT("Awaiting delivery");
		case AMSim::EConstructionStage::Building: return TEXT("Building");
		case AMSim::EConstructionStage::Inspection: return TEXT("Safety inspection");
		case AMSim::EConstructionStage::ReadyToOpen: return TEXT("Ready to open");
		case AMSim::EConstructionStage::Operational: return TEXT("Operational");
		default: return TEXT("Unknown");
		}
	}

	FString OfferName(const AMSim::EOfferState State)
	{
		switch (State)
		{
		case AMSim::EOfferState::Unavailable: return TEXT("No offer");
		case AMSim::EOfferState::Available: return TEXT("Offer available");
		case AMSim::EOfferState::Declined: return TEXT("Offer declined");
		case AMSim::EOfferState::Accepted: return TEXT("Contract accepted");
		case AMSim::EOfferState::Scheduled: return TEXT("Flight scheduled");
		case AMSim::EOfferState::Completed: return TEXT("Contract completed");
		default: return TEXT("Unknown");
		}
	}

	FString FlightName(const AMSim::EFlightState State)
	{
		switch (State)
		{
		case AMSim::EFlightState::None: return TEXT("No active flight");
		case AMSim::EFlightState::Scheduled: return TEXT("Scheduled");
		case AMSim::EFlightState::Inbound: return TEXT("Inbound");
		case AMSim::EFlightState::Approach: return TEXT("On approach");
		case AMSim::EFlightState::Landing: return TEXT("Landing");
		case AMSim::EFlightState::RunwayRoll: return TEXT("Runway roll");
		case AMSim::EFlightState::TaxiIn: return TEXT("Taxiing to stand");
		case AMSim::EFlightState::Parked: return TEXT("Parked");
		case AMSim::EFlightState::Turnaround: return TEXT("Turnaround");
		case AMSim::EFlightState::Ready: return TEXT("Ready for departure");
		case AMSim::EFlightState::TaxiOut: return TEXT("Taxiing out");
		case AMSim::EFlightState::Takeoff: return TEXT("Taking off");
		case AMSim::EFlightState::Outbound: return TEXT("Outbound");
		case AMSim::EFlightState::Completed: return TEXT("Flight completed");
		default: return TEXT("Unknown");
		}
	}

	FString ServiceName(const AMSim::EServiceTaskState State)
	{
		switch (State)
		{
		case AMSim::EServiceTaskState::Unavailable: return TEXT("--");
		case AMSim::EServiceTaskState::Waiting: return TEXT("Waiting");
		case AMSim::EServiceTaskState::Active: return TEXT("In progress");
		case AMSim::EServiceTaskState::Completed: return TEXT("Complete");
		default: return TEXT("Unknown");
		}
	}

	FString RatingName(const FName Component)
	{
		if (Component == TEXT("SafetyReadiness"))
		{
			return TEXT("Safety/readiness");
		}
		if (Component == TEXT("OperationalReliability"))
		{
			return TEXT("Operational reliability");
		}
		return Component.ToString();
	}

	FString FormatGameTime(const int64 GameTimeMilliseconds)
	{
		const int64 TotalMinutes = GameTimeMilliseconds / 60000;
		return FString::Printf(
			TEXT("D%lld %02lld:%02lld"),
			1 + TotalMinutes / (24 * 60),
			(TotalMinutes / 60) % 24,
			TotalMinutes % 60);
	}
}

UAMSimRootScreen::UAMSimRootScreen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PrimaryButtonWidget(
		TEXT("/Game/UI/WBP_PrimaryActionButton"));
	if (PrimaryButtonWidget.Succeeded())
	{
		PrimaryButtonWidgetClass = PrimaryButtonWidget.Class;
	}
}

FUIInputConfig UAMSimRootScreen::MakeGameplayInputConfig()
{
	return FUIInputConfig(
		ECommonInputMode::All,
		EMouseCaptureMode::CaptureDuringMouseDown,
		EMouseLockMode::DoNotLock,
		false);
}

TOptional<FUIInputConfig> UAMSimRootScreen::GetDesiredInputConfig() const
{
	return MakeGameplayInputConfig();
}

TSharedRef<SWidget> UAMSimRootScreen::RebuildWidget()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Phase1Root"));
	WidgetTree->RootWidget = Root;
	UBorder* BackgroundBorder = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("Background"));
	BackgroundBorder->SetBrushColor(FLinearColor::Transparent);
	Root->AddChildToOverlay(BackgroundBorder);

	UVerticalBox* Page = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("Page"));
	UOverlaySlot* PageSlot = Root->AddChildToOverlay(Page);
	PageSlot->SetHorizontalAlignment(HAlign_Fill);
	PageSlot->SetVerticalAlignment(VAlign_Fill);

	const UUserInterfaceSettings* InterfaceSettings = GetDefault<UUserInterfaceSettings>();
	const float InterfaceScale = InterfaceSettings
		? InterfaceSettings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	const bool bCompactLayout = InterfaceScale >= 1.75f;
	const int32 AirportHeaderSize = bCompactLayout ? 18 : 26;
	const int32 ClockHeaderSize = bCompactLayout ? 15 : 18;
	const int32 FundsHeaderSize = bCompactLayout ? 18 : 22;

	UBorder* Header = MakePanel(WidgetTree, TEXT("Header"));
	Header->SetPadding(bCompactLayout ? FMargin(12.0f, 6.0f) : FMargin(24.0f, 12.0f));
	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("HeaderRow"));
	Header->SetContent(HeaderRow);
	AirportNameText = MakeText(
		WidgetTree,
		TEXT("AirportName"),
		TEXT("NEW AIRPORT"),
		AirportHeaderSize,
		Cyan);
	UHorizontalBoxSlot* AirportSlot = HeaderRow->AddChildToHorizontalBox(AirportNameText);
	AirportSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ClockText = MakeText(
		WidgetTree,
		TEXT("Clock"),
		TEXT("DAY 1  00:00  PAUSED"),
		ClockHeaderSize,
		Muted);
	ClockText->SetJustification(ETextJustify::Center);
	UHorizontalBoxSlot* ClockSlot = HeaderRow->AddChildToHorizontalBox(ClockText);
	ClockSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	FundsText = MakeText(
		WidgetTree,
		TEXT("Funds"),
		TEXT("5,000 CR  |  0 AP"),
		FundsHeaderSize,
		Amber);
	FundsText->SetJustification(ETextJustify::Right);
	UHorizontalBoxSlot* FundsSlot = HeaderRow->AddChildToHorizontalBox(FundsText);
	FundsSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UVerticalBoxSlot* HeaderSlot = Page->AddChildToVerticalBox(Header);
	HeaderSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	const float RailWrapWidth = 420.0f / FMath::Max(InterfaceScale, 1.0f);
	UHorizontalBox* Main = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("Main"));
	UVerticalBoxSlot* MainSlot = Page->AddChildToVerticalBox(Main);
	MainSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MainSlot->SetPadding(FMargin(12.0f));

	UBorder* LeftPanel = MakePanel(WidgetTree, TEXT("LeftPanel"));
	ObjectiveDrawer = LeftPanel;
	UVerticalBox* Left = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("LeftColumn"));
	UScrollBox* LeftScroll = WidgetTree->ConstructWidget<UScrollBox>(
		UScrollBox::StaticClass(),
		TEXT("LeftScroll"));
	LeftPanel->SetContent(LeftScroll);
	LeftScroll->AddChild(Left);
	AddVertical(Left, MakeText(WidgetTree, TEXT("ObjectiveHeader"), TEXT("ACTIVE OBJECTIVE"), 14, Amber));
	ObjectiveText = MakeText(WidgetTree, TEXT("Objective"), TEXT("Create your airport"), 22);
	ObjectiveText->SetWrapTextAt(RailWrapWidth);
	USizeBox* ObjectiveHeight = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("ObjectiveHeight"));
	ObjectiveHeight->SetMinDesiredHeight(90.0f);
	ObjectiveHeight->SetContent(ObjectiveText);
	AddVertical(Left, ObjectiveHeight, 18.0f);
	AddVertical(Left, MakeText(WidgetTree, TEXT("StatusHeader"), TEXT("OPERATION STATUS"), 14, Amber), 15.0f);
	StatusText = MakeText(WidgetTree, TEXT("Status"), TEXT("Ready to begin"), 19);
	StatusText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Left, StatusText);
	CauseText = MakeText(WidgetTree, TEXT("Cause"), TEXT(""), 15, ErrorColor);
	CauseText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Left, CauseText);
	RemedyText = MakeText(WidgetTree, TEXT("Remedy"), TEXT(""), 15, Cyan);
	RemedyText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Left, RemedyText);
	ProjectText = MakeText(WidgetTree, TEXT("Project"), TEXT("Starter airfield: not funded"), 17, Muted);
	ProjectText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Left, ProjectText, 15.0f);
	AddVertical(Left, MakeText(WidgetTree, TEXT("AirportNameEntryHeader"), TEXT("AIRPORT NAME"), 13, Muted), 10.0f);
	AirportNameEntry = MakeEntry(
		WidgetTree,
		TEXT("AirportNameEntry"),
		TEXT("Riverbend Field"),
		TEXT("Enter airport name"));
	AddVertical(Left, AirportNameEntry, 2.0f);
	AddVertical(Left, MakeText(WidgetTree, TEXT("SaveSlotEntryHeader"), TEXT("LOCAL SAVE SLOT"), 13, Muted), 10.0f);
	SaveSlotEntry = MakeEntry(
		WidgetTree,
		TEXT("SaveSlotEntry"),
		TEXT("Phase1Auto"),
		TEXT("Letters, numbers, - or _"));
	AddVertical(Left, SaveSlotEntry, 2.0f);
	if (PrimaryButtonWidgetClass)
	{
		CreateButtonWidget = WidgetTree->ConstructWidget<UUserWidget>(
			PrimaryButtonWidgetClass,
			TEXT("CreateAirportTemplate"));
		CreateButton = Cast<UButton>(
			CreateButtonWidget
				? CreateButtonWidget->GetWidgetFromName(TEXT("PrimaryButton"))
				: nullptr);
	}
	if (!CreateButton)
	{
		CreateButton = MakeButton(WidgetTree, TEXT("CreateAirport"), TEXT("CREATE AIRPORT"), true);
		CreateButtonWidget = nullptr;
	}
	CreateButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::CreateAirport);
	AddVertical(Left, CreateButtonWidget ? static_cast<UWidget*>(CreateButtonWidget) : CreateButton);
	BuildButton = MakeButton(WidgetTree, TEXT("BuildStarter"), TEXT("COMMIT STARTER PLAN  3,400 CR"), true);
	BuildButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::CommitStarterPlan);
	AddVertical(Left, BuildButton);
	CancelBuildButton = MakeButton(WidgetTree, TEXT("CancelStarter"), TEXT("CANCEL PROJECT / FULL REFUND"));
	CancelBuildButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::CancelStarterPlan);
	AddVertical(Left, CancelBuildButton);
	OpenButton = MakeButton(WidgetTree, TEXT("OpenAirport"), TEXT("OPEN AIRPORT"), true);
	OpenButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::OpenAirport);
	AddVertical(Left, OpenButton);
	CloseButton = MakeButton(WidgetTree, TEXT("CloseAirport"), TEXT("CLOSE AIRPORT"));
	CloseButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::CloseAirport);
	AddVertical(Left, CloseButton);
	RecoveryButton = MakeButton(WidgetTree, TEXT("Recovery"), TEXT("REQUEST RECOVERY REVIEW"));
	RecoveryButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::RequestRecovery);
	AddVertical(Left, RecoveryButton);
	USizeBox* LeftRailWidth = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("LeftRailWidth"));
	LeftRailWidth->SetWidthOverride(360.0f / FMath::Max(InterfaceScale, 1.0f));
	LeftRailWidth->SetContent(LeftPanel);
	UHorizontalBoxSlot* LeftSlot = Main->AddChildToHorizontalBox(LeftRailWidth);
	LeftSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
	if (bCompactLayout)
	{
		LeftPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UBorder* MapPanel = MakePanel(WidgetTree, TEXT("MapPanel"));
	MapPanel->SetPadding(FMargin(10.0f));
	MapPanel->SetBrushColor(FLinearColor::Transparent);
	UCanvasPanel* Map = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("AirfieldMap"));
	MapPanel->SetContent(Map);
	UBorder* GrassBackground = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("GrassParcel"));
	GrassBackground->SetBrushColor(Grass);
	GrassBackground->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, GrassBackground, FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	UTextBlock* ParcelLabel = MakeText(
		WidgetTree,
		TEXT("ParcelLabel"),
		TEXT("TEMPERATE STARTER PARCEL  /  NORTH"),
		14,
		FLinearColor(0.72f, 0.82f, 0.68f, 1.0f));
	PlaceCanvas(Map, ParcelLabel, FAnchors(0.03f, 0.03f, 0.55f, 0.10f));
	ParcelLabel->SetVisibility(ESlateVisibility::Hidden);

	RunwayVisual = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Runway"));
	RunwayVisual->SetBrushColor(FLinearColor(0.31f, 0.43f, 0.25f, 0.75f));
	RunwayVisual->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, RunwayVisual, FAnchors(0.08f, 0.35f, 0.92f, 0.49f));
	TaxiVisual = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Taxiway"));
	TaxiVisual->SetBrushColor(FLinearColor(0.31f, 0.43f, 0.25f, 0.75f));
	TaxiVisual->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, TaxiVisual, FAnchors(0.58f, 0.48f, 0.65f, 0.64f));
	StandVisual = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Stand"));
	StandVisual->SetBrushColor(FLinearColor(0.24f, 0.37f, 0.24f, 0.75f));
	StandVisual->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, StandVisual, FAnchors(0.54f, 0.63f, 0.75f, 0.77f));
	HutVisual = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("OperationsHut"));
	HutVisual->SetBrushColor(FLinearColor(0.34f, 0.25f, 0.17f, 0.75f));
	HutVisual->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, HutVisual, FAnchors(0.76f, 0.67f, 0.84f, 0.77f));

	AircraftMarker = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("AircraftMarker"));
	PlaceCanvas(Map, AircraftMarker, FAnchors(0.03f, 0.20f), FMargin(0.0f, 0.0f, 54.0f, 54.0f));
	AircraftMarker->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	AircraftMarker->SetVisibility(ESlateVisibility::Hidden);
	UBorder* Wings = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("AircraftWings"));
	Wings->SetBrushColor(Cyan);
	PlaceCanvas(AircraftMarker, Wings, FAnchors(0.05f, 0.36f, 0.95f, 0.56f));
	UBorder* Fuselage = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("AircraftFuselage"));
	Fuselage->SetBrushColor(Cyan);
	PlaceCanvas(AircraftMarker, Fuselage, FAnchors(0.43f, 0.03f, 0.57f, 0.95f));
	UBorder* Tailplane = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("AircraftTailplane"));
	Tailplane->SetBrushColor(Cyan);
	PlaceCanvas(AircraftMarker, Tailplane, FAnchors(0.25f, 0.76f, 0.75f, 0.89f));
	AircraftLabel = MakeText(WidgetTree, TEXT("AircraftLabel"), TEXT("RB-021"), 13, White);
	AircraftLabel->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, AircraftLabel, FAnchors(0.03f, 0.27f, 0.17f, 0.32f));

	ContextPanel = MakePanel(WidgetTree, TEXT("ContextPanel"));
	ContextPanel->SetPadding(
		bCompactLayout ? FMargin(11.0f, 8.0f) : FMargin(22.0f, 16.0f));
	ContextPanel->SetBrushColor(FLinearColor(0.008f, 0.022f, 0.032f, 0.97f));
	UVerticalBox* ContextColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ContextColumn"));
	ContextPanel->SetContent(ContextColumn);
	ContextHeaderText = MakeText(
		WidgetTree,
		TEXT("ContextHeader"),
		TEXT("STARTER AIRFIELD"),
		bCompactLayout ? 12 : 14,
		Amber);
	AddVertical(ContextColumn, ContextHeaderText, 2.0f);
	ContextBodyText = MakeText(
		WidgetTree,
		TEXT("ContextBody"),
		TEXT("Presentation context"),
		bCompactLayout ? 16 : 20,
		White);
	ContextBodyText->SetWrapTextAt(620.0f);
	AddVertical(ContextColumn, ContextBodyText, 5.0f);
	ContextStatusText = MakeText(
		WidgetTree,
		TEXT("ContextStatus"),
		TEXT("Status"),
		bCompactLayout ? 13 : 15,
		Cyan);
	ContextStatusText->SetWrapTextAt(620.0f);
	AddVertical(ContextColumn, ContextStatusText, 5.0f);
	ContextPanel->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, ContextPanel, FAnchors(0.10f, 0.61f, 0.90f, 0.95f));
	UHorizontalBoxSlot* MapSlot = Main->AddChildToHorizontalBox(MapPanel);
	MapSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UBorder* RightPanel = MakePanel(WidgetTree, TEXT("RightPanel"));
	OperationsDrawer = RightPanel;
	UVerticalBox* Right = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("RightColumn"));
	UScrollBox* RightScroll = WidgetTree->ConstructWidget<UScrollBox>(
		UScrollBox::StaticClass(),
		TEXT("RightScroll"));
	RightPanel->SetContent(RightScroll);
	RightScroll->AddChild(Right);
	AddVertical(Right, MakeText(WidgetTree, TEXT("OfferHeader"), TEXT("RIVERBEND FLYING CLUB"), 14, Amber));
	OfferText = MakeText(WidgetTree, TEXT("Offer"), TEXT("No offer"), 22);
	OfferText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, OfferText, 18.0f);
	CompatibilityText = MakeText(WidgetTree, TEXT("Compatibility"), TEXT("Complete the airfield to unlock offers."), 15, Muted);
	CompatibilityText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, CompatibilityText);
	PinOfferButton = MakeButton(WidgetTree, TEXT("PinOffer"), TEXT("PIN OFFER"));
	PinOfferButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::PinOffer);
	AddVertical(Right, PinOfferButton);
	DeclineOfferButton = MakeButton(WidgetTree, TEXT("DeclineOffer"), TEXT("DECLINE OFFER"));
	DeclineOfferButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::DeclineOffer);
	AddVertical(Right, DeclineOfferButton);
	AcceptButton = MakeButton(WidgetTree, TEXT("AcceptOffer"), TEXT("ACCEPT FIRST-FLIGHT OFFER"), true);
	AcceptButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::AcceptOffer);
	AddVertical(Right, AcceptButton);
	ScheduleButton = MakeButton(WidgetTree, TEXT("ScheduleFlight"), TEXT("SCHEDULE NEXT 5-MINUTE SLOT"), true);
	ScheduleButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::ScheduleFlight);
	AddVertical(Right, ScheduleButton);
	AddVertical(Right, MakeText(WidgetTree, TEXT("FlightHeader"), TEXT("FLIGHT OPERATIONS"), 14, Amber), 15.0f);
	FlightText = MakeText(WidgetTree, TEXT("Flight"), TEXT("No active flight"), 20);
	FlightText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, FlightText);
	TimetableText = MakeText(
		WidgetTree,
		TEXT("Timetable"),
		TEXT("No timetable entry. Slots use exact five-minute increments."),
		15,
		Muted);
	TimetableText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, TimetableText);
	ServicesText = MakeText(WidgetTree, TEXT("Services"), TEXT("Inspection: --\nFuel: --"), 16, Muted);
	ServicesText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, ServicesText);
	LedgerText = MakeText(
		WidgetTree,
		TEXT("Ledger"),
		TEXT("LEDGER  No transactions"),
		15,
		Muted);
	LedgerText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, LedgerText);
	RatingText = MakeText(WidgetTree, TEXT("Rating"), TEXT("Airport rating: 0"), 16, Cyan);
	RatingText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, RatingText);
	USizeBox* RightRailWidth = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("RightRailWidth"));
	RightRailWidth->SetWidthOverride(380.0f / FMath::Max(InterfaceScale, 1.0f));
	RightRailWidth->SetContent(RightPanel);
	UHorizontalBoxSlot* RightSlot = Main->AddChildToHorizontalBox(RightRailWidth);
	RightSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightSlot->SetPadding(FMargin(12.0f, 0.0f, 0.0f, 0.0f));
	if (bCompactLayout)
	{
		RightPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UBorder* Footer = MakePanel(WidgetTree, TEXT("Footer"));
	Footer->SetPadding(FMargin(20.0f, 10.0f));
	UVerticalBox* FooterColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("FooterColumn"));
	Footer->SetContent(FooterColumn);
	CaptionText = MakeText(
		WidgetTree,
		TEXT("RadioCaption"),
		TEXT("RADIO  /  Captions are enabled for every operational call."),
		17,
		White);
	AddVertical(FooterColumn, CaptionText, 2.0f);
	UHorizontalBox* Controls = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("Controls"));
	if (bCompactLayout)
	{
		UScrollBox* ControlsScroll = WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("CompactControlsScroll"));
		ControlsScroll->SetOrientation(Orient_Horizontal);
		ControlsScroll->SetScrollBarVisibility(ESlateVisibility::Hidden);
		ControlsScroll->AddChild(Controls);
		AddVertical(FooterColumn, ControlsScroll, 2.0f);
		UButton* ObjectiveToggle = MakeButton(
			WidgetTree,
			TEXT("ObjectiveDrawerToggle"),
			TEXT("OBJECTIVE"));
		ObjectiveToggle->OnClicked.AddDynamic(this, &UAMSimRootScreen::ToggleObjectiveDrawer);
		Controls->AddChildToHorizontalBox(ObjectiveToggle)->SetPadding(FMargin(3.0f));
		UButton* OperationsToggle = MakeButton(
			WidgetTree,
			TEXT("OperationsDrawerToggle"),
			TEXT("OPERATIONS"));
		OperationsToggle->OnClicked.AddDynamic(this, &UAMSimRootScreen::ToggleOperationsDrawer);
		Controls->AddChildToHorizontalBox(OperationsToggle)->SetPadding(FMargin(3.0f));
	}
	else
	{
		AddVertical(FooterColumn, Controls, 2.0f);
	}
	UButton* Pause = MakeButton(WidgetTree, TEXT("Pause"), TEXT("PAUSE"));
	Pause->OnClicked.AddDynamic(this, &UAMSimRootScreen::PauseSimulation);
	Controls->AddChildToHorizontalBox(Pause)->SetPadding(FMargin(3.0f));
	UButton* SpeedOne = MakeButton(WidgetTree, TEXT("SpeedOne"), TEXT("1x"));
	SpeedOne->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedOne);
	Controls->AddChildToHorizontalBox(SpeedOne)->SetPadding(FMargin(3.0f));
	UButton* SpeedTwo = MakeButton(WidgetTree, TEXT("SpeedTwo"), TEXT("2x"));
	SpeedTwo->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedTwo);
	Controls->AddChildToHorizontalBox(SpeedTwo)->SetPadding(FMargin(3.0f));
	UButton* SpeedFour = MakeButton(WidgetTree, TEXT("SpeedFour"), TEXT("4x"));
	SpeedFour->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedFour);
	Controls->AddChildToHorizontalBox(SpeedFour)->SetPadding(FMargin(3.0f));
	UButton* SpeedEight = MakeButton(WidgetTree, TEXT("SpeedEight"), TEXT("8x"));
	SpeedEight->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedEight);
	Controls->AddChildToHorizontalBox(SpeedEight)->SetPadding(FMargin(3.0f));
	UButton* Save = MakeButton(WidgetTree, TEXT("Save"), TEXT("SAVE"));
	Save->OnClicked.AddDynamic(this, &UAMSimRootScreen::SaveGame);
	Controls->AddChildToHorizontalBox(Save)->SetPadding(FMargin(16.0f, 3.0f, 3.0f, 3.0f));
	UButton* Load = MakeButton(WidgetTree, TEXT("Load"), TEXT("LOAD"));
	Load->OnClicked.AddDynamic(this, &UAMSimRootScreen::LoadGame);
	Controls->AddChildToHorizontalBox(Load)->SetPadding(FMargin(3.0f));
	InteractionText = MakeText(WidgetTree, TEXT("Interaction"), TEXT("Ready."), 15, Muted);
	InteractionText->SetJustification(ETextJustify::Right);
	AddVertical(FooterColumn, InteractionText, 2.0f);
	UVerticalBoxSlot* FooterSlot = Page->AddChildToVerticalBox(Footer);
	FooterSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	SpeechProvider = CreateAMSimLocalSpeechProvider();
	const bool bSpeechReady = SpeechProvider->Initialize();
	SetInteractionMessage(
		bSpeechReady
			? TEXT("Local radio speech and captions ready.")
			: TEXT("Caption mode active; local radio cue will be used."),
		true);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimRootScreen::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

void UAMSimRootScreen::NativeDestruct()
{
	if (SpeechProvider)
	{
		SpeechProvider->Shutdown();
		SpeechProvider.Reset();
	}
	Super::NativeDestruct();
}

void UAMSimRootScreen::CreateAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CreateAirport;
	Command.AirportName = AirportNameEntry
		? AirportNameEntry->GetText().ToString().TrimStartAndEnd()
		: TEXT("Riverbend Field");
	Command.MapId = AMSim::GetPhase1Fixture().MapId;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Airport identity created.") : TEXT("Enter a valid airport name."),
		bAccepted);
}

void UAMSimRootScreen::CommitStarterPlan()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
	Command.Proposal = AMSim::CreateDefaultStarterPlan();
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Starter airfield funded; delivery is underway.") : TEXT("Starter plan was rejected."),
		bAccepted);
}

void UAMSimRootScreen::CancelStarterPlan()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CancelStarterPlan;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Project cancelled before delivery; 3,400 Credits refunded.")
			: TEXT("The project can only be cancelled before materials arrive."),
		bAccepted);
}

void UAMSimRootScreen::OpenAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::OpenAirport;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Riverbend Field is open.") : TEXT("Opening prerequisites are not complete."),
		bAccepted);
}

void UAMSimRootScreen::CloseAirport()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::CloseAirport;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Airfield closed safely.") : TEXT("Complete the active visit before closing."),
		bAccepted);
}

void UAMSimRootScreen::PinOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::PinStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Offer pinned for review.") : TEXT("Only an available offer can be pinned."),
		bAccepted);
}

void UAMSimRootScreen::DeclineOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::DeclineStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Offer declined. Close and reopen for a fresh starter offer.")
			: TEXT("Only an available offer can be declined."),
		bAccepted);
}

void UAMSimRootScreen::AcceptOffer()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::AcceptStarterOffer;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(bAccepted ? TEXT("First-flight contract accepted.") : TEXT("Offer could not be accepted."), bAccepted);
}

void UAMSimRootScreen::ScheduleFlight()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::ScheduleStarterFlight;
	Command.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
	Command.ScheduledArrivalGameMilliseconds = Subsystem->GetRecommendedStarterArrivalTime();
	const bool bAccepted =
		Subsystem->SubmitPhase1Command(Command) == AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Arrival scheduled in the next five-minute slot.") : TEXT("Schedule selection was rejected."),
		bAccepted);
}

void UAMSimRootScreen::RequestRecovery()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::RequestRecovery;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(
		bAccepted ? TEXT("Recovery assistance granted and recorded.") : TEXT("Recovery is not currently eligible."),
		bAccepted);
}

void UAMSimRootScreen::PauseSimulation()
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::SetPaused;
	Command.bPaused = true;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(TEXT("Simulation paused."), bAccepted);
}

void UAMSimRootScreen::SetSpeedOne()
{
	SubmitSpeed(1);
}

void UAMSimRootScreen::SetSpeedTwo()
{
	SubmitSpeed(2);
}

void UAMSimRootScreen::SetSpeedFour()
{
	SubmitSpeed(4);
}

void UAMSimRootScreen::SetSpeedEight()
{
	SubmitSpeed(8);
}

void UAMSimRootScreen::SubmitSpeed(const int32 Multiplier)
{
	AMSim::FPhase1Command Command;
	Command.Type = AMSim::EPhase1CommandType::SetSpeed;
	Command.SpeedMultiplier = Multiplier;
	const bool bAccepted =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->SubmitPhase1Command(Command) ==
		AMSim::EPhase1CommandResult::Accepted;
	SetInteractionMessage(FString::Printf(TEXT("Simulation running at %dx."), Multiplier), bAccepted);
}

void UAMSimRootScreen::SaveGame()
{
	UAMSimAirportSimulationSubsystem* SimulationSubsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	UAMSimGameInstanceSubsystem* GameInstanceSubsystem =
		GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>();
	AMSim::FSaveMetadata Metadata;
	Metadata.PlayerLabel = TEXT("Phase 1");
	Metadata.AirportName = SimulationSubsystem->GetPhase1Query().AirportName;
	const FString SlotId = SaveSlotEntry
		? SaveSlotEntry->GetText().ToString().TrimStartAndEnd()
		: TEXT("Phase1Auto");
	const AMSim::FSaveResult Result = GameInstanceSubsystem->SaveSnapshotAsync(
		SlotId,
		SimulationSubsystem->CreateSnapshot(),
		MoveTemp(Metadata)).Get();
	SetInteractionMessage(
		Result.bSucceeded ? TEXT("Phase 1 save completed.") : FString::Printf(TEXT("Save failed: %s"), *Result.Error),
		Result.bSucceeded);
}

void UAMSimRootScreen::LoadGame()
{
	UAMSimGameInstanceSubsystem* GameInstanceSubsystem =
		GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>();
	AMSim::FSnapshot Snapshot;
	bool bUsedBackup = false;
	const FString SlotId = SaveSlotEntry
		? SaveSlotEntry->GetText().ToString().TrimStartAndEnd()
		: TEXT("Phase1Auto");
	const bool bLoaded =
		GameInstanceSubsystem->LoadSnapshot(SlotId, Snapshot, bUsedBackup) &&
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()->RestoreSnapshot(Snapshot);
	SetInteractionMessage(
		bLoaded
			? bUsedBackup ? TEXT("Backup save restored.") : TEXT("Phase 1 save restored.")
			: TEXT("No valid Phase 1 save was found."),
		bLoaded);
}

void UAMSimRootScreen::ToggleObjectiveDrawer()
{
	if (!ObjectiveDrawer || !OperationsDrawer)
	{
		return;
	}
	const bool bOpen = ObjectiveDrawer->GetVisibility() == ESlateVisibility::Collapsed;
	ObjectiveDrawer->SetVisibility(bOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	OperationsDrawer->SetVisibility(ESlateVisibility::Collapsed);
}

void UAMSimRootScreen::ToggleOperationsDrawer()
{
	if (!ObjectiveDrawer || !OperationsDrawer)
	{
		return;
	}
	const bool bOpen = OperationsDrawer->GetVisibility() == ESlateVisibility::Collapsed;
	OperationsDrawer->SetVisibility(bOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ObjectiveDrawer->SetVisibility(ESlateVisibility::Collapsed);
}

void UAMSimRootScreen::RefreshFromSimulation()
{
	if (!GetWorld() || !AirportNameText)
	{
		return;
	}
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase1QuerySnapshot Query = Subsystem->GetPhase1Query();
	const AMSim::FPhase1State& State = Subsystem->GetSimulation().GetPhase1State();
	if (LastAppliedRevision == Query.Revision)
	{
		return;
	}
	LastAppliedRevision = Query.Revision;
	CurrentViewState = AMSim::MakePhase1ViewState(Query, State);

	const auto SetText = [](UTextBlock* Widget, const FString& Value)
	{
		if (Widget)
		{
			Widget->SetText(FText::FromString(Value));
		}
	};
	SetText(AirportNameText, CurrentViewState.AirportName);
	SetText(ClockText, CurrentViewState.Clock);
	SetText(FundsText, CurrentViewState.Funds);
	SetText(ObjectiveText, CurrentViewState.Objective);
	SetText(StatusText, CurrentViewState.Status);
	SetText(CauseText, TEXT("CAUSE  ") + CurrentViewState.Cause);
	SetText(RemedyText, TEXT("NEXT  ") + CurrentViewState.Remedy);
	SetText(ProjectText, CurrentViewState.Project);
	SetText(OfferText, CurrentViewState.Offer);
	SetText(CompatibilityText, CurrentViewState.Compatibility);
	SetText(FlightText, CurrentViewState.Flight);
	SetText(TimetableText, CurrentViewState.Timetable);
	SetText(ServicesText, CurrentViewState.Services);
	SetText(LedgerText, CurrentViewState.Ledger);
	SetText(RatingText, CurrentViewState.Rating);
	SetText(CaptionText, CurrentViewState.Caption);
	if (State.PhraseIntents.Num() > LastPhraseCount && SpeechProvider)
	{
		SpeechProvider->Speak(GetWorld(), State.PhraseIntents.Last().Caption);
	}

	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->ApplySnapshot(Query);
		break;
	}

	if (ContextPanel)
	{
		FString ContextHeader;
		FString ContextBody;
		FString ContextStatus;
		if (Query.FlightState == AMSim::EFlightState::Turnaround)
		{
			ContextHeader = TEXT("AIRCRAFT TURNAROUND  /  STAND A1");
			ContextBody = FString::Printf(
				TEXT("%s is parked and protected on Stand A1."),
				State.Airframe.TailNumber.IsEmpty() ? TEXT("RB-021") : *State.Airframe.TailNumber);
			ContextStatus = CurrentViewState.Services + TEXT("\nComplete both services before departure.");
		}
		else if (Query.OfferState == AMSim::EOfferState::Available ||
			Query.OfferState == AMSim::EOfferState::Accepted)
		{
			ContextHeader = TEXT("FIRST-FLIGHT OFFER  /  RIVERBEND FLYING CLUB");
			ContextBody = TEXT("RIVERBEND 21   Light piston trainer   Local visit\n")
				TEXT("RECOMMENDED SLOT  D1 00:10   Stand A1   45-minute turnaround");
			ContextStatus = CurrentViewState.Compatibility +
				TEXT("\nREWARD  600 Credits  |  5 Airport Points");
		}
		else if (Query.ConstructionStage >= AMSim::EConstructionStage::Funded &&
			Query.ConstructionStage < AMSim::EConstructionStage::ReadyToOpen)
		{
			ContextHeader = TEXT("STARTER AIRFIELD  /  CONSTRUCTION");
			ContextBody = CurrentViewState.Project +
				TEXT("\nGrass runway 09/27   Taxiway A   Stand A1   Operations hut");
			ContextStatus = TEXT("3,400 Credits funded  |  Full connected package\n") +
				CurrentViewState.Status;
		}
		const bool bShowContext = !ContextHeader.IsEmpty();
		ContextPanel->SetVisibility(
			bShowContext ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		if (bShowContext)
		{
			SetText(ContextHeaderText, ContextHeader);
			SetText(ContextBodyText, ContextBody);
			SetText(ContextStatusText, ContextStatus);
			if (UCanvasPanelSlot* ContextSlot = Cast<UCanvasPanelSlot>(ContextPanel->Slot))
			{
				const UUserInterfaceSettings* InterfaceSettings =
					GetDefault<UUserInterfaceSettings>();
				const bool bCompactContext = InterfaceSettings &&
					InterfaceSettings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080)) >= 1.75f;
				if (Query.FlightState == AMSim::EFlightState::Turnaround)
				{
					ContextSlot->SetAnchors(
						bCompactContext
							? FAnchors(0.04f, 0.04f, 0.72f, 0.60f)
							: FAnchors(0.07f, 0.06f, 0.57f, 0.38f));
				}
				else if (Query.OfferState == AMSim::EOfferState::Available ||
					Query.OfferState == AMSim::EOfferState::Accepted)
				{
					ContextSlot->SetAnchors(
						bCompactContext
							? FAnchors(0.05f, 0.46f, 0.95f, 0.88f)
							: FAnchors(0.10f, 0.52f, 0.90f, 0.82f));
				}
				else
				{
					ContextSlot->SetAnchors(
						bCompactContext
							? FAnchors(0.04f, 0.52f, 0.72f, 0.93f)
							: FAnchors(0.08f, 0.65f, 0.62f, 0.93f));
				}
			}
		}
	}

	if (CreateButton) CreateButton->SetIsEnabled(!CurrentViewState.bAirportInitialized);
	if (AirportNameEntry) AirportNameEntry->SetIsReadOnly(CurrentViewState.bAirportInitialized);
	if (BuildButton) BuildButton->SetIsEnabled(CurrentViewState.bCanBuild);
	if (CancelBuildButton) CancelBuildButton->SetIsEnabled(CurrentViewState.bCanCancelBuild);
	if (OpenButton) OpenButton->SetIsEnabled(CurrentViewState.bCanOpen);
	if (CloseButton) CloseButton->SetIsEnabled(CurrentViewState.bCanClose);
	if (PinOfferButton) PinOfferButton->SetIsEnabled(CurrentViewState.bCanPinOffer);
	if (DeclineOfferButton) DeclineOfferButton->SetIsEnabled(CurrentViewState.bCanDeclineOffer);
	if (AcceptButton) AcceptButton->SetIsEnabled(CurrentViewState.bCanAcceptOffer);
	if (ScheduleButton) ScheduleButton->SetIsEnabled(CurrentViewState.bCanSchedule);
	if (RecoveryButton) RecoveryButton->SetIsEnabled(CurrentViewState.bCanRequestRecovery);
	LastPhraseCount = State.PhraseIntents.Num();
}

void UAMSimRootScreen::SetInteractionMessage(const FString& Message, const bool bSucceeded)
{
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(Message));
		InteractionText->SetColorAndOpacity(FSlateColor(bSucceeded ? Cyan : ErrorColor));
	}
}
