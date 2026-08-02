#include "AMSimCameraPawn.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimExpandingToolButton.h"
#include "AMSimPhase1WorldGeometry.h"
#include "AMSimGameMode.h"
#include "AMSimOverviewView.h"
#include "AMSimPhase1AircraftPresentation.h"
#include "AMSimPhase1ConstructionPresentation.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1HudPresentation.h"
#include "AMSimPhase1ViewState.h"
#include "AMSimPlayerController.h"
#include "AMSimPresentationProxyPool.h"
#include "AMSimProgressionView.h"
#include "AMSimProgressionViewState.h"
#include "AMSimRegionalOperationsView.h"
#include "AMSimRootScreen.h"
#include "AMSimTerminalView.h"
#include "AMSimTimetableGeometry.h"
#include "AMSimTurnaroundView.h"
#include "AMSimUITheme.h"
#include "AMSimWorldPresenter.h"
#include "Camera/CameraComponent.h"
#include "Components/Button.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimOrthographicCameraTest,
	"AMSim.Phase0.Presentation.OrthographicCamera",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimOrthographicCameraTest::RunTest(const FString& Parameters)
{
	const AAMSimCameraPawn* Pawn = GetDefault<AAMSimCameraPawn>();
	const AAMSimGameMode* GameMode = GetDefault<AAMSimGameMode>();
	const AAMSimPlayerController* PlayerController = GetDefault<AAMSimPlayerController>();
	const FUIInputConfig InputConfig = UAMSimRootScreen::MakeGameplayInputConfig();
	TestEqual(TEXT("Camera projection is orthographic"), Pawn->GetCamera()->ProjectionMode, ECameraProjectionMode::Orthographic);
	TestEqual(TEXT("Camera frames the Phase 1.5 parcel"), Pawn->GetCamera()->OrthoWidth, 105000.0f);
	const FVector MousePan = AAMSimCameraPawn::CalculateScreenPanDelta(
		FVector2D(120.0f, -60.0f),
		108000.0f);
	TestTrue(
		TEXT("Middle-drag produces a non-zero world pan"),
		!MousePan.IsNearlyZero());
	TestTrue(
		TEXT("Middle-drag maps screen axes into the top-down world"),
		MousePan.Equals(FVector(-6000.0f, -12000.0f, 0.0f)));
	TestTrue(
		TEXT("Phase 1 timetable exposes multiple selectable increments"),
		AMSim::GetPhase1Fixture().MaximumArrivalDelayMilliseconds >=
			4 * AMSim::GetPhase1Fixture().TimetableIncrementMilliseconds);
	TestFalse(TEXT("Camera pawn has no per-frame tick"), Pawn->PrimaryActorTick.bCanEverTick);
	TestTrue(
		TEXT("Game mode uses the cursor-preserving player controller"),
		GameMode->PlayerControllerClass == AAMSimPlayerController::StaticClass());
	TestTrue(TEXT("Gameplay cursor is visible by default"), PlayerController->bShowMouseCursor);
	TestTrue(TEXT("Gameplay click events remain enabled"), PlayerController->bEnableClickEvents);
	TestTrue(TEXT("Gameplay hover events remain enabled"), PlayerController->bEnableMouseOverEvents);
	TestEqual(TEXT("Root screen accepts game and UI input"), InputConfig.GetInputMode(), ECommonInputMode::All);
	TestEqual(
		TEXT("Root screen captures only while clicking"),
		InputConfig.GetMouseCaptureMode(),
		EMouseCaptureMode::CaptureDuringMouseDown);
	TestEqual(
		TEXT("Root screen does not lock the mouse"),
		InputConfig.GetMouseLockMode(),
		EMouseLockMode::DoNotLock);
	TestFalse(
		TEXT("Root screen does not hide the cursor during capture"),
		InputConfig.HideCursorDuringViewportCapture());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimCompactHudPresentationTest,
	"AMSim.Phase1_5.Presentation.CompactHud",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimCompactHudPresentationTest::RunTest(const FString& Parameters)
{
	const UAMSimRootScreen* RootScreen = GetDefault<UAMSimRootScreen>();
	TestEqual(
		TEXT("Compact navigation loads every cooker-visible icon reference"),
		RootScreen->GetLoadedNavigationIconCount(),
		8);

	UAMSimExpandingToolButton* Tool = NewObject<UAMSimExpandingToolButton>();
	Tool->Configure(nullptr, TEXT("BUILD"), AMSim::UITheme::EButton::Primary);
	Tool->TakeWidget();
	TestNotNull(TEXT("Compact tool exposes a stable action button"), Tool->GetActionButton());
	TestEqual(TEXT("Compact tool retains its accessible label"), Tool->GetActionLabel(), TEXT("BUILD"));
	TestEqual(
		TEXT("Compact tool uses its label as a tooltip"),
		Tool->GetActionButton()->GetToolTipText().ToString(),
		TEXT("BUILD"));
	TestEqual(
		TEXT("Compact tool begins without a flyout"),
		Tool->GetFlyoutVisibilityForTest(),
		ESlateVisibility::Collapsed);
	Tool->GetActionButton()->OnHovered.Broadcast();
	TestEqual(
		TEXT("The actual hover event reveals a hit-test-inert label flyout"),
		Tool->GetFlyoutVisibilityForTest(),
		ESlateVisibility::HitTestInvisible);
	Tool->GetActionButton()->OnUnhovered.Broadcast();
	TestEqual(
		TEXT("The actual unhover event collapses the label flyout"),
		Tool->GetFlyoutVisibilityForTest(),
		ESlateVisibility::Collapsed);
	Tool->SetActionEnabled(false);
	TestFalse(TEXT("Disabled compact tools remain non-interactive"), Tool->IsActionEnabled());
	TestTrue(TEXT("Disabled tools retain an enabled explanatory wrapper"), Tool->GetIsEnabled());
	Tool->SetExpandedForTest(true);
	TestEqual(
		TEXT("Disabled tools can still explain themselves on hover"),
		Tool->GetFlyoutVisibilityForTest(),
		ESlateVisibility::HitTestInvisible);

	const AMSim::FStarterPlanProposal Proposal = AMSim::CreateDefaultStarterPlan();
	const AMSim::FPhase1ConstructionActivityCard Building =
		AMSim::FPhase1HudPresentation::MakeConstructionActivityCard(
			AMSim::EConstructionStage::Building,
			Proposal);
	const AMSim::FPhase1ConstructionActivityCard Inspection =
		AMSim::FPhase1HudPresentation::MakeConstructionActivityCard(
			AMSim::EConstructionStage::Inspection,
			Proposal);
	TestEqual(
		TEXT("Building card presents one concise stage eyebrow"),
		Building.Header,
		TEXT("CONSTRUCTION  •  SURFACE WORK"));
	TestEqual(
		TEXT("Construction detail keeps only runway identity and cost"),
		Building.Detail,
		TEXT("RWY 09/27  •  3,400 CR"));
	TestEqual(
		TEXT("Inspection card presents the explicit safety stage"),
		Inspection.Header,
		TEXT("CONSTRUCTION  •  SAFETY INSPECTION"));
	TestFalse(TEXT("Compact card omits repeated starter title"), Building.Header.Contains(TEXT("STARTER")));
	TestFalse(TEXT("Compact card omits taxi-network prose"), Building.Detail.Contains(TEXT("TAXI")));
	TestFalse(TEXT("Compact card omits gate prose"), Building.Detail.Contains(TEXT("GATE")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase15ComponentLanguageTest,
	"AMSim.Phase1_5.Presentation.ComponentLanguage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase15ComponentLanguageTest::RunTest(const FString& Parameters)
{
	const FSlateBrush Panel =
		AMSim::UITheme::SurfaceBrush(AMSim::UITheme::ESurface::Panel);
	const FButtonStyle Primary =
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Primary);
	const FButtonStyle Positive =
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Positive);

	TestEqual(
		TEXT("Panel surfaces use rounded geometry"),
		Panel.DrawAs,
		ESlateBrushDrawType::RoundedBox);
	TestTrue(
		TEXT("Panel surfaces retain an outline"),
		Panel.OutlineSettings.Width >= 1.0f);
	TestEqual(
		TEXT("Primary buttons use rounded geometry"),
		Primary.Normal.DrawAs,
		ESlateBrushDrawType::RoundedBox);
	TestTrue(
		TEXT("Positive and primary actions have distinct treatments"),
		Positive.Normal.TintColor != Primary.Normal.TintColor);
	TestTrue(
		TEXT("Disabled controls remain visibly distinct"),
		Primary.Disabled.TintColor != Primary.Normal.TintColor);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase15ViewStateTest,
	"AMSim.Phase1_5.Presentation.ViewStateMapping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase15ViewStateTest::RunTest(const FString& Parameters)
{
	AMSim::FPhase1QuerySnapshot Query;
	Query.Revision = 42;
	Query.bInitialized = true;
	Query.AirportName = TEXT("Riverbend Field");
	Query.Credits = 1600;
	Query.AirportPoints = 3;
	Query.CurrentObjective = TEXT("Open the airfield");
	Query.PrimaryStatus = TEXT("Safety inspection");
	Query.ConstructionStage = AMSim::EConstructionStage::Inspection;
	AMSim::FPhase1State State;
	State.SpeedMultiplier = 2;

	const AMSim::FPhase1ViewState View = AMSim::MakePhase1ViewState(Query, State);
	TestEqual(TEXT("Revision is preserved"), View.Revision, Query.Revision);
	TestEqual(TEXT("Airport name uses display casing"), View.AirportName, FString(TEXT("RIVERBEND FIELD")));
	TestTrue(TEXT("Funds include player-facing units"), View.Funds.Contains(TEXT("1600 CR")));
	TestTrue(TEXT("Construction status is mapped"), View.Project.Contains(TEXT("Safety inspection")));
	TestFalse(TEXT("Inspection state cannot be reopened"), View.bCanOpen);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase45ConstructionValidationContentTest,
	"AMSim.Phase4_5.Presentation.ConstructionValidationContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase45ConstructionValidationContentTest::RunTest(
	const FString& Parameters)
{
	const AMSim::FPhase1Validation Valid = AMSim::ValidateStarterPlan(
		UAMSimConstructionProposalView::MakePresentationProposal(false));
	const AMSim::FPhase1Validation Conflict = AMSim::ValidateStarterPlan(
		UAMSimConstructionProposalView::MakePresentationProposal(true));
	TestTrue(
		TEXT("Normal construction presentation uses a valid authoritative proposal"),
		Valid.bValid);
	const AMSim::FStarterPlanProposal Empty =
		UAMSimConstructionProposalView::MakeEmptyProposal();
	TestTrue(TEXT("Empty editor has no runway extent"), Empty.RunwayStart == Empty.RunwayEnd);
	TestTrue(TEXT("Empty editor has no taxiway extent"), Empty.TaxiStart == Empty.TaxiEnd);
	TestFalse(
		TEXT("An empty completion mask cannot confirm construction"),
		UAMSimConstructionProposalView::AreAllRequiredPlacementsComplete(0));
	TestTrue(
		TEXT("Runway and taxiway satisfy the required completion gate"),
		UAMSimConstructionProposalView::AreAllRequiredPlacementsComplete(
			UAMSimConstructionProposalView::AllPlacementParts));
	TestFalse(
		TEXT("Conflict proof is rejected by the authoritative validator"),
		Conflict.bValid);
	TestEqual(
		TEXT("Conflict proof localizes the owned-parcel violation"),
		Conflict.ReasonCode,
		FName(TEXT("Build.OutsideOwnedLand")));
	TestTrue(
		TEXT("Conflict proof exposes a concise corrective remedy"),
		Conflict.Remedy.Contains(TEXT("cyan parcel boundary")));
	AMSim::FStarterPlanProposal Moved =
		UAMSimConstructionProposalView::MakePresentationProposal(false);
	UAMSimConstructionProposalView::TranslateProposal(Moved, 5000, -5000);
	const AMSim::FPhase1Validation MovedValidation =
		AMSim::ValidateStarterPlan(Moved);
	TestFalse(
		TEXT("Moving a network away from fixed gates requires a reconnect"),
		MovedValidation.bValid);
	TestEqual(
		TEXT("Repositioning moves the optional road with the editable plan"),
		Moved.AccessEnd.Y,
		95000ll);
	const AMSim::FPhase1Point TopLeft =
		UAMSimConstructionProposalView::MapLocalPositionToParcel(
			FVector2D(205.0, 135.0),
			FVector2D(1000.0, 1000.0));
	const AMSim::FPhase1Point BottomRight =
		UAMSimConstructionProposalView::MapLocalPositionToParcel(
			FVector2D(785.0, 825.0),
			FVector2D(1000.0, 1000.0));
	TestEqual(TEXT("Full map left edge maps to parcel origin"), TopLeft.X, 0ll);
	TestEqual(TEXT("Full map top edge maps to parcel origin"), TopLeft.Y, 0ll);
	TestEqual(
		TEXT("Full map right edge reaches the parcel boundary"),
		BottomRight.X,
		100000ll);
	TestEqual(
		TEXT("Full map bottom edge reaches the parcel boundary"),
		BottomRight.Y,
		100000ll);
	const AMSim::FStarterPlanProposal RunwayOnly =
		UAMSimConstructionProposalView::TranslateToolPlacement(
			AMSim::CreateDefaultStarterPlan(),
			UAMSimConstructionProposalView::EPlacementTool::Runway,
			3000,
			-2000);
	TestEqual(
		TEXT("Direct runway drag moves the runway"),
		RunwayOnly.RunwayStart.X,
		23000ll);
	TestEqual(
		TEXT("Direct runway drag does not move the taxiway"),
		RunwayOnly.TaxiStart.X,
		45000ll);
	const AMSim::FStarterPlanProposal TaxiOnly =
		UAMSimConstructionProposalView::TranslateToolPlacement(
			AMSim::CreateDefaultStarterPlan(),
			UAMSimConstructionProposalView::EPlacementTool::Taxiway,
			-4000,
			1000,
			0);
	TestEqual(
		TEXT("Taxiway drag moves only the selected network segment"),
		TaxiOnly.TaxiwaySegments[0].Start.X,
		41000ll);
	TestEqual(
		TEXT("Taxiway drag leaves the fixed starter terminal in place"),
		TaxiOnly.OperationsHutCenter.X,
		50000ll);
	TestEqual(
		TEXT("Taxiway drag does not silently move optional road access"),
		TaxiOnly.AccessStart.X,
		50000ll);
	const AMSim::FStarterPlanProposal RoadOnly =
		UAMSimConstructionProposalView::TranslateToolPlacement(
			AMSim::CreateDefaultStarterPlan(),
			UAMSimConstructionProposalView::EPlacementTool::RoadAccess,
			5000,
			0);
	TestEqual(TEXT("Road drag moves its first endpoint"), RoadOnly.AccessStart.X, 55000ll);
	TestEqual(TEXT("Road drag moves its second endpoint"), RoadOnly.AccessEnd.X, 55000ll);
	TestEqual(TEXT("Road drag leaves the fixed terminal gate in place"), RoadOnly.StandCenter.X, 45000ll);

	const AMSim::FStarterPlanProposal SnappedTaxi =
		UAMSimConstructionProposalView::SetToolEndpoint(
			AMSim::CreateDefaultStarterPlan(),
			UAMSimConstructionProposalView::EPlacementTool::Taxiway,
			0,
			{30000, 43000});
	TestEqual(
		TEXT("Taxi start magnetically snaps to the authoritative runway centerline"),
		SnappedTaxi.TaxiStart.Y,
		40000ll);
	AMSim::FStarterPlanProposal DisconnectedStand = AMSim::CreateDefaultStarterPlan();
	DisconnectedStand.TaxiwaySegments = {{{1000, 1000}, {1000, 12000}}};
	DisconnectedStand.TaxiStart = DisconnectedStand.TaxiwaySegments[0].Start;
	DisconnectedStand.TaxiEnd = DisconnectedStand.TaxiwaySegments[0].End;
	const AMSim::FPhase1Validation DisconnectedValidation =
		AMSim::ValidateStarterPlan(DisconnectedStand);
	const TArray<UAMSimConstructionProposalView::FPlacementDiagnostic> Diagnostics =
		UAMSimConstructionProposalView::MakePlacementDiagnostics(
			DisconnectedStand,
			DisconnectedValidation);
	TestTrue(
		TEXT("Disconnected proposal produces localized map diagnostics"),
		Diagnostics.ContainsByPredicate([](
			const UAMSimConstructionProposalView::FPlacementDiagnostic& Diagnostic)
		{
			return Diagnostic.Label == TEXT("CONNECT TAXIWAY TO RUNWAY");
		}));
	TestTrue(
		TEXT("Runway inspector reports actual heading, length, and width"),
		UAMSimConstructionProposalView::DescribeRunwayGeometry(
			AMSim::CreateDefaultStarterPlan()).Contains(TEXT("09/27")) &&
		UAMSimConstructionProposalView::DescribeRunwayGeometry(
			AMSim::CreateDefaultStarterPlan()).Contains(TEXT("600 M x 20 M")));
	const AMSim::FPhase1WorldSegmentGeometry WorldRunway =
		AMSim::MakePhase1WorldSegmentGeometry(
			AMSim::CreateDefaultStarterPlan().RunwayStart,
			AMSim::CreateDefaultStarterPlan().RunwayEnd,
			2000,
			10.0);
	TestEqual(TEXT("Placed runway world length follows drawn length"), WorldRunway.Scale.X, 45.0, 0.01);
	TestEqual(TEXT("Placed runway world width follows chosen width"), WorldRunway.Scale.Z, 1.5, 0.01);
	TestEqual(TEXT("East-west map runway rotates correctly in world"), WorldRunway.YawDegrees, 90.0f, 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase45ProgressionContentTest,
	"AMSim.Phase4_5.Presentation.ProgressionContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase45ProgressionContentTest::RunTest(
	const FString& Parameters)
{
	AMSim::FPhase1QuerySnapshot Phase1;
	Phase1.Revision = 1;
	Phase1.Credits = 12000;
	Phase1.AirportPoints = 14;
	AMSim::FPhase2QuerySnapshot Phase2;
	Phase2.Revision = 2;
	Phase2.bInitialized = true;
	Phase2.AverageRating = 88;
	AMSim::FPhase2State Phase2State;
	Phase2State.SelectedSpecialization =
		AMSim::EAirportSpecialization::FlightSchool;
	AMSim::FPhase3QuerySnapshot Phase3;
	Phase3.Revision = 3;
	Phase3.bTerminalOpen = true;
	Phase3.PassengerCount = 52;
	Phase3.CompletedPassengerCount = 40;
	Phase3.bSecurityIntegrityValid = true;
	Phase3.bAccessibleRouteValid = true;
	AMSim::FPhase4QuerySnapshot Phase4;
	Phase4.Revision = 4;
	Phase4.bInitialized = true;
	Phase4.bTimetablePublished = true;
	Phase4.FlightCount = 21;
	Phase4.CompletedFlightCount = 9;

	const AMSim::FProgressionViewState View =
		AMSim::MakeProgressionViewState(
			Phase1,
			Phase2,
			Phase2State,
			Phase3,
			Phase4);
	TestEqual(TEXT("Capability map contains six paths"), View.Paths.Num(), 6);
	TestEqual(
		TEXT("Regional passenger play selects the passenger path"),
		View.SelectedPath,
		FString(TEXT("PASSENGER")));
	TestTrue(
		TEXT("Cargo remains honestly future locked"),
		View.Paths[3].bFutureLocked &&
			View.Paths[3].Status.Contains(TEXT("FUTURE")));
	TestEqual(
		TEXT("Three suggested objectives remain visible"),
		View.Objectives.Num(),
		3);
	TestTrue(
		TEXT("Inspector reports real timetable evidence"),
		View.Inspector.Contains(TEXT("PUBLISHED")));
	TestTrue(
		TEXT("Selected-path inspector exposes requirements, evidence, next band, and reward"),
		View.Inspector.Contains(TEXT("REQUIREMENTS")) &&
			View.Inspector.Contains(TEXT("CURRENT EVIDENCE")) &&
			View.Inspector.Contains(TEXT("NEXT CAPABILITY BAND")) &&
			View.Inspector.Contains(TEXT("REWARD / OUTCOME")));
	TestTrue(
		TEXT("Selected-path inspector explains spatial and business implications"),
		View.Inspector.Contains(TEXT("SPATIAL FOOTPRINT")) &&
			View.Inspector.Contains(TEXT("BUSINESS EFFECT")));
	TestTrue(
		TEXT("Combined paths are capability-based and non-exclusive"),
		View.CombinedPathExplanation.Contains(TEXT("not an exclusive class")) &&
			View.CombinedPathExplanation.Contains(TEXT("no artificial class bonus")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase45SharedIconKitTest,
	"AMSim.Phase4_5.Presentation.SharedIconKit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase45SharedIconKitTest::RunTest(
	const FString& Parameters)
{
	const UAMSimOverviewView* Overview =
		GetDefault<UAMSimOverviewView>();
	const UAMSimProgressionView* Progression =
		GetDefault<UAMSimProgressionView>();
	const UAMSimRegionalOperationsView* Regional =
		GetDefault<UAMSimRegionalOperationsView>();
	const UAMSimTerminalView* Terminal =
		GetDefault<UAMSimTerminalView>();
	const UAMSimTurnaroundView* Turnaround =
		GetDefault<UAMSimTurnaroundView>();
	TestTrue(
		TEXT("Overview resolves all five tool and four activity icons"),
		Overview->HasRequiredIconKit());
	TestEqual(
		TEXT("Overview icon kit contains nine cooker-visible textures"),
		Overview->GetLoadedIconCount(),
		9);
	TestTrue(
		TEXT("Selected-flight inspector resolves cooker-visible aircraft art"),
		Overview->HasRequiredIdentityArt());
	TestTrue(
		TEXT("Capability map resolves all six path thumbnails"),
		Progression->HasRequiredThumbnailKit());
	TestEqual(
		TEXT("Capability map thumbnail kit contains six textures"),
		Progression->GetLoadedThumbnailCount(),
		6);
	TestTrue(
		TEXT("Timetable resolves three operator identities and aircraft art"),
		Regional->HasRequiredContractIdentityArt());
	TestEqual(
		TEXT("Timetable operator identity kit contains three textures"),
		Regional->GetLoadedContractIdentityCount(),
		3);
	TestTrue(
		TEXT("Weather rail resolves its cooker-visible identity art"),
		Regional->HasRequiredWeatherArt());
	TestTrue(
		TEXT("Passenger inspector resolves cooker-visible family identity art"),
		Terminal->HasRequiredPassengerIdentityArt());
	TestTrue(
		TEXT("Turnaround inspector resolves cooker-visible aircraft identity art"),
		Turnaround->HasRequiredIdentityArt());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase45TimetableGeometryTest,
	"AMSim.Phase4_5.Presentation.TimetableGeometry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase45TimetableGeometryTest::RunTest(
	const FString& Parameters)
{
	using AMSim::FTimetableGeometry;
	const AMSim::FTimetableCardGeometry Morning =
		FTimetableGeometry::MakeCard(1, 7 * 60, 8 * 60 + 30, false);
	const AMSim::FTimetableCardGeometry Evening =
		FTimetableGeometry::MakeCard(1, 18 * 60, 20 * 60, false);
	const AMSim::FTimetableCardGeometry NextDay =
		FTimetableGeometry::MakeCard(2, 7 * 60, 8 * 60 + 30, false);
	const AMSim::FTimetableCardGeometry Compact =
		FTimetableGeometry::MakeCard(4, 12 * 60, 13 * 60, true);

	TestTrue(
		TEXT("Later flights render lower in the time grid"),
		Evening.Top > Morning.Top);
	TestTrue(
		TEXT("Departure time determines a positive card duration"),
		Morning.Bottom > Morning.Top);
	TestTrue(
		TEXT("Later days render in later columns"),
		NextDay.Left > Morning.Left);
	TestEqual(
		TEXT("Compact cards use one shared day rail"),
		Compact.Left,
		FTimetableGeometry::MakeCard(1, 12 * 60, 13 * 60, true).Left);
	TestTrue(
		TEXT("Card geometry remains inside the visible grid"),
		Morning.Top >= FTimetableGeometry::HeaderBottom &&
			Evening.Bottom <= FTimetableGeometry::GridBottom);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1ConstructionPresentationMappingTest,
	"AMSim.Phase1_5.Presentation.ConstructionProgressMapping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1ConstructionPresentationMappingTest::RunTest(
	const FString& Parameters)
{
	AMSim::FConstructionProjectRecord Project;
	Project.Stage = AMSim::EConstructionStage::AwaitingDelivery;
	Project.Proposal = AMSim::CreateDefaultStarterPlan();
	Project.Proposal.AccessEnd = Project.Proposal.AccessStart;
	Project.FundedAtGameMilliseconds = 1000;
	AMSim::FPhase1QuerySnapshot Query;
	Query.GameTimeMilliseconds = 1000;
	AMSim::FPhase1ConstructionVisualState Visual =
		AMSim::FPhase1ConstructionPresentation::Derive(Query, Project);
	const AMSim::FPhase1Fixture& Fixture = AMSim::GetPhase1Fixture();
	TestTrue(TEXT("Crew dispatch is visible immediately"), Visual.bCrewVisible);
	TestTrue(TEXT("Truck dispatch is visible immediately"), Visual.bTruckVisible);
	TestTrue(TEXT("Earthwork bed appears immediately"), Visual.bEarthworkVisible);
	TestTrue(TEXT("Travel begins at the access origin"),
		FMath::IsNearlyZero(Visual.DeliveryTravelProgress));
	TestTrue(TEXT("Finished surface is absent before building"),
		FMath::IsNearlyZero(Visual.SurfaceProgress));

	Query.GameTimeMilliseconds = Project.FundedAtGameMilliseconds +
		Fixture.DeliveryAtMilliseconds / 2;
	Visual = AMSim::FPhase1ConstructionPresentation::Derive(Query, Project);
	TestTrue(TEXT("Travel is halfway to the site at 15 seconds"),
		FMath::IsNearlyEqual(Visual.DeliveryTravelProgress, 0.5f));

	Project.Stage = AMSim::EConstructionStage::Building;
	Project.bDeliveryArrived = true;
	Query.GameTimeMilliseconds = Project.FundedAtGameMilliseconds +
		(Fixture.BuildingAtMilliseconds + Fixture.InspectionAtMilliseconds) / 2;
	Visual = AMSim::FPhase1ConstructionPresentation::Derive(Query, Project);
	TestTrue(TEXT("Surface reveal is halfway through the build stage"),
		FMath::IsNearlyEqual(Visual.SurfaceProgress, 0.5f));

	Project.Stage = AMSim::EConstructionStage::Inspection;
	Visual = AMSim::FPhase1ConstructionPresentation::Derive(Query, Project);
	TestTrue(TEXT("Inspection receives the complete surface"),
		FMath::IsNearlyEqual(Visual.SurfaceProgress, 1.0f));
	TestFalse(TEXT("Inspection removes the graded-earth underlay"),
		Visual.bEarthworkVisible);
	TestTrue(TEXT("Inspection exposes runway markings"),
		Visual.bFinishedMarkingsVisible);

	const TArray<double> Lengths{60.0, 30.0, 10.0};
	const TArray<float> Progress =
		AMSim::FPhase1ConstructionPresentation::AllocateLengthProgress(
			0.65f,
			Lengths);
	TestTrue(TEXT("Runway receives proportional build progress"),
		FMath::IsNearlyEqual(Progress[0], 0.65f));
	TestTrue(TEXT("Taxiway receives proportional build progress"),
		FMath::IsNearlyEqual(Progress[1], 0.65f));
	TestTrue(TEXT("Road receives proportional build progress"),
		FMath::IsNearlyEqual(Progress[2], 0.65f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase15AircraftJourneyPresentationTest,
	"AMSim.Phase1_5.Presentation.AutonomousAircraftJourney",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase15AircraftJourneyPresentationTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	FPhase1State State;
	State.Project.Proposal = CreateDefaultStarterPlan();
	State.Flight.ScheduledArrivalGameMilliseconds = 600000;
	FPhase1QuerySnapshot Query;
	Query.FlightState = EFlightState::Scheduled;
	Query.GameTimeMilliseconds = 300000;
	TestFalse(
		TEXT("A merely scheduled aircraft remains off-map"),
		FPhase1AircraftPresentation::Derive(Query, State).bVisible);

	const TArray<FVector> DirectTaxiPath =
		FPhase1AircraftPresentation::BuildTaxiPath(State.Project.Proposal);
	if (!TestTrue(
		TEXT("The committed starter network yields a runway-to-gate taxi path"),
		DirectTaxiPath.Num() >= 2))
	{
		return false;
	}

	FStarterPlanProposal Chained = State.Project.Proposal;
	Chained.TaxiwaySegments = {
		{{45000, 40000}, {45000, 57000}},
		{{45000, 57000}, {45000, 74000}}};
	Chained.TaxiStart = Chained.TaxiwaySegments[0].Start;
	Chained.TaxiEnd = Chained.TaxiwaySegments.Last().End;
	const TArray<FVector> ChainedTaxiPath =
		FPhase1AircraftPresentation::BuildTaxiPath(Chained);
	TestTrue(
		TEXT("A connected multi-segment network retains its intermediate join"),
		ChainedTaxiPath.Num() >= 3);

	const auto VisualAt = [&Query, &State](
		const EFlightState FlightState,
		const float Progress)
	{
		Query.FlightState = FlightState;
		const int64 Start = State.Flight.ScheduledArrivalGameMilliseconds +
			GetPhase1FlightStateOffsetMilliseconds(FlightState);
		const int64 End = State.Flight.ScheduledArrivalGameMilliseconds +
			GetPhase1FlightStateEndOffsetMilliseconds(FlightState);
		Query.GameTimeMilliseconds = FMath::RoundToInt64(FMath::Lerp(
			static_cast<double>(Start),
			static_cast<double>(End),
			Progress));
		return FPhase1AircraftPresentation::Derive(Query, State);
	};

	for (const EFlightState MovingState : {
		EFlightState::Inbound,
		EFlightState::Approach,
		EFlightState::Landing,
		EFlightState::RunwayRoll,
		EFlightState::TaxiIn,
		EFlightState::TaxiOut,
		EFlightState::Takeoff,
		EFlightState::Outbound})
	{
		const FPhase1AircraftVisualState Early = VisualAt(MovingState, 0.25f);
		const FPhase1AircraftVisualState Late = VisualAt(MovingState, 0.75f);
		TestTrue(TEXT("Active movement state is visible"), Early.bVisible);
		TestFalse(
			TEXT("Aircraft advances continuously within each movement state"),
			Early.Location.Equals(Late.Location, 1.0f));
		TestFalse(
			TEXT("Movement supplies a stable facing direction"),
			Early.Direction.IsNearlyZero());
	}

	const FPhase1AircraftVisualState Parked = VisualAt(EFlightState::Turnaround, 0.5f);
	TestTrue(TEXT("Aircraft remains visible while parked for services"), Parked.bVisible);
	TestTrue(
		TEXT("Parked aircraft is located at the connected gate"),
		Parked.Location.Equals(DirectTaxiPath.Last(), 1.0f));
	Query.FlightState = EFlightState::Completed;
	TestFalse(
		TEXT("Aircraft leaves the map after outbound completion"),
		FPhase1AircraftPresentation::Derive(Query, State).bVisible);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase15WorldPresenterTest,
	"AMSim.Phase1_5.Presentation.Paper2DWorld",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase15WorldPresenterTest::RunTest(const FString& Parameters)
{
	AAMSimWorldPresenter* Presenter = GetMutableDefault<AAMSimWorldPresenter>();
	TestFalse(TEXT("World presenter has no per-frame tick"), Presenter->PrimaryActorTick.bCanEverTick);
	TestTrue(TEXT("All serialized presentation assets resolve"), Presenter->HasRequiredPresentationAssets());
	TestEqual(
		TEXT("Accessible path owns four dashes across five route legs"),
		Presenter->GetTerminalAccessibleDashProxyCount(),
		20);
	TestEqual(
		TEXT("Incident response route owns six continuous connectors"),
		Presenter->GetIncidentRouteSegmentProxyCount(),
		6);
	TestEqual(
		TEXT("Baggage flow owns a dedicated three-leg exception branch"),
		Presenter->GetTerminalExceptionRouteProxyCount(),
		4);
	TestEqual(
		TEXT("Incident world owns runway, taxiway, and apron wet layers"),
		Presenter->GetWetSurfaceProxyCount(),
		3);
	TestTrue(
		TEXT("Mature overview owns a selected-entity world proxy"),
		Presenter->HasMatureSelectionProxy());
	TestTrue(
		TEXT("Runway, taxiway, and service-road surfaces resolve independently"),
		Presenter->HasDistinctPhase1MovementSurfaceAssets());
	TestEqual(
		TEXT("Primary runway number faces the reciprocal approach"),
		AMSim::MakePhase1RunwayNumberYawDegrees(0.0f, false),
		180.0f);
	TestEqual(
		TEXT("Reciprocal runway number faces the primary approach"),
		AMSim::MakePhase1RunwayNumberYawDegrees(0.0f, true),
		0.0f);
	TestEqual(
		TEXT("Vertical primary number remains reciprocally oriented"),
		AMSim::MakePhase1RunwayNumberYawDegrees(90.0f, false),
		270.0f);
	TestEqual(
		TEXT("Positive X movement faces the negative-Y-forward sprite east"),
		AMSim::MakePhase1TopDownMovementYawDegrees(FVector(1.0, 0.0, 0.0)),
		90.0f);
	TestEqual(
		TEXT("Positive Y movement faces the negative-Y-forward sprite north"),
		AMSim::MakePhase1TopDownMovementYawDegrees(FVector(0.0, 1.0, 0.0)),
		180.0f);
	TestEqual(
		TEXT("Negative X movement faces the negative-Y-forward sprite west"),
		AMSim::MakePhase1TopDownMovementYawDegrees(FVector(-1.0, 0.0, 0.0)),
		270.0f);
	TestEqual(
		TEXT("Negative Y movement preserves the source-art forward axis"),
		AMSim::MakePhase1TopDownMovementYawDegrees(FVector(0.0, -1.0, 0.0)),
		0.0f);
	const AMSim::FPhase1PatrolMotion PatrolStart =
		AMSim::FPhase1ConstructionPresentation::CalculatePatrolMotion(0, 0);
	const AMSim::FPhase1PatrolMotion PatrolTurn =
		AMSim::FPhase1ConstructionPresentation::CalculatePatrolMotion(450000, 0);
	const AMSim::FPhase1PatrolMotion PatrolLoop =
		AMSim::FPhase1ConstructionPresentation::CalculatePatrolMotion(900000, 0);
	const AMSim::FPhase1PatrolMotion StaggeredWorker =
		AMSim::FPhase1ConstructionPresentation::CalculatePatrolMotion(0, 1);
	TestTrue(TEXT("Patrol begins at its first endpoint"),
		FMath::IsNearlyZero(PatrolStart.Progress) && PatrolStart.bForward);
	TestTrue(TEXT("Patrol reverses continuously at its far endpoint"),
		FMath::IsNearlyEqual(PatrolTurn.Progress, 1.0f) && !PatrolTurn.bForward);
	TestTrue(TEXT("Patrol cycle returns to its first endpoint"),
		FMath::IsNearlyZero(PatrolLoop.Progress) && PatrolLoop.bForward);
	TestTrue(TEXT("Adjacent workers use staggered patrol phases"),
		FMath::IsNearlyEqual(StaggeredWorker.Progress, 0.5f));
	TestEqual(
		TEXT("Parked aircraft uses the stand-facing heading"),
		AAMSimWorldPresenter::GetHeadingIndex(AMSim::EFlightState::Parked),
		8);
	AMSim::FPhase1QuerySnapshot Construction;
	Construction.Revision = 901;
	Construction.GameTimeMilliseconds = 0;
	Construction.ConstructionStage = AMSim::EConstructionStage::AwaitingDelivery;
	AMSim::FPhase1State ConstructionState;
	ConstructionState.Project.Stage = AMSim::EConstructionStage::AwaitingDelivery;
	ConstructionState.Project.Proposal = AMSim::CreateDefaultStarterPlan();
	Presenter->ApplySnapshot(Construction, ConstructionState);
	TestEqual(
		TEXT("Purchase exposes truck, four workers, bounded cones, and work zone immediately"),
		Presenter->GetActivePhase1ConstructionProxyCount(),
		11);
	TestEqual(
		TEXT("Purchase exposes runway, taxiway, and road earthwork beds"),
		Presenter->GetActivePhase1EarthworkProxyCount(),
		3);
	TestTrue(
		TEXT("Every placeholder bed renders below its in-progress surface"),
		Presenter->ArePhase1ConstructionBedsBelowSurfaces());
	TestTrue(
		TEXT("Crew begins at the travel origin"),
		FMath::IsNearlyZero(Presenter->GetPhase1ConstructionTravelProgress()));
	TestTrue(
		TEXT("No final surface is visible before building"),
		FMath::IsNearlyZero(Presenter->GetPhase1ConstructionSurfaceProgress()));

	Construction.Revision = 902;
	const AMSim::FPhase1Fixture& Fixture = AMSim::GetPhase1Fixture();
	const int64 RoadBuildingAt = Fixture.BuildingAtMilliseconds * 80 / 100;
	const int64 RoadInspectionAt = Fixture.InspectionAtMilliseconds * 80 / 100;
	Construction.GameTimeMilliseconds = (RoadBuildingAt + RoadInspectionAt) / 2;
	Construction.ConstructionStage = AMSim::EConstructionStage::Building;
	ConstructionState.Project.Stage = AMSim::EConstructionStage::Building;
	ConstructionState.Project.bDeliveryArrived = true;
	Presenter->ApplySnapshot(Construction, ConstructionState);
	TestTrue(
		TEXT("Workers reach the site before construction begins"),
		FMath::IsNearlyEqual(Presenter->GetPhase1ConstructionTravelProgress(), 1.0f));
	TestTrue(
		TEXT("Road-assisted build reveals half of the total network at its midpoint"),
		FMath::IsNearlyEqual(Presenter->GetPhase1ConstructionSurfaceProgress(), 0.5f));
	TestTrue(
		TEXT("Placeholder beds remain below partially revealed surfaces"),
		Presenter->ArePhase1ConstructionBedsBelowSurfaces());
	const FVector FirstPatrolLocation =
		Presenter->GetPhase1ConstructionWorkerLocation(0);
	const float FirstPatrolFacing =
		Presenter->GetPhase1ConstructionWorkerFacingYawDegrees(0);
	Construction.Revision = 903;
	Construction.GameTimeMilliseconds += 112500;
	Presenter->ApplySnapshot(Construction, ConstructionState);
	const FVector SecondPatrolLocation =
		Presenter->GetPhase1ConstructionWorkerLocation(0);
	const float SecondPatrolFacing =
		Presenter->GetPhase1ConstructionWorkerFacingYawDegrees(0);
	TestFalse(
		TEXT("A construction worker changes position during active surface work"),
		FirstPatrolLocation.Equals(SecondPatrolLocation));
	TestFalse(
		TEXT("A construction worker turns when its patrol reverses"),
		FMath::IsNearlyEqual(FirstPatrolFacing, SecondPatrolFacing));
	Construction.Revision = 904;
	Construction.bPaused = true;
	Presenter->ApplySnapshot(Construction, ConstructionState);
	TestTrue(
		TEXT("Paused game time freezes worker position"),
		Presenter->GetPhase1ConstructionWorkerLocation(0).Equals(
			SecondPatrolLocation));
	TestTrue(
		TEXT("Paused game time freezes worker facing"),
		FMath::IsNearlyEqual(
			Presenter->GetPhase1ConstructionWorkerFacingYawDegrees(0),
			SecondPatrolFacing));
	Construction.Revision = 905;
	Construction.bPaused = false;
	Construction.ConstructionStage = AMSim::EConstructionStage::Inspection;
	ConstructionState.Project.Stage = AMSim::EConstructionStage::Inspection;
	Presenter->ApplySnapshot(Construction, ConstructionState);
	TestTrue(
		TEXT("Inspection exposes the complete finished network"),
		FMath::IsNearlyEqual(Presenter->GetPhase1ConstructionSurfaceProgress(), 1.0f));
	TestEqual(
		TEXT("Completed movement surfaces hide every placeholder bed"),
		Presenter->GetActivePhase1EarthworkProxyCount(),
		0);
	TestTrue(
		TEXT("Completed surfaces retain the explicit layer order"),
		Presenter->ArePhase1ConstructionBedsBelowSurfaces());

	AMSim::FPhase1QuerySnapshot Turnaround;
	Turnaround.Revision = 906;
	Turnaround.ConstructionStage = AMSim::EConstructionStage::Operational;
	Turnaround.FlightState = AMSim::EFlightState::Turnaround;
	Turnaround.InspectionState = AMSim::EServiceTaskState::Active;
	Turnaround.FuelingState = AMSim::EServiceTaskState::Active;
	AMSim::FPhase1State TurnaroundState;
	TurnaroundState.Project.Stage = AMSim::EConstructionStage::Operational;
	TurnaroundState.Project.Proposal = AMSim::CreateDefaultStarterPlan();
	Presenter->ApplySnapshot(Turnaround, TurnaroundState);
	TestEqual(
		TEXT("Turnaround exposes two safe paths plus vehicle, worker, cones, and zone"),
		Presenter->GetActiveTurnaroundSupportProxyCount(),
		6);
	TestEqual(
		TEXT("Operational airport clears construction support"),
		Presenter->GetActivePhase1ConstructionProxyCount(),
		0);
	AMSim::FPhase1QuerySnapshot MovedQuery = Turnaround;
	MovedQuery.Revision = 907;
	AMSim::FPhase1State MovedState = TurnaroundState;
	const FVector OriginalRunwayCenter = Presenter->GetPhase1RunwayCenter();
	UAMSimConstructionProposalView::TranslateProposal(
		MovedState.Project.Proposal,
		5000,
		0);
	Presenter->ApplySnapshot(MovedQuery, MovedState);
	TestFalse(
		TEXT("Committed runway placement drives world geometry"),
		Presenter->GetPhase1RunwayCenter().Equals(OriginalRunwayCenter));
	TestTrue(
		TEXT("The starter terminal and gate anchor remain fixed"),
		Presenter->GetPhase1GeometryOffset().IsNearlyZero());

	AMSim::FPhase1QuerySnapshot Cleared;
	Cleared.Revision = 908;
	Presenter->ApplySnapshot(Cleared, AMSim::FPhase1State());
	TestTrue(
		TEXT("Empty airport resets proposal geometry"),
		Presenter->GetPhase1GeometryOffset().IsNearlyZero());
	TestEqual(
		TEXT("Uncommitted site clears every construction proxy"),
		Presenter->GetActivePhase1ConstructionProxyCount(),
		0);
	TestEqual(
		TEXT("Completed turnaround clears every support proxy"),
		Presenter->GetActiveTurnaroundSupportProxyCount(),
		0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPresentationProxyPoolTest,
	"AMSim.Phase0.Presentation.ProxyPoolReuse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPresentationProxyPoolTest::RunTest(const FString& Parameters)
{
	AMSim::FPresentationProxyPool Pool;
	const AMSim::FPresentationProxyHandle First = Pool.Acquire({1});
	TestEqual(TEXT("Same entity returns same proxy"), Pool.Acquire({1}), First);
	TestTrue(TEXT("Release succeeds"), Pool.Release({1}));
	const AMSim::FPresentationProxyHandle Reused = Pool.Acquire({2});
	TestEqual(TEXT("Released slot is reused"), Reused.Index, First.Index);
	TestTrue(TEXT("Generation changes across reuse"), Reused.Generation > First.Generation);
	TestEqual(TEXT("One proxy remains active"), Pool.GetActiveCount(), 1);
	return true;
}

#endif
