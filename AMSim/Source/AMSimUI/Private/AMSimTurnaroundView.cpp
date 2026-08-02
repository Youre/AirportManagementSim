#include "AMSimTurnaroundView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
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

namespace AMSimTurnaroundPrivate
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
		Result->SetJustification(ETextJustify::Center);
		AMSim::UITheme::StyleText(Result, Size, Color, bBold, true);
		return Result;
	}

	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Kind,
		const FMargin Padding = FMargin(11.0f, 8.0f))
	{
		UBorder* Result =
			Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		AMSim::UITheme::StyleSurface(Result, Kind, Padding, 14.0f, 1.5f);
		return Result;
	}

	void AddVertical(UVerticalBox* Box, UWidget* Widget, const float Bottom = 4.0f)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, Bottom));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	}

	void AddHorizontal(
		UHorizontalBox* Box,
		UWidget* Widget,
		const float Weight,
		const float Right = 6.0f)
	{
		UHorizontalBoxSlot* Slot = Box->AddChildToHorizontalBox(Widget);
		FSlateChildSize Size(ESlateSizeRule::Fill);
		Size.Value = Weight;
		Slot->SetSize(Size);
		Slot->SetPadding(FMargin(0.0f, 0.0f, Right, 0.0f));
		Slot->SetVerticalAlignment(VAlign_Fill);
	}

	FString TaskState(const AMSim::EServiceTaskState State)
	{
		switch (State)
		{
		case AMSim::EServiceTaskState::Waiting: return TEXT("WAITING");
		case AMSim::EServiceTaskState::Active: return TEXT("ACTIVE");
		case AMSim::EServiceTaskState::Completed: return TEXT("COMPLETE");
		case AMSim::EServiceTaskState::Unavailable:
		default: return TEXT("NOT REQUIRED");
		}
	}

	AMSim::UITheme::ESurface TaskSurface(const AMSim::EServiceTaskState State)
	{
		switch (State)
		{
		case AMSim::EServiceTaskState::Completed:
			return AMSim::UITheme::ESurface::Positive;
		case AMSim::EServiceTaskState::Active:
		case AMSim::EServiceTaskState::Waiting:
			return AMSim::UITheme::ESurface::Warning;
		case AMSim::EServiceTaskState::Unavailable:
		default:
			return AMSim::UITheme::ESurface::Card;
		}
	}

	UProgressBar* Progress(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FLinearColor& Fill)
	{
		UProgressBar* Result = Tree->ConstructWidget<UProgressBar>(
			UProgressBar::StaticClass(),
			FName(Name));
		Result->SetFillColorAndOpacity(Fill);
		Result->SetPercent(0.0f);
		return Result;
	}

	FString Clock(const int64 GameTimeMilliseconds)
	{
		const int64 TotalMinutes =
			FMath::Max<int64>(0, GameTimeMilliseconds) / 60000;
		return FString::Printf(
			TEXT("D%lld %02lld:%02lld"),
			1 + TotalMinutes / (24 * 60),
			(TotalMinutes / 60) % 24,
			TotalMinutes % 60);
	}

	float TaskProgress(
		const AMSim::EServiceTaskState State,
		const float ActiveProgress)
	{
		switch (State)
		{
		case AMSim::EServiceTaskState::Completed: return 1.0f;
		case AMSim::EServiceTaskState::Active: return ActiveProgress;
		case AMSim::EServiceTaskState::Waiting: return 0.05f;
		case AMSim::EServiceTaskState::Unavailable:
		default: return 0.0f;
		}
	}
}

UAMSimTurnaroundView::UAMSimTurnaroundView(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinderOptional<UTexture2D> AircraftFinder(
		TEXT("/Game/Phase1/Presentation/Textures/Aircraft/T_Cessna152_Heading_08.T_Cessna152_Heading_08"));
	AircraftIdentityTexture = AircraftFinder.Get();
}

