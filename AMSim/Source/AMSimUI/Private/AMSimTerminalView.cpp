#include "AMSimTerminalView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimCameraPawn.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimExpandingToolButton.h"
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
#include "InputCoreTypes.h"
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
	SetIsFocusable(true);
	static ConstructorHelpers::FObjectFinder<UTexture2D> PassengerFamily(
		TEXT("/Game/Phase45/Presentation/Textures/Operations/T_PassengerFamily.T_PassengerFamily"));
	PassengerFamilyTexture = PassengerFamily.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> StructureIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconStructure.T_IconStructure"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DoorsIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconDoors.T_IconDoors"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> FurnitureIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconFurniture.T_IconFurniture"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> AmenitiesIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconAmenities.T_IconAmenities"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> OperationsIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconOperations.T_IconOperations"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ZonesIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconZones.T_IconZones"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DemolishIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconDemolish.T_IconDemolish"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> RotateIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconRotate.T_IconRotate"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CopyIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconCopy.T_IconCopy"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> UndoIcon(
		TEXT("/Game/TerminalGrowth/Presentation/Textures/Icon/T_IconUndo.T_IconUndo"));
	TerminalEditorIcons = {
		StructureIcon.Object,
		DoorsIcon.Object,
		FurnitureIcon.Object,
		AmenitiesIcon.Object,
		OperationsIcon.Object,
		ZonesIcon.Object,
		DemolishIcon.Object,
		RotateIcon.Object,
		CopyIcon.Object,
		UndoIcon.Object};
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
	bCompactLayout = bCompact;
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
	BrandText = MakeText(
		WidgetTree,
		TEXT("TerminalBrand"),
		bCompact ? TEXT("RIVERBEND  ·  TERMINAL") :
			TEXT("RIVERBEND AIRPORT\nDOMESTIC TERMINAL"),
		bCompact ? 15 : 18,
		AMSim::UITheme::White(),
		true);
	UHorizontalBoxSlot* BrandSlot = TopRow->AddChildToHorizontalBox(BrandText);
	BrandSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BrandSlot->SetVerticalAlignment(VAlign_Center);
	UButton* ReturnButton = MakeButton(
		WidgetTree,
		TEXT("TerminalReturn"),
		TEXT("BACK"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	ReturnButton->OnClicked.AddUniqueDynamic(
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
	AdvancedOperationsButton->OnClicked.AddUniqueDynamic(
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
	MajorOperationsButton->OnClicked.AddUniqueDynamic(
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
	UHorizontalBox* ModeRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("TerminalModeRow"));
	BuildModeButton = MakeButton(
		WidgetTree, TEXT("TerminalBuildMode"), TEXT("BUILD"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	BuildModeButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::ShowBuildMode);
	OperationsModeButton = MakeButton(
		WidgetTree, TEXT("TerminalOperationsMode"), TEXT("OPERATE"),
		AMSim::UITheme::EButton::Secondary, SmallSize);
	OperationsModeButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::ShowOperationsMode);
	ModeRow->AddChildToHorizontalBox(BuildModeButton)->SetPadding(FMargin(0, 0, 3, 0));
	ModeRow->AddChildToHorizontalBox(OperationsModeButton)->SetPadding(FMargin(3, 0, 0, 0));
	AddVertical(Tools, ModeRow, 8.0f);

	TerminalBuildToolsPanel = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("TerminalBuildToolsPanel"));
	AddVertical(Tools, TerminalBuildToolsPanel, 4.0f);
	const auto AddEditorTool = [this](
		UVerticalBox* Parent,
		const TCHAR* Name,
		const int32 IconIndex,
		const FString& Label,
		void (UAMSimTerminalView::*Handler)())
	{
		UAMSimExpandingToolButton* Tool =
			WidgetTree->ConstructWidget<UAMSimExpandingToolButton>(
				UAMSimExpandingToolButton::StaticClass(), Name);
		Tool->Configure(
			TerminalEditorIcons.IsValidIndex(IconIndex)
				? TerminalEditorIcons[IconIndex].Get() : nullptr,
			Label);
		Tool->OnActivated.BindUObject(this, Handler);
		TerminalEditorToolButtons.Add(Tool);
		AddVertical(Parent, Tool, 2.0f);
	};
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalFloorTool"), 0,
		TEXT("FLOOR"), &UAMSimTerminalView::SelectFloorTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalFloorFunction"), 5,
		TEXT("FLOOR USE"), &UAMSimTerminalView::CycleFloorFunction);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalWallTool"), 0,
		TEXT("WALL"), &UAMSimTerminalView::SelectWallTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalDoorTool"), 1,
		TEXT("DOOR"), &UAMSimTerminalView::SelectDoorTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalSeatingTool"), 2,
		TEXT("SEATING"), &UAMSimTerminalView::SelectSeatingTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalInformationTool"), 4,
		TEXT("INFORMATION"), &UAMSimTerminalView::SelectInformationTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalRestroomTool"), 3,
		TEXT("RESTROOM"), &UAMSimTerminalView::SelectRestroomTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalStaffDeskTool"), 5,
		TEXT("STAFF DESK"), &UAMSimTerminalView::SelectStaffDeskTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalDemolishTool"), 6,
		TEXT("DEMOLISH"), &UAMSimTerminalView::SelectDemolishTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalRotateTool"), 7,
		TEXT("ROTATE"), &UAMSimTerminalView::SelectRotateTool);
	AddEditorTool(TerminalBuildToolsPanel, TEXT("TerminalUndoTool"), 9,
		TEXT("UNDO"), &UAMSimTerminalView::UndoTerminalEdit);
	TerminalBuildToolsPanel->SetVisibility(ESlateVisibility::Collapsed);

	TerminalOperationsToolsPanel = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("TerminalOperationsToolsPanel"));
	AddVertical(Tools, TerminalOperationsToolsPanel, 4.0f);

	InitializeButton = MakeButton(
		WidgetTree, TEXT("InitializeTerminal"), TEXT("OPEN TERMINAL PLANNING"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	InitializeButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::InitializePassengerAirport);
	AddVertical(TerminalOperationsToolsPanel, InitializeButton, 7.0f);
	FundButton = MakeButton(
		WidgetTree, TEXT("FundTerminal"), TEXT("FUND TERMINAL SHELL"),
		AMSim::UITheme::EButton::Positive, SmallSize);
	FundButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::FundTerminal);
	AddVertical(TerminalOperationsToolsPanel, FundButton, 7.0f);
	ConnectButton = MakeButton(
		WidgetTree, TEXT("ConnectTerminal"), TEXT("CONNECT NEXT FLOW"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	ConnectButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::ConnectNextNetwork);
	AddVertical(TerminalOperationsToolsPanel, ConnectButton, 7.0f);
	OpenButton = MakeButton(
		WidgetTree, TEXT("OpenTerminal"), TEXT("OPEN TERMINAL"),
		AMSim::UITheme::EButton::Positive, SmallSize);
	OpenButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::OpenTerminal);
	AddVertical(TerminalOperationsToolsPanel, OpenButton, 7.0f);
	ScheduleButton = MakeButton(
		WidgetTree, TEXT("SchedulePassengerService"),
		TEXT("SCHEDULE RB 304"),
		AMSim::UITheme::EButton::Primary, SmallSize);
	ScheduleButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::SchedulePassengerService);
	AddVertical(TerminalOperationsToolsPanel, ScheduleButton, 12.0f);

	AddSectionTitle(
		WidgetTree,
		TerminalOperationsToolsPanel,
		TEXT("FlowToolsTitle"),
		TEXT("OPERATIONS"),
		bCompact ? 12 : 14);
	SecurityButton = MakeButton(
		WidgetTree, TEXT("SecurityLane"), TEXT("TOGGLE SECURITY LANE"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	SecurityButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::ToggleSecurityLane);
	AddVertical(TerminalOperationsToolsPanel, SecurityButton, 7.0f);
	AssistanceButton = MakeButton(
		WidgetTree, TEXT("PassengerAssistance"), TEXT("ASSIGN ROUTE ASSISTANCE"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	AssistanceButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::RequestPassengerAssistance);
	AddVertical(TerminalOperationsToolsPanel, AssistanceButton, 7.0f);
	BaggageButton = MakeButton(
		WidgetTree, TEXT("BaggageRecovery"), TEXT("RESOLVE BAG EXCEPTION"),
		AMSim::UITheme::EButton::Tool, SmallSize);
	BaggageButton->OnClicked.AddUniqueDynamic(
		this, &UAMSimTerminalView::ResolveBaggageException);
	AddVertical(TerminalOperationsToolsPanel, BaggageButton, 9.0f);
	UButton* OverlayButton = MakeButton(
		WidgetTree, TEXT("OverlayMode"), TEXT("CYCLE FLOW OVERLAY"),
		AMSim::UITheme::EButton::Secondary, SmallSize);
	OverlayButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::CycleOverlay);
	AddVertical(TerminalOperationsToolsPanel, OverlayButton, 5.0f);
	OverlayText = MakeText(
		WidgetTree,
		TEXT("OverlayModeText"),
		TEXT("ALL FLOWS"),
		SmallSize,
		AMSim::UITheme::Cyan(),
		true);
	AddVertical(TerminalOperationsToolsPanel, OverlayText, 5.0f);
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
			? FAnchors(0.01f, 0.80f, 0.49f, 0.985f)
			: FAnchors(0.008f, 0.105f, 0.176f, 0.89f),
		FMargin(0.0f),
		9);

	UBorder* PartyRail = MakeSurface(
		WidgetTree,
		TEXT("PassengerPartyRail"),
		AMSim::UITheme::ESurface::Panel,
		bCompact ? FMargin(11.0f) : FMargin(15.0f),
		17.0f);
	TerminalOperationsInspector = PartyRail;
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
			? FAnchors(0.74f, 0.105f, 0.992f, 0.43f)
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
			? FAnchors(0.01f, 0.10f, 0.99f, 0.985f)
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
		TerminalLegendTexts.Add(Text);
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
						Button->OnClicked.AddUniqueDynamic(
							this, &UAMSimTerminalView::PauseSimulation);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("1x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddUniqueDynamic(
							this, &UAMSimTerminalView::SetSpeedOne);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("2x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddUniqueDynamic(
							this, &UAMSimTerminalView::SetSpeedTwo);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("4x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddUniqueDynamic(
							this, &UAMSimTerminalView::SetSpeedFour);
					}),
			TPair<FString, TFunction<void(UButton*)>>(
				TEXT("8x"),
				[this](UButton* Button)
					{
						Button->OnClicked.AddUniqueDynamic(
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
	SaveButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::SaveGame);
	LegendRow->AddChildToHorizontalBox(SaveButton)->SetPadding(FMargin(6.0f, 0.0f, 3.0f, 0.0f));
	UButton* LoadButton = MakeButton(
		WidgetTree,
		TEXT("TerminalLoad"),
		TEXT("LOAD"),
		AMSim::UITheme::EButton::Secondary,
		SmallSize);
	LoadButton->OnClicked.AddUniqueDynamic(this, &UAMSimTerminalView::LoadGame);
	LegendRow->AddChildToHorizontalBox(LoadButton)->SetPadding(FMargin(3.0f, 0.0f));
	AddAnchored(
		Canvas,
		Legend,
		bCompact
			? FAnchors(0.51f, 0.80f, 0.99f, 0.885f)
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

FReply UAMSimTerminalView::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bRightMousePanning = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	if (bBuildMode && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton &&
		TerminalLabels)
	{
		const FGeometry MapGeometry = TerminalLabels->GetCachedGeometry();
		const FVector2D Local = MapGeometry.AbsoluteToLocal(
			InMouseEvent.GetScreenSpacePosition());
		const FVector2D Size = MapGeometry.GetLocalSize();
		if (Local.X >= 0.0f && Local.Y >= 0.0f &&
			Local.X <= Size.X && Local.Y <= Size.Y)
		{
			TerminalGestureStart = PointerToTerminalCell(
				InGeometry,
				InMouseEvent.GetScreenSpacePosition());
			bTerminalGestureActive = true;
			return FReply::Handled().CaptureMouse(TakeWidget());
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UAMSimTerminalView::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton &&
		bRightMousePanning)
	{
		bRightMousePanning = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton &&
		bTerminalGestureActive)
	{
		const AMSim::FTerminalCellCoord End = PointerToTerminalCell(
			InGeometry,
			InMouseEvent.GetScreenSpacePosition());
		bTerminalGestureActive = false;
		for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
		{
			It->ClearTerminalPlacementPreview();
			break;
		}
		CommitTerminalGesture(TerminalGestureStart, End);
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UAMSimTerminalView::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (bRightMousePanning)
	{
		if (AAMSimCameraPawn* CameraPawn = GetOwningPlayer()
			? Cast<AAMSimCameraPawn>(GetOwningPlayer()->GetPawn()) : nullptr)
		{
			CameraPawn->PanByScreenDelta(InMouseEvent.GetCursorDelta());
		}
		return FReply::Handled();
	}
	if (bTerminalGestureActive && InteractionText)
	{
		const AMSim::FTerminalCellCoord End = PointerToTerminalCell(
			InGeometry,
			InMouseEvent.GetScreenSpacePosition());
		const int32 Width = FMath::Abs(End.X - TerminalGestureStart.X) + 1;
		const int32 Height = FMath::Abs(End.Y - TerminalGestureStart.Y) + 1;
		const int64 PreviewCost = ActiveEditorTool == EAMSimTerminalEditorTool::Floor
			? static_cast<int64>(Width) * Height * 25
			: ActiveEditorTool == EAMSimTerminalEditorTool::Wall
				? static_cast<int64>(Width + Height - 2) * 35
				: 0;
		int32 PreviewState = 1;
		if (const UAMSimAirportSimulationSubsystem* Subsystem =
			GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr)
		{
			const AMSim::FTerminalLayoutState& Layout =
				Subsystem->GetSimulation().GetPhase3State().TerminalLayout;
			if (PreviewCost > Subsystem->GetSimulation().GetPhase1State().Credits)
			{
				PreviewState = 3;
			}
			else if (ActiveEditorTool == EAMSimTerminalEditorTool::Floor)
			{
				const int32 MinX = FMath::Min(TerminalGestureStart.X, End.X);
				const int32 MaxX = FMath::Max(TerminalGestureStart.X, End.X);
				const int32 MinY = FMath::Min(TerminalGestureStart.Y, End.Y);
				const int32 MaxY = FMath::Max(TerminalGestureStart.Y, End.Y);
				if (Layout.FloorCells.ContainsByPredicate(
					[MinX, MaxX, MinY, MaxY](const AMSim::FTerminalFloorCellRecord& Cell)
					{
						return Cell.Cell.X >= MinX && Cell.Cell.X <= MaxX &&
							Cell.Cell.Y >= MinY && Cell.Cell.Y <= MaxY;
					}))
				{
					PreviewState = 2;
				}
			}
		}
		for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
		{
			It->SetTerminalPlacementPreview(TerminalGestureStart, End, PreviewState);
			break;
		}
		InteractionText->SetText(FText::FromString(FString::Printf(
			TEXT("%dm x %dm  \u00b7  %lld CR  \u00b7  release to commit"),
			Width,
			Height,
			PreviewCost)));
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UAMSimTerminalView::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (bPresentationOpen && InKeyEvent.GetKey() == EKeys::Escape)
	{
		ReturnToAirport();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

AMSim::FTerminalCellCoord UAMSimTerminalView::PointerToTerminalCell(
	const FGeometry& Geometry,
	const FVector2D& ScreenPosition) const
{
	(void)Geometry;
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		FVector RayOrigin;
		FVector RayDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(
			ScreenPosition.X, ScreenPosition.Y, RayOrigin, RayDirection) &&
			!FMath::IsNearlyZero(RayDirection.Z))
		{
			for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
			{
				const AMSim::FTerminalLayoutState* Layout = nullptr;
				if (const UAMSimAirportSimulationSubsystem* Subsystem =
					GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>())
				{
					Layout = &Subsystem->GetSimulation().GetPhase3State().TerminalLayout;
				}
				if (Layout && Layout->bSeeded)
				{
					const double Distance = (42.0 - RayOrigin.Z) / RayDirection.Z;
					const FVector WorldPoint = RayOrigin + RayDirection * Distance;
					const FVector Center = It->GetTerminalWorldCenter();
					const double Width = (Layout->MaximumX - Layout->MinimumX + 1) * 100.0;
					const double Depth = (Layout->MaximumY - Layout->MinimumY + 1) * 100.0;
					return {
						Layout->MinimumX + static_cast<int32>(FMath::FloorToInt(
							(WorldPoint.Y - Center.Y + Width * 0.5) / 100.0)),
						Layout->MinimumY + static_cast<int32>(FMath::FloorToInt(
							(WorldPoint.X - Center.X + Depth * 0.5) / 100.0))};
				}
				break;
			}
		}
	}
	const FGeometry MapGeometry = TerminalLabels
		? TerminalLabels->GetCachedGeometry()
		: GetCachedGeometry();
	const FVector2D Local = MapGeometry.AbsoluteToLocal(ScreenPosition);
	const FVector2D Size = MapGeometry.GetLocalSize();
	int32 MinimumX = 0;
	int32 MinimumY = 0;
	int32 MaximumX = 17;
	int32 MaximumY = 11;
	if (const UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr)
	{
		const AMSim::FTerminalLayoutState& Layout =
			Subsystem->GetSimulation().GetPhase3State().TerminalLayout;
		MinimumX = Layout.MinimumX;
		MinimumY = Layout.MinimumY;
		MaximumX = Layout.MaximumX;
		MaximumY = Layout.MaximumY;
	}
	const float AlphaX = FMath::Clamp(Local.X / FMath::Max(Size.X, 1.0f), 0.0f, 0.999f);
	const float AlphaY = FMath::Clamp(Local.Y / FMath::Max(Size.Y, 1.0f), 0.0f, 0.999f);
	return {
		MinimumX + FMath::FloorToInt(AlphaX * (MaximumX - MinimumX + 1)),
		MinimumY + FMath::FloorToInt(AlphaY * (MaximumY - MinimumY + 1))};
}

void UAMSimTerminalView::CommitTerminalGesture(
	const AMSim::FTerminalCellCoord& Start,
	const AMSim::FTerminalCellCoord& End)
{
	AMSim::FPhase3Command Command;
	Command.StartCell = Start;
	Command.EndCell = End;
	switch (ActiveEditorTool)
	{
	case EAMSimTerminalEditorTool::Floor:
		Command.Type = AMSim::EPhase3CommandType::PlaceTerminalFloor;
		Command.FloorKind = ActiveFloorKind;
		break;
	case EAMSimTerminalEditorTool::Wall:
		Command.Type = AMSim::EPhase3CommandType::PlaceTerminalWall;
		Command.EdgeKind = AMSim::ETerminalEdgeKind::InteriorWall;
		if (FMath::Abs(End.X - Start.X) >= FMath::Abs(End.Y - Start.Y))
		{
			Command.EndCell.Y = Start.Y;
		}
		else
		{
			Command.EndCell.X = Start.X;
		}
		if (Command.StartCell == Command.EndCell)
		{
			Command.EndCell.X += 1;
		}
		break;
	case EAMSimTerminalEditorTool::Door:
		Command.Type = AMSim::EPhase3CommandType::PlaceTerminalDoor;
		Command.EdgeKind = AMSim::ETerminalEdgeKind::StandardDoor;
		Command.EndCell = FMath::Abs(End.X - Start.X) >= FMath::Abs(End.Y - Start.Y)
			? AMSim::FTerminalCellCoord{Start.X + (End.X < Start.X ? -1 : 1), Start.Y}
			: AMSim::FTerminalCellCoord{Start.X, Start.Y + (End.Y < Start.Y ? -1 : 1)};
		break;
	case EAMSimTerminalEditorTool::Seating:
	case EAMSimTerminalEditorTool::Information:
	case EAMSimTerminalEditorTool::Restroom:
	case EAMSimTerminalEditorTool::StaffDesk:
		Command.Type = AMSim::EPhase3CommandType::PlaceTerminalObject;
		Command.ObjectKind = ActiveEditorTool == EAMSimTerminalEditorTool::Seating
			? AMSim::ETerminalObjectKind::SeatGroup4
			: ActiveEditorTool == EAMSimTerminalEditorTool::Information
				? AMSim::ETerminalObjectKind::InformationDesk
				: ActiveEditorTool == EAMSimTerminalEditorTool::Restroom
					? AMSim::ETerminalObjectKind::Restroom
					: AMSim::ETerminalObjectKind::StaffDesk;
		Command.StartCell = End;
		break;
	case EAMSimTerminalEditorTool::Demolish:
	case EAMSimTerminalEditorTool::Rotate:
	{
		const UAMSimAirportSimulationSubsystem* Subsystem =
			GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr;
		if (!Subsystem)
		{
			return;
		}
		const AMSim::FTerminalLayoutState& Layout =
			Subsystem->GetSimulation().GetPhase3State().TerminalLayout;
		const AMSim::FTerminalPlacedObjectRecord* Object = Layout.Objects.FindByPredicate(
			[End](const AMSim::FTerminalPlacedObjectRecord& Entry)
			{
				return End.X >= Entry.Anchor.X &&
					End.X < Entry.Anchor.X + Entry.FootprintWidth &&
					End.Y >= Entry.Anchor.Y &&
					End.Y < Entry.Anchor.Y + Entry.FootprintHeight;
			});
		if (Object)
		{
			Command.TerminalElementId = Object->Id;
		}
		else if (const AMSim::FTerminalFloorCellRecord* Cell = Layout.FloorCells.FindByPredicate(
			[End](const AMSim::FTerminalFloorCellRecord& Entry) { return Entry.Cell == End; }))
		{
			Command.TerminalElementId = Cell->Id;
		}
		if (!Command.TerminalElementId.IsValid())
		{
			if (InteractionText) InteractionText->SetText(FText::FromString(TEXT("Nothing selectable in this cell.")));
			return;
		}
		Command.Type = ActiveEditorTool == EAMSimTerminalEditorTool::Demolish
			? AMSim::EPhase3CommandType::DemolishTerminalElement
			: AMSim::EPhase3CommandType::RotateTerminalObject;
		break;
	}
	}
	Submit(Command, TEXT("Terminal edit committed; workers are moving to the site."));
}

void UAMSimTerminalView::ShowBuildMode()
{
	bBuildMode = true;
	ViewState.Revision = MAX_uint64;
	if (TerminalBuildToolsPanel) TerminalBuildToolsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (TerminalOperationsToolsPanel) TerminalOperationsToolsPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (TerminalOperationsInspector) TerminalOperationsInspector->SetVisibility(ESlateVisibility::Collapsed);
	if (PlanningCard) PlanningCard->SetVisibility(ESlateVisibility::Collapsed);
	if (InteractionText) InteractionText->SetText(FText::FromString(TEXT("FLOOR  \u00b7  drag on the terminal grid  \u00b7  right-drag to pan")));
	const TCHAR* BuildLegend[] = {
		bCompactLayout ? TEXT("FLOOR  ·  25CR") : TEXT("FLOOR  ·  25 CR / CELL"), TEXT("CYAN  ·  VALID"),
		TEXT("RIGHT-DRAG  ·  PAN"), TEXT("ESC  ·  BACK")};
	for (int32 Index = 0; Index < TerminalLegendTexts.Num() && Index < UE_ARRAY_COUNT(BuildLegend); ++Index)
	{
		TerminalLegendTexts[Index]->SetText(FText::FromString(BuildLegend[Index]));
		TerminalLegendTexts[Index]->SetVisibility(
			bCompactLayout ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	RefreshFromSimulation();
}

void UAMSimTerminalView::ShowOperationsMode()
{
	bBuildMode = false;
	if (BrandText)
	{
		BrandText->SetText(FText::FromString(
			bCompactLayout ? TEXT("RIVERBEND  ·  DOMESTIC TERMINAL") :
				TEXT("RIVERBEND AIRPORT\nDOMESTIC TERMINAL")));
	}
	const TCHAR* OperationsLegend[] = {
		TEXT("GREEN  PASSENGER FLOW"), TEXT("CYAN  SECURE AREA"),
		TEXT("AMBER  CONGESTION"), TEXT("DASHED  ACCESSIBLE ROUTE")};
	for (int32 Index = 0; Index < TerminalLegendTexts.Num() && Index < UE_ARRAY_COUNT(OperationsLegend); ++Index)
	{
		TerminalLegendTexts[Index]->SetText(FText::FromString(OperationsLegend[Index]));
		TerminalLegendTexts[Index]->SetVisibility(ESlateVisibility::Visible);
	}
	if (TerminalBuildToolsPanel) TerminalBuildToolsPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (TerminalOperationsToolsPanel) TerminalOperationsToolsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (TerminalOperationsInspector) TerminalOperationsInspector->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RefreshFromSimulation();
}

void UAMSimTerminalView::SelectEditorTool(
	const EAMSimTerminalEditorTool Tool,
	const FString& Label)
{
	ActiveEditorTool = Tool;
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(FString::Printf(
			TEXT("%s  \u00b7  drag or click on the grid  \u00b7  release to purchase"), *Label)));
	}
}

void UAMSimTerminalView::SelectFloorTool() { SelectEditorTool(EAMSimTerminalEditorTool::Floor, TEXT("FLOOR")); }
void UAMSimTerminalView::CycleFloorFunction()
{
	switch (ActiveFloorKind)
	{
	case AMSim::ETerminalFloorKind::Public:
		ActiveFloorKind = AMSim::ETerminalFloorKind::StaffService;
		SelectEditorTool(EAMSimTerminalEditorTool::Floor, TEXT("FLOOR: STAFF / SERVICE"));
		break;
	case AMSim::ETerminalFloorKind::StaffService:
		ActiveFloorKind = AMSim::ETerminalFloorKind::Restroom;
		SelectEditorTool(EAMSimTerminalEditorTool::Floor, TEXT("FLOOR: RESTROOM"));
		break;
	case AMSim::ETerminalFloorKind::Restroom:
		ActiveFloorKind = AMSim::ETerminalFloorKind::EntranceThreshold;
		SelectEditorTool(EAMSimTerminalEditorTool::Floor, TEXT("FLOOR: ENTRANCE"));
		break;
	default:
		ActiveFloorKind = AMSim::ETerminalFloorKind::Public;
		SelectEditorTool(EAMSimTerminalEditorTool::Floor, TEXT("FLOOR: PUBLIC"));
		break;
	}
}
void UAMSimTerminalView::SelectWallTool() { SelectEditorTool(EAMSimTerminalEditorTool::Wall, TEXT("WALL")); }
void UAMSimTerminalView::SelectDoorTool() { SelectEditorTool(EAMSimTerminalEditorTool::Door, TEXT("DOOR")); }
void UAMSimTerminalView::SelectSeatingTool() { SelectEditorTool(EAMSimTerminalEditorTool::Seating, TEXT("SEATING")); }
void UAMSimTerminalView::SelectInformationTool() { SelectEditorTool(EAMSimTerminalEditorTool::Information, TEXT("INFORMATION")); }
void UAMSimTerminalView::SelectRestroomTool() { SelectEditorTool(EAMSimTerminalEditorTool::Restroom, TEXT("RESTROOM")); }
void UAMSimTerminalView::SelectStaffDeskTool() { SelectEditorTool(EAMSimTerminalEditorTool::StaffDesk, TEXT("STAFF DESK")); }
void UAMSimTerminalView::SelectDemolishTool() { SelectEditorTool(EAMSimTerminalEditorTool::Demolish, TEXT("DEMOLISH")); }
void UAMSimTerminalView::SelectRotateTool() { SelectEditorTool(EAMSimTerminalEditorTool::Rotate, TEXT("ROTATE")); }

void UAMSimTerminalView::UndoTerminalEdit()
{
	AMSim::FPhase3Command Command;
	Command.Type = AMSim::EPhase3CommandType::UndoTerminalEdit;
	Submit(Command, TEXT("Queued terminal edit undone and fully refunded."));
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
	ShowAdvancedOperations();
}

void UAMSimTerminalView::OpenMajorOperations()
{
	ShowMajorOperations();
}

void UAMSimTerminalView::ShowRegionalOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Regional;
	SetTerminalCutawayEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowRegionalOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowAdvancedOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Advanced;
	SetTerminalCutawayEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowAdvancedOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowMajorOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Major;
	SetTerminalCutawayEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowMajorOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowPresentation()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Terminal;
	// Refresh the loaded layout before hiding the exterior world. A save can
	// legitimately carry the same raw Phase 3 revision as the prior state.
	// Entering cutaway with the previous cached layout produced a blank map.
	RefreshFromSimulation();
	SetTerminalCutawayEnabled(true);
	SetKeyboardFocus();
}

void UAMSimTerminalView::SetTerminalCutawayEnabled(const bool bEnabled)
{
	FVector TerminalCenter = FVector(-27000.0f, 36000.0f, 40.0f);
	float TerminalOrthoWidth = 2000.0f;
	if (const UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld() ? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>() : nullptr)
	{
		const AMSim::FTerminalLayoutState& Layout =
			Subsystem->GetSimulation().GetPhase3State().TerminalLayout;
		const float HorizontalMeters =
			static_cast<float>(Layout.MaximumX - Layout.MinimumX + 1);
		const float VerticalMeters =
			static_cast<float>(Layout.MaximumY - Layout.MinimumY + 1);
		if (HorizontalMeters > 18.0f || VerticalMeters > 12.0f)
		{
			TerminalOrthoWidth = FMath::Max(
				2000.0f,
				FMath::Max(HorizontalMeters * 112.0f, VerticalMeters * 190.0f));
		}
	}
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetTerminalCutawayMode(bEnabled);
		TerminalCenter = It->GetTerminalWorldCenter();
	}
	for (TActorIterator<AAMSimCameraPawn> It(GetWorld()); It; ++It)
	{
		It->SetTerminalCutawayMode(
			bEnabled,
			bEnabled ? TerminalCenter : FVector::ZeroVector,
			TerminalOrthoWidth);
	}
}

void UAMSimTerminalView::ClosePresentation()
{
	bPresentationOpen = false;
	PresentationDestination = EAMSimTerminalPresentationDestination::Terminal;
	SetTerminalCutawayEnabled(false);
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
	const AMSim::FPhase1State& Phase1 =
		Subsystem->GetSimulation().GetPhase1State();
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
	if (PresentationDestination !=
		EAMSimTerminalPresentationDestination::Terminal)
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
	if (!Phase1.bInitialized)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RestoreTerminalShellVisibility();
	UpdateWorldPresentation(Query, State);
	const bool bStarterGATerminal =
		!Query.bInitialized && State.TerminalLayout.bReady;
	const bool bWasStarterGATerminal = bStarterGAShellActive;
	bStarterGAShellActive = bStarterGATerminal;
	if (bStarterGATerminal)
	{
		if (BrandText)
		{
			BrandText->SetText(FText::FromString(
				bCompactLayout ? TEXT("RIVERBEND  ·  STARTER GA TERMINAL") :
					TEXT("RIVERBEND AIRPORT\nSTARTER GA TERMINAL")));
		}
		if (StatusText)
		{
			StatusText->SetText(FText::FromString(
				TEXT("STARTER TERMINAL  ·  READY")));
		}
		if (ConstructionText)
		{
			ConstructionText->SetText(FText::FromString(FString::Printf(
				TEXT("FURNISHED  ·  %d AIRSIDE GATES  ·  EDITABLE"),
				State.TerminalLayout.ValidAirsideGateCount)));
		}
		if (!bBuildMode)
		{
			if (TerminalOperationsToolsPanel)
			{
				TerminalOperationsToolsPanel->SetVisibility(
					ESlateVisibility::Collapsed);
			}
			if (TerminalOperationsInspector)
			{
				TerminalOperationsInspector->SetVisibility(
					ESlateVisibility::Collapsed);
			}
			if (InteractionText)
			{
				InteractionText->SetText(FText::FromString(
					TEXT("GA visitors use this terminal automatically. Select BUILD to remodel it.")));
			}
			const TCHAR* StarterLegend[] = {
				TEXT("GREEN  GA VISITOR ROUTE"), TEXT("CYAN  AIRSIDE ACCESS"),
				TEXT("AMBER  LOCAL CLOSURE"), TEXT("DASHED  ACCESSIBLE ROUTE")};
			for (int32 Index = 0;
				Index < TerminalLegendTexts.Num() &&
				Index < UE_ARRAY_COUNT(StarterLegend);
				++Index)
			{
				TerminalLegendTexts[Index]->SetText(
					FText::FromString(StarterLegend[Index]));
			}
		}
	}
	else
	{
		if (BrandText)
		{
			BrandText->SetText(FText::FromString(
				bCompactLayout ? TEXT("RIVERBEND  ·  DOMESTIC TERMINAL") :
					TEXT("RIVERBEND AIRPORT\nDOMESTIC TERMINAL")));
		}
		if (!bBuildMode)
		{
			const TCHAR* PassengerLegend[] = {
				TEXT("GREEN  PASSENGER FLOW"), TEXT("CYAN  SECURE AREA"),
				TEXT("AMBER  CONGESTION"), TEXT("DASHED  ACCESSIBLE ROUTE")};
			for (int32 Index = 0;
				Index < TerminalLegendTexts.Num() &&
				Index < UE_ARRAY_COUNT(PassengerLegend);
				++Index)
			{
				TerminalLegendTexts[Index]->SetText(
					FText::FromString(PassengerLegend[Index]));
			}
			if (bWasStarterGATerminal && InteractionText)
			{
				InteractionText->SetText(
					FText::FromString(TEXT("Passenger terminal controls ready.")));
			}
		}
	}
	if (ViewState.Revision == Query.Revision)
	{
		return;
	}
	ViewState = AMSim::MakePhase3ViewState(Query, State);
	if (!bStarterGATerminal)
	{
		StatusText->SetText(FText::FromString(ViewState.Status));
		ConstructionText->SetText(FText::FromString(ViewState.Construction));
	}
	if (bBuildMode && bStarterGATerminal)
	{
		if (BrandText)
		{
			BrandText->SetText(FText::FromString(
				bCompactLayout ? TEXT("RIVERBEND  ·  STARTER GA TERMINAL") :
					TEXT("RIVERBEND AIRPORT\nSTARTER GA TERMINAL")));
		}
		StatusText->SetText(FText::FromString(TEXT("STARTER TERMINAL  ·  READY")));
		ConstructionText->SetText(FText::FromString(FString::Printf(
			TEXT("FURNISHED  ·  %d AIRSIDE GATES  ·  EDITABLE"),
			State.TerminalLayout.ValidAirsideGateCount)));
	}
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
		bBuildMode || ViewState.bShowTerminal || !Query.bUnlocked
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible);
	TerminalLabels->SetVisibility(
		bBuildMode
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	// The legacy labels are screen-fixed and visibly drift when the camera pans
	// or zooms. Retain this canvas only as transparent build-gesture geometry
	// until labels are derived from terminal cells in world space.
	TerminalLabels->SetRenderOpacity(0.0f);
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

void UAMSimTerminalView::RestoreTerminalShellVisibility()
{
	if (!TerminalCanvas)
	{
		return;
	}
	for (int32 Index = 0; Index < TerminalCanvas->GetChildrenCount(); ++Index)
	{
		UWidget* Child = TerminalCanvas->GetChildAt(Index);
		if (Child == RegionalOperationsView)
		{
			Child->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (Child != TerminalLabels && Child != PlanningCard)
		{
			Child->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
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
