#include "AMSimPhase6View.h"

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

namespace AMSimPhase6ViewPrivate
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
		UTextBlock* LabelText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			10,
			White(),
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Button->SetContent(LabelText);
		return Button;
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

	void AddAnchored(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors Anchors,
		const int32 ZOrder)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(FMargin(0.0f));
		Slot->SetZOrder(ZOrder);
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

TSharedRef<SWidget> UAMSimPhase6View::RebuildWidget()
{
	using namespace AMSimPhase6ViewPrivate;
	using namespace AMSim::UITheme;
	if (!WidgetTree)
	{
		return Super::RebuildWidget();
	}

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("Phase6Canvas"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* Header = MakeSurface(
		WidgetTree,
		TEXT("Phase6Header"),
		ESurface::Chrome,
		FMargin(18.0f, 9.0f),
		0.0f);
	UHorizontalBox* HeaderRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("Phase6HeaderRow"));
	Header->SetContent(HeaderRow);
	AddHorizontal(
		HeaderRow,
		MakeText(
			WidgetTree,
			TEXT("Phase6Brand"),
			TEXT("RIVERBEND  /  MAJOR AIRPORT"),
			15,
			White(),
			true),
		16.0f);
	StatusText = MakeText(
		WidgetTree,
		TEXT("Phase6Status"),
		TEXT("Major capability planning"),
		10,
		Muted());
	AddHorizontal(HeaderRow, StatusText, 8.0f, true);
	InitializeButton = MakeButton(
		WidgetTree,
		TEXT("Phase6Initialize"),
		TEXT("OPEN PHASE 6"),
		EButton::Primary);
	InitializeButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase6View::InitializeMajor);
	AddHorizontal(HeaderRow, InitializeButton, 0.0f);
	AddAnchored(
		RootCanvas,
		Header,
		FAnchors(0.0f, 0.0f, 1.0f, 0.075f),
		10);

	UBorder* Tabs = MakeSurface(
		WidgetTree,
		TEXT("Phase6Tabs"),
		ESurface::Panel,
		FMargin(8.0f),
		14.0f);
	UHorizontalBox* TabRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("Phase6TabRow"));
	Tabs->SetContent(TabRow);
	struct FTabSpec
	{
		const TCHAR* Name;
		const TCHAR* Label;
		FName HandlerName;
	};
	const FTabSpec TabSpecs[] = {
		{TEXT("RunwayTab"), TEXT("RUNWAYS"), TEXT("ShowRunways")},
		{TEXT("CapacityTab"), TEXT("CAPACITY"), TEXT("ShowCapacity")},
		{TEXT("AircraftTab"), TEXT("RL 602"), TEXT("ShowAircraft")},
		{TEXT("IncidentTab"), TEXT("INCIDENT"), TEXT("ShowIncident")},
		{TEXT("MajorTab"), TEXT("MAJOR"), TEXT("ShowProgression")}};
	for (const FTabSpec& Spec : TabSpecs)
	{
		UButton* Button = MakeButton(
			WidgetTree,
			Spec.Name,
			Spec.Label,
			EButton::Tool);
		FScriptDelegate Delegate;
		Delegate.BindUFunction(this, Spec.HandlerName);
		Button->OnClicked.Add(Delegate);
		AddHorizontal(TabRow, Button, 5.0f, true);
	}
	AddAnchored(
		RootCanvas,
		Tabs,
		FAnchors(0.21f, 0.085f, 0.80f, 0.145f),
		11);

	UBorder* SummaryRail = MakeSurface(
		WidgetTree,
		TEXT("Phase6SummaryRail"),
		ESurface::Panel,
		FMargin(13.0f),
		16.0f);
	UVerticalBox* SummaryColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("Phase6SummaryColumn"));
	SummaryRail->SetContent(SummaryColumn);
	AddVertical(
		SummaryColumn,
		MakeText(
			WidgetTree,
			TEXT("Phase6SummaryEyebrow"),
			TEXT("OPERATIONS BOARD"),
			9,
			Cyan(),
			true),
		6.0f);
	SummaryText = MakeText(
		WidgetTree,
		TEXT("Phase6Summary"),
		TEXT("Major capability locked"),
		12,
		White(),
		true);
	AddVertical(SummaryColumn, SummaryText, 12.0f);
	PathHeadlineText = MakeText(
		WidgetTree,
		TEXT("Phase6PathHeadline"),
		TEXT("GENERAL AVIATION"),
		10,
		Amber(),
		true);
	AddVertical(SummaryColumn, PathHeadlineText, 5.0f);
	PathEvidenceText = MakeText(
		WidgetTree,
		TEXT("Phase6PathEvidence"),
		TEXT("No evidence yet"),
		10,
		White());
	AddVertical(SummaryColumn, PathEvidenceText, 10.0f, true);
	UButton* NextPathButton = MakeButton(
		WidgetTree,
		TEXT("Phase6NextPath"),
		TEXT("NEXT PATH"),
		EButton::Secondary);
	NextPathButton->OnClicked.AddDynamic(
		this,
		&UAMSimPhase6View::NextPath);
	AddVertical(SummaryColumn, NextPathButton, 0.0f);
	AddAnchored(
		RootCanvas,
		SummaryRail,
		FAnchors(0.012f, 0.09f, 0.198f, 0.975f),
		8);

	auto BuildPanel = [this](
		const TCHAR* Name,
		const TCHAR* Eyebrow,
		TObjectPtr<UVerticalBox>& CardBox)
	{
		using namespace AMSimPhase6ViewPrivate;
		using namespace AMSim::UITheme;
		UBorder* Panel = MakeSurface(
			WidgetTree,
			Name,
			ESurface::Panel,
			FMargin(14.0f),
			18.0f);
		UVerticalBox* Column =
			WidgetTree->ConstructWidget<UVerticalBox>(
				UVerticalBox::StaticClass(),
				*FString::Printf(TEXT("%sColumn"), Name));
		Panel->SetContent(Column);
		AddVertical(
			Column,
			MakeText(
				WidgetTree,
				*FString::Printf(TEXT("%sTitle"), Name),
				Eyebrow,
				10,
				Cyan(),
				true),
			9.0f);
		UScrollBox* Scroll =
			WidgetTree->ConstructWidget<UScrollBox>(
				UScrollBox::StaticClass(),
				*FString::Printf(TEXT("%sScroll"), Name));
		CardBox = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			*FString::Printf(TEXT("%sCards"), Name));
		Scroll->AddChild(CardBox);
		AddVertical(Column, Scroll, 0.0f, true);
		Panels.Add(Panel);
		AddAnchored(
			RootCanvas,
			Panel,
			FAnchors(0.21f, 0.16f, 0.79f, 0.975f),
			7);
	};
	BuildPanel(
		TEXT("Phase6RunwayPanel"),
		TEXT("PARALLEL RUNWAY CONTROL"),
		RunwayCards);
	BuildPanel(
		TEXT("Phase6CapacityPanel"),
		TEXT("MAJOR CAPACITY PROGRAM"),
		FacilityCards);
	BuildPanel(
		TEXT("Phase6AircraftPanel"),
		TEXT("LARGE-AIRCRAFT TURNAROUND"),
		AircraftCards);
	BuildPanel(
		TEXT("Phase6IncidentPanel"),
		TEXT("SERIOUS INCIDENT & RECOVERY"),
		IncidentCards);
	BuildPanel(
		TEXT("Phase6ProgressionPanel"),
		TEXT("SIX INDEPENDENT MAJOR PATHS"),
		CapabilityCards);

	UBorder* ActionRail = MakeSurface(
		WidgetTree,
		TEXT("Phase6ActionRail"),
		ESurface::Panel,
		FMargin(12.0f),
		16.0f);
	UVerticalBox* ActionColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("Phase6ActionColumn"));
	ActionRail->SetContent(ActionColumn);
	AddVertical(
		ActionColumn,
		MakeText(
			WidgetTree,
			TEXT("Phase6ActionEyebrow"),
			TEXT("CONTEXT ACTIONS"),
			9,
			Cyan(),
			true),
		8.0f);
	auto AddAction = [this, ActionColumn](
		const TCHAR* Name,
		const TCHAR* Label,
		TObjectPtr<UButton>& Target,
		const FName HandlerName)
	{
		using namespace AMSimPhase6ViewPrivate;
		using namespace AMSim::UITheme;
		Target = MakeButton(
			WidgetTree,
			Name,
			Label,
			EButton::Primary);
		FScriptDelegate Delegate;
		Delegate.BindUFunction(this, HandlerName);
		Target->OnClicked.Add(Delegate);
		AddVertical(ActionColumn, Target, 7.0f);
	};
	AddAction(
		TEXT("Phase6Construct"),
		TEXT("BUILD NEXT FACILITY"),
		ConstructButton,
		TEXT("ConstructNextFacility"));
	AddAction(
		TEXT("Phase6ActivateParallel"),
		TEXT("ACTIVATE PARALLEL"),
		ActivateParallelButton,
		TEXT("ActivateParallelRunways"));
	AddAction(
		TEXT("Phase6AcceptAircraft"),
		TEXT("ACCEPT RL 602"),
		AcceptAircraftButton,
		TEXT("AcceptLargeAircraft"));
	AddAction(
		TEXT("Phase6StartTurn"),
		TEXT("START TURNAROUND"),
		StartTurnaroundButton,
		TEXT("StartLargeAircraftTurnaround"));
	AddAction(
		TEXT("Phase6AdvanceAircraft"),
		TEXT("ADVANCE AIRCRAFT"),
		AdvanceAircraftButton,
		TEXT("AdvanceLargeAircraft"));
	AddAction(
		TEXT("Phase6RecordEvidence"),
		TEXT("COMPLETE EVIDENCE"),
		RecordEvidenceButton,
		TEXT("RecordNextEvidence"));
	AddAction(
		TEXT("Phase6AdvanceDay"),
		TEXT("CLOSE OPERATING DAY"),
		AdvanceDayButton,
		TEXT("AdvanceOperatingDay"));
	AddAction(
		TEXT("Phase6ResolveConflict"),
		TEXT("RECOVER CONFLICT"),
		ResolveConflictButton,
		TEXT("ResolveSharedConflict"));
	AddAction(
		TEXT("Phase6IncidentAction"),
		TEXT("REVIEW RISK"),
		IncidentButton,
		TEXT("AdvanceIncident"));
	InteractionText = MakeText(
		WidgetTree,
		TEXT("Phase6Interaction"),
		TEXT("Select a surface to review cause, consequence, and next action."),
		9,
		Muted());
	AddVertical(ActionColumn, InteractionText, 0.0f, true);
	AddAnchored(
		RootCanvas,
		ActionRail,
		FAnchors(0.802f, 0.09f, 0.988f, 0.975f),
		9);

	ShowPanel(0);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimPhase6View::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

