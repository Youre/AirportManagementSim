#include "AMSimPhase5View.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimUITheme.h"
#include "AMSimWorldPresenter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EngineUtils.h"

namespace AMSimPhase5ViewPrivate
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
		Text->SetLineHeightPercentage(0.94f);
		StyleText(Text, Size, Color, bBold, true);
		return Text;
	}

	UBorder* MakeSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const ESurface Surface,
		const FMargin Padding = FMargin(12.0f),
		const float Radius = 16.0f)
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
		const EButton Kind)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			Name);
		Button->SetStyle(ButtonStyle(Kind, 11.0f));
		UTextBlock* Text = MakeText(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			10,
			White(),
			true);
		Text->SetJustification(ETextJustify::Center);
		Button->SetContent(Text);
		return Button;
	}

	UCanvasPanelSlot* AddAnchored(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors Anchors,
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(FMargin(0.0f));
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

	void AddHorizontal(
		UHorizontalBox* Box,
		UWidget* Widget,
		const float Right = 6.0f,
		const bool bFill = false)
	{
		UHorizontalBoxSlot* Slot = Box->AddChildToHorizontalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, 0.0f, Right, 0.0f));
		Slot->SetSize(FSlateChildSize(
			bFill ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic));
		Slot->SetVerticalAlignment(VAlign_Center);
	}

	void SetAction(UButton* Button, const bool bVisible)
	{
		if (!Button)
		{
			return;
		}
		Button->SetVisibility(
			bVisible
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
		Button->SetIsEnabled(bVisible);
	}
}

