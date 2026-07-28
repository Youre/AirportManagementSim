#include "AMSimOverviewView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

namespace AMSimOverviewPrivate
{
	UTextBlock* Text(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Result =
			Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Result->SetText(FText::FromString(Value));
		Result->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Result, Size, Color, bBold, true);
		return Result;
	}

	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Kind,
		const FMargin Padding = FMargin(12.0f))
	{
		UBorder* Result =
			Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		AMSim::UITheme::StyleSurface(Result, Kind, Padding, 16.0f, 1.5f);
		return Result;
	}

	void AddVertical(UVerticalBox* Box, UWidget* Widget, const float Padding = 4.0f)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, Padding));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	}

	void Place(UCanvasPanel* Canvas, UWidget* Widget, const FAnchors Anchors)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(FMargin(0.0f));
	}

	UBorder* ActivityCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Heading,
		TObjectPtr<UTextBlock>& Body,
		UTexture2D* Icon,
		const AMSim::UITheme::ESurface Kind = AMSim::UITheme::ESurface::Card)
	{
		UBorder* Card = Surface(Tree, Name, Kind, FMargin(11.0f, 9.0f));
		UVerticalBox* Column = Tree->ConstructWidget<UVerticalBox>();
		UHorizontalBox* HeadingRow = Tree->ConstructWidget<UHorizontalBox>();
		if (Icon)
		{
			USizeBox* IconSize = Tree->ConstructWidget<USizeBox>();
			IconSize->SetWidthOverride(25.0f);
			IconSize->SetHeightOverride(25.0f);
			UImage* IconImage = Tree->ConstructWidget<UImage>();
			IconImage->SetBrushFromTexture(Icon, true);
			IconSize->SetContent(IconImage);
			UHorizontalBoxSlot* IconSlot =
				HeadingRow->AddChildToHorizontalBox(IconSize);
			IconSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		UTextBlock* HeadingText = Text(
			Tree,
			*(FString(Name) + TEXT("Heading")),
			Heading,
			11,
			AMSim::UITheme::Cyan(),
			true);
		UHorizontalBoxSlot* HeadingSlot =
			HeadingRow->AddChildToHorizontalBox(HeadingText);
		HeadingSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		HeadingSlot->SetVerticalAlignment(VAlign_Center);
		AddVertical(Column, HeadingRow, 0.0f);
		Body = Text(
			Tree,
			*(FString(Name) + TEXT("Body")),
			TEXT("Waiting for operations"),
			13,
			AMSim::UITheme::White(),
			true);
		AddVertical(Column, Body, 2.0f);
		Card->SetContent(Column);
		return Card;
	}

	UVerticalBox* Meter(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Label,
		TObjectPtr<UProgressBar>& OutBar,
		const FLinearColor& Fill)
	{
		UVerticalBox* Column = Tree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			FName(Name));
		AddVertical(
			Column,
			Text(
				Tree,
				*(FString(Name) + TEXT("Label")),
				Label,
				9,
				AMSim::UITheme::Muted(),
				true),
			0.0f);
		OutBar = Tree->ConstructWidget<UProgressBar>(
			UProgressBar::StaticClass(),
			*(FString(Name) + TEXT("Bar")));
		OutBar->SetPercent(0.0f);
		OutBar->SetFillColorAndOpacity(Fill);
		AddVertical(Column, OutBar, 1.0f);
		return Column;
	}
}