AMSim::EPhase6CommandResult UAMSimPhase6View::Submit(
	AMSim::FPhase6Command Command,
	const FString& SuccessMessage)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return AMSim::EPhase6CommandResult::RejectedInvalidState;
	}
	const AMSim::EPhase6CommandResult Result =
		Subsystem->SubmitPhase6Command(Command);
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			Result == AMSim::EPhase6CommandResult::Accepted
				? SuccessMessage
				: Result ==
						AMSim::EPhase6CommandResult::AcceptedWithWarning
					? TEXT("WARNED RISK · click again to confirm the disclosed consequence.")
					: TEXT("Action is not ready. Review the current cause and remedy.")));
	}
	RefreshFromSimulation();
	return Result;
}

void UAMSimPhase6View::InitializeMajor()
{
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::InitializeMajorCapability;
	Submit(Command, TEXT("Major capability planning opened."));
}

void UAMSimPhase6View::ConstructNextFacility()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FMajorFacilityRecord* Facility =
		Subsystem->GetSimulation().GetPhase6State().Facilities.FindByPredicate(
			[](const AMSim::FMajorFacilityRecord& Candidate)
			{
				return Candidate.State ==
					AMSim::EMajorFacilityState::Proposed;
			});
	if (!Facility)
	{
		return;
	}
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::ConstructFacility;
	Command.FacilityId = Facility->Id;
	Submit(
		Command,
		FString::Printf(
			TEXT("%s funded."),
			*Facility->DisplayName));
}

