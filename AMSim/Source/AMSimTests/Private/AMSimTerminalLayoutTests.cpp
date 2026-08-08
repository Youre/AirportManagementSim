#include "AMSimPhase1Fixture.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "Misc/AutomationTest.h"

namespace
{
	using namespace AMSim;

	bool CreateAirport(FAutomationTestBase& Test, FSimulation& Simulation)
	{
		FPhase1Command Command;
		Command.Id = {1};
		Command.Type = EPhase1CommandType::CreateAirport;
		Command.MapId = GetPhase1Fixture().MapId;
		Command.AirportName = TEXT("Terminal Test Field");
		if (!Test.TestEqual(
			TEXT("Airport creation accepted"),
			Simulation.QueuePhase1Command(Command),
			EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	void AdvanceBy(FSimulation& Simulation, const int64 Milliseconds)
	{
		const int64 Target =
			Simulation.CreateQuerySnapshot().GameTimeMilliseconds + Milliseconds;
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds < Target)
		{
			Simulation.Step();
		}
	}

	bool PrepareConstructionTeam(
		FAutomationTestBase& Test,
		FSimulation& Simulation)
	{
		FPhase1Command Build;
		Build.Id = {2};
		Build.Type = EPhase1CommandType::CommitStarterPlan;
		Build.Proposal = CreateDefaultStarterPlan();
		if (!Test.TestEqual(
			TEXT("Starter build accepted"),
			Simulation.QueuePhase1Command(Build),
			EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		AdvanceBy(Simulation, GetPhase1Fixture().ReadyToOpenAtMilliseconds);
		return Test.TestTrue(
			TEXT("Ordinary construction team is available"),
			Simulation.GetPhase1State().Teams.ContainsByPredicate(
				[](const FStaffTeamRecord& Team)
				{
					return Team.RoleId == TEXT("Staff.Role.Construction") &&
						Team.bAvailable;
				}));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimTerminalStarterLayoutTest,
	"AMSim.TerminalGrowth.Layout.StarterReadinessAndUndo",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimTerminalStarterLayoutTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(1701);
	if (!CreateAirport(*this, Simulation))
	{
		return false;
	}
	const FTerminalLayoutState& Starter =
		Simulation.GetPhase3State().TerminalLayout;
	TestTrue(TEXT("Starter layout is seeded"), Starter.bSeeded);
	TestTrue(TEXT("Starter layout is operationally ready"), Starter.bReady);
	TestEqual(TEXT("Starter has two airside gates"), Starter.ValidAirsideGateCount, 2);
	TestTrue(TEXT("Starter has furnished floor"), Starter.FloorCells.Num() >= 200);
	TestTrue(TEXT("Starter has information point"), Starter.bInformationPointAvailable);
	TestTrue(TEXT("Starter has accessible restroom route"), Starter.bAccessibleRestroomRoute);
	const FPhase3QuerySnapshot StarterQuery = Simulation.CreatePhase3QuerySnapshot();
	TestTrue(TEXT("Room boundaries infer multiple stable room records"),
		StarterQuery.TerminalLayout.Rooms.Num() >= 3);
	TestTrue(TEXT("Every inferred room has a stable identity and assigned function"),
		StarterQuery.TerminalLayout.Rooms.ContainsByPredicate(
			[](const FTerminalSpatialRoomRecord& Room)
			{
				return Room.StableId != 0 &&
					Room.Function == ETerminalFloorKind::StaffService;
			}));

	const int32 InitialFloorCount = Starter.FloorCells.Num();
	const int64 InitialCredits = Simulation.GetPhase1State().Credits;
	FPhase3Command AddFloor;
	AddFloor.Id = {1};
	AddFloor.Type = EPhase3CommandType::PlaceTerminalFloor;
	AddFloor.StartCell = {18, 0};
	AddFloor.EndCell = {19, 0};
	TestEqual(
		TEXT("Floor edit accepted"),
		Simulation.QueuePhase3Command(AddFloor),
		EPhase3CommandResult::Accepted);
	Simulation.Step();
	TestEqual(
		TEXT("Floor cells commit immediately"),
		Simulation.GetPhase3State().TerminalLayout.FloorCells.Num(),
		InitialFloorCount + 2);
	TestEqual(
		TEXT("Floor edit spends immediately"),
		Simulation.GetPhase1State().Credits,
		InitialCredits - 50);

	FPhase3Command Undo;
	Undo.Id = {2};
	Undo.Type = EPhase3CommandType::UndoTerminalEdit;
	TestEqual(
		TEXT("Active edit can be undone"),
		Simulation.QueuePhase3Command(Undo),
		EPhase3CommandResult::Accepted);
	Simulation.Step();
	TestEqual(
		TEXT("Undo removes planned floor"),
		Simulation.GetPhase3State().TerminalLayout.FloorCells.Num(),
		InitialFloorCount);
	TestEqual(
		TEXT("Undo fully refunds active work"),
		Simulation.GetPhase1State().Credits,
		InitialCredits);

	const FTerminalPlacedObjectRecord* Seat =
		Simulation.GetPhase3State().TerminalLayout.Objects.FindByPredicate(
			[](const FTerminalPlacedObjectRecord& Object)
			{
				return Object.Kind == ETerminalObjectKind::SeatGroup6;
			});
	if (!TestNotNull(TEXT("Rotatable seating exists"), Seat))
	{
		return false;
	}
	const FTerminalElementId SeatId = Seat->Id;
	const int32 InitialSeatWidth = Seat->FootprintWidth;
	const int32 InitialSeatHeight = Seat->FootprintHeight;
	FPhase3Command Rotate;
	Rotate.Id = {3};
	Rotate.Type = EPhase3CommandType::RotateTerminalObject;
	Rotate.TerminalElementId = SeatId;
	TestEqual(TEXT("Rotation accepted"), Simulation.QueuePhase3Command(Rotate),
		EPhase3CommandResult::Accepted);
	Simulation.Step();
	Seat = Simulation.GetPhase3State().TerminalLayout.Objects.FindByPredicate(
		[SeatId](const FTerminalPlacedObjectRecord& Object) { return Object.Id == SeatId; });
	TestTrue(TEXT("Rotation swaps object footprint"), Seat &&
		Seat->FootprintWidth == InitialSeatHeight &&
		Seat->FootprintHeight == InitialSeatWidth);

	Undo.Id = {4};
	TestEqual(TEXT("Rotation undo accepted"), Simulation.QueuePhase3Command(Undo),
		EPhase3CommandResult::Accepted);
	Simulation.Step();
	Seat = Simulation.GetPhase3State().TerminalLayout.Objects.FindByPredicate(
		[SeatId](const FTerminalPlacedObjectRecord& Object) { return Object.Id == SeatId; });
	return TestTrue(TEXT("Rotation undo restores angle and footprint"), Seat &&
		Seat->QuarterTurns == 0 && Seat->FootprintWidth == InitialSeatWidth &&
		Seat->FootprintHeight == InitialSeatHeight);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimTerminalConstructionAndPersistenceTest,
	"AMSim.TerminalGrowth.Construction.WorkersAndSchema10RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimTerminalConstructionAndPersistenceTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(1702);
	if (!CreateAirport(*this, Simulation) ||
		!PrepareConstructionTeam(*this, Simulation))
	{
		return false;
	}
	FPhase3Command AddFloor;
	AddFloor.Id = {10};
	AddFloor.Type = EPhase3CommandType::PlaceTerminalFloor;
	AddFloor.StartCell = {18, 0};
	AddFloor.EndCell = {18, 0};
	TestEqual(
		TEXT("Construction floor accepted"),
		Simulation.QueuePhase3Command(AddFloor),
		EPhase3CommandResult::Accepted);
	Simulation.Step();
	const FTerminalConstructionJobRecord* InitialJob =
		Simulation.GetPhase3State().TerminalLayout.ConstructionJobs.FindByPredicate(
			[](const FTerminalConstructionJobRecord& Job)
			{
				return Job.Stage != ETerminalConstructionWorkStage::Cancelled;
			});
	TestNotNull(TEXT("Construction job is created"), InitialJob);
	TestTrue(TEXT("Ordinary team is assigned"), InitialJob && InitialJob->AssignedTeamId.IsValid());

	AdvanceBy(Simulation, 38000);
	const FTerminalConstructionJobRecord& CompletedJob =
		Simulation.GetPhase3State().TerminalLayout.ConstructionJobs.Last();
	TestEqual(
		TEXT("Worker-driven job passes inspection"),
		CompletedJob.Stage,
		ETerminalConstructionWorkStage::Completed);
	const FTerminalFloorCellRecord* CompletedFloor =
		Simulation.GetPhase3State().TerminalLayout.FloorCells.FindByPredicate(
			[](const FTerminalFloorCellRecord& Cell) { return Cell.Cell == FTerminalCellCoord{18, 0}; });
	TestTrue(TEXT("Completed floor becomes built"), CompletedFloor && CompletedFloor->bBuilt);
	if (!CompletedFloor)
	{
		return false;
	}
	const FTerminalElementId CompletedFloorId = CompletedFloor->Id;
	const int64 CreditsBeforeDemolition = Simulation.GetPhase1State().Credits;
	FPhase3Command Demolish;
	Demolish.Id = {11};
	Demolish.Type = EPhase3CommandType::DemolishTerminalElement;
	Demolish.TerminalElementId = CompletedFloorId;
	TestEqual(TEXT("Completed floor demolition accepted"),
		Simulation.QueuePhase3Command(Demolish), EPhase3CommandResult::Accepted);
	Simulation.Step();
	AdvanceBy(Simulation, 38000);
	TestFalse(TEXT("Completed demolition removes the element"),
		Simulation.GetPhase3State().TerminalLayout.FloorCells.ContainsByPredicate(
			[CompletedFloorId](const FTerminalFloorCellRecord& Cell)
			{
				return Cell.Id == CompletedFloorId;
			}));
	TestEqual(TEXT("Completed demolition returns 25 percent salvage"),
		Simulation.GetPhase1State().Credits, CreditsBeforeDemolition + 6);

	TArray<uint8> Bytes;
	FSnapshot Loaded;
	TestTrue(TEXT("Schema 10 layout serializes"),
		SerializeSnapshot(Simulation.CreateSnapshot(), Bytes));
	TestTrue(TEXT("Schema 10 layout deserializes"), DeserializeSnapshot(Bytes, Loaded));
	FPassengerTerminalSimulation RestoredTerminal(Loaded.MasterSeed);
	TestTrue(TEXT("Schema 10 terminal domain validates"),
		RestoredTerminal.RestoreState(Loaded.Phase3, Loaded.Phase1, Loaded.Phase2));
	FSimulation Restored;
	TestTrue(TEXT("Schema 10 layout restores"), Restored.RestoreSnapshot(Loaded));
	return TestEqual(
		TEXT("Spatial layout checksum survives save/load"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimTerminalEditValidationTest,
	"AMSim.TerminalGrowth.Layout.EditValidationAndAffordability",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimTerminalEditValidationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(1704);
	if (!CreateAirport(*this, Simulation))
	{
		return false;
	}

	FPhase3Command OverlappingFloor;
	OverlappingFloor.Id = {20};
	OverlappingFloor.Type = EPhase3CommandType::PlaceTerminalFloor;
	OverlappingFloor.StartCell = {0, 0};
	OverlappingFloor.EndCell = {0, 0};
	TestEqual(TEXT("Floor strokes cannot overwrite completed floor"),
		Simulation.QueuePhase3Command(OverlappingFloor),
		EPhase3CommandResult::RejectedOccupied);

	FPhase3Command DiagonalWall;
	DiagonalWall.Id = {21};
	DiagonalWall.Type = EPhase3CommandType::PlaceTerminalWall;
	DiagonalWall.StartCell = {0, 0};
	DiagonalWall.EndCell = {2, 2};
	TestEqual(TEXT("Wall strokes must remain grid aligned"),
		Simulation.QueuePhase3Command(DiagonalWall),
		EPhase3CommandResult::RejectedInvalidTopology);

	const FTerminalPlacedObjectRecord* ExistingObject =
		Simulation.GetPhase3State().TerminalLayout.Objects.FindByPredicate(
			[](const FTerminalPlacedObjectRecord& Object)
			{
				return Object.Kind == ETerminalObjectKind::InformationDesk;
			});
	if (!TestNotNull(TEXT("Starter information desk exists"), ExistingObject))
	{
		return false;
	}
	FPhase3Command OverlappingObject;
	OverlappingObject.Id = {22};
	OverlappingObject.Type = EPhase3CommandType::PlaceTerminalObject;
	OverlappingObject.StartCell = ExistingObject->Anchor;
	OverlappingObject.ObjectKind = ETerminalObjectKind::Vending;
	TestEqual(TEXT("Object footprints cannot overlap furnishings"),
		Simulation.QueuePhase3Command(OverlappingObject),
		EPhase3CommandResult::RejectedOccupied);

	const int64 CreditsBeforeQuote = Simulation.GetPhase1State().Credits;
	FPhase3Command UnaffordableFloor;
	UnaffordableFloor.Id = {23};
	UnaffordableFloor.Type = EPhase3CommandType::PlaceTerminalFloor;
	UnaffordableFloor.StartCell = {40, 40};
	UnaffordableFloor.EndCell = {59, 59};
	TestEqual(TEXT("Unaffordable edit is rejected before mutation"),
		Simulation.QueuePhase3Command(UnaffordableFloor),
		EPhase3CommandResult::RejectedInsufficientCredits);
	Simulation.Step();
	TestEqual(TEXT("Rejected quote never changes the economy ledger"),
		Simulation.GetPhase1State().Credits, CreditsBeforeQuote);
	return TestFalse(TEXT("Rejected quote creates no remote floor cells"),
		Simulation.GetPhase3State().TerminalLayout.FloorCells.ContainsByPredicate(
			[](const FTerminalFloorCellRecord& Cell)
			{
				return Cell.Cell.X >= 40 || Cell.Cell.Y >= 40;
			}));
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimTerminalSchema9MigrationTest,
	"AMSim.TerminalGrowth.Persistence.Schema9SeedsSpatialLayout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimTerminalSchema9MigrationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(1703);
	FSnapshot Legacy = Simulation.CreateSnapshot();
	Legacy.SchemaVersion = 9;
	Legacy.Phase3.TerminalLayout = {};
	TArray<uint8> Bytes;
	FSnapshot Migrated;
	if (!TestTrue(TEXT("Schema 9 fixture serializes"), SerializeSnapshot(Legacy, Bytes)) ||
		!TestTrue(TEXT("Schema 9 fixture migrates"), DeserializeSnapshot(Bytes, Migrated)))
	{
		return false;
	}
	TestEqual(TEXT("Migration advances to schema 10"), Migrated.SchemaVersion, 10u);
	TestTrue(TEXT("Migration seeds furnished terminal"), Migrated.Phase3.TerminalLayout.bSeeded);
	TestTrue(TEXT("Migrated starter remains ready"), Migrated.Phase3.TerminalLayout.bReady);
	return TestTrue(
		TEXT("Migrated terminal IDs advance the phase domain allocator"),
		Migrated.Phase3.NextDomainId > (1ull << 48));
}