TSharedRef<SWidget> UAMSimPhase5View::RebuildWidget()
{
	using namespace AMSimPhase5ViewPrivate;
	using namespace AMSim::UITheme;
	if (!WidgetTree)
	{
		return Super::RebuildWidget();
	}

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("Phase5Canvas"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* Header = MakeSurface(
		WidgetTree,
		TEXT("Phase5Header"),
		ESurface::Chrome,
		FMargin(18.0f, 9.0f),
		0.0f);
	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("Phase5HeaderRow"));
	Header->SetContent(HeaderRow);
	UTextBlock* Brand = MakeText(
		WidgetTree,
		TEXT("Phase5Brand"),
		TEXT("RIVERBEND  /  ADVANCED OPERATIONS"),
		15,
		White(),
		true);
	AddHorizontal(HeaderRow, Brand, 14.0f);
	StatusText = MakeText(
		WidgetTree,
		TEXT("Phase5Status"),
		TEXT("Cargo and specialization breadth"),
		10,
		Muted());
	AddHorizontal(HeaderRow, StatusText, 10.0f, true);
	InitializeButton = MakeButton(
		WidgetTree,
		TEXT("Phase5Initialize"),
		TEXT("OPEN PHASE 5"),
		EButton::Primary);
	InitializeButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::InitializeBreadth);
	AddHorizontal(HeaderRow, InitializeButton, 0.0f);
	AddAnchored(
		RootCanvas,
		Header,
		FAnchors(0.0f, 0.0f, 1.0f, 0.075f),
		10);

	UBorder* Tabs = MakeSurface(
		WidgetTree,
		TEXT("Phase5Tabs"),
		ESurface::Panel,
		FMargin(8.0f),
		14.0f);
	UHorizontalBox* TabRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("Phase5TabRow"));
	Tabs->SetContent(TabRow);
	UButton* CargoTab = MakeButton(
		WidgetTree,
		TEXT("CargoTab"),
		TEXT("CARGO"),
		EButton::Primary);
	CargoTab->OnClicked.AddDynamic(this, &UAMSimPhase5View::ShowCargo);
	AddHorizontal(TabRow, CargoTab, 5.0f, true);
	UButton* ProviderTab = MakeButton(
		WidgetTree,
		TEXT("ProviderTab"),
		TEXT("TENANTS"),
		EButton::Tool);
	ProviderTab->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::ShowProviders);
	AddHorizontal(TabRow, ProviderTab, 5.0f, true);
	UButton* EventTab = MakeButton(
		WidgetTree,
		TEXT("EventTab"),
		TEXT("EVENTS"),
		EButton::Tool);
	EventTab->OnClicked.AddDynamic(this, &UAMSimPhase5View::ShowEvents);
	AddHorizontal(TabRow, EventTab, 5.0f, true);
	UButton* CapabilityTab = MakeButton(
		WidgetTree,
		TEXT("CapabilityTab"),
		TEXT("CAPABILITY"),
		EButton::Tool);
	CapabilityTab->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::ShowCapabilities);
	AddHorizontal(TabRow, CapabilityTab, 0.0f, true);
	AddAnchored(
		RootCanvas,
		Tabs,
		FAnchors(0.22f, 0.085f, 0.78f, 0.145f),
		11);

	UBorder* ContractRail = MakeSurface(
		WidgetTree,
		TEXT("CargoContractRail"),
		ESurface::Panel,
		FMargin(11.0f),
		16.0f);
	UVerticalBox* ContractColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("CargoContractColumn"));
	ContractRail->SetContent(ContractColumn);
	AddVertical(
		ContractColumn,
		MakeText(
			WidgetTree,
			TEXT("ContractEyebrow"),
			TEXT("CARGO BOARD"),
			9,
			Cyan(),
			true),
		5.0f);
	SummaryText = MakeText(
		WidgetTree,
		TEXT("CargoSummary"),
		TEXT("No shipments"),
		12,
		White(),
		true);
	AddVertical(ContractColumn, SummaryText, 10.0f);
	UScrollBox* ContractScroll =
		WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("ContractScroll"));
	ContractCards = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ContractCards"));
	ContractScroll->AddChild(ContractCards);
	AddVertical(ContractColumn, ContractScroll, 9.0f, true);
	AcceptCargoButton = MakeButton(
		WidgetTree,
		TEXT("AcceptCargo"),
		TEXT("ACCEPT CONTRACT"),
		EButton::Primary);
	AcceptCargoButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::AcceptCargo);
	AddVertical(ContractColumn, AcceptCargoButton, 0.0f);
	AddAnchored(
		RootCanvas,
		ContractRail,
		FAnchors(0.012f, 0.09f, 0.205f, 0.975f),
		8);

	auto BuildPanel = [this](
		const TCHAR* Name,
		const TCHAR* Eyebrow,
		TObjectPtr<UVerticalBox>& CardBox,
		UButton* Primary,
		UButton* Secondary)
	{
		using namespace AMSimPhase5ViewPrivate;
		using namespace AMSim::UITheme;
		UBorder* Panel = MakeSurface(
			WidgetTree,
			Name,
			ESurface::Panel,
			FMargin(11.0f),
			16.0f);
		UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			*FString::Printf(TEXT("%sColumn"), Name));
		Panel->SetContent(Column);
		AddVertical(
			Column,
			MakeText(
				WidgetTree,
				*FString::Printf(TEXT("%sTitle"), Name),
				Eyebrow,
				9,
				Cyan(),
				true),
			8.0f);
		UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			*FString::Printf(TEXT("%sScroll"), Name));
		CardBox = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			*FString::Printf(TEXT("%sCards"), Name));
		Scroll->AddChild(CardBox);
		AddVertical(Column, Scroll, 9.0f, true);
		if (Primary)
		{
			AddVertical(Column, Primary, Secondary ? 6.0f : 0.0f);
		}
		if (Secondary)
		{
			AddVertical(Column, Secondary, 0.0f);
		}
		Panels.Add(Panel);
		AddAnchored(
			RootCanvas,
			Panel,
			FAnchors(0.795f, 0.09f, 0.988f, 0.975f),
			8);
	};

	AdvanceCargoButton = MakeButton(
		WidgetTree,
		TEXT("AdvanceCargo"),
		TEXT("DISPATCH NEXT"),
		EButton::Positive);
	AdvanceCargoButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::AdvanceCargo);
	BuildPanel(
		TEXT("WarehousePanel"),
		TEXT("WAREHOUSE / CAPACITY"),
		ZoneCards,
		AdvanceCargoButton,
		nullptr);

	AcceptProviderButton = MakeButton(
		WidgetTree,
		TEXT("AcceptProvider"),
		TEXT("ACCEPT OFFER"),
		EButton::Primary);
	AcceptProviderButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::AcceptProvider);
	RecoverProviderButton = MakeButton(
		WidgetTree,
		TEXT("RecoverProvider"),
		TEXT("RESTORE REQUIREMENT"),
		EButton::Positive);
	RecoverProviderButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::RecoverProvider);
	BuildPanel(
		TEXT("ProviderPanel"),
		TEXT("TENANTS / PROVIDERS"),
		ProviderCards,
		AcceptProviderButton,
		RecoverProviderButton);

	AcceptEventButton = MakeButton(
		WidgetTree,
		TEXT("AcceptEvent"),
		TEXT("ACCEPT EVENT"),
		EButton::Primary);
	AcceptEventButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::AcceptEvent);
	AdvanceEventButton = MakeButton(
		WidgetTree,
		TEXT("AdvanceEvent"),
		TEXT("ADVANCE LIFECYCLE"),
		EButton::Positive);
	AdvanceEventButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase5View::AdvanceEvent);
	BuildPanel(
		TEXT("EventPanel"),
		TEXT("SPECIAL EVENTS"),
		EventCards,
		AcceptEventButton,
		AdvanceEventButton);
	BuildPanel(
		TEXT("CapabilityPanel"),
		TEXT("ADVANCED EVIDENCE"),
		CapabilityCards,
		nullptr,
		nullptr);

	UBorder* Inspector = MakeSurface(
		WidgetTree,
		TEXT("CargoInspector"),
		ESurface::RaisedCard,
		FMargin(16.0f, 12.0f),
		18.0f);
	UVerticalBox* InspectorColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("CargoInspectorColumn"));
	Inspector->SetContent(InspectorColumn);
	ShipmentHeadlineText = MakeText(
		WidgetTree,
		TEXT("ShipmentHeadline"),
		TEXT("CARGO OPERATIONS"),
		13,
		White(),
		true);
	AddVertical(InspectorColumn, ShipmentHeadlineText, 4.0f);
	ShipmentDetailText = MakeText(
		WidgetTree,
		TEXT("ShipmentDetail"),
		TEXT("Select a contract"),
		10,
		Muted());
	AddVertical(InspectorColumn, ShipmentDetailText, 6.0f);
	FlowText = MakeText(
		WidgetTree,
		TEXT("CargoFlow"),
		TEXT("ROAD  →  SECURITY  →  STORAGE  →  AIRSIDE"),
		9,
		Cyan(),
		true);
	AddVertical(InspectorColumn, FlowText, 5.0f);
	CauseText = MakeText(
		WidgetTree,
		TEXT("CargoCause"),
		TEXT(""),
		9,
		Amber(),
		true);
	AddVertical(InspectorColumn, CauseText, 3.0f);
	RemedyText = MakeText(
		WidgetTree,
		TEXT("CargoRemedy"),
		TEXT(""),
		9,
		White());
	AddVertical(InspectorColumn, RemedyText, 0.0f);
	AddAnchored(
		RootCanvas,
		Inspector,
		FAnchors(0.22f, 0.79f, 0.78f, 0.97f),
		9);

	SetVisibility(ESlateVisibility::Collapsed);
	ShowPanel(0);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimPhase5View::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