void UAMSimPhase6View::ActivateParallelRunways()
{
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::ActivateParallelRunways;
	Submit(Command, TEXT("Parallel runway configuration activated."));
}

void UAMSimPhase6View::AcceptLargeAircraft()
{
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::AcceptLargeAircraftOffer;
	Submit(Command, TEXT("RL 602 accepted with compatibility verified."));
}

void UAMSimPhase6View::StartLargeAircraftTurnaround()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FLargeAircraftOperationRecord* Operation =
		Subsystem->GetSimulation().GetPhase6State()
			.LargeAircraftOperations.FindByPredicate(
				[](const AMSim::FLargeAircraftOperationRecord& Candidate)
				{
					return Candidate.State ==
						AMSim::ELargeAircraftOperationState::Arrived;
				});
	if (!Operation)
	{
		return;
	}
	AMSim::FPhase6Command Command;
	Command.Type =
		AMSim::EPhase6CommandType::StartLargeAircraftTurnaround;
	Command.LargeAircraftOperationId = Operation->Id;
	Submit(Command, TEXT("Automatic dispatch started for RL 602."));
}

void UAMSimPhase6View::AdvanceLargeAircraft()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FLargeAircraftOperationRecord* Operation =
		Subsystem->GetSimulation().GetPhase6State()
			.LargeAircraftOperations.FindByPredicate(
				[](const AMSim::FLargeAircraftOperationRecord& Candidate)
				{
					return Candidate.State ==
							AMSim::ELargeAircraftOperationState::Turnaround ||
						Candidate.State ==
							AMSim::ELargeAircraftOperationState::DepartureReady ||
						Candidate.State ==
							AMSim::ELargeAircraftOperationState::Departed;
				});
	if (!Operation)
	{
		return;
	}
	AMSim::FPhase6Command Command;
	Command.Type =
		AMSim::EPhase6CommandType::AdvanceLargeAircraftOperation;
	Command.LargeAircraftOperationId = Operation->Id;
	Submit(Command, TEXT("RL 602 operation advanced."));
}