UAMSimOverviewView::UAMSimOverviewView(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> Build(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Build.T_Build"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Routes(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Routes.T_Routes"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Timetable(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Timetable.T_Timetable"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Staff(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Staff.T_Staff"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Overlays(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Overlays.T_Overlays"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Alerts(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Alerts.T_Alerts"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Flights(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Flights.T_Flights"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Projects(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Projects.T_Projects"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Services(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Services.T_Services"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> SelectedAircraft(
		TEXT("/Game/Phase1/Presentation/Textures/Aircraft/T_Cessna152_Heading_08.T_Cessna152_Heading_08"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> SelectedFacility(
		TEXT("/Game/Phase45/Presentation/Textures/Site/T_RegionalTerminal.T_RegionalTerminal"));

	ToolIcons = {
		Build.Object,
		Routes.Object,
		Timetable.Object,
		Staff.Object,
		Overlays.Object};
	ActivityIcons = {
		Alerts.Object,
		Flights.Object,
		Projects.Object,
		Services.Object};
	SelectedAircraftTexture = SelectedAircraft.Object;
	SelectedFacilityTexture = SelectedFacility.Object;
}

int32 UAMSimOverviewView::GetLoadedIconCount() const
{
	int32 Count = 0;
	for (const UTexture2D* Icon : ToolIcons)
	{
		Count += Icon ? 1 : 0;
	}
	for (const UTexture2D* Icon : ActivityIcons)
	{
		Count += Icon ? 1 : 0;
	}
	return Count;
}

bool UAMSimOverviewView::HasRequiredIconKit() const
{
	return ToolIcons.Num() == 5 &&
		ActivityIcons.Num() == 4 &&
		GetLoadedIconCount() == 9;
}

bool UAMSimOverviewView::HasRequiredIdentityArt() const
{
	return SelectedAircraftTexture != nullptr &&
		SelectedFacilityTexture != nullptr;
}

TSharedRef<SWidget> UAMSimOverviewView::RebuildWidget()
{
	using namespace AMSimOverviewPrivate;
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("OverviewTree"));
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("OverviewCanvas"));
	WidgetTree->RootWidget = Root;

	UBorder* Tools = Surface(
		WidgetTree,
		TEXT("OverviewTools"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(13.0f));
	UVerticalBox* ToolColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Tools->SetContent(ToolColumn);
	AddVertical(
		ToolColumn,
		Text(
			WidgetTree,
			TEXT("OverviewToolsTitle"),
			TEXT("AIRPORT TOOLS"),
			17,
			AMSim::UITheme::White(),
			true),
		0.0f);
	AddVertical(
		ToolColumn,
		Text(
			WidgetTree,
			TEXT("OverviewToolsHint"),
			TEXT("WORLD VIEW  •  ALL OPERATIONS"),
			11,
			AMSim::UITheme::Cyan(),
			true),
		1.0f);
	int32 ToolIndex = 0;
	for (const TPair<const TCHAR*, const TCHAR*>& Tool : {
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverviewAirfield"), TEXT("AIRFIELD  •  BUILD")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverviewRoutes"), TEXT("ZONES  •  ROUTES")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverviewSchedule"), TEXT("FLIGHTS  •  TIMETABLE")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverviewStaff"), TEXT("TEAMS  •  STAFF")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("OverviewLayers"), TEXT("WORLD  •  OVERLAYS"))})
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			FName(Tool.Key));
		Button->SetStyle(AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Tool));
		UHorizontalBox* ToolRow =
			WidgetTree->ConstructWidget<UHorizontalBox>();
		if (ToolIcons.IsValidIndex(ToolIndex) && ToolIcons[ToolIndex])
		{
			USizeBox* IconSize = WidgetTree->ConstructWidget<USizeBox>();
			IconSize->SetWidthOverride(38.0f);
			IconSize->SetHeightOverride(38.0f);
			UImage* Icon = WidgetTree->ConstructWidget<UImage>();
			Icon->SetBrushFromTexture(ToolIcons[ToolIndex], true);
			IconSize->SetContent(Icon);
			UHorizontalBoxSlot* IconSlot =
				ToolRow->AddChildToHorizontalBox(IconSize);
			IconSlot->SetPadding(FMargin(4.0f, 2.0f, 10.0f, 2.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		UTextBlock* Label = Text(
			WidgetTree,
			*(FString(Tool.Key) + TEXT("Label")),
			Tool.Value,
			12,
			AMSim::UITheme::White(),
			true);
		UHorizontalBoxSlot* LabelSlot =
			ToolRow->AddChildToHorizontalBox(Label);
		LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		LabelSlot->SetVerticalAlignment(VAlign_Center);
		Button->SetContent(ToolRow);
		AddVertical(ToolColumn, Button, 4.0f);
		++ToolIndex;
	}
	Place(Root, Tools, FAnchors(0.01f, 0.02f, 0.205f, 0.82f));

	UBorder* Activity = Surface(
		WidgetTree,
		TEXT("OverviewActivity"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(13.0f));
	UVerticalBox* ActivityColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Activity->SetContent(ActivityColumn);
	AddVertical(
		ActivityColumn,
		Text(
			WidgetTree,
			TEXT("OverviewActivityTitle"),
			TEXT("LIVE ACTIVITY"),
			17,
			AMSim::UITheme::White(),
			true),
		0.0f);
	AddVertical(
		ActivityColumn,
		ActivityCard(
			WidgetTree,
			TEXT("OverviewAlerts"),
			TEXT("ALERTS & WEATHER"),
			AlertText,
			ActivityIcons.IsValidIndex(0) ? ActivityIcons[0] : nullptr,
			AMSim::UITheme::ESurface::Warning),
		5.0f);
	AddVertical(
		ActivityColumn,
		ActivityCard(
			WidgetTree,
			TEXT("OverviewFlights"),
			TEXT("ACTIVE FLIGHTS"),
			FlightsText,
			ActivityIcons.IsValidIndex(1) ? ActivityIcons[1] : nullptr),
		5.0f);
	AddVertical(
		ActivityColumn,
		ActivityCard(
			WidgetTree,
			TEXT("OverviewProjects"),
			TEXT("PROJECTS"),
			ProjectsText,
			ActivityIcons.IsValidIndex(2) ? ActivityIcons[2] : nullptr),
		5.0f);
	AddVertical(
		ActivityColumn,
		ActivityCard(
			WidgetTree,
			TEXT("OverviewGround"),
			TEXT("GROUND FLOW"),
			GroundText,
			ActivityIcons.IsValidIndex(3) ? ActivityIcons[3] : nullptr),
		5.0f);
	Place(Root, Activity, FAnchors(0.795f, 0.02f, 0.99f, 0.82f));

	UBorder* Inspector = Surface(
		WidgetTree,
		TEXT("OverviewInspector"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(15.0f, 12.0f));
	UVerticalBox* InspectorColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Inspector->SetContent(InspectorColumn);
	InspectorTitleText = Text(
		WidgetTree,
		TEXT("OverviewInspectorTitle"),
		TEXT("SELECTED FLIGHT  •  OPERATIONAL READINESS"),
		12,
		AMSim::UITheme::Cyan(),
		true);
	AddVertical(InspectorColumn, InspectorTitleText, 0.0f);
	UHorizontalBox* SelectionRow =
		WidgetTree->ConstructWidget<UHorizontalBox>();
	AddVertical(InspectorColumn, SelectionRow, 2.0f);
	AircraftSelectionAction = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("OverviewSelectAircraft"));
	AircraftSelectionAction->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Primary));
	AircraftSelectionAction->SetContent(Text(
		WidgetTree,
		TEXT("OverviewSelectAircraftLabel"),
		TEXT("AIRCRAFT"),
		9,
		AMSim::UITheme::White(),
		true));
	AircraftSelectionAction->OnClicked.AddDynamic(
		this,
		&UAMSimOverviewView::SelectAircraft);
	UHorizontalBoxSlot* AircraftSelectionSlot =
		SelectionRow->AddChildToHorizontalBox(AircraftSelectionAction);
	AircraftSelectionSlot->SetSize(
		FSlateChildSize(ESlateSizeRule::Fill));
	AircraftSelectionSlot->SetPadding(
		FMargin(0.0f, 0.0f, 7.0f, 0.0f));
	FacilitySelectionAction = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("OverviewSelectFacility"));
	FacilitySelectionAction->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Secondary));
	FacilitySelectionAction->SetContent(Text(
		WidgetTree,
		TEXT("OverviewSelectFacilityLabel"),
		TEXT("REGIONAL TERMINAL"),
		9,
		AMSim::UITheme::White(),
		true));
	FacilitySelectionAction->OnClicked.AddDynamic(
		this,
		&UAMSimOverviewView::SelectFacility);
	UHorizontalBoxSlot* FacilitySelectionSlot =
		SelectionRow->AddChildToHorizontalBox(FacilitySelectionAction);
	FacilitySelectionSlot->SetSize(
		FSlateChildSize(ESlateSizeRule::Fill));
	UHorizontalBox* InspectorRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	AddVertical(InspectorColumn, InspectorRow, 3.0f);
	if (SelectedAircraftTexture)
	{
		USizeBox* AircraftSize = WidgetTree->ConstructWidget<USizeBox>();
		AircraftSize->SetWidthOverride(135.0f);
		AircraftSize->SetHeightOverride(112.0f);
		SelectedIdentityImage = WidgetTree->ConstructWidget<UImage>();
		SelectedIdentityImage->SetBrushFromTexture(
			SelectedAircraftTexture,
			true);
		AircraftSize->SetContent(SelectedIdentityImage);
		UHorizontalBoxSlot* AircraftSlot =
			InspectorRow->AddChildToHorizontalBox(AircraftSize);
		AircraftSlot->SetPadding(FMargin(0.0f, 0.0f, 13.0f, 0.0f));
		AircraftSlot->SetVerticalAlignment(VAlign_Center);
	}
	UVerticalBox* IdentityColumn =
		WidgetTree->ConstructWidget<UVerticalBox>();
	SelectedText = Text(
		WidgetTree,
		TEXT("OverviewSelected"),
		TEXT("NO FLIGHT SELECTED"),
		15,
		AMSim::UITheme::White(),
		true);
	AddVertical(IdentityColumn, SelectedText, 2.0f);
	SelectedDetailText = Text(
		WidgetTree,
		TEXT("OverviewSelectedDetail"),
		TEXT("Schedule and stand"),
		12,
		AMSim::UITheme::Muted(),
		true);
	AddVertical(IdentityColumn, SelectedDetailText, 5.0f);
	CapacityText = Text(
		WidgetTree,
		TEXT("OverviewCapacity"),
		TEXT("PASSENGER, BORDER, AND TRANSPORT STATUS"),
		11,
		AMSim::UITheme::Cyan(),
		true);
	AddVertical(IdentityColumn, CapacityText, 0.0f);
	UHorizontalBoxSlot* IdentitySlot =
		InspectorRow->AddChildToHorizontalBox(IdentityColumn);
	FSlateChildSize IdentitySize(ESlateSizeRule::Fill);
	IdentitySize.Value = 0.44f;
	IdentitySlot->SetSize(IdentitySize);
	IdentitySlot->SetPadding(FMargin(0.0f, 0.0f, 13.0f, 0.0f));

	UVerticalBox* ReadinessColumn =
		WidgetTree->ConstructWidget<UVerticalBox>();
	AddVertical(
		ReadinessColumn,
		Text(
			WidgetTree,
			TEXT("OverviewReadinessTitle"),
			TEXT("READINESS & SERVICES"),
			11,
			AMSim::UITheme::Cyan(),
			true),
		1.0f);
	ReadinessText = Text(
		WidgetTree,
		TEXT("OverviewReadiness"),
		TEXT("READINESS UNKNOWN"),
		12,
		AMSim::UITheme::Green(),
		true);
	AddVertical(ReadinessColumn, ReadinessText, 3.0f);
	ServiceText = Text(
		WidgetTree,
		TEXT("OverviewServiceEvidence"),
		TEXT("CONNECTIONS AND BAGS"),
		11,
		AMSim::UITheme::White(),
		true);
	AddVertical(ReadinessColumn, ServiceText, 0.0f);
	UHorizontalBoxSlot* ReadinessSlot =
		InspectorRow->AddChildToHorizontalBox(ReadinessColumn);
	FSlateChildSize ReadinessSize(ESlateSizeRule::Fill);
	ReadinessSize.Value = 0.56f;
	ReadinessSlot->SetSize(ReadinessSize);

	UHorizontalBox* MeterRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	AddVertical(InspectorColumn, MeterRow, 1.0f);
	for (UVerticalBox* MeterWidget : {
		Meter(
			WidgetTree,
			TEXT("OverviewReadinessMeter"),
			TEXT("SCHEDULE READINESS"),
			ReadinessMeter,
			AMSim::UITheme::Green()),
		Meter(
			WidgetTree,
			TEXT("OverviewConnectionMeter"),
			TEXT("CONNECTION FLOW"),
			ConnectionMeter,
			AMSim::UITheme::Cyan()),
		Meter(
			WidgetTree,
			TEXT("OverviewBagMeter"),
			TEXT("TRANSFER BAGS"),
			BagMeter,
			AMSim::UITheme::Amber())})
	{
		UHorizontalBoxSlot* MeterSlot =
			MeterRow->AddChildToHorizontalBox(MeterWidget);
		MeterSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		MeterSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
	}

	UHorizontalBox* ActionRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	AddVertical(InspectorColumn, ActionRow, 1.0f);
	TimetableAction = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("OverviewOpenTimetable"));
	TimetableAction->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Primary));
	TimetableAction->SetContent(Text(
		WidgetTree,
		TEXT("OverviewOpenTimetableLabel"),
		TEXT("OPEN TIMETABLE"),
		10,
		AMSim::UITheme::White(),
		true));
	TimetableAction->OnClicked.AddDynamic(
		this,
		&UAMSimOverviewView::OpenTimetable);
	UHorizontalBoxSlot* TimetableSlot =
		ActionRow->AddChildToHorizontalBox(TimetableAction);
	TimetableSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TimetableSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));

	CapabilitiesAction = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("OverviewOpenCapabilities"));
	CapabilitiesAction->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Secondary));
	CapabilitiesAction->SetContent(Text(
		WidgetTree,
		TEXT("OverviewOpenCapabilitiesLabel"),
		TEXT("REVIEW CAPABILITIES"),
		10,
		AMSim::UITheme::White(),
		true));
	CapabilitiesAction->OnClicked.AddDynamic(
		this,
		&UAMSimOverviewView::OpenCapabilities);
	UHorizontalBoxSlot* CapabilitiesSlot =
		ActionRow->AddChildToHorizontalBox(CapabilitiesAction);
	CapabilitiesSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	Place(Root, Inspector, FAnchors(0.20f, 0.69f, 0.80f, 0.97f));

	return Root->TakeWidget();
}