TSharedRef<SWidget> UAMSimTurnaroundView::RebuildWidget()
{
	using namespace AMSimTurnaroundPrivate;
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("TurnaroundTree"));
	UBorder* Root = Surface(
		WidgetTree,
		TEXT("TurnaroundRoot"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(15.0f, 11.0f));
	UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>();
	Root->SetContent(Layout);
	AddVertical(
		Layout,
		Text(
			WidgetTree,
			TEXT("TurnaroundTitle"),
			TEXT("ARRIVAL  >  INSPECTION + FUELING IN PARALLEL  >  READY  >  DEPARTURE"),
			13,
			AMSim::UITheme::Cyan(),
			true),
		2.0f);
	UHorizontalBox* Timeline = WidgetTree->ConstructWidget<UHorizontalBox>();
	AddVertical(Layout, Timeline, 5.0f);

	UBorder* Identity = Surface(
		WidgetTree,
		TEXT("TurnaroundIdentity"),
		AMSim::UITheme::ESurface::Field,
		FMargin(8.0f));
	UHorizontalBox* IdentityRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	Identity->SetContent(IdentityRow);
	USizeBox* IdentityArtBox = WidgetTree->ConstructWidget<USizeBox>();
	IdentityArtBox->SetWidthOverride(70.0f);
	IdentityArtBox->SetHeightOverride(52.0f);
	UImage* IdentityArt = WidgetTree->ConstructWidget<UImage>(
		UImage::StaticClass(),
		TEXT("TurnaroundAircraftArt"));
	IdentityArt->SetBrushFromTexture(AircraftIdentityTexture, true);
	IdentityArtBox->SetContent(IdentityArt);
	AddHorizontal(IdentityRow, IdentityArtBox, 0.55f);
	IdentityText = Text(
		WidgetTree,
		TEXT("TurnaroundIdentityText"),
		TEXT("RIVERBEND FLYING CLUB / RB-021\nRIVERBEND 21 / LIGHT PISTON / 4 PLACE\nSTAND A1 / SCHEDULED"),
		10,
		AMSim::UITheme::White(),
		true);
	AddHorizontal(IdentityRow, IdentityText, 1.45f, 0.0f);
	AddHorizontal(Timeline, Identity, 1.45f);

	UBorder* Arrival = Surface(
		WidgetTree,
		TEXT("TurnaroundArrival"),
		AMSim::UITheme::ESurface::Positive);
	Arrival->SetContent(
		Text(
			WidgetTree,
			TEXT("TurnaroundArrivalText"),
			TEXT("1  ARRIVAL\nCOMPLETE"),
			11,
			AMSim::UITheme::White(),
			true));
	AddHorizontal(Timeline, Arrival, 0.85f);

	InspectionCard = Surface(
		WidgetTree,
		TEXT("TurnaroundInspection"),
		AMSim::UITheme::ESurface::Warning);
	UVerticalBox* InspectionColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	InspectionCard->SetContent(InspectionColumn);
	InspectionText = Text(
		WidgetTree,
		TEXT("TurnaroundInspectionText"),
		TEXT("2A  INSPECTION\nACTIVE / 0%"),
		11,
		AMSim::UITheme::White(),
		true);
	AddVertical(InspectionColumn, InspectionText, 3.0f);
	InspectionProgress = Progress(
		WidgetTree,
		TEXT("TurnaroundInspectionProgress"),
		AMSim::UITheme::Green());
	AddVertical(InspectionColumn, InspectionProgress, 0.0f);
	AddHorizontal(Timeline, InspectionCard, 1.0f);

	FuelCard = Surface(
		WidgetTree,
		TEXT("TurnaroundFuel"),
		AMSim::UITheme::ESurface::Warning);
	UVerticalBox* FuelColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	FuelCard->SetContent(FuelColumn);
	FuelText = Text(
		WidgetTree,
		TEXT("TurnaroundFuelText"),
		TEXT("2B  FUELING\nACTIVE / 0%"),
		11,
		AMSim::UITheme::White(),
		true);
	AddVertical(FuelColumn, FuelText, 3.0f);
	FuelProgress = Progress(
		WidgetTree,
		TEXT("TurnaroundFuelProgress"),
		AMSim::UITheme::Cyan());
	AddVertical(FuelColumn, FuelProgress, 0.0f);
	AddHorizontal(Timeline, FuelCard, 1.0f);

	ReadyCard = Surface(
		WidgetTree,
		TEXT("TurnaroundReady"),
		AMSim::UITheme::ESurface::Card);
	UVerticalBox* ReadyColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	ReadyCard->SetContent(ReadyColumn);
	ReadyText = Text(
		WidgetTree,
		TEXT("TurnaroundReadyText"),
		TEXT("3  READY\nBLOCKED BY 2A + 2B"),
		11,
		AMSim::UITheme::White(),
		true);
	AddVertical(ReadyColumn, ReadyText, 3.0f);
	ReadyProgress = Progress(
		WidgetTree,
		TEXT("TurnaroundReadyProgress"),
		AMSim::UITheme::Amber());
	AddVertical(ReadyColumn, ReadyProgress, 0.0f);
	AddHorizontal(Timeline, ReadyCard, 1.1f);

	UBorder* Departure = Surface(
		WidgetTree,
		TEXT("TurnaroundDeparture"),
		AMSim::UITheme::ESurface::Card);
	Departure->SetContent(
		Text(
			WidgetTree,
			TEXT("TurnaroundDepartureText"),
			TEXT("4  DEPARTURE\nQUEUED"),
			11,
			AMSim::UITheme::White(),
			true));
	AddHorizontal(Timeline, Departure, 0.85f, 0.0f);

	PredictionText = Text(
		WidgetTree,
		TEXT("TurnaroundPrediction"),
		TEXT("AUTO-DISPATCHED  /  MARKED SAFE PATHS  /  PLAYER SETS PRIORITY, NOT STEERING  /  READY IN <1 MIN"),
		10,
		AMSim::UITheme::White(),
		true);
	UBorder* DispatchCard = Surface(
		WidgetTree,
		TEXT("TurnaroundDispatchPolicy"),
		AMSim::UITheme::ESurface::Positive,
		FMargin(10.0f, 5.0f));
	DispatchCard->SetContent(PredictionText);
	AddVertical(Layout, DispatchCard, 0.0f);
	WidgetTree->RootWidget = Root;
	SetVisibility(ESlateVisibility::Collapsed);
	return Root->TakeWidget();
}

