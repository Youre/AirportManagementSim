#include "AMSimAccessibilityProfile.h"
#include "AMSimContextHelp.h"
#include "AMSimPhase7Content.h"
#include "AMSimRadioSubsystem.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	int32 WordCount(const FString& Copy)
	{
		TArray<FString> Words;
		Copy.ParseIntoArrayWS(Words);
		return Words.Num();
	}

	bool LoadJson(
		FAutomationTestBase& Test,
		const FString& RelativePath,
		TSharedPtr<FJsonObject>& Output)
	{
		const FString Path =
			FPaths::ConvertRelativePathToFull(
				FPaths::Combine(
					FPaths::ProjectDir(),
					RelativePath));
		FString Json;
		if (!Test.TestTrue(
			*FString::Printf(
				TEXT("%s is readable"),
				*RelativePath),
			FFileHelper::LoadFileToString(Json, *Path)))
		{
			return false;
		}
		const TSharedRef<TJsonReader<>> Reader =
			TJsonReaderFactory<>::Create(Json);
		return Test.TestTrue(
			*FString::Printf(
				TEXT("%s is valid JSON"),
				*RelativePath),
			FJsonSerializer::Deserialize(Reader, Output) &&
				Output.IsValid());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase7ContextHelpTest,
	"AMSim.Phase7.Guidance.SaveLocalContextHelp",
	EAutomationTestFlags::EditorContext |
		EAutomationTestFlags::EngineFilter)

bool FAMSimPhase7ContextHelpTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	const TArray<FContextHelpDefinition>& Catalog =
		GetPhase1ContextHelpCatalog();
	TestEqual(
		TEXT("Six starter context cards exist"),
		Catalog.Num(),
		6);
	TSet<FName> Ids;
	for (const FContextHelpDefinition& Help : Catalog)
	{
		TestFalse(
			TEXT("Help ID is stable"),
			Help.Id.IsNone());
		TestFalse(
			TEXT("Help ID is unique"),
			Ids.Contains(Help.Id));
		TestTrue(
			TEXT("Context body stays within 45 words"),
			WordCount(Help.Body) <= 45);
		TestFalse(
			TEXT("Context title is present"),
			Help.Title.IsEmpty());
		TestFalse(
			TEXT("Context next action is present"),
			Help.NextAction.IsEmpty());
		Ids.Add(Help.Id);
	}

	FSimulation Simulation(0xA17F0007);
	const uint64 Before =
		Simulation.CalculateChecksum();
	FPhase1Command Acknowledge;
	Acknowledge.Id = {1};
	Acknowledge.Type =
		EPhase1CommandType::AcknowledgeContextHelp;
	Acknowledge.ContextHelpId =
		TEXT("Help.Start.NameAirport");
	TestEqual(
		TEXT("Known help acknowledgement is accepted"),
		Simulation.QueuePhase1Command(Acknowledge),
		EPhase1CommandResult::Accepted);
	Simulation.Step();
	TestTrue(
		TEXT("Acknowledged help is authoritative state"),
		Simulation.GetPhase1State()
			.AcknowledgedContextHelp.Contains(
				Acknowledge.ContextHelpId));
	TestNotEqual(
		TEXT("Help acknowledgement participates in checksum"),
		Simulation.CalculateChecksum(),
		Before);
	TestEqual(
		TEXT("Duplicate acknowledgement is rejected"),
		Simulation.QueuePhase1Command(Acknowledge),
		EPhase1CommandResult::RejectedInvalidState);
	Acknowledge.Id = {2};
	Acknowledge.ContextHelpId =
		TEXT("Help.Unknown");
	TestEqual(
		TEXT("Unknown help is rejected"),
		Simulation.QueuePhase1Command(Acknowledge),
		EPhase1CommandResult::RejectedInvalidCommand);

	TArray<uint8> Bytes;
	const FSnapshot Snapshot = Simulation.CreateSnapshot();
	TestTrue(
		TEXT("Schema 8 help snapshot serializes"),
		SerializeSnapshot(Snapshot, Bytes));
	FSnapshot RoundTrip;
	TestTrue(
		TEXT("Schema 8 help snapshot deserializes"),
		DeserializeSnapshot(Bytes, RoundTrip));
	TestEqual(
		TEXT("Schema 8 is current"),
		RoundTrip.SchemaVersion,
		SnapshotSchemaVersion);
	TestTrue(
		TEXT("Help acknowledgement survives round trip"),
		RoundTrip.Phase1.AcknowledgedContextHelp.Contains(
			TEXT("Help.Start.NameAirport")));

	FSnapshot Duplicate = RoundTrip;
	Duplicate.Phase1.AcknowledgedContextHelp.Add(
		TEXT("Help.Start.NameAirport"));
	FSimulation RejectDuplicate(0xA17F0007);
	TestFalse(
		TEXT("Duplicate restored help IDs are rejected"),
		RejectDuplicate.RestoreSnapshot(Duplicate));
	FSnapshot Unknown = RoundTrip;
	Unknown.Phase1.AcknowledgedContextHelp = {
		TEXT("Help.Unknown")};
	FSimulation RejectUnknown(0xA17F0007);
	TestFalse(
		TEXT("Unknown restored help IDs are rejected"),
		RejectUnknown.RestoreSnapshot(Unknown));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase7MigrationMatrixTest,
	"AMSim.Phase7.Persistence.Schemas1Through8",
	EAutomationTestFlags::EditorContext |
		EAutomationTestFlags::EngineFilter)