bool UAMSimPhase5View::Submit(
	const AMSim::EPhase5CommandType Type,
	const FString& SuccessMessage)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return false;
	}
	AMSim::FPhase5Command Command;
	Command.Type = Type;
	const bool bAccepted =
		Subsystem->SubmitPhase5Command(Command) ==
			AMSim::EPhase5CommandResult::Accepted;
	if (bAccepted && StatusText)
	{
		StatusText->SetText(FText::FromString(SuccessMessage));
	}
	return bAccepted;
}

void UAMSimPhase5View::InitializeBreadth()
{
	Submit(
		AMSim::EPhase5CommandType::InitializeBreadth,
		TEXT("Cargo, provider, event, and capability systems opened."));
}

void UAMSimPhase5View::AcceptCargo()
{
	Submit(
		AMSim::EPhase5CommandType::AcceptNextCargoContract,
		TEXT("Cargo contract accepted."));
}

void UAMSimPhase5View::AdvanceCargo()
{
	Submit(
		AMSim::EPhase5CommandType::AdvanceNextCargoShipment,
		TEXT("Cargo work entered ordinary dispatch."));
}

void UAMSimPhase5View::AcceptProvider()
{
	Submit(
		AMSim::EPhase5CommandType::AcceptNextProviderTenant,
		TEXT("Provider offer accepted."));
}

void UAMSimPhase5View::RecoverProvider()
{
	Submit(
		AMSim::EPhase5CommandType::RecoverTenant,
		TEXT("Provider requirement restored."));
}

void UAMSimPhase5View::AcceptEvent()
{
	Submit(
		AMSim::EPhase5CommandType::AcceptNextEvent,
		TEXT("Event accepted; preparation opened."));
}

void UAMSimPhase5View::AdvanceEvent()
{
	Submit(
		AMSim::EPhase5CommandType::AdvanceActiveEvent,
		TEXT("Event lifecycle advanced."));
}

void UAMSimPhase5View::ShowCargo()
{
	ShowPanel(0);
}

void UAMSimPhase5View::ShowProviders()
{
	ShowPanel(1);
}