void UAMSimTurnaroundView::RefreshFromSimulation()
{
	using namespace AMSimTurnaroundPrivate;
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
	const AMSim::FPhase1QuerySnapshot Query = Subsystem->GetPhase1Query();
	SetVisibility(
		Query.FlightState == AMSim::EFlightState::Turnaround
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	if (!InspectionText ||
		!FuelText ||
		!ReadyText ||
		!PredictionText ||
		!IdentityText ||
		!InspectionProgress ||
		!FuelProgress ||
		!ReadyProgress)
	{
		return;
	}
	if (Query.Revision == LastRevision)
	{
		return;
	}
	LastRevision = Query.Revision;

	const AMSim::FPhase1State& State =
		Subsystem->GetSimulation().GetPhase1State();
	const AMSim::FPhase1Fixture& Fixture = AMSim::GetPhase1Fixture();
	const int64 TurnaroundStart =
		State.Flight.ScheduledArrivalGameMilliseconds +
			Fixture.FlightTurnaroundOffsetMilliseconds;
	const int64 ReadyAt =
		State.Flight.ScheduledArrivalGameMilliseconds +
			Fixture.FlightReadyOffsetMilliseconds;
	const float ActiveProgress = FMath::Clamp(
		static_cast<float>(
			Query.GameTimeMilliseconds - TurnaroundStart) /
			static_cast<float>(ReadyAt - TurnaroundStart),
		0.0f,
		1.0f);
	const float InspectionValue =
		TaskProgress(Query.InspectionState, ActiveProgress);
	const float FuelValue =
		TaskProgress(Query.FuelingState, ActiveProgress);
	InspectionProgress->SetPercent(InspectionValue);
	FuelProgress->SetPercent(FuelValue);

	InspectionText->SetText(FText::FromString(FString::Printf(
		TEXT("2A  INSPECTION\n%s / %d%%"),
		*TaskState(Query.InspectionState),
		FMath::RoundToInt(InspectionValue * 100.0f))));
	FuelText->SetText(FText::FromString(FString::Printf(
		TEXT("2B  FUELING\n%s / %d%%"),
		*TaskState(Query.FuelingState),
		FMath::RoundToInt(FuelValue * 100.0f))));
	AMSim::UITheme::StyleSurface(
		InspectionCard,
		TaskSurface(Query.InspectionState),
		FMargin(11.0f, 8.0f),
		14.0f,
		1.5f);
	AMSim::UITheme::StyleSurface(
		FuelCard,
		TaskSurface(Query.FuelingState),
		FMargin(11.0f, 8.0f),
		14.0f,
		1.5f);

	const bool bReady =
		Query.InspectionState == AMSim::EServiceTaskState::Completed &&
		Query.FuelingState == AMSim::EServiceTaskState::Completed;
	ReadyText->SetText(FText::FromString(
		bReady
			? TEXT("3  READY\nCOMPLETE")
			: TEXT("3  READY\nBLOCKED BY 2A + 2B")));
	ReadyProgress->SetPercent(
		bReady ? 1.0f : FMath::Min(InspectionValue, FuelValue));
	AMSim::UITheme::StyleSurface(
		ReadyCard,
		bReady
			? AMSim::UITheme::ESurface::Positive
			: AMSim::UITheme::ESurface::Card,
		FMargin(11.0f, 8.0f),
		14.0f,
		1.5f);

	const FString Tail = State.Airframe.TailNumber.IsEmpty()
		? TEXT("RB-021")
		: State.Airframe.TailNumber;
	IdentityText->SetText(FText::FromString(FString::Printf(
		TEXT("RIVERBEND FLYING CLUB / %s\nRIVERBEND 21 / LIGHT PISTON / 4 PLACE\nSTAND A1 / ARR %s / READY %s"),
		*Tail,
		*Clock(State.Flight.ScheduledArrivalGameMilliseconds),
		*Clock(ReadyAt))));

	const int64 RemainingGameMinutes = FMath::Max<int64>(
		0,
		(ReadyAt - Query.GameTimeMilliseconds + 59999) / 60000);
	PredictionText->SetText(FText::FromString(FString::Printf(
		TEXT("AUTO-DISPATCHED  /  INSPECTION + FUEL  /  NO PLAYER ACTION REQUIRED  /  READY IN %lld GAME MIN"),
		RemainingGameMinutes)));
}
