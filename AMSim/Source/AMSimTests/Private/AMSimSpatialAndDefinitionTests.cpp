#include "AMSimDefinitions.h"
#include "AMSimSpatial.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimChunkAddressTest,
	"AMSim.Phase0.Spatial.ChunkAddress",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimChunkAddressTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Origin chunk"), AMSim::GetChunkAddress({0, 0}).X, 0);
	TestEqual(TEXT("Positive chunk boundary"), AMSim::GetChunkAddress({6400, 0}).X, 1);
	TestEqual(TEXT("Negative centimeters floor to negative chunk"), AMSim::GetChunkAddress({-1, 0}).X, -1);
	TestEqual(TEXT("Last cell in negative chunk"), AMSim::GetPlanningCellIndex({-1, -1}), 4095);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimBaseOccupancyTest,
	"AMSim.Phase0.Spatial.BaseOccupancy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimBaseOccupancyTest::RunTest(const FString& Parameters)
{
	AMSim::FSpatialGrid Grid;
	const AMSim::FIntPoint64 Position{-1, 6400};
	const AMSim::FChunkAddress Address = AMSim::GetChunkAddress(Position);
	TestFalse(TEXT("Inactive chunks consume no occupancy storage"), Grid.IsChunkAllocated(Address));
	TestFalse(
		TEXT("Clearing an inactive cell is a no-op"),
		Grid.SetOccupied(Position, AMSim::ESpatialLayer::StructureOccupancy, false));
	TestTrue(
		TEXT("Setting occupancy allocates the active chunk"),
		Grid.SetOccupied(Position, AMSim::ESpatialLayer::StructureOccupancy, true));
	TestTrue(
		TEXT("Layer occupancy is readable"),
		Grid.IsOccupied(Position, AMSim::ESpatialLayer::StructureOccupancy));
	TestFalse(
		TEXT("Occupancy remains separated by layer"),
		Grid.IsOccupied(Position, AMSim::ESpatialLayer::Walkability));
	TestEqual(TEXT("First mutation increments chunk revision"), Grid.GetChunkRevision(Address), 1ull);
	TestFalse(
		TEXT("Writing an identical value does not dirty the chunk"),
		Grid.SetOccupied(Position, AMSim::ESpatialLayer::StructureOccupancy, true));
	TestEqual(TEXT("No-op writes preserve revision"), Grid.GetChunkRevision(Address), 1ull);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimReservationOrderTest,
	"AMSim.Phase0.Spatial.ReservationOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimReservationOrderTest::RunTest(const FString& Parameters)
{
	const TArray<AMSim::FReservationRequest> Ordered = AMSim::ResolveReservationOrder({
		{{3}, 1, 2, 0, 1},
		{{2}, 2, 3, 0, 1},
		{{1}, 2, 1, 0, 1}
	});
	TestEqual(TEXT("Higher priority, earlier sequence wins"), Ordered[0].Owner.Value, 1ull);
	TestEqual(TEXT("Higher priority, later sequence follows"), Ordered[1].Owner.Value, 2ull);
	TestEqual(TEXT("Lower priority follows"), Ordered[2].Owner.Value, 3ull);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimDefinitionValidationTest,
	"AMSim.Phase0.Content.DefinitionValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimDefinitionValidationTest::RunTest(const FString& Parameters)
{
	AMSim::FDefinition Definition;
	TestFalse(TEXT("Empty provenance definition is rejected"), AMSim::ValidateDefinition(Definition, true).bValid);
	Definition.StableId = TEXT("Aircraft.Cessna152");
	Definition.OwningPhase = TEXT("Phase1");
	Definition.Provenance = AMSim::EProvenanceStatus::InternalPrototype;
	Definition.SourcePath = TEXT("source.png");
	Definition.SourceChecksum = TEXT("sha256:test");
	Definition.Tags.Add(TEXT("Aircraft.Role.GeneralAviation"));
	TestTrue(TEXT("Complete provenance definition is accepted"), AMSim::ValidateDefinition(Definition, true).bValid);

	AMSim::FDefinition Duplicate = Definition;
	Duplicate.Tags = {TEXT("Unknown.Root.Value")};
	const AMSim::FDefinitionValidation Registry =
		AMSim::ValidateDefinitionRegistry({Definition, Duplicate}, true);
	TestFalse(TEXT("Registry detects duplicate IDs and unknown tag roots"), Registry.bValid);
	return true;
}

#endif
