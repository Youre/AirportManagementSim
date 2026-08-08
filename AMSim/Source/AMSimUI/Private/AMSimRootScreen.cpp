#include "AMSimRootScreen.h"

#include "AMSimAircraftPresentation.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimCameraPawn.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimContextHelpCard.h"
#include "AMSimExpandingToolButton.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1HudPresentation.h"
#include "AMSimPhase1OperationsHubView.h"
#include "AMSimPhase1StaffView.h"
#include "AMSimRadioSubsystem.h"
#include "AMSimReleaseGuideView.h"
#include "AMSimSaveLoadView.h"
#include "AMSimSchedulePickerView.h"
#include "AMSimTerminalView.h"
#include "AMSimTurnaroundView.h"
#include "AMSimUITheme.h"
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
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Texture2D.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "AMSimWorldPresenter.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Styling/CoreStyle.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealClient.h"

namespace AMSimRootScreenPrivate
{
	const FLinearColor Background = AMSim::UITheme::Navy900();
	const FLinearColor Panel = AMSim::UITheme::Navy800();
	const FLinearColor PanelLight = AMSim::UITheme::Navy700();
	const FLinearColor Grass(0.145f, 0.255f, 0.165f, 1.0f);
	const FLinearColor GrassBuilt(0.39f, 0.58f, 0.31f, 1.0f);
	const FLinearColor Amber = AMSim::UITheme::Amber();
	const FLinearColor Cyan = AMSim::UITheme::Cyan();
	const FLinearColor White = AMSim::UITheme::White();
	const FLinearColor Muted = AMSim::UITheme::Muted();
	const FLinearColor ErrorColor = AMSim::UITheme::Coral();

	UTextBlock* MakeText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Text,
		const int32 Size,
		const FLinearColor Color = White,
		const bool bBold = false,
		const bool bShadow = false)
	{
		UTextBlock* Widget = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			FName(Name));
		Widget->SetText(FText::FromString(Text));
		AMSim::UITheme::StyleText(Widget, Size, Color, bBold, bShadow);
		Widget->SetAutoWrapText(true);
		return Widget;
	}

	UButton* MakeButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const AMSim::UITheme::EButton Kind = AMSim::UITheme::EButton::Secondary,
		const int32 LabelSize = 15)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
		Button->SetStyle(AMSim::UITheme::ButtonStyle(Kind));
		Button->SetBackgroundColor(FLinearColor::White);
		UTextBlock* LabelText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			LabelSize,
			Kind == AMSim::UITheme::EButton::Positive ? White : White,
			true,
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Button->SetContent(LabelText);
		return Button;
	}

	UAMSimExpandingToolButton* MakeNavigationButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		UTexture2D* Icon,
		const AMSim::UITheme::EButton Kind = AMSim::UITheme::EButton::Tool)
	{
		UAMSimExpandingToolButton* Button =
			Tree->ConstructWidget<UAMSimExpandingToolButton>(
				UAMSimExpandingToolButton::StaticClass(),
				FName(Name));
		Button->Configure(Icon, Label, Kind);
		return Button;
	}

	void PlaceNavigationButton(
		UCanvasPanel* Canvas,
		UAMSimExpandingToolButton* Button,
		const int32 Row)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Button);
		Slot->SetAnchors(FAnchors(0.0f, 0.0f));
		Slot->SetPosition(FVector2D(10.0f, 10.0f + Row * 64.0f));
		Slot->SetSize(UAMSimExpandingToolButton::GetHostSize());
		Slot->SetZOrder(2);
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
		Entry->SetWidgetStyle(AMSim::UITheme::EntryStyle());
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
		AMSim::UITheme::StyleSurface(
			Border,
			AMSim::UITheme::ESurface::Panel,
			FMargin(20.0f),
			18.0f,
			1.5f);
		return Border;
	}

	UBorder* MakeSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Surface,
		const FMargin Padding,
		const float Radius = 14.0f,
		const float StrokeWidth = 1.0f)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		AMSim::UITheme::StyleSurface(Border, Surface, Padding, Radius, StrokeWidth);
		return Border;
	}

	UBorder* MakeTextCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Eyebrow,
		UTextBlock*& Body,
		const FString& InitialBody,
		const FLinearColor& Accent = Cyan,
		const int32 BodySize = 18)
	{
		UBorder* Card = MakeSurface(
			Tree,
			Name,
			AMSim::UITheme::ESurface::Card,
			FMargin(16.0f, 14.0f),
			14.0f,
			1.2f);
		UVerticalBox* Column = Tree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			FName(*(FString(Name) + TEXT("Column"))));
		Card->SetContent(Column);
		AddVertical(
			Column,
			MakeText(
				Tree,
				*FString::Printf(TEXT("%sEyebrow"), Name),
				Eyebrow,
				12,
				Accent,
				true),
			0.0f);
		Body = MakeText(
			Tree,
			*FString::Printf(TEXT("%sBody"), Name),
			InitialBody,
			BodySize,
			White,
			true,
			true);
		AddVertical(Column, Body, 5.0f);
		return Card;
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

using namespace AMSimRootScreenPrivate;

