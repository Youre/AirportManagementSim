#include "AMSimCameraPawn.h"
#include "AMSimConstructionProposalView.h"
#include "AMSimGameMode.h"
#include "AMSimOverviewView.h"
#include "AMSimPhase1Fixture.h"
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
	TestEqual(
		TEXT("Parked aircraft uses the stand-facing heading"),
		AAMSimWorldPresenter::GetHeadingIndex(AMSim::EFlightState::Parked),
		8);
	AMSim::FPhase1QuerySnapshot Construction;
	Construction.Revision = 901;
	Construction.ConstructionStage = AMSim::EConstructionStage::Building;
	Presenter->ApplySnapshot(Construction);
	TestEqual(
		TEXT("Building stage exposes localized truck, worker, cones, and work zone"),
		Presenter->GetActivePhase1ConstructionProxyCount(),
		4);

	AMSim::FPhase1QuerySnapshot Turnaround;
	Turnaround.Revision = 902;
	Turnaround.ConstructionStage = AMSim::EConstructionStage::Operational;
	Turnaround.FlightState = AMSim::EFlightState::Turnaround;
	Turnaround.InspectionState = AMSim::EServiceTaskState::Active;
	Turnaround.FuelingState = AMSim::EServiceTaskState::Active;
	Presenter->ApplySnapshot(Turnaround);
	TestEqual(
		TEXT("Turnaround exposes two safe paths plus vehicle, worker, cones, and zone"),
		Presenter->GetActiveTurnaroundSupportProxyCount(),
		6);
	TestEqual(
		TEXT("Operational airport clears construction support"),
		Presenter->GetActivePhase1ConstructionProxyCount(),
		0);

	AMSim::FPhase1QuerySnapshot Cleared;
	Cleared.Revision = 903;
	Presenter->ApplySnapshot(Cleared);
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
