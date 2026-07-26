#include "AMSimSimulation.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimLogicalScaleFixtureTest,
	"AMSim.Phase0.Performance.LogicalScaleFixture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimLogicalScaleFixtureTest::RunTest(const FString& Parameters)
{
	AMSim::FSimulation Simulation(20260725);
	for (uint64 Index = 1; Index <= 10000; ++Index)
	{
		TestEqual(
			TEXT("Scale-fixture create command is accepted"),
			Simulation.QueueCommand({{Index}, AMSim::ECommandType::CreateEntity, 0, {}}),
			AMSim::ECommandResult::Accepted);
	}

	const double StepStart = FPlatformTime::Seconds();
	Simulation.Step();
	const double StepMilliseconds = (FPlatformTime::Seconds() - StepStart) * 1000.0;
	const AMSim::FSimulationDiagnostics Diagnostics = Simulation.CreateDiagnostics();
	TestEqual(TEXT("Fixture retains 10,000 logical entities"), Diagnostics.EntityCount, 10000);
	TestEqual(TEXT("Fixture advances exactly one fixed step"), Diagnostics.StepIndex, 1ull);
	TestTrue(TEXT("10,000-entity creation barrier completes under one second"), StepMilliseconds < 1000.0);

	const double SnapshotStart = FPlatformTime::Seconds();
	const AMSim::FSnapshot Snapshot = Simulation.CreateSnapshot();
	const double SnapshotMilliseconds = (FPlatformTime::Seconds() - SnapshotStart) * 1000.0;
	TestEqual(TEXT("Snapshot contains every logical entity"), Snapshot.Entities.Num(), 10000);
	TestTrue(TEXT("Phase 0 snapshot capture stays under 50 ms"), SnapshotMilliseconds < 50.0);
	AddInfo(FString::Printf(
		TEXT("Phase0ScaleFixture: step_ms=%.3f snapshot_ms=%.3f checksum=%llu"),
		StepMilliseconds,
		SnapshotMilliseconds,
		Diagnostics.StateChecksum));
	return true;
}

#endif