bool FAMSimPhase7MigrationMatrixTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(0xA17F0008);
	const FSnapshot Current = Simulation.CreateSnapshot();
	for (uint32 Schema = 1;
		Schema < SnapshotSchemaVersion;
		++Schema)
	{
		FSnapshot Legacy = Current;
		Legacy.SchemaVersion = Schema;
		Legacy.Phase1.AcknowledgedContextHelp.Reset();
		TArray<uint8> Bytes;
		TestTrue(
			*FString::Printf(
				TEXT("Schema %u fixture serializes"),
				Schema),
			SerializeSnapshot(Legacy, Bytes));
		FSnapshot Migrated;
		TestTrue(
			*FString::Printf(
				TEXT("Schema %u migrates"),
				Schema),
			DeserializeSnapshot(Bytes, Migrated));
		TestEqual(
			*FString::Printf(
				TEXT("Schema %u reaches current"),
				Schema),
			Migrated.SchemaVersion,
			SnapshotSchemaVersion);
		TestTrue(
			TEXT("Migrated help history starts empty"),
			Migrated.Phase1
				.AcknowledgedContextHelp.IsEmpty());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase7AccessibilityTest,
	"AMSim.Phase7.Accessibility.ScaleAndDefaultPolicy",
	EAutomationTestFlags::EditorContext |
		EAutomationTestFlags::EngineFilter)

