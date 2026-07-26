#include "AMSimDeterminism.h"
#include "AMSimSimulation.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimFixedClockTest,
	"AMSim.Phase0.Simulation.FixedClock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimFixedClockTest::RunTest(const FString& Parameters)
{
	AMSim::FSimulationClock Clock;
	Clock.AdvanceSteps(4);
	TestEqual(TEXT("Four fixed steps equal one game second"), Clock.GetGameTimeMilliseconds(), 1000ll);
	TestEqual(TEXT("Step index is stable"), Clock.GetStepIndex(), 4ull);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimNamedRandomStreamTest,
	"AMSim.Phase0.Simulation.NamedRandomStream",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimNamedRandomStreamTest::RunTest(const FString& Parameters)
{
	const uint64 Seed = AMSim::FDeterministicStream::SeedNamedStream(42, TEXT("Weather"));
	AMSim::FDeterministicStream First(Seed);
	AMSim::FDeterministicStream Second(Seed);
	for (int32 Index = 0; Index < 32; ++Index)
	{
		TestEqual(TEXT("Streams with the same name and seed match"), First.NextUInt64(), Second.NextUInt64());
	}
	TestNotEqual(
		TEXT("Different stream names are isolated"),
		Seed,
		AMSim::FDeterministicStream::SeedNamedStream(42, TEXT("Economy")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimReplayChecksumTest,
	"AMSim.Phase0.Simulation.ReplayChecksum",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimReplayChecksumTest::RunTest(const FString& Parameters)
{
	const TArray<AMSim::FCommand> Commands{
		{{2}, AMSim::ECommandType::CreateEntity, 0, {}},
		{{1}, AMSim::ECommandType::CreateEntity, 0, {}},
		{{3}, AMSim::ECommandType::NoOp, 0, {}}
	};
	const AMSim::FReplayResult First = AMSim::Replay(1234, Commands, 2);
	const AMSim::FReplayResult Second = AMSim::Replay(1234, Commands, 2);
	TestEqual(TEXT("Replay checksums match"), First.Checksum, Second.Checksum);
	TestEqual(TEXT("Replay event counts match"), First.Events.Num(), Second.Events.Num());
	for (int32 Index = 0; Index < First.Events.Num(); ++Index)
	{
		TestEqual(TEXT("Event sequence matches"), First.Events[Index].Sequence, Second.Events[Index].Sequence);
		TestEqual(TEXT("Event entity matches"), First.Events[Index].Entity.Value, Second.Events[Index].Entity.Value);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimSnapshotContinuationTest,
	"AMSim.Phase0.Simulation.SnapshotContinuation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimSnapshotContinuationTest::RunTest(const FString& Parameters)
{
	AMSim::FSimulation Uninterrupted(99);
	Uninterrupted.QueueCommand({{1}, AMSim::ECommandType::CreateEntity, 0, {}});
	Uninterrupted.Step();
	const AMSim::FSnapshot Snapshot = Uninterrupted.CreateSnapshot();

	AMSim::FSimulation Restored;
	TestTrue(TEXT("Version-one snapshot restores"), Restored.RestoreSnapshot(Snapshot));
	Uninterrupted.QueueCommand({{2}, AMSim::ECommandType::CreateEntity, 250, {}});
	Restored.QueueCommand({{2}, AMSim::ECommandType::CreateEntity, 250, {}});
	Uninterrupted.Step();
	Restored.Step();
	TestEqual(TEXT("Post-load continuation checksum matches"), Uninterrupted.CalculateChecksum(), Restored.CalculateChecksum());

	AMSim::FSnapshot Invalid = Snapshot;
	Invalid.GameTimeMilliseconds += 1;
	const uint64 BeforeRejectedRestore = Restored.CalculateChecksum();
	TestFalse(TEXT("Misaligned snapshot time is rejected"), Restored.RestoreSnapshot(Invalid));
	TestEqual(
		TEXT("Rejected restore leaves active state unchanged"),
		Restored.CalculateChecksum(),
		BeforeRejectedRestore);
	return true;
}

#endif