void UAMSimPhase6View::RecordNextEvidence()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase6PathMetricRecord* Metric =
		Subsystem->GetSimulation().GetPhase6State().PathMetrics
			.FindByPredicate(
				[this](const AMSim::FPhase6PathMetricRecord& Candidate)
				{
					return Candidate.Path == SelectedPath;
				});
	if (!Metric)
	{
		return;
	}
	FName Kind;
	switch (SelectedPath)
	{
	case AMSim::ESpecializationPath::GeneralAviation:
		Kind = Metric->DistinctRolesOrClasses < 5
			? TEXT("Role.GA")
			: !Metric->bFlyInCompleted
				? TEXT("Event.GAFlyIn")
				: TEXT("Movement.GA");
		break;
	case AMSim::ESpecializationPath::FlightSchool:
		Kind = Metric->DistinctRolesOrClasses < 4
			? TEXT("AircraftRole.Trainer")
			: !Metric->bOpenDayCompleted
				? TEXT("Event.FlightSchoolOpenDay")
				: TEXT("Training.Block");
		break;
	case AMSim::ESpecializationPath::Charter:
		Kind = Metric->ShortNoticeOrVipMovements < 4
			? TEXT("Movement.Charter.ShortNoticeVip")
			: TEXT("Movement.Charter");
		break;
	case AMSim::ESpecializationPath::Cargo:
		Kind = Metric->DedicatedFreighterMovements < 12
			? TEXT("Movement.Cargo.DedicatedFreighter")
			: Metric->NightOperations < 6
				? TEXT("Movement.Cargo.Night")
				: TEXT("Shipment.Cargo");
		break;
	case AMSim::ESpecializationPath::Passenger:
		Kind = Metric->CompletedPassengers < 500
			? TEXT("Journey.Passenger")
			: TEXT("Flight.Passenger");
		break;
	case AMSim::ESpecializationPath::Mixed:
		Kind = TEXT("Shared.ResourceDay");
		break;
	}
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::RecordMajorOperation;
	Command.Path = SelectedPath;
	Command.OperationKind = Kind;
	Submit(Command, TEXT("Major-path evidence recorded."));
}

