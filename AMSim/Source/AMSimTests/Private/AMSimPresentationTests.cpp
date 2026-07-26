#include "AMSimCameraPawn.h"
#include "AMSimGameMode.h"
#include "AMSimPhase1ViewState.h"
#include "AMSimPlayerController.h"
#include "AMSimPresentationProxyPool.h"
#include "AMSimRootScreen.h"
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
	FAMSimPhase15WorldPresenterTest,
	"AMSim.Phase1_5.Presentation.Paper2DWorld",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase15WorldPresenterTest::RunTest(const FString& Parameters)
{
	const AAMSimWorldPresenter* Presenter = GetDefault<AAMSimWorldPresenter>();
	TestFalse(TEXT("World presenter has no per-frame tick"), Presenter->PrimaryActorTick.bCanEverTick);
	TestTrue(TEXT("All serialized presentation assets resolve"), Presenter->HasRequiredPresentationAssets());
	TestEqual(
		TEXT("Parked aircraft uses the stand-facing heading"),
		AAMSimWorldPresenter::GetHeadingIndex(AMSim::EFlightState::Parked),
		8);
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