void UAMSimPhase5View::ShowEvents()
{
	ShowPanel(2);
}

void UAMSimPhase5View::ShowCapabilities()
{
	ShowPanel(3);
}

void UAMSimPhase5View::ShowPanel(const int32 PanelIndex)
{
	ActivePanel = FMath::Clamp(PanelIndex, 0, Panels.Num() - 1);
	for (int32 Index = 0; Index < Panels.Num(); ++Index)
	{
		Panels[Index]->SetVisibility(
			Index == ActivePanel
				? ESlateVisibility::SelfHitTestInvisible
				: ESlateVisibility::Collapsed);
	}
}

void UAMSimPhase5View::RefreshCards(
	UVerticalBox* Box,
	const TArray<FString>& Cards,
	const FLinearColor& Accent)
{
	using namespace AMSimPhase5ViewPrivate;
	using namespace AMSim::UITheme;
	if (!Box)
	{
		return;
	}
	Box->ClearChildren();
	const int32 VisibleCount = FMath::Min(Cards.Num(), 8);
	for (int32 Index = 0; Index < VisibleCount; ++Index)
	{
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("Phase5Card%d_%d"), ActivePanel, Index),
			Index == 0 ? ESurface::RaisedCard : ESurface::Card,
			FMargin(10.0f),
			13.0f);
		UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass());
		Card->SetContent(Column);
		UBorder* Marker = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("Phase5Marker%d_%d"), ActivePanel, Index),
			ESurface::Chip,
			FMargin(0.0f),
			4.0f);
		Marker->SetBrushColor(Accent);
		AddVertical(Column, Marker, 5.0f);
		AddVertical(
			Column,
			MakeText(
				WidgetTree,
				*FString::Printf(
					TEXT("Phase5CardText%d_%d"),
					ActivePanel,
					Index),
				Cards[Index],
				9,
				Index == 0 ? White() : Muted(),
				Index == 0),
			0.0f);
		AddVertical(Box, Card, 7.0f);
	}
}

void UAMSimPhase5View::RefreshFromSimulation()
{
	using namespace AMSimPhase5ViewPrivate;
	using namespace AMSim::UITheme;
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase5QuerySnapshot Query = Subsystem->GetPhase5Query();
	if (!Query.bUnlocked && !Query.bInitialized)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const AMSim::FPhase5State& State =
		Subsystem->GetSimulation().GetPhase5State();
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->ApplyPhase5Snapshot(Query, State);
		break;
	}
	if (ViewState.Revision == Query.Revision)
	{
		return;
	}
	ViewState = AMSim::MakePhase5ViewState(Query, State);
	StatusText->SetText(FText::FromString(ViewState.Status));
	SummaryText->SetText(FText::FromString(ViewState.CargoSummary));
	ShipmentHeadlineText->SetText(
		FText::FromString(ViewState.ShipmentHeadline));
	ShipmentDetailText->SetText(
		FText::FromString(ViewState.ShipmentDetail));
	FlowText->SetText(FText::FromString(ViewState.Flow));
	CauseText->SetText(FText::FromString(
		ViewState.Cause.IsEmpty()
			? TEXT("")
			: FString::Printf(TEXT("BLOCKED / %s"), *ViewState.Cause)));
	RemedyText->SetText(FText::FromString(ViewState.Remedy));
	CauseText->SetVisibility(
		ViewState.Cause.IsEmpty()
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible);
	RemedyText->SetVisibility(
		ViewState.Remedy.IsEmpty()
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible);
	RefreshCards(ContractCards, ViewState.ContractCards, Cyan());
	RefreshCards(ZoneCards, ViewState.ZoneCards, Green());
	RefreshCards(ProviderCards, ViewState.ProviderCards, Amber());
	RefreshCards(EventCards, ViewState.EventCards, Coral());
	RefreshCards(CapabilityCards, ViewState.CapabilityCards, CyanSoft());
	SetAction(InitializeButton, ViewState.bCanInitialize);
	SetAction(AcceptCargoButton, ViewState.bCanAcceptCargo);
	SetAction(AdvanceCargoButton, ViewState.bCanAdvanceCargo);
	SetAction(AcceptProviderButton, ViewState.bCanAcceptProvider);
	SetAction(RecoverProviderButton, ViewState.bCanRecover);
	SetAction(AcceptEventButton, ViewState.bCanAcceptEvent);
	SetAction(AdvanceEventButton, ViewState.bCanAdvanceEvent);
	ShowPanel(ActivePanel);
}