UAMSimRootScreen::UAMSimRootScreen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PrimaryButtonWidget(
		TEXT("/Game/UI/WBP_PrimaryActionButton"));
	if (PrimaryButtonWidget.Succeeded())
	{
		PrimaryButtonWidgetClass = PrimaryButtonWidget.Class;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> BuildIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Build.T_Build"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScheduleIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Timetable.T_Timetable"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> StaffIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Staff.T_Staff"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> OverlayIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Overlays.T_Overlays"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> AlertIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Alerts.T_Alerts"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ProjectIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Projects.T_Projects"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ServiceIcon(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Services.T_Services"));
	NavigationIcons = {
		BuildIcon.Object,
		ScheduleIcon.Object,
		StaffIcon.Object,
		OverlayIcon.Object,
		AlertIcon.Object,
		ProjectIcon.Object,
		ProjectIcon.Object,
		ServiceIcon.Object};
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
	Phase1Page = Page;
	UOverlaySlot* PageSlot = Root->AddChildToOverlay(Page);
	PageSlot->SetHorizontalAlignment(HAlign_Fill);
	PageSlot->SetVerticalAlignment(VAlign_Fill);

	const UUserInterfaceSettings* InterfaceSettings = GetDefault<UUserInterfaceSettings>();
	const float InterfaceScale = InterfaceSettings
		? InterfaceSettings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	FIntPoint ViewportSize(1920, 1080);
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FIntPoint ReportedViewportSize =
			GEngine->GameViewport->Viewport->GetSizeXY();
		if (ReportedViewportSize.X > 0 && ReportedViewportSize.Y > 0)
		{
			ViewportSize = ReportedViewportSize;
		}
	}
	const bool bCompactLayout =
		AMSim::FPhase1HudPresentation::ShouldUseCompactLayout(
			InterfaceScale,
			ViewportSize);
	bCompactLayoutActive = bCompactLayout;
	const int32 AirportHeaderSize = bCompactLayout ? 18 : 24;
	const int32 ClockHeaderSize = bCompactLayout ? 14 : 16;
	const int32 FundsHeaderSize = bCompactLayout ? 16 : 19;

	UBorder* Header = MakePanel(WidgetTree, TEXT("Header"));
	AMSim::UITheme::StyleSurface(
		Header,
		AMSim::UITheme::ESurface::Chrome,
		bCompactLayout ? FMargin(12.0f, 7.0f) : FMargin(20.0f, 12.0f),
		18.0f,
		1.5f);
	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("HeaderRow"));
	Header->SetContent(HeaderRow);
	UTextBlock* BrandText = MakeText(
		WidgetTree,
		TEXT("Brand"),
		bCompactLayout ? TEXT("AMS") : TEXT("AIRPORT\nMGMT SIM"),
		bCompactLayout ? 15 : 17,
		White,
		true,
		true);
	UHorizontalBoxSlot* BrandSlot = HeaderRow->AddChildToHorizontalBox(BrandText);
	BrandSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	BrandSlot->SetPadding(FMargin(0.0f, 0.0f, bCompactLayout ? 12.0f : 24.0f, 0.0f));
	BrandSlot->SetVerticalAlignment(VAlign_Center);
	AirportNameText = MakeText(
		WidgetTree,
		TEXT("AirportName"),
		TEXT("NEW AIRPORT"),
		AirportHeaderSize,
		White,
		true,
		true);
	UHorizontalBoxSlot* AirportSlot = HeaderRow->AddChildToHorizontalBox(AirportNameText);
	AirportSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AirportSlot->SetVerticalAlignment(VAlign_Center);
	ClockText = MakeText(
		WidgetTree,
		TEXT("Clock"),
		TEXT("DAY 1  00:00  PAUSED"),
		ClockHeaderSize,
		White,
		true);
	ClockText->SetJustification(ETextJustify::Center);
	UBorder* ClockPill = MakeSurface(
		WidgetTree,
		TEXT("ClockPill"),
		AMSim::UITheme::ESurface::Field,
		FMargin(14.0f, 8.0f),
		12.0f,
		1.0f);
	ClockPill->SetContent(ClockText);
	UHorizontalBoxSlot* ClockSlot = HeaderRow->AddChildToHorizontalBox(ClockPill);
	ClockSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	ClockSlot->SetPadding(FMargin(8.0f, 0.0f));
	ClockSlot->SetVerticalAlignment(VAlign_Center);
	FundsText = MakeText(
		WidgetTree,
		TEXT("Funds"),
		TEXT("5,000 CR  |  0 AP"),
		FundsHeaderSize,
		Amber,
		true,
		true);
	FundsText->SetJustification(ETextJustify::Right);
	UBorder* FundsPill = MakeSurface(
		WidgetTree,
		TEXT("FundsPill"),
		AMSim::UITheme::ESurface::Field,
		FMargin(14.0f, 8.0f),
		12.0f,
		1.0f);
	FundsPill->SetContent(FundsText);
	UHorizontalBoxSlot* FundsSlot = HeaderRow->AddChildToHorizontalBox(FundsPill);
	FundsSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	FundsSlot->SetVerticalAlignment(VAlign_Center);
	UVerticalBoxSlot* HeaderSlot = Page->AddChildToVerticalBox(Header);
	HeaderSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	const float RailWrapWidth = 350.0f / FMath::Max(InterfaceScale, 1.0f);
	UOverlay* MainLayer = WidgetTree->ConstructWidget<UOverlay>(
		UOverlay::StaticClass(),
		TEXT("MainLayer"));
	UVerticalBoxSlot* MainSlot = Page->AddChildToVerticalBox(MainLayer);
	MainSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MainSlot->SetPadding(FMargin(10.0f, 8.0f));
	UHorizontalBox* Main = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("Main"));
	UOverlaySlot* MainLayerSlot = MainLayer->AddChildToOverlay(Main);
	MainLayerSlot->SetHorizontalAlignment(HAlign_Fill);
	MainLayerSlot->SetVerticalAlignment(VAlign_Fill);

	UCanvasPanel* NavigationLayer = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("NavigationLayer"));
	NavigationLayer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	BuildModeLeftChrome = NavigationLayer;
	ObjectiveDrawer = NavigationLayer;
	UOverlaySlot* NavigationLayerSlot = MainLayer->AddChildToOverlay(NavigationLayer);
	NavigationLayerSlot->SetHorizontalAlignment(HAlign_Fill);
	NavigationLayerSlot->SetVerticalAlignment(VAlign_Fill);
	UBorder* LeftPanel = MakePanel(WidgetTree, TEXT("LeftPanel"));
	AMSim::UITheme::StyleSurface(
		LeftPanel,
		AMSim::UITheme::ESurface::Panel,
		FMargin(6.0f),
		18.0f,
		1.5f);
	PlaceCanvas(
		NavigationLayer,
		LeftPanel,
		FAnchors(0.0f, 0.0f, 0.0f, 1.0f),
		FMargin(0.0f, 0.0f, 112.0f, 0.0f));
	AirportNameEntry = MakeEntry(
		WidgetTree,
		TEXT("AirportNameEntry"),
		TEXT("Riverbend Field"),
		TEXT("Enter airport name"));
	SaveSlotEntry = MakeEntry(
		WidgetTree,
		TEXT("SaveSlotEntry"),
		TEXT("Phase1Auto"),
		TEXT("Letters, numbers, - or _"));
	CreateButton = MakeButton(
		WidgetTree,
		TEXT("CreateAirport"),
		TEXT("CREATE AIRPORT"),
		AMSim::UITheme::EButton::Positive,
		16);
	CreateButtonWidget = nullptr;
	CreateButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::CreateAirport);
	NavigationButtons.Reset();
	BuildButton = MakeNavigationButton(
		WidgetTree,
		TEXT("BuildStarter"),
		TEXT("BUILD"),
		NavigationIcons.IsValidIndex(0) ? NavigationIcons[0] : nullptr,
		AMSim::UITheme::EButton::Primary);
	BuildButton->OnActivated.BindUObject(this, &UAMSimRootScreen::ToggleConstructionProposal);
	PlaceNavigationButton(NavigationLayer, BuildButton, 0);
	NavigationButtons.Add(BuildButton);
	int32 NavigationIndex = 1;
	for (const TPair<const TCHAR*, const TCHAR*>& Tool : {
		TPair<const TCHAR*, const TCHAR*>(TEXT("ScheduleTool"), TEXT("SCHEDULE")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("StaffTool"), TEXT("STAFF")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverlayTool"), TEXT("OVERLAYS"))})
	{
		UAMSimExpandingToolButton* ToolButton = MakeNavigationButton(
			WidgetTree,
			Tool.Key,
			Tool.Value,
			NavigationIcons.IsValidIndex(NavigationIndex)
				? NavigationIcons[NavigationIndex]
				: nullptr);
		ToolButton->SetActionEnabled(false);
		PlaceNavigationButton(NavigationLayer, ToolButton, NavigationIndex);
		NavigationButtons.Add(ToolButton);
		++NavigationIndex;
	}
	BindPhase1UtilityNavigation();
	CancelBuildButton = MakeNavigationButton(
		WidgetTree,
		TEXT("CancelStarter"),
		TEXT("CANCEL PROJECT"),
		NavigationIcons.IsValidIndex(4) ? NavigationIcons[4] : nullptr,
		AMSim::UITheme::EButton::Destructive);
	CancelBuildButton->OnActivated.BindUObject(this, &UAMSimRootScreen::CancelStarterPlan);
	PlaceNavigationButton(NavigationLayer, CancelBuildButton, 4);
	NavigationButtons.Add(CancelBuildButton);
	OpenButton = MakeNavigationButton(
		WidgetTree,
		TEXT("OpenAirport"),
		TEXT("OPEN AIRPORT"),
		NavigationIcons.IsValidIndex(5) ? NavigationIcons[5] : nullptr,
		AMSim::UITheme::EButton::Positive);
	OpenButton->OnActivated.BindUObject(this, &UAMSimRootScreen::OpenAirport);
	PlaceNavigationButton(NavigationLayer, OpenButton, 4);
	NavigationButtons.Add(OpenButton);
	CloseButton = MakeNavigationButton(
		WidgetTree,
		TEXT("CloseAirport"),
		TEXT("CLOSE AIRPORT"),
		NavigationIcons.IsValidIndex(6) ? NavigationIcons[6] : nullptr,
		AMSim::UITheme::EButton::Destructive);
	CloseButton->OnActivated.BindUObject(this, &UAMSimRootScreen::CloseAirport);
	PlaceNavigationButton(NavigationLayer, CloseButton, 4);
	NavigationButtons.Add(CloseButton);
	RecoveryButton = MakeNavigationButton(
		WidgetTree,
		TEXT("Recovery"),
		TEXT("RECOVERY"),
		NavigationIcons.IsValidIndex(7) ? NavigationIcons[7] : nullptr,
		AMSim::UITheme::EButton::Secondary);
	RecoveryButton->OnActivated.BindUObject(this, &UAMSimRootScreen::RequestRecovery);
	PlaceNavigationButton(NavigationLayer, RecoveryButton, 4);
	NavigationButtons.Add(RecoveryButton);
#if WITH_DEV_AUTOMATION_TESTS
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimCompactHudHoverProof")))
	{
		BuildButton->SetExpandedForTest(true);
	}
#endif
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
	const AMSim::FPhase1AirportCreationLayout CreationLayout =
		AMSim::FPhase1HudPresentation::MakeAirportCreationLayout(bCompactLayout);
	CreateAirportTray = MakeSurface(
		WidgetTree,
		TEXT("CreateAirportTray"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(CreationLayout.HorizontalPadding, CreationLayout.VerticalPadding),
		18.0f,
		1.8f);
	if (bCompactLayout)
	{
		UVerticalBox* CreateColumn = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("CreateAirportCompactColumn"));
		CreateAirportTray->SetContent(CreateColumn);
		AddVertical(
			CreateColumn,
			MakeText(
				WidgetTree,
				TEXT("CompactCreateTitle"),
				TEXT("RIVERBEND PLAINS  /  NEW AIRPORT"),
				13,
				Cyan,
				true),
			0.0f);
		AddVertical(CreateColumn, AirportNameEntry, 3.0f);
		AddVertical(CreateColumn, CreateButton, 4.0f);
	}
	else
	{
		UHorizontalBox* CreateRow = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("CreateAirportRow"));
		CreateAirportTray->SetContent(CreateRow);

		UVerticalBox* ParcelSummary = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ParcelSummary"));
		AddVertical(
			ParcelSummary,
			MakeText(
				WidgetTree,
				TEXT("ParcelName"),
				TEXT("RIVERBEND PLAINS"),
				17,
				White,
				true,
				true),
			0.0f);
		AddVertical(
			ParcelSummary,
			MakeText(
				WidgetTree,
				TEXT("ParcelFacts"),
				TEXT("TEMPERATE  •  LARGE BUILD AREA"),
				12,
				Cyan,
				true),
			3.0f);
		UHorizontalBoxSlot* ParcelSlot = CreateRow->AddChildToHorizontalBox(ParcelSummary);
		FSlateChildSize ParcelSize(ESlateSizeRule::Fill);
		ParcelSize.Value = CreationLayout.ParcelWeight;
		ParcelSlot->SetSize(ParcelSize);
		ParcelSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
		ParcelSlot->SetVerticalAlignment(VAlign_Center);

		UVerticalBox* NameColumn = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("AirportNameColumn"));
		AddVertical(
			NameColumn,
			MakeText(
				WidgetTree,
				TEXT("AirportNameEntryHeader"),
				TEXT("AIRPORT NAME"),
				11,
				Muted,
				true),
			0.0f);
		AddVertical(NameColumn, AirportNameEntry, 2.0f);
		UHorizontalBoxSlot* NameSlot = CreateRow->AddChildToHorizontalBox(NameColumn);
		FSlateChildSize NameSize(ESlateSizeRule::Fill);
		NameSize.Value = CreationLayout.NameWeight;
		NameSlot->SetSize(NameSize);
		NameSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
		NameSlot->SetVerticalAlignment(VAlign_Center);

		UHorizontalBoxSlot* CreateSlot = CreateRow->AddChildToHorizontalBox(CreateButton);
		CreateSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		CreateSlot->SetVerticalAlignment(VAlign_Center);
	}
	PlaceCanvas(
		Map,
		CreateAirportTray,
		FAnchors(
			CreationLayout.MinX,
			CreationLayout.MinY,
			CreationLayout.MaxX,
			CreationLayout.MaxY));
	ContextPanel = MakePanel(WidgetTree, TEXT("ContextPanel"));
	AMSim::UITheme::StyleSurface(
		ContextPanel,
		AMSim::UITheme::ESurface::RaisedCard,
		bCompactLayout ? FMargin(12.0f, 9.0f) : FMargin(18.0f, 15.0f),
		18.0f,
		1.8f);
	UVerticalBox* ContextColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ContextColumn"));
	ContextPanel->SetContent(ContextColumn);
	ContextHeaderText = MakeText(
		WidgetTree,
		TEXT("ContextHeader"),
		TEXT("STARTER AIRFIELD"),
		bCompactLayout ? 12 : 14,
		Cyan,
		true);
	AddVertical(ContextColumn, ContextHeaderText, 2.0f);
	ContextCompactText = MakeText(
		WidgetTree,
		TEXT("ContextCompactDetail"),
		TEXT("RWY 09/27  •  3,400 CR"),
		bCompactLayout ? 14 : 16,
		White,
		true,
		true);
	ContextCompactText->SetVisibility(ESlateVisibility::Collapsed);
	AddVertical(ContextColumn, ContextCompactText, 3.0f);
	ContextBodyText = MakeText(
		WidgetTree,
		TEXT("ContextBody"),
		TEXT("Presentation context"),
		bCompactLayout ? 15 : 18,
		White,
		true,
		true);
	ContextStatusText = MakeText(
		WidgetTree,
		TEXT("ContextStatus"),
		TEXT("Status"),
		bCompactLayout ? 12 : 14,
		Amber,
		true);
	UHorizontalBox* ContextDetailRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("ContextDetailRow"));
	ContextDetailRowWidget = ContextDetailRow;
	UBorder* ContextIdentitySurface = MakeSurface(
		WidgetTree,
		TEXT("ContextIdentitySurface"),
		AMSim::UITheme::ESurface::Field,
		FMargin(11.0f, 8.0f),
		10.0f,
		1.0f);
	ContextIdentitySurface->SetContent(ContextBodyText);
	UHorizontalBoxSlot* ContextIdentitySlot =
		ContextDetailRow->AddChildToHorizontalBox(ContextIdentitySurface);
	FSlateChildSize ContextIdentitySize(ESlateSizeRule::Fill);
	ContextIdentitySize.Value = 1.0f;
	ContextIdentitySlot->SetSize(ContextIdentitySize);
	ContextIdentitySlot->SetPadding(FMargin(0.0f, 0.0f, 4.0f, 0.0f));
	UBorder* ContextEvidenceSurface = MakeSurface(
		WidgetTree,
		TEXT("ContextEvidenceSurface"),
		AMSim::UITheme::ESurface::Chip,
		FMargin(11.0f, 8.0f),
		10.0f,
		1.0f);
	ContextEvidenceSurface->SetContent(ContextStatusText);
	UHorizontalBoxSlot* ContextEvidenceSlot =
		ContextDetailRow->AddChildToHorizontalBox(ContextEvidenceSurface);
	FSlateChildSize ContextEvidenceSize(ESlateSizeRule::Fill);
	ContextEvidenceSize.Value = 1.0f;
	ContextEvidenceSlot->SetSize(ContextEvidenceSize);
	ContextEvidenceSlot->SetPadding(FMargin(4.0f, 0.0f, 0.0f, 0.0f));
	AddVertical(ContextColumn, ContextDetailRow, 5.0f);
	UHorizontalBox* ContextActions = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("ContextActions"));
	AddVertical(ContextColumn, ContextActions, 4.0f);
	ContextPanel->SetVisibility(ESlateVisibility::Hidden);
	PlaceCanvas(Map, ContextPanel, FAnchors(0.68f, 0.02f, 0.985f, 0.15f));
	ConstructionProposalView =
		WidgetTree->ConstructWidget<UAMSimConstructionProposalView>(
			UAMSimConstructionProposalView::StaticClass(),
			TEXT("ConstructionProposalView"));
	ConstructionProposalView->OnBuildModeVisibilityChanged.BindUObject(
		this, &UAMSimRootScreen::SetConstructionModeChrome);
	PlaceCanvas(
		Map,
		ConstructionProposalView,
		FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	StaffView = WidgetTree->ConstructWidget<UAMSimPhase1StaffView>(
		UAMSimPhase1StaffView::StaticClass(), TEXT("Phase1StaffView"));
	PlaceCanvas(Map, StaffView, FAnchors(0.09f, 0.08f, 0.40f, 0.51f));
	StaffView->ClosePanel();
	Phase1OperationsHubView = WidgetTree->ConstructWidget<UAMSimPhase1OperationsHubView>(
		UAMSimPhase1OperationsHubView::StaticClass(), TEXT("Phase1OperationsHubView"));
	PlaceCanvas(Map, Phase1OperationsHubView, FAnchors(0.09f, 0.08f, 0.47f, 0.64f));
	TurnaroundView =
		WidgetTree->ConstructWidget<UAMSimTurnaroundView>(
			UAMSimTurnaroundView::StaticClass(),
			TEXT("TurnaroundView"));
	PlaceCanvas(Map, TurnaroundView, FAnchors(0.04f, 0.05f, 0.96f, 0.28f));
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
	AMSim::UITheme::StyleSurface(
		RightPanel,
		AMSim::UITheme::ESurface::Panel,
		FMargin(12.0f),
		18.0f,
		1.5f);
	RightPanel->SetContent(RightScroll);
	RightScroll->AddChild(Right);
	AddVertical(
		Right,
		MakeText(
			WidgetTree,
			TEXT("ObjectiveSectionHeader"),
			TEXT("OBJECTIVES"),
			13,
			Cyan,
			true),
		0.0f);
	UTextBlock* ObjectiveBody = nullptr;
	UBorder* ObjectiveCard = MakeTextCard(
		WidgetTree,
		TEXT("ObjectiveCard"),
		TEXT("ACTIVE"),
		ObjectiveBody,
		TEXT("Start a new airport"),
		Cyan,
		18);
	ObjectiveText = ObjectiveBody;
	ObjectiveText->SetWrapTextAt(RailWrapWidth);
	AddVertical(Right, ObjectiveCard, 5.0f);

	UBorder* StatusCard = MakeSurface(
		WidgetTree,
		TEXT("StatusCard"),
		AMSim::UITheme::ESurface::Card,
		FMargin(15.0f, 13.0f),
		14.0f,
		1.2f);
	UVerticalBox* StatusColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("StatusColumn"));
	StatusCard->SetContent(StatusColumn);
	AddVertical(
		StatusColumn,
		MakeText(
			WidgetTree,
			TEXT("StatusHeader"),
			TEXT("STATUS"),
			11,
			Amber,
			true),
		0.0f);
	StatusText = MakeText(
		WidgetTree,
		TEXT("Status"),
		TEXT("Ready to begin"),
		16,
		White,
		true);
	StatusText->SetWrapTextAt(RailWrapWidth);
	AddVertical(StatusColumn, StatusText, 4.0f);
	CauseText = MakeText(WidgetTree, TEXT("Cause"), TEXT(""), 13, ErrorColor, true);
	CauseText->SetWrapTextAt(RailWrapWidth);
	AddVertical(StatusColumn, CauseText, 3.0f);
	RemedyText = MakeText(WidgetTree, TEXT("Remedy"), TEXT(""), 13, Cyan, true);
	RemedyText->SetWrapTextAt(RailWrapWidth);
	AddVertical(StatusColumn, RemedyText, 3.0f);
	AddVertical(Right, StatusCard, 5.0f);

	UBorder* ActivityCard = MakeSurface(
		WidgetTree,
		TEXT("ActivityCard"),
		AMSim::UITheme::ESurface::Card,
		FMargin(15.0f, 13.0f),
		14.0f,
		1.2f);
	Phase1ActivityCard = ActivityCard;
	UVerticalBox* ActivityColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ActivityColumn"));
	ActivityCard->SetContent(ActivityColumn);
	AddVertical(
		ActivityColumn,
		MakeText(
			WidgetTree,
			TEXT("OfferHeader"),
			TEXT("FLYING CLUB"),
			11,
			Amber,
			true),
		0.0f);
	OfferText = MakeText(WidgetTree, TEXT("Offer"), TEXT("No offer"), 17, White, true);
	OfferText->SetWrapTextAt(RailWrapWidth);
	AddVertical(ActivityColumn, OfferText, 3.0f);
	CompatibilityText = MakeText(
		WidgetTree,
		TEXT("Compatibility"),
		TEXT("Complete the airfield to unlock offers."),
		13,
		Muted);
	CompatibilityText->SetWrapTextAt(RailWrapWidth);
	AddVertical(ActivityColumn, CompatibilityText, 3.0f);
	PinOfferButton = MakeButton(
		WidgetTree,
		TEXT("PinOffer"),
		TEXT("PIN"),
		AMSim::UITheme::EButton::Quiet,
		12);
	PinOfferButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::PinOffer);
	UHorizontalBoxSlot* PinSlot = ContextActions->AddChildToHorizontalBox(PinOfferButton);
	PinSlot->SetPadding(FMargin(4.0f));
	PinSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	DeclineOfferButton = MakeButton(
		WidgetTree,
		TEXT("DeclineOffer"),
		TEXT("DECLINE"),
		AMSim::UITheme::EButton::Destructive,
		12);
	DeclineOfferButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::DeclineOffer);
	UHorizontalBoxSlot* DeclineSlot = ContextActions->AddChildToHorizontalBox(DeclineOfferButton);
	DeclineSlot->SetPadding(FMargin(4.0f));
	DeclineSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AcceptButton = MakeButton(
		WidgetTree,
		TEXT("AcceptOffer"),
		TEXT("ACCEPT OFFER"),
		AMSim::UITheme::EButton::Positive,
		13);
	AcceptButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::AcceptOffer);
	UHorizontalBoxSlot* AcceptSlot = ContextActions->AddChildToHorizontalBox(AcceptButton);
	AcceptSlot->SetPadding(FMargin(4.0f));
	AcceptSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ScheduleButton = MakeButton(
		WidgetTree,
		TEXT("ScheduleFlight"),
		TEXT("SCHEDULE FLIGHT"),
		AMSim::UITheme::EButton::Primary,
		13);
	ScheduleButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::ScheduleFlight);
	UHorizontalBoxSlot* ScheduleSlot = ContextActions->AddChildToHorizontalBox(ScheduleButton);
	ScheduleSlot->SetPadding(FMargin(4.0f));
	ScheduleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AddVertical(Right, ActivityCard, 5.0f);

	UBorder* FlightCard = MakeSurface(
		WidgetTree,
		TEXT("FlightCard"),
		AMSim::UITheme::ESurface::Card,
		FMargin(15.0f, 13.0f),
		14.0f,
		1.2f);
	Phase1FlightCard = FlightCard;
	UVerticalBox* FlightColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("FlightColumn"));
	FlightCard->SetContent(FlightColumn);
	AddVertical(
		FlightColumn,
		MakeText(
			WidgetTree,
			TEXT("FlightHeader"),
			TEXT("FLIGHT OPERATIONS"),
			11,
			Amber,
			true),
		0.0f);
	FlightText = MakeText(
		WidgetTree,
		TEXT("Flight"),
		TEXT("No active flight"),
		17,
		White,
		true);
	FlightText->SetWrapTextAt(RailWrapWidth);
	AddVertical(FlightColumn, FlightText, 3.0f);
	TimetableText = MakeText(
		WidgetTree,
		TEXT("Timetable"),
		TEXT("No timetable entry. Slots use exact five-minute increments."),
		13,
		Muted);
	TimetableText->SetWrapTextAt(RailWrapWidth);
	AddVertical(FlightColumn, TimetableText, 3.0f);
	ServicesText = MakeText(
		WidgetTree,
		TEXT("Services"),
		TEXT("Inspection: --\nFuel: --"),
		13,
		Cyan,
		true);
	ServicesText->SetWrapTextAt(RailWrapWidth);
	AddVertical(FlightColumn, ServicesText, 3.0f);
	LedgerText = MakeText(
		WidgetTree,
		TEXT("Ledger"),
		TEXT("LEDGER  No transactions"),
		12,
		Muted);
	LedgerText->SetWrapTextAt(RailWrapWidth);
	AddVertical(FlightColumn, LedgerText, 3.0f);
	RatingText = MakeText(
		WidgetTree,
		TEXT("Rating"),
		TEXT("Airport rating: 0"),
		13,
		Amber,
		true);
	RatingText->SetWrapTextAt(RailWrapWidth);
	AddVertical(FlightColumn, RatingText, 3.0f);
	AddVertical(Right, FlightCard, 5.0f);

	Phase2Panel = MakeSurface(
		WidgetTree,
		TEXT("LivingAirportCard"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(15.0f, 13.0f),
		16.0f,
		1.5f);
	UVerticalBox* Phase2Column = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("LivingAirportColumn"));
	Phase2Panel->SetContent(Phase2Column);
	AddVertical(
		Phase2Column,
		MakeText(
			WidgetTree,
			TEXT("LivingAirportHeader"),
			TEXT("LIVING AIRPORT"),
			12,
			Cyan,
			true),
		0.0f);
	Phase2StatusText = MakeText(
		WidgetTree,
		TEXT("LivingAirportStatus"),
		TEXT("Complete the first visit to unlock."),
		17,
		White,
		true,
		true);
	AddVertical(Phase2Column, Phase2StatusText, 3.0f);
	Phase2OperationsText = MakeText(
		WidgetTree,
		TEXT("LivingAirportOperations"),
		TEXT("Fleet and timetable"),
		13,
		Amber,
		true);
	Phase2WeatherText = MakeText(
		WidgetTree,
		TEXT("LivingAirportWeather"),
		TEXT("Forecast unavailable"),
		12,
		Muted);
	Phase2StaffText = MakeText(
		WidgetTree,
		TEXT("LivingAirportStaff"),
		TEXT("Staff and vehicles"),
		12,
		Cyan);
	Phase2TenantText = MakeText(
		WidgetTree,
		TEXT("LivingAirportTenant"),
		TEXT("Choose an identity"),
		12,
		White);
	Phase2EconomyText = MakeText(
		WidgetTree,
		TEXT("LivingAirportEconomy"),
		TEXT("Shared airport ledger"),
		12,
		Muted);
	for (UTextBlock* Detail : {
		Phase2OperationsText,
		Phase2WeatherText,
		Phase2StaffText,
		Phase2TenantText,
		Phase2EconomyText})
	{
		Detail->SetWrapTextAt(RailWrapWidth);
		AddVertical(Phase2Column, Detail, 2.0f);
	}
	InitializePhase2Button = MakeButton(
		WidgetTree,
		TEXT("InitializeLivingAirport"),
		TEXT("START LIVING AIRPORT"),
		AMSim::UITheme::EButton::Positive,
		13);
	InitializePhase2Button->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::InitializePhase2);
	AddVertical(Phase2Column, InitializePhase2Button, 5.0f);

	auto AddPhase2ActionRow = [Phase2Column](
		UHorizontalBox* Row,
		UButton* First,
		UButton* Second)
	{
		UHorizontalBoxSlot* FirstSlot = Row->AddChildToHorizontalBox(First);
		FirstSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		FirstSlot->SetPadding(FMargin(0.0f, 0.0f, 3.0f, 0.0f));
		if (Second)
		{
			UHorizontalBoxSlot* SecondSlot = Row->AddChildToHorizontalBox(Second);
			SecondSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			SecondSlot->SetPadding(FMargin(3.0f, 0.0f, 0.0f, 0.0f));
		}
		AddVertical(Phase2Column, Row, 3.0f);
	};
	SelectGAButton = MakeButton(
		WidgetTree,
		TEXT("SelectGA"),
		TEXT("GA"),
		AMSim::UITheme::EButton::Quiet,
		11);
	SelectGAButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::SelectGeneralAviation);
	SelectSchoolButton = MakeButton(
		WidgetTree,
		TEXT("SelectSchool"),
		TEXT("SCHOOL"),
		AMSim::UITheme::EButton::Quiet,
		11);
	SelectSchoolButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::SelectFlightSchool);
	SelectCharterButton = MakeButton(
		WidgetTree,
		TEXT("SelectCharter"),
		TEXT("CHARTER"),
		AMSim::UITheme::EButton::Quiet,
		11);
	SelectCharterButton->OnClicked.AddDynamic(this, &UAMSimRootScreen::SelectCharter);
	AcceptPhase2ContractButton = MakeButton(
		WidgetTree,
		TEXT("AcceptLivingContract"),
		TEXT("ACCEPT"),
		AMSim::UITheme::EButton::Positive,
		11);
	AcceptPhase2ContractButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::AcceptPhase2Contract);
	CancelPhase2ContractButton = MakeButton(
		WidgetTree,
		TEXT("CancelLivingContract"),
		TEXT("END"),
		AMSim::UITheme::EButton::Destructive,
		11);
	CancelPhase2ContractButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::CancelPhase2Contract);
	ReschedulePhase2FlightButton = MakeButton(
		WidgetTree,
		TEXT("RescheduleLivingFlight"),
		TEXT("RESLOT"),
		AMSim::UITheme::EButton::Secondary,
		11);
	ReschedulePhase2FlightButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::ReschedulePhase2Flight);
	DispatchPhase2ServiceButton = MakeButton(
		WidgetTree,
		TEXT("DispatchLivingService"),
		TEXT("DISPATCH"),
		AMSim::UITheme::EButton::Primary,
		11);
	DispatchPhase2ServiceButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::DispatchPhase2Service);
	TogglePhase2RunwayButton = MakeButton(
		WidgetTree,
		TEXT("ToggleLivingRunway"),
		TEXT("RUNWAY"),
		AMSim::UITheme::EButton::Tool,
		11);
	TogglePhase2RunwayButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::TogglePhase2Runway);
	TowPhase2AircraftButton = MakeButton(
		WidgetTree,
		TEXT("TowLivingAircraft"),
		TEXT("TOW"),
		AMSim::UITheme::EButton::Tool,
		11);
	TowPhase2AircraftButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::TowPhase2Aircraft);
	RotatePhase2TeamZoneButton = MakeButton(
		WidgetTree,
		TEXT("RotateLivingTeamZone"),
		TEXT("ZONE"),
		AMSim::UITheme::EButton::Tool,
		11);
	RotatePhase2TeamZoneButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::RotatePhase2TeamZone);
	PurchasePhase2ParcelButton = MakeButton(
		WidgetTree,
		TEXT("PurchaseLivingParcel"),
		TEXT("BUY LAND"),
		AMSim::UITheme::EButton::Secondary,
		11);
	PurchasePhase2ParcelButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::PurchasePhase2Parcel);
	StartPhase2ExpansionButton = MakeButton(
		WidgetTree,
		TEXT("StartLivingExpansion"),
		TEXT("EXPAND"),
		AMSim::UITheme::EButton::Primary,
		11);
	StartPhase2ExpansionButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::StartPhase2Expansion);
	RespondPhase2IncidentButton = MakeButton(
		WidgetTree,
		TEXT("RespondLivingIncident"),
		TEXT("RESPOND"),
		AMSim::UITheme::EButton::Destructive,
		11);
	RespondPhase2IncidentButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::RespondPhase2Incident);

	UHorizontalBox* IdentityRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("LivingIdentityRow"));
	AddPhase2ActionRow(IdentityRow, SelectGAButton, SelectSchoolButton);
	AddVertical(Phase2Column, SelectCharterButton, 3.0f);
	UHorizontalBox* ContractRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("LivingContractRow"));
	AddPhase2ActionRow(ContractRow, AcceptPhase2ContractButton, DispatchPhase2ServiceButton);
	UHorizontalBox* ScheduleLifecycleRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("LivingScheduleLifecycleRow"));
	AddPhase2ActionRow(
		ScheduleLifecycleRow,
		ReschedulePhase2FlightButton,
		CancelPhase2ContractButton);
	UHorizontalBox* AirsideRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("LivingAirsideRow"));
	AddPhase2ActionRow(AirsideRow, TogglePhase2RunwayButton, TowPhase2AircraftButton);
	UHorizontalBox* GrowthRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("LivingGrowthRow"));
	AddPhase2ActionRow(GrowthRow, PurchasePhase2ParcelButton, StartPhase2ExpansionButton);
	UHorizontalBox* ReadinessRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("LivingReadinessRow"));
	AddPhase2ActionRow(ReadinessRow, RotatePhase2TeamZoneButton, RespondPhase2IncidentButton);
	Phase2Panel->SetVisibility(ESlateVisibility::Collapsed);
	AddVertical(Right, Phase2Panel, 5.0f);
	USizeBox* RightRailWidth = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("RightRailWidth"));
	BuildModeRightChrome = RightRailWidth;
	RightRailWidth->SetWidthOverride(320.0f / FMath::Max(InterfaceScale, 1.0f));
	RightRailWidth->SetContent(RightPanel);
	UHorizontalBoxSlot* RightSlot = Main->AddChildToHorizontalBox(RightRailWidth);
	RightSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightSlot->SetPadding(FMargin(12.0f, 0.0f, 0.0f, 0.0f));
	if (bCompactLayout)
	{
		RightPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UBorder* Footer = MakePanel(WidgetTree, TEXT("Footer"));
	BuildModeFooterChrome = Footer;
	AMSim::UITheme::StyleSurface(
		Footer,
		AMSim::UITheme::ESurface::Chrome,
		FMargin(16.0f, 8.0f),
		18.0f,
		1.5f);
	UVerticalBox* FooterColumn = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("FooterColumn"));
	Footer->SetContent(FooterColumn);
	CaptionText = MakeText(
		WidgetTree,
		TEXT("RadioCaption"),
		TEXT("RADIO  /  Captions are enabled for every operational call."),
		bCompactLayout ? 12 : 14,
		White,
		true);
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
			TEXT("OBJECTIVE"),
			AMSim::UITheme::EButton::Tool,
			12);
		ObjectiveToggle->OnClicked.AddDynamic(this, &UAMSimRootScreen::ToggleObjectiveDrawer);
		Controls->AddChildToHorizontalBox(ObjectiveToggle)->SetPadding(FMargin(3.0f));
		UButton* OperationsToggle = MakeButton(
			WidgetTree,
			TEXT("OperationsDrawerToggle"),
			TEXT("OPERATIONS"),
			AMSim::UITheme::EButton::Tool,
			12);
		OperationsToggle->OnClicked.AddDynamic(this, &UAMSimRootScreen::ToggleOperationsDrawer);
		Controls->AddChildToHorizontalBox(OperationsToggle)->SetPadding(FMargin(3.0f));
	}
	else
	{
		AddVertical(FooterColumn, Controls, 2.0f);
	}
	PauseButton = MakeButton(
		WidgetTree,
		TEXT("Pause"),
		TEXT("PLAY 1x"),
		AMSim::UITheme::EButton::Tool,
		12);
	PauseButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::PauseSimulation);
	Controls->AddChildToHorizontalBox(
		PauseButton)->SetPadding(FMargin(3.0f));
	UButton* SpeedOne = MakeButton(
		WidgetTree,
		TEXT("SpeedOne"),
		TEXT("1x"),
		AMSim::UITheme::EButton::Quiet,
		12);
	SpeedOne->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedOne);
	Controls->AddChildToHorizontalBox(SpeedOne)->SetPadding(FMargin(3.0f));
	UButton* SpeedTwo = MakeButton(
		WidgetTree,
		TEXT("SpeedTwo"),
		TEXT("2x"),
		AMSim::UITheme::EButton::Quiet,
		12);
	SpeedTwo->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedTwo);
	Controls->AddChildToHorizontalBox(SpeedTwo)->SetPadding(FMargin(3.0f));
	UButton* SpeedFour = MakeButton(
		WidgetTree,
		TEXT("SpeedFour"),
		TEXT("4x"),
		AMSim::UITheme::EButton::Quiet,
		12);
	SpeedFour->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedFour);
	Controls->AddChildToHorizontalBox(SpeedFour)->SetPadding(FMargin(3.0f));
	UButton* SpeedEight = MakeButton(
		WidgetTree,
		TEXT("SpeedEight"),
		TEXT("8x"),
		AMSim::UITheme::EButton::Quiet,
		12);
	SpeedEight->OnClicked.AddDynamic(this, &UAMSimRootScreen::SetSpeedEight);
	Controls->AddChildToHorizontalBox(SpeedEight)->SetPadding(FMargin(3.0f));
	UButton* Save = MakeButton(
		WidgetTree,
		TEXT("Save"),
		TEXT("SAVE"),
		AMSim::UITheme::EButton::Secondary,
		12);
	Save->OnClicked.AddDynamic(this, &UAMSimRootScreen::SaveGame);
	Controls->AddChildToHorizontalBox(Save)->SetPadding(FMargin(3.0f));
	UButton* Load = MakeButton(
		WidgetTree,
		TEXT("Load"),
		TEXT("LOAD"),
		AMSim::UITheme::EButton::Secondary,
		12);
	Load->OnClicked.AddDynamic(this, &UAMSimRootScreen::LoadGame);
	Controls->AddChildToHorizontalBox(Load)->SetPadding(FMargin(3.0f));
	UButton* AirportNavigation = MakeButton(
		WidgetTree,
		TEXT("AirportNavigation"),
		TEXT("AIRPORT"),
		AMSim::UITheme::EButton::Primary,
		11);
	AirportNavigation->SetIsEnabled(false);
	AirportNavigation->SetToolTipText(
		FText::FromString(TEXT("Current destination")));
	Controls->AddChildToHorizontalBox(
		AirportNavigation)->SetPadding(FMargin(12.0f, 3.0f, 3.0f, 3.0f));
	TerminalNavigationButton = MakeButton(
		WidgetTree,
		TEXT("TerminalNavigation"),
		bCompactLayout ? TEXT("OPS") : TEXT("TERMINAL"),
		AMSim::UITheme::EButton::Secondary,
		12);
	TerminalNavigationButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::ToggleTerminalPresentation);
	TerminalNavigationButton->SetToolTipText(
		FText::FromString(TEXT("Locked until terminal planning is available")));
	Controls->AddChildToHorizontalBox(
		TerminalNavigationButton)->SetPadding(FMargin(3.0f));
	RegionalNavigationButton = MakeButton(
		WidgetTree,
		TEXT("RegionalNavigation"),
		TEXT("REGIONAL (LOCKED)"),
		AMSim::UITheme::EButton::Secondary,
		11);
	RegionalNavigationButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::OpenRegionalPresentation);
	Controls->AddChildToHorizontalBox(
		RegionalNavigationButton)->SetPadding(FMargin(3.0f));
	AdvancedNavigationButton = MakeButton(
		WidgetTree,
		TEXT("AdvancedNavigation"),
		TEXT("ADVANCED (LOCKED)"),
		AMSim::UITheme::EButton::Secondary,
		11);
	AdvancedNavigationButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::OpenAdvancedPresentation);
	Controls->AddChildToHorizontalBox(
		AdvancedNavigationButton)->SetPadding(FMargin(3.0f));
	MajorNavigationButton = MakeButton(
		WidgetTree,
		TEXT("MajorNavigation"),
		TEXT("MAJOR (LOCKED)"),
		AMSim::UITheme::EButton::Secondary,
		11);
	MajorNavigationButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::OpenMajorPresentation);
	Controls->AddChildToHorizontalBox(
		MajorNavigationButton)->SetPadding(FMargin(3.0f));
	WindowModeButton = MakeButton(
		WidgetTree,
		TEXT("WindowMode"),
		TEXT("WINDOWED"),
		AMSim::UITheme::EButton::Quiet,
		11);
	WindowModeButton->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::ToggleWindowMode);
	Controls->AddChildToHorizontalBox(
		WindowModeButton)->SetPadding(FMargin(3.0f));
	UButton* Help = MakeButton(
		WidgetTree,
		TEXT("Help"),
		TEXT("HELP"),
		AMSim::UITheme::EButton::Tool,
		12);
	Help->OnClicked.AddDynamic(
		this,
		&UAMSimRootScreen::ToggleReleaseGuide);
	Controls->AddChildToHorizontalBox(Help)->SetPadding(FMargin(3.0f));
	InteractionText = MakeText(
		WidgetTree,
		TEXT("Interaction"),
		TEXT("Ready."),
		bCompactLayout ? 11 : 12,
		Muted,
		true);
	InteractionText->SetJustification(ETextJustify::Right);
	AddVertical(FooterColumn, InteractionText, 2.0f);
	UVerticalBoxSlot* FooterSlot = Page->AddChildToVerticalBox(Footer);
	FooterSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	TerminalView = WidgetTree->ConstructWidget<UAMSimTerminalView>(
		UAMSimTerminalView::StaticClass(),
		TEXT("PassengerTerminalView"));
	TerminalView->OnReturnRequested.BindUObject(
		this,
		&UAMSimRootScreen::CloseTerminalPresentation);
	UOverlaySlot* TerminalSlot = Root->AddChildToOverlay(TerminalView);
	TerminalSlot->SetHorizontalAlignment(HAlign_Fill);
	TerminalSlot->SetVerticalAlignment(VAlign_Fill);

	ContextHelpCard =
		WidgetTree->ConstructWidget<UAMSimContextHelpCard>(
			UAMSimContextHelpCard::StaticClass(),
			TEXT("ContextHelpCard"));
	UOverlaySlot* ContextHelpSlot =
		Root->AddChildToOverlay(ContextHelpCard);
	ContextHelpSlot->SetHorizontalAlignment(HAlign_Fill);
	ContextHelpSlot->SetVerticalAlignment(VAlign_Fill);
	ReleaseGuideView =
		WidgetTree->ConstructWidget<UAMSimReleaseGuideView>(
			UAMSimReleaseGuideView::StaticClass(),
			TEXT("ReleaseGuideView"));
	UOverlaySlot* ReleaseGuideSlot =
		Root->AddChildToOverlay(ReleaseGuideView);
	ReleaseGuideSlot->SetHorizontalAlignment(HAlign_Fill);
	ReleaseGuideSlot->SetVerticalAlignment(VAlign_Fill);
	SchedulePickerView =
		WidgetTree->ConstructWidget<UAMSimSchedulePickerView>(
			UAMSimSchedulePickerView::StaticClass(),
			TEXT("SchedulePickerView"));
	SchedulePickerView->OnScheduleRequested =
		[this](const int64 ScheduledArrival)
		{
			return SubmitScheduleAt(ScheduledArrival);
		};
	UOverlaySlot* SchedulePickerSlot =
		Root->AddChildToOverlay(SchedulePickerView);
	SchedulePickerSlot->SetHorizontalAlignment(HAlign_Fill);
	SchedulePickerSlot->SetVerticalAlignment(VAlign_Fill);
	SaveLoadView =
		WidgetTree->ConstructWidget<UAMSimSaveLoadView>(
			UAMSimSaveLoadView::StaticClass(),
			TEXT("SaveLoadView"));
	SaveLoadView->OnLoadRequested =
		[this](const FString& SlotId)
		{
			return LoadSlotById(SlotId);
		};
	UOverlaySlot* SaveLoadSlot = Root->AddChildToOverlay(SaveLoadView);
	SaveLoadSlot->SetHorizontalAlignment(HAlign_Fill);
	SaveLoadSlot->SetVerticalAlignment(VAlign_Fill);
	if (bOpenReleaseGuideWhenReady ||
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimReleaseGuide")))
	{
		ReleaseGuideView->OpenGuide();
	}
	RefreshSaveSlots();
#if !UE_BUILD_SHIPPING
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimLoadMenuProof")))
	{
		if (UAMSimGameInstanceSubsystem* GameSubsystem =
			GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>())
		{
			SaveLoadView->OpenPicker(GameSubsystem->ListSaveSlots());
		}
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("AMSimSchedulePickerProof")))
	{
		SchedulePickerView->OpenPicker(
			{600000, 900000, 1200000, 1500000},
			360000);
	}