void UAMSimPhase6View::AdvanceOperatingDay()
{
	AMSim::FPhase6Command Command;
	Command.Type = AMSim::EPhase6CommandType::AdvanceMajorOperatingDay;
	Submit(Command, TEXT("Qualifying-day evidence evaluated."));
}

void UAMSimPhase6View::ResolveSharedConflict()
{
	AMSim::FPhase6Command Command;
	Command.Type =
		AMSim::EPhase6CommandType::ResolveSharedResourceConflict;
	Submit(Command, TEXT("Shared-resource conflict recovered."));
}

void UAMSimPhase6View::AdvanceIncident()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase6State& State =
		Subsystem->GetSimulation().GetPhase6State();
	AMSim::FPhase6Command Command;
	switch (State.SeriousIncident.Lifecycle)
	{
	case AMSim::ESeriousIncidentLifecycle::None:
		Command.Type = AMSim::EPhase6CommandType::AcknowledgeSeriousRisk;
		Command.Path = SelectedPath;
		Command.bConfirmWarning = bIncidentWarningReviewed;
		break;
	case AMSim::ESeriousIncidentLifecycle::RiskAcknowledged:
		Command.Type =
			AMSim::EPhase6CommandType::MaterializeSeriousIncident;
		break;
	case AMSim::ESeriousIncidentLifecycle::Materialized:
	case AMSim::ESeriousIncidentLifecycle::Alerted:
		Command.Type =
			AMSim::EPhase6CommandType::DispatchEmergencyResponse;
		break;
	case AMSim::ESeriousIncidentLifecycle::ResourcesDispatched:
		Command.Type = AMSim::EPhase6CommandType::ProtectIncidentArea;
		break;
	case AMSim::ESeriousIncidentLifecycle::AreaProtected:
		Command.Type = AMSim::EPhase6CommandType::StabilizeIncident;
		break;
	case AMSim::ESeriousIncidentLifecycle::Stabilized:
		Command.Type = AMSim::EPhase6CommandType::InvestigateIncident;
		break;
	case AMSim::ESeriousIncidentLifecycle::Investigating:
		Command.Type = State.SeriousIncident.bRecoveryFundingApplied
			? AMSim::EPhase6CommandType::BeginIncidentRepair
			: AMSim::EPhase6CommandType::ApplyIncidentRecovery;
		break;
	case AMSim::ESeriousIncidentLifecycle::Repairing:
		Command.Type = State.RepairProject.bCompleted
			? AMSim::EPhase6CommandType::ReopenIncidentArea
			: AMSim::EPhase6CommandType::CompleteIncidentRepair;
		break;
	default:
		return;
	}
	const AMSim::EPhase6CommandResult Result =
		Submit(Command, TEXT("Incident lifecycle advanced."));
	if (Result == AMSim::EPhase6CommandResult::AcceptedWithWarning)
	{
		bIncidentWarningReviewed = true;
	}
	else if (Result == AMSim::EPhase6CommandResult::Accepted)
	{
		bIncidentWarningReviewed = false;
	}
}

void UAMSimPhase6View::NextPath()
{
	SelectedPath = static_cast<AMSim::ESpecializationPath>(
		(static_cast<int32>(SelectedPath) + 1) % 6);
	ViewState.Revision = TNumericLimits<uint64>::Max();
	RefreshFromSimulation();
}

void UAMSimPhase6View::ShowPanel(const int32 PanelIndex)
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

void UAMSimPhase6View::ShowRunways() { ShowPanel(0); }
void UAMSimPhase6View::ShowCapacity() { ShowPanel(1); }
void UAMSimPhase6View::ShowAircraft() { ShowPanel(2); }
void UAMSimPhase6View::ShowIncident() { ShowPanel(3); }
void UAMSimPhase6View::ShowProgression() { ShowPanel(4); }