void UAMSimOverviewView::OpenTimetable()
{
	OnOpenTimetable.ExecuteIfBound();
}

void UAMSimOverviewView::OpenCapabilities()
{
	OnOpenCapabilities.ExecuteIfBound();
}

void UAMSimOverviewView::SetFacilitySelected(const bool bSelected)
{
	bFacilitySelected = bSelected;
	RefreshInspector(LastQuery);
	if (bFacilitySelected)
	{
		OnSelectFacility.ExecuteIfBound();
	}
	else
	{
		OnSelectAircraft.ExecuteIfBound();
	}
}

void UAMSimOverviewView::SelectAircraft()
{
	SetFacilitySelected(false);
}

void UAMSimOverviewView::SelectFacility()
{
	SetFacilitySelected(true);
}

void UAMSimOverviewView::RefreshInspector(
	const AMSim::FPhase4QuerySnapshot& Query)
{
	const auto Set = [](UTextBlock* Widget, const FString& Value)
	{
		if (Widget)
		{
			Widget->SetText(FText::FromString(Value));
		}
	};
	if (SelectedIdentityImage)
	{
		SelectedIdentityImage->SetBrushFromTexture(
			bFacilitySelected
				? SelectedFacilityTexture
				: SelectedAircraftTexture,
			true);
	}
	if (AircraftSelectionAction)
	{
		AircraftSelectionAction->SetStyle(
			AMSim::UITheme::ButtonStyle(
				bFacilitySelected
					? AMSim::UITheme::EButton::Secondary
					: AMSim::UITheme::EButton::Primary));
	}
	if (FacilitySelectionAction)
	{
		FacilitySelectionAction->SetStyle(
			AMSim::UITheme::ButtonStyle(
				bFacilitySelected
					? AMSim::UITheme::EButton::Primary
					: AMSim::UITheme::EButton::Secondary));
	}

	if (bFacilitySelected)
	{
		Set(
			InspectorTitleText,
			TEXT("SELECTED FACILITY  •  CAPACITY & SERVICES"));
		Set(SelectedText, TEXT("RIVERBEND REGIONAL TERMINAL"));
		Set(SelectedDetailText, Query.BorderSummary);
		Set(
			CapacityText,
			FString::Printf(
				TEXT("%d INTERNATIONAL PAX  •  %d BORDER PROCESSED"),
				Query.InternationalPassengerCount,
				Query.BorderProcessedPassengerCount));
		Set(
			ReadinessText,
			Query.bTimetablePublished
				? TEXT("OPEN  •  TIMETABLE PUBLISHED")
				: TEXT("OPEN  •  SCHEDULE REVIEW"));
		Set(ServiceText, Query.TransportSummary);
	}
	else
	{
		Set(
			InspectorTitleText,
			TEXT("SELECTED FLIGHT  •  OPERATIONAL READINESS"));
		Set(SelectedText, Query.SelectedFlight);
		Set(SelectedDetailText, Query.SelectedFlightDetail);
		Set(
			CapacityText,
			FString::Printf(
				TEXT("%s\n%s"),
				*Query.BorderSummary,
				*Query.TransportSummary));
		Set(ReadinessText, Query.FeasibilitySummary);
		Set(ServiceText, Query.ConnectionSummary);
	}

	const float FlightDenominator =
		static_cast<float>(FMath::Max(1, Query.FlightCount));
	if (ReadinessMeter)
	{
		ReadinessMeter->SetPercent(FMath::Clamp(
			1.0f -
				static_cast<float>(
					Query.LateFlightCount +
					Query.WeatherRestrictedFlightCount) /
					FlightDenominator,
			0.0f,
			1.0f));
	}
	if (ConnectionMeter)
	{
		const int32 ConnectionTotal =
			Query.ConnectedPassengerCount +
			Query.MissedConnectionPassengerCount;
		ConnectionMeter->SetPercent(
			ConnectionTotal > 0
				? static_cast<float>(Query.ConnectedPassengerCount) /
					static_cast<float>(ConnectionTotal)
				: 0.0f);
	}
	if (BagMeter)
	{
		BagMeter->SetPercent(
			Query.TransferBagCount > 0
				? static_cast<float>(Query.CompletedTransferBagCount) /
					static_cast<float>(Query.TransferBagCount)
				: 0.0f);
	}
}

