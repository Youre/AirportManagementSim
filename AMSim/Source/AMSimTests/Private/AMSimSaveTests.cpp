#include "AMSimSaveStore.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimSaveRoundTripTest,
	"AMSim.Phase0.Save.RoundTripAndBackup",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimSaveRoundTripTest::RunTest(const FString& Parameters)
{
	const FString Slot = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation/AMSimPhase0Save"));
	IFileManager::Get().DeleteDirectory(*Slot, false, true);

	AMSim::FSimulation Simulation(77);
	Simulation.QueueCommand({{1}, AMSim::ECommandType::CreateEntity, 0, {}});
	Simulation.Step();
	TestTrue(TEXT("First async save succeeds"), AMSim::FSaveStore::WriteAsync(Slot, Simulation.CreateSnapshot()).Get().bSucceeded);

	Simulation.QueueCommand({{2}, AMSim::ECommandType::CreateEntity, 250, {}});
	Simulation.Step();
	TestTrue(TEXT("Second async save succeeds"), AMSim::FSaveStore::WriteAsync(Slot, Simulation.CreateSnapshot()).Get().bSucceeded);

	AMSim::FSnapshot Loaded;
	bool bUsedBackup = false;
	TestTrue(TEXT("Current save loads"), AMSim::FSaveStore::LoadCurrentOrBackup(Slot, Loaded, bUsedBackup));
	TestFalse(TEXT("Valid current does not use backup"), bUsedBackup);
	TestEqual(TEXT("Current save has two entities"), Loaded.Entities.Num(), 2);
	AMSim::FSaveMetadata Metadata;
	TestTrue(TEXT("Slot metadata loads independently"), AMSim::FSaveStore::LoadMetadata(Slot, Metadata));
	TestEqual(TEXT("Metadata tracks game time"), Metadata.GameTimeMilliseconds, 500ll);
	TestEqual(TEXT("Metadata tracks schema"), Metadata.SaveSchema, AMSim::SnapshotSchemaVersion);

	IFileManager::Get().Delete(*AMSim::FSaveStore::CurrentPath(Slot), false, true);
	TestTrue(TEXT("Backup loads when current is missing"), AMSim::FSaveStore::LoadCurrentOrBackup(Slot, Loaded, bUsedBackup));
	TestTrue(TEXT("Fallback identifies backup"), bUsedBackup);
	TestEqual(TEXT("Backup retains previous entity count"), Loaded.Entities.Num(), 1);

	IFileManager::Get().DeleteDirectory(*Slot, false, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimSaveCoalescingAndCorruptionTest,
	"AMSim.Phase0.Save.CoalescingAndCorruption",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimSaveCoalescingAndCorruptionTest::RunTest(const FString& Parameters)
{
	const FString Slot = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation/AMSimPhase0Coalescing"));
	IFileManager::Get().DeleteDirectory(*Slot, false, true);

	AMSim::FSimulation Simulation(88);
	TestTrue(
		TEXT("Baseline save succeeds before coalescing fixture"),
		AMSim::FSaveStore::WriteAsync(Slot, Simulation.CreateSnapshot()).Get().bSucceeded);
	Simulation.Step();
	TFuture<AMSim::FSaveResult> First = AMSim::FSaveStore::WriteAsync(Slot, Simulation.CreateSnapshot());
	Simulation.QueueCommand({{1}, AMSim::ECommandType::CreateEntity, 250, {}});
	Simulation.Step();
	TFuture<AMSim::FSaveResult> Latest = AMSim::FSaveStore::WriteAsync(Slot, Simulation.CreateSnapshot());
	TestTrue(TEXT("First coalesced request completes"), First.Get().bSucceeded);
	TestTrue(TEXT("Latest coalesced request completes"), Latest.Get().bSucceeded);

	AMSim::FSnapshot Loaded;
	bool bUsedBackup = false;
	TestTrue(TEXT("Latest requested state is persisted"), AMSim::FSaveStore::LoadCurrentOrBackup(Slot, Loaded, bUsedBackup));
	TestEqual(TEXT("Latest requested entity exists"), Loaded.Entities.Num(), 1);

	TArray<uint8> Corrupt{0, 1, 2, 3};
	TestTrue(
		TEXT("Corrupt current fixture is written"),
		FFileHelper::SaveArrayToFile(Corrupt, *AMSim::FSaveStore::CurrentPath(Slot)));
	TestTrue(TEXT("Corrupt current falls back to backup"), AMSim::FSaveStore::LoadCurrentOrBackup(Slot, Loaded, bUsedBackup));
	TestTrue(TEXT("Corrupt fallback identifies backup"), bUsedBackup);

	AMSim::FSaveMetadata Oversized;
	Oversized.SlotId = TEXT("slot");
	Oversized.PlayerLabel = FString::ChrN(65, TEXT('x'));
	FString Error;
	TestFalse(TEXT("Free-text save labels are bounded"), AMSim::FSaveStore::ValidateMetadata(Oversized, Error));

	IFileManager::Get().DeleteDirectory(*Slot, false, true);
	return true;
}

#endif
