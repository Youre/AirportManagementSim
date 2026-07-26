#include "AMSimCameraPawn.h"
#include "AMSimPresentationProxyPool.h"
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
	TestEqual(TEXT("Camera projection is orthographic"), Pawn->GetCamera()->ProjectionMode, ECameraProjectionMode::Orthographic);
	TestTrue(TEXT("Camera width is positive"), Pawn->GetCamera()->OrthoWidth > 0.0f);
	TestFalse(TEXT("Camera pawn has no per-frame tick"), Pawn->PrimaryActorTick.bCanEverTick);
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