void UAMSimOverviewView::RefreshFromSimulation()
{
	if (!GetWorld())
	{
		return;
	}
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>();
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase4QuerySnapshot Query = Subsystem->GetPhase4Query();
	if (Query.Revision == LastRevision)
	{
		return;
	}
	LastRevision = Query.Revision;
	LastQuery = Query;
	const auto Set = [](UTextBlock* Widget, const FString& Value)
	{
		if (Widget)
		{
			Widget->SetText(FText::FromString(Value));
		}
	};
	Set(
		AlertText,
		FString::Printf(
			TEXT("%s\n%d WEATHER-RESTRICTED FLIGHTS"),
			*Query.WeatherSummary,
			Query.WeatherRestrictedFlightCount));
	Set(
		FlightsText,
		FString::Printf(
			TEXT("%d OF %d COMPLETE\n%s"),
			Query.CompletedFlightCount,
			Query.FlightCount,
			*Query.PrimaryStatus));
	Set(
		ProjectsText,
		TEXT("REGIONAL FACILITIES  ✓\nTERMINAL + BORDER ROUTES  ✓"));
	Set(
		GroundText,
		FString::Printf(
			TEXT("%d CONNECTED PASSENGERS\n%d / %d TRANSFER BAGS"),
			Query.ConnectedPassengerCount,
			Query.CompletedTransferBagCount,
			Query.TransferBagCount));
	RefreshInspector(Query);
}