void UAMSimPhase6View::RefreshCards(
	UVerticalBox* Box,
	const TArray<FString>& Cards,
	const FLinearColor& Accent)
{
	using namespace AMSimPhase6ViewPrivate;
	using namespace AMSim::UITheme;
	if (!Box)
	{
		return;
	}
	Box->ClearChildren();
	for (int32 Index = 0; Index < Cards.Num(); ++Index)
	{
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("Phase6Card%d"), Index),
			ESurface::Card,
			FMargin(13.0f, 10.0f),
			14.0f);
		Card->SetBrushColor(
			Index == 0
				? FLinearColor(
					Accent.R * 0.28f,
					Accent.G * 0.28f,
					Accent.B * 0.28f,
					0.98f)
				: Card->GetBrushColor());
		Card->SetContent(MakeText(
			WidgetTree,
			*FString::Printf(TEXT("Phase6CardText%d"), Index),
			Cards[Index],
			10,
			White(),
			Index == 0));
		AddVertical(Box, Card, 8.0f);
	}
}

void UAMSimPhase6View::RefreshFromSimulation()
{
	using namespace AMSimPhase6ViewPrivate;
	using namespace AMSim::UITheme;
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase6QuerySnapshot Query =
		Subsystem->GetPhase6Query();
	const AMSim::FPhase6State& State =
		Subsystem->GetSimulation().GetPhase6State();
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->ApplyPhase6Snapshot(Query, State);
		break;
	}
	if (ViewState.Revision == Query.Revision)
	{
		return;
	}
	ViewState = AMSim::MakePhase6ViewState(
		Query,
		State,
		SelectedPath);
	if (StatusText) StatusText->SetText(FText::FromString(ViewState.Status));
	if (SummaryText)
	{
		const FString* Summary = &ViewState.RunwaySummary;
		if (ActivePanel == 1) Summary = &ViewState.CapacitySummary;
		else if (ActivePanel == 2) Summary = &ViewState.LargeAircraftSummary;
		else if (ActivePanel == 3) Summary = &ViewState.IncidentSummary;
		else if (ActivePanel == 4) Summary = &ViewState.ProgressionSummary;
		SummaryText->SetText(FText::FromString(*Summary));
	}
	if (PathHeadlineText)
	{
		PathHeadlineText->SetText(
			FText::FromString(ViewState.SelectedPathHeadline));
	}
	if (PathEvidenceText)
	{
		PathEvidenceText->SetText(
			FText::FromString(ViewState.SelectedPathEvidence));
	}
	if (RecordEvidenceButton)
	{
		if (UTextBlock* Label =
			Cast<UTextBlock>(RecordEvidenceButton->GetContent()))
		{
			Label->SetText(FText::FromString(ViewState.NextEvidenceLabel));
		}
	}
	RefreshCards(RunwayCards, ViewState.RunwayCards, Cyan());
	RefreshCards(FacilityCards, ViewState.FacilityCards, Amber());
	RefreshCards(AircraftCards, ViewState.AircraftCards, CyanSoft());
	RefreshCards(IncidentCards, ViewState.IncidentCards, Coral());
	RefreshCards(CapabilityCards, ViewState.CapabilityCards, Green());
	SetAction(InitializeButton, ViewState.bCanInitialize);
	SetAction(ConstructButton, ViewState.bCanConstruct);
	SetAction(ActivateParallelButton, ViewState.bCanActivateParallel);
	SetAction(AcceptAircraftButton, ViewState.bCanAcceptLargeAircraft);
	SetAction(StartTurnaroundButton, ViewState.bCanStartTurnaround);
	SetAction(AdvanceAircraftButton, ViewState.bCanAdvanceLargeAircraft);
	SetAction(RecordEvidenceButton, Query.bInitialized);
	SetAction(AdvanceDayButton, ViewState.bCanAdvanceDay);
	SetAction(ResolveConflictButton, ViewState.bCanResolveConflict);
	SetAction(IncidentButton, ViewState.bCanAdvanceIncident);
}