#endif

	const UAMSimRadioSubsystem* Radio =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<
				UAMSimRadioSubsystem>()
			: nullptr;
	const bool bSpeechReady =
		Radio && Radio->IsSpeechReady();
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
	Super::NativeDestruct();
}

void UAMSimRootScreen::ToggleObjectiveDrawer()
{
	if (!ObjectiveDrawer || !OperationsDrawer)
	{
		return;
	}
	const bool bOpen = ObjectiveDrawer->GetVisibility() == ESlateVisibility::Collapsed;
	ObjectiveDrawer->SetVisibility(
		bOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
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
	const AMSim::FPhase2QuerySnapshot Phase2Query = Subsystem->GetPhase2Query();
	const AMSim::FPhase2State& Phase2State = Subsystem->GetSimulation().GetPhase2State();
	const AMSim::FPhase3QuerySnapshot Phase3Query = Subsystem->GetPhase3Query();
	const AMSim::FPhase4QuerySnapshot Phase4Query =
		Subsystem->GetPhase4Query();
	const AMSim::FPhase5QuerySnapshot Phase5Query =
		Subsystem->GetPhase5Query();
	const AMSim::FPhase6QuerySnapshot Phase6Query =
		Subsystem->GetPhase6Query();
	RefreshAudioFeedback(
		Query,
		State,
		Phase2Query,
		Phase4Query,
		Phase5Query,
		Phase6Query);
	RefreshDestinationButtons(
		Query,
		Phase3Query,
		Phase4Query,
		Phase5Query,
		Phase6Query);
	if (PauseButton)
	{
		SetButtonLabel(
			PauseButton,
			Query.bPaused ? TEXT("PLAY 1x") : TEXT("PAUSE"));
	}
	const bool bOperationalEvidence =
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimPhase45OperationalEvidence"));
	const bool bShowPhase2ClosureProof =
		bOperationalEvidence &&
		Phase2State.Expansion.Stage != AMSim::EExpansionStage::None &&
		Phase2State.Expansion.Stage != AMSim::EExpansionStage::Operational;
	for (TActorIterator<AAMSimCameraPawn> It(GetWorld()); It; ++It)
	{
		It->SetCloseOperationsMode(
			Query.FlightState == AMSim::EFlightState::Turnaround);
	}
	if (TerminalView)
	{
		if (!bTerminalGrowthProofOpened && Query.bInitialized &&
			FParse::Param(FCommandLine::Get(), TEXT("AMSimTerminalGrowthProof")))
		{
			TerminalView->ShowPresentation();
			TerminalView->ShowBuildMode();
			bTerminalGrowthProofOpened = true;
		}
		if (Phase3Query.bInitialized &&
			!TerminalView->HasBeenOpened())
		{
			TerminalView->ShowPresentation();
		}
		TerminalView->RefreshFromSimulation();
		if (bShowPhase2ClosureProof)
		{
			TerminalView->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (ContextHelpCard)
	{
		ContextHelpCard->SetSuppressed(
			TerminalView && TerminalView->IsPresentationOpen());
	}
	if (TurnaroundView)
	{
		TurnaroundView->RefreshFromSimulation();
	}
	RefreshPhase1OperationsHub(Query, State);
	if (Phase1Page)
	{
		Phase1Page->SetVisibility(
			TerminalView &&
			TerminalView->IsPresentationOpen() &&
			!bShowPhase2ClosureProof
				? ESlateVisibility::Collapsed
				: ESlateVisibility::Visible);
	}
	if (LastAppliedRevision == Query.Revision)
	{
		return;
	}
	LastAppliedRevision = Query.Revision;
	CurrentViewState = AMSim::MakePhase1ViewState(Query, State);
	if (ConstructionProposalView)
	{
		const bool bAutomatedProposalProof =
			FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase45ConstructionProof"));
		if (bAutomatedProposalProof &&
			CurrentViewState.bCanBuild &&
			!ConstructionProposalView->IsProposalOpen())
		{
			ConstructionProposalView->OpenProposal();
		}
		else if (!CurrentViewState.bCanBuild)
		{
			ConstructionProposalView->CloseProposal();
		}
	}
	if (StaffView)
	{
		StaffView->RefreshFromSnapshot(Query, State);
	}

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
	SetText(CauseText, CurrentViewState.Cause);
	SetText(RemedyText, TEXT("NEXT: ") + CurrentViewState.Remedy);
	SetText(ProjectText, CurrentViewState.Project);
	SetText(OfferText, CurrentViewState.Offer);
	SetText(CompatibilityText, CurrentViewState.Compatibility);
	SetText(FlightText, CurrentViewState.Flight);
	SetText(TimetableText, CurrentViewState.Timetable);
	SetText(ServicesText, CurrentViewState.Services);
	SetText(LedgerText, CurrentViewState.Ledger);
	SetText(RatingText, CurrentViewState.Rating);
	SetText(CaptionText, CurrentViewState.Caption);
	const bool bShowConstraint =
		Query.bInitialized &&
		(!Query.Cause.IsEmpty() || !Query.Remedy.IsEmpty()) &&
		Query.ConstructionStage != AMSim::EConstructionStage::Operational;
	if (CauseText)
	{
		CauseText->SetVisibility(
			bShowConstraint && !Query.Cause.IsEmpty()
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (RemedyText)
	{
		RemedyText->SetVisibility(
			bShowConstraint && !Query.Remedy.IsEmpty()
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (CreateAirportTray)
	{
		CreateAirportTray->SetVisibility(
			Query.bInitialized ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	if (CompatibilityText)
	{
		// Compatibility evidence is already presented in the contextual offer card.
		// Keeping it in the rail duplicates copy and makes the persistent card unstable.
		CompatibilityText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (TimetableText)
	{
		TimetableText->SetVisibility(
			State.Flight.Id.IsValid() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (ServicesText)
	{
		ServicesText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LedgerText)
	{
		LedgerText->SetVisibility(
			Query.FlightState == AMSim::EFlightState::Completed &&
			!State.Transactions.IsEmpty()
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (RatingText)
	{
		RatingText->SetVisibility(
			Query.FlightState == AMSim::EFlightState::Completed &&
			!State.RatingContributions.IsEmpty()
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (!bPhraseCursorInitialized)
	{
		LastPhraseCount = State.PhraseIntents.Num();
		bPhraseCursorInitialized = true;
	}
	else if (State.PhraseIntents.Num() < LastPhraseCount)
	{
		LastPhraseCount = State.PhraseIntents.Num();
	}
	else if (State.PhraseIntents.Num() > LastPhraseCount)
	{
		if (UAMSimRadioSubsystem* Radio =
			GetGameInstance()
				? GetGameInstance()->GetSubsystem<
					UAMSimRadioSubsystem>()
				: nullptr)
		{
			for (int32 Index = LastPhraseCount;
				Index < State.PhraseIntents.Num();
				++Index)
			{
				const AMSim::FPhraseIntentRecord& Phrase =
					State.PhraseIntents[Index];
				Radio->PresentCaption(
					GetWorld(),
					Phrase.PhraseId,
					Phrase.Caption);
			}
		}
	}

	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		if (!Phase3Query.bInitialized ||
			Phase3Query.TerminalStage <
				AMSim::ETerminalConstructionStage::ShellReady)
		{
			It->ApplySnapshot(Query, State);
			It->ApplyPhase2Snapshot(Phase2Query, Phase2State);
		}
		break;
	}

	RefreshPhase1ContextPanel(Query, State);

	if (CreateButton) CreateButton->SetIsEnabled(!CurrentViewState.bAirportInitialized);
	if (AirportNameEntry) AirportNameEntry->SetIsReadOnly(CurrentViewState.bAirportInitialized);
	if (BuildButton)
	{
		BuildButton->SetActionEnabled(Query.bInitialized);
		BuildButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (ScheduleNavigationButton)
	{
		ScheduleNavigationButton->SetActionEnabled(
			Query.bInitialized &&
			Query.ConstructionStage >= AMSim::EConstructionStage::ReadyToOpen);
		ScheduleNavigationButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (StaffButton)
	{
		StaffButton->SetActionEnabled(Query.bInitialized);
		StaffButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (OverlayButton)
	{
		OverlayButton->SetActionEnabled(
			Query.bInitialized &&
			Query.ConstructionStage >= AMSim::EConstructionStage::ReadyToOpen);
		OverlayButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (CancelBuildButton)
	{
		CancelBuildButton->SetActionEnabled(CurrentViewState.bCanCancelBuild);
		CancelBuildButton->SetVisibility(
			CurrentViewState.bCanCancelBuild
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (OpenButton)
	{
		OpenButton->SetActionEnabled(CurrentViewState.bCanOpen);
		OpenButton->SetVisibility(
			CurrentViewState.bCanOpen
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (CloseButton)
	{
		CloseButton->SetActionEnabled(CurrentViewState.bCanClose);
		CloseButton->SetVisibility(
			CurrentViewState.bCanClose
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (PinOfferButton)
	{
		PinOfferButton->SetIsEnabled(CurrentViewState.bCanPinOffer);
		PinOfferButton->SetVisibility(
			Query.OfferState == AMSim::EOfferState::Available
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (DeclineOfferButton)
	{
		DeclineOfferButton->SetIsEnabled(CurrentViewState.bCanDeclineOffer);
		DeclineOfferButton->SetVisibility(
			Query.OfferState == AMSim::EOfferState::Available
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (AcceptButton)
	{
		AcceptButton->SetIsEnabled(CurrentViewState.bCanAcceptOffer);
		AcceptButton->SetVisibility(
			Query.OfferState == AMSim::EOfferState::Available
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (ScheduleButton)
	{
		ScheduleButton->SetIsEnabled(CurrentViewState.bCanSchedule);
		ScheduleButton->SetVisibility(
			Query.OfferState == AMSim::EOfferState::Accepted
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (RecoveryButton)
	{
		RecoveryButton->SetActionEnabled(CurrentViewState.bCanRequestRecovery);
		RecoveryButton->SetVisibility(
			CurrentViewState.bCanRequestRecovery
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	RefreshPhase2Presentation(Phase2Query, Phase2State, State);
	if (TerminalView)
	{
		TerminalView->RefreshFromSimulation();
	}
	LastPhraseCount = State.PhraseIntents.Num();
	ForceLayoutPrepass();
}

void UAMSimRootScreen::SetInteractionMessage(const FString& Message, const bool bSucceeded)
{
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(Message));
		InteractionText->SetColorAndOpacity(FSlateColor(bSucceeded ? Cyan : ErrorColor));
	}
}