bool FAMSimPhase7AccessibilityTest::RunTest(
	const FString& Parameters)
{
	const TArray<float>& Scales =
		UAMSimAccessibilityProfile::SupportedScales();
	TestEqual(
		TEXT("Five supported scales exist"),
		Scales.Num(),
		5);
	TestEqual(
		TEXT("Low values clamp to 100 percent"),
		UAMSimAccessibilityProfile::NormalizeInterfaceScale(
			0.2f),
		1.0f);
	TestEqual(
		TEXT("Near 150 percent resolves to 150"),
		UAMSimAccessibilityProfile::NormalizeInterfaceScale(
			1.49f),
		1.5f);
	TestEqual(
		TEXT("High values clamp to 200 percent"),
		UAMSimAccessibilityProfile::NormalizeInterfaceScale(
			4.0f),
		2.0f);
	const UAMSimAccessibilityProfile* Profile =
		UAMSimAccessibilityProfile::Get();
	TestTrue(
		TEXT("Captions cannot be disabled by the release profile"),
		Profile->AreCaptionsEnabled());
	TestFalse(
		TEXT("Camera shake remains disabled"),
		Profile->IsCameraShakeEnabled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase7RadioTest,
	"AMSim.Phase7.Audio.CaptionsFirstCoverageAndDeduplication",
	EAutomationTestFlags::EditorContext |
		EAutomationTestFlags::EngineFilter)

bool FAMSimPhase7RadioTest::RunTest(
	const FString& Parameters)
{
	const TArray<FAMSimReleasePhraseFamily>& Families =
		FAMSimPhase7ContentCatalog::RequiredPhraseFamilies();
	TestEqual(
		TEXT("All approved phrase families are covered"),
		Families.Num(),
		15);
	TSet<FName> Ids;
	for (const FAMSimReleasePhraseFamily& Family : Families)
	{
		TestFalse(
			TEXT("Phrase family ID is unique"),
			Ids.Contains(Family.FamilyId));
		TestFalse(
			TEXT("Phrase has an exact caption example"),
			Family.ExampleCaption.IsEmpty());
		TestFalse(
			TEXT("Phrase has a plain meaning"),
			Family.PlainMeaning.IsEmpty());
		Ids.Add(Family.FamilyId);
	}
	FAMSimCaptionDeduplicator Deduplicator;
	TestTrue(
		TEXT("First caption is presented"),
		Deduplicator.Accept(
			TEXT("Phase4.Operations"),
			TEXT("REGIONAL OPS: Timetable monitored.")));
	TestFalse(
		TEXT("Unchanged caption is not repeated every refresh"),
		Deduplicator.Accept(
			TEXT("Phase4.Operations"),
			TEXT("REGIONAL OPS: Timetable monitored.")));
	TestTrue(
		TEXT("Changed urgent caption is presented"),
		Deduplicator.Accept(
			TEXT("Phase4.Operations"),
			TEXT("AIRPORT 1: Rescue vehicles entering Runway 27.")));
	TestFalse(
		TEXT("Empty caption is rejected"),
		Deduplicator.Accept(
			TEXT("Phase4.Operations"),
			TEXT("")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase7ContentTest,
	"AMSim.Phase7.Content.ReleaseCatalogAndManifests",
	EAutomationTestFlags::EditorContext |
		EAutomationTestFlags::EngineFilter)

bool FAMSimPhase7ContentTest::RunTest(
	const FString& Parameters)
{
	const FAMSimPhase7CatalogValidation Validation =
		FAMSimPhase7ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(
		TEXT("Phase 7 release Primary Assets validate"),
		Validation.bValid);
	TestEqual(
		TEXT("All required release definitions are loaded"),
		Validation.Assets.Num(),
		FAMSimPhase7ContentCatalog::RequiredContentIds().Num());

	TSharedPtr<FJsonObject> Manifest;
	if (!LoadJson(
		*this,
		TEXT("Config/Phase7/Phase7ReleaseManifest.json"),
		Manifest))
	{
		return false;
	}
	TestEqual(
		TEXT("Release manifest save schema is current"),
		Manifest->GetIntegerField(TEXT("saveSchema")),
		static_cast<int32>(AMSim::SnapshotSchemaVersion));
	TestTrue(
		TEXT("Release manifest requires offline runtime"),
		Manifest->GetObjectField(
			TEXT("runtimeBoundaries"))
			->GetBoolField(TEXT("offlineRequired")));
	TestEqual(
		TEXT("No required 3D assets are declared"),
		Manifest->GetObjectField(
			TEXT("runtimeBoundaries"))
			->GetIntegerField(TEXT("required3DAssets")),
		0);

	TSharedPtr<FJsonObject> Rights;
	if (!LoadJson(
		*this,
		TEXT("Config/Phase7/Phase7RightsInventory.json"),
		Rights))
	{
		return false;
	}
	TestEqual(
		TEXT("No unreviewed runtime rights items remain"),
		Rights->GetArrayField(
			TEXT("unreviewedRuntimeItems")).Num(),
		0);

	TSharedPtr<FJsonObject> Radio;
	if (!LoadJson(
		*this,
		TEXT("Config/Phase7/Phase7RadioPhraseCatalog.json"),
		Radio))
	{
		return false;
	}
	TestTrue(
		TEXT("Radio catalog is captions first"),
		Radio->GetBoolField(TEXT("captionsFirst")));
	TestTrue(
		TEXT("Radio catalog is offline only"),
		Radio->GetBoolField(TEXT("offlineOnly")));
	TestEqual(
		TEXT("JSON and compiled radio catalogs agree"),
		Radio->GetArrayField(TEXT("families")).Num(),
		FAMSimPhase7ContentCatalog::RequiredPhraseFamilies().Num());
	return true;
}
