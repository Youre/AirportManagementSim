#include "AMSimAircraftPresentation.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	using namespace AMSim;

	FPhase1Command MakeCommand(const uint64 Id, const EPhase1CommandType Type)
	{
		FPhase1Command Command;
		Command.Id = {Id};
		Command.Type = Type;
		return Command;
	}

	bool QueueAndStep(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		const FPhase1Command& Command,
		const TCHAR* Context)
	{
		const EPhase1CommandResult Result = Simulation.QueuePhase1Command(Command);
		if (!Test.TestEqual(Context, Result, EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	void AdvanceTo(FSimulation& Simulation, const int64 TargetMilliseconds)
	{
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds < TargetMilliseconds)
		{
			Simulation.Step();
		}
	}

	bool CreateAndBuildStarterAirfield(FAutomationTestBase& Test, FSimulation& Simulation)
	{
		FPhase1Command Create = MakeCommand(1, EPhase1CommandType::CreateAirport);
		Create.AirportName = TEXT("Riverbend Field");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueueAndStep(Test, Simulation, Create, TEXT("Create airport accepted")))
		{
			return false;
		}

		FPhase1Command Build = MakeCommand(2, EPhase1CommandType::CommitStarterPlan);
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueueAndStep(Test, Simulation, Build, TEXT("Starter plan accepted")))
		{
			return false;
		}
		const int64 ReadyAt =
			Simulation.CreateSnapshot().Phase1.Project.FundedAtGameMilliseconds +
			GetPhase1Fixture().ReadyToOpenAtMilliseconds;
		AdvanceTo(Simulation, ReadyAt);
		return Test.TestEqual(
			TEXT("Construction reaches ready-to-open"),
			Simulation.GetPhase1State().Project.Stage,
			EConstructionStage::ReadyToOpen);
	}

	bool OpenAcceptAndSchedule(FAutomationTestBase& Test, FSimulation& Simulation)
	{
		if (!QueueAndStep(
			Test,
			Simulation,
			MakeCommand(3, EPhase1CommandType::OpenAirport),
			TEXT("Open airport accepted")))
		{
			return false;
		}
		if (!Test.TestTrue(TEXT("Airport remains paused after opening"), Simulation.GetPhase1State().bPaused) ||
			!Test.TestEqual(
				TEXT("Starter offer is immediate"),
				Simulation.GetPhase1State().Offer.State,
				EOfferState::Available))
		{
			return false;
		}
		if (!QueueAndStep(
			Test,
			Simulation,
			MakeCommand(4, EPhase1CommandType::AcceptStarterOffer),
			TEXT("Offer accepted")))
		{
			return false;
		}

		FPhase1Command Schedule = MakeCommand(5, EPhase1CommandType::ScheduleStarterFlight);
		Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
		const int64 Now = Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
		const int64 Increment = GetPhase1Fixture().TimetableIncrementMilliseconds;
		Schedule.ScheduledArrivalGameMilliseconds = ((Now + Increment - 1) / Increment) * Increment;
		if (!QueueAndStep(Test, Simulation, Schedule, TEXT("Recommended slot accepted")))
		{
			return false;
		}
		return Test.TestTrue(
			TEXT("Arrival is within three game minutes of schedule confirmation"),
			Schedule.ScheduledArrivalGameMilliseconds - Now <=
				GetPhase1Fixture().MaximumArrivalDelayMilliseconds);
	}

	bool VerifyRoundTripContinuation(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		const FString& Context)
	{
		TArray<uint8> Bytes;
		if (!Test.TestTrue(
			*FString::Printf(TEXT("%s serializes"), *Context),
			SerializeSnapshot(Simulation.CreateSnapshot(), Bytes)))
		{
			return false;
		}
		FSnapshot Snapshot;
		FSimulation Restored;
		if (!Test.TestTrue(
				*FString::Printf(TEXT("%s deserializes"), *Context),
				DeserializeSnapshot(Bytes, Snapshot)) ||
			!Test.TestTrue(
				*FString::Printf(TEXT("%s restores"), *Context),
				Restored.RestoreSnapshot(Snapshot)) ||
			!Test.TestEqual(
				*FString::Printf(TEXT("%s immediate checksum"), *Context),
				Restored.CalculateChecksum(),
				Simulation.CalculateChecksum()))
		{
			return false;
		}
		for (int32 StepIndex = 0; StepIndex < 10; ++StepIndex)
		{
			Simulation.Step();
			Restored.Step();
		}
		return Test.TestEqual(
			*FString::Printf(TEXT("%s continued checksum"), *Context),
			Restored.CalculateChecksum(),
			Simulation.CalculateChecksum());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1FixtureTest,
	"AMSim.Phase1.Foundation.FixtureAndContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1FixtureTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	const FPhase1Fixture& Fixture = GetPhase1Fixture();
	TestEqual(TEXT("Stable scenario ID"), Fixture.ScenarioId, FName(TEXT("S01.StarterGrassAirfield")));
	TestEqual(TEXT("Stable map ID"), Fixture.MapId, FName(TEXT("Map.TemperateStarter")));
	TestEqual(TEXT("Stable aircraft ID"), Fixture.AircraftId, FName(TEXT("Aircraft.LightPiston.Starter")));
	TestTrue(TEXT("Starter plan stays within maximum"), Fixture.StarterPlanCost <= Fixture.MaximumStarterPlanCost);
	TestTrue(
		TEXT("Starter plan leaves at least 30 percent contingency"),
		CalculateContingencyPercent(Fixture.StartingCredits, Fixture.StarterPlanCost) >=
			Fixture.MinimumContingencyPercent);
	TestTrue(TEXT("Definition catalog validates"), ValidatePhase1FixtureDefinitions().bValid);
	TestTrue(TEXT("Default starter proposal validates"), ValidateStarterPlan(CreateDefaultStarterPlan()).bValid);
	const FStarterPlanProposal DefaultPlan = CreateDefaultStarterPlan();
	TestEqual(
		TEXT("Reviewed starter runway is exactly 600 m"),
		DefaultPlan.RunwayEnd.X - DefaultPlan.RunwayStart.X,
		static_cast<int64>(60000));
	TestEqual(
		TEXT("Starter runway is 20 m wide"),
		DefaultPlan.RunwayWidthCentimeters,
		2000);

	FStarterPlanProposal TooShort = DefaultPlan;
	TooShort.RunwayEnd.X -= 100;
	const FPhase1Validation TooShortResult = ValidateStarterPlan(TooShort);
	TestFalse(TEXT("A 599 m starter runway is rejected"), TooShortResult.bValid);
	TestEqual(
		TEXT("Short runway reason is stable"),
		TooShortResult.ReasonCode,
		FName(TEXT("Build.Runway.TooSmall")));
	TestTrue(
		TEXT("Short runway remedy names the reviewed minimum"),
		TooShortResult.Remedy.Contains(TEXT("600 m")));

	FStarterPlanProposal Outside = CreateDefaultStarterPlan();
	Outside.OperationsHutCenter.X = 110000;
	const FPhase1Validation OutsideResult = ValidateStarterPlan(Outside);
	TestEqual(
		TEXT("Outside-land result is stable"),
		OutsideResult.Result,
		EPhase1CommandResult::RejectedOutsideOwnedLand);
	TestFalse(TEXT("Outside-land plan is rejected"), OutsideResult.bValid);
	TestFalse(TEXT("Outside-land cause is present"), OutsideResult.Cause.IsEmpty());
	TestFalse(TEXT("Outside-land remedy is present"), OutsideResult.Remedy.IsEmpty());

	FString FixtureJson;
	const FString FixturePath = FPaths::Combine(
		FPaths::ProjectConfigDir(),
		TEXT("Phase1/S01-StarterGrassAirfield.json"));
	TestTrue(TEXT("Machine-readable S01 fixture exists"), FFileHelper::LoadFileToString(FixtureJson, *FixturePath));
	TestTrue(TEXT("Fixture declares no required 3D asset"), FixtureJson.Contains(TEXT("no_required_3d_gameplay_asset")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1ConstructionTest,
	"AMSim.Phase1.Construction.StarterAirfield",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1ConstructionTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation))
	{
		return false;
	}
	const FPhase1State& State = Simulation.GetPhase1State();
	TestEqual(TEXT("Exact plan cost reserved"), State.Project.QuotedCost, static_cast<int64>(3400));
	TestEqual(TEXT("Credits preserve contingency"), State.Credits, static_cast<int64>(1600));
	TestTrue(TEXT("Materials arrived"), State.Project.bDeliveryArrived);
	TestTrue(TEXT("Inspection passed"), State.Project.bInspectionPassed);
	TestEqual(TEXT("Six starter facilities built"), State.Facilities.Num(), 6);
	TestEqual(TEXT("Three starter teams exist"), State.Teams.Num(), 3);
	for (const FFacilityRecord& Facility : State.Facilities)
	{
		TestTrue(TEXT("Facility is built"), Facility.bBuilt);
		TestFalse(TEXT("Facility remains closed until player opens"), Facility.bOpen);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1JourneyTest,
	"AMSim.Phase1.Integration.S01StarterGrassAirfield",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1JourneyTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation) ||
		!OpenAcceptAndSchedule(*this, Simulation))
	{
		return false;
	}
	const int64 CompletionAt =
		Simulation.GetPhase1State().Flight.ScheduledArrivalGameMilliseconds + 130000;
	AdvanceTo(Simulation, CompletionAt);

	const FPhase1State& State = Simulation.GetPhase1State();
	TestEqual(TEXT("First visit completes"), State.Flight.State, EFlightState::Completed);
	TestEqual(TEXT("Airframe visit history persists"), State.Airframe.VisitCount, 1);
	TestEqual(
		TEXT("Airframe uses the approved fictional registration"),
		State.Airframe.TailNumber,
		FString(TEXT("RB-021")));
	TestTrue(TEXT("Inspection completed"), State.Flight.Inspection == EServiceTaskState::Completed);
	TestTrue(TEXT("Fueling completed"), State.Flight.Fueling == EServiceTaskState::Completed);
	TestTrue(TEXT("Reward recognized once"), State.Flight.bRewardRecognized);
	TestEqual(TEXT("Credits reconcile"), State.Credits, static_cast<int64>(2200));
	TestEqual(TEXT("Established capability awards five points"), State.AirportPoints, 5);
	TestEqual(TEXT("Construction and flight transactions recorded"), State.Transactions.Num(), 2);
	TestEqual(TEXT("Path-neutral rating contributions recorded"), State.RatingContributions.Num(), 2);
	TestTrue(TEXT("Essential captioned calls emitted"), State.PhraseIntents.Num() >= 5);
	TestTrue(TEXT("No movement reservation remains after departure"), State.Reservations.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1SaveMigrationTest,
	"AMSim.Phase1.Save.Schema2MigrationAndContinuation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1SaveMigrationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation) ||
		!OpenAcceptAndSchedule(*this, Simulation))
	{
		return false;
	}
	const int64 TurnaroundAt =
		Simulation.GetPhase1State().Flight.ScheduledArrivalGameMilliseconds + 60000;
	AdvanceTo(Simulation, TurnaroundAt);
	TestEqual(TEXT("Fixture reaches turnaround"), Simulation.GetPhase1State().Flight.State, EFlightState::Turnaround);

	TArray<uint8> Bytes;
	TestTrue(TEXT("Schema 2 snapshot serializes"), SerializeSnapshot(Simulation.CreateSnapshot(), Bytes));
	FSnapshot Loaded;
	TestTrue(TEXT("Schema 2 snapshot deserializes"), DeserializeSnapshot(Bytes, Loaded));
	TestEqual(TEXT("Loaded snapshot is schema 2"), Loaded.SchemaVersion, static_cast<uint32>(2));

	FSimulation Restored;
	TestTrue(TEXT("Schema 2 snapshot restores"), Restored.RestoreSnapshot(Loaded));
	const int64 CompletionAt = Loaded.Phase1.Flight.ScheduledArrivalGameMilliseconds + 130000;
	AdvanceTo(Simulation, CompletionAt);
	AdvanceTo(Restored, CompletionAt);
	TestEqual(
		TEXT("Post-load continuation checksum matches uninterrupted run"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());

	FSnapshot Schema1;
	Schema1.SchemaVersion = 1;
	Schema1.MasterSeed = 77;
	Schema1.NextEntityId = 1;
	Schema1.NextEventSequence = 1;
	Schema1.Revision = 4;
	Schema1.GameTimeMilliseconds = 1000;
	TArray<uint8> Schema1Bytes;
	TestTrue(TEXT("Schema 1 fixture serializes"), SerializeSnapshot(Schema1, Schema1Bytes));
	FSnapshot Migrated;
	TestTrue(TEXT("Schema 1 fixture migrates during load"), DeserializeSnapshot(Schema1Bytes, Migrated));
	TestEqual(TEXT("Migrated schema is 2"), Migrated.SchemaVersion, static_cast<uint32>(2));
	TestFalse(TEXT("Migration does not invent a completed airport"), Migrated.Phase1.bInitialized);
	TestEqual(TEXT("Migration preserves master seed"), Migrated.Phase1.MasterSeed, static_cast<uint64>(77));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1SaveBoundaryMatrixTest,
	"AMSim.Phase1.Save.BoundaryContinuationMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1SaveBoundaryMatrixTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	FPhase1Command Create = MakeCommand(1, EPhase1CommandType::CreateAirport);
	Create.AirportName = TEXT("Riverbend Field");
	Create.MapId = GetPhase1Fixture().MapId;
	if (!QueueAndStep(*this, Simulation, Create, TEXT("Create accepted")) ||
		!VerifyRoundTripContinuation(*this, Simulation, TEXT("Paused before construction")))
	{
		return false;
	}

	FPhase1Command Build = MakeCommand(2, EPhase1CommandType::CommitStarterPlan);
	Build.Proposal = CreateDefaultStarterPlan();
	if (!QueueAndStep(*this, Simulation, Build, TEXT("Build accepted")) ||
		!VerifyRoundTripContinuation(*this, Simulation, TEXT("Awaiting delivery")))
	{
		return false;
	}
	const int64 FundedAt = Simulation.GetPhase1State().Project.FundedAtGameMilliseconds;
	for (const TPair<int64, FString>& Boundary : {
		TPair<int64, FString>(GetPhase1Fixture().BuildingAtMilliseconds, TEXT("Building")),
		TPair<int64, FString>(GetPhase1Fixture().InspectionAtMilliseconds, TEXT("Inspection")),
		TPair<int64, FString>(GetPhase1Fixture().ReadyToOpenAtMilliseconds, TEXT("Ready to open"))})
	{
		AdvanceTo(Simulation, FundedAt + Boundary.Key);
		if (!VerifyRoundTripContinuation(*this, Simulation, Boundary.Value))
		{
			return false;
		}
	}

	if (!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(3, EPhase1CommandType::OpenAirport),
			TEXT("Open accepted")) ||
		!VerifyRoundTripContinuation(*this, Simulation, TEXT("Airport open")) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(4, EPhase1CommandType::AcceptStarterOffer),
			TEXT("Offer accepted")))
	{
		return false;
	}
	FPhase1Command Schedule = MakeCommand(5, EPhase1CommandType::ScheduleStarterFlight);
	Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
	const int64 Now = Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
	const int64 Increment = GetPhase1Fixture().TimetableIncrementMilliseconds;
	Schedule.ScheduledArrivalGameMilliseconds = ((Now + Increment - 1) / Increment) * Increment;
	if (!QueueAndStep(*this, Simulation, Schedule, TEXT("Schedule accepted")))
	{
		return false;
	}
	const int64 Arrival = Schedule.ScheduledArrivalGameMilliseconds;
	for (const TPair<int64, FString>& Boundary : {
		TPair<int64, FString>(Arrival - 30000, TEXT("Inbound")),
		TPair<int64, FString>(Arrival + 30000, TEXT("Taxi in")),
		TPair<int64, FString>(Arrival + 60000, TEXT("Turnaround")),
		TPair<int64, FString>(Arrival + 120000, TEXT("Outbound")),
		TPair<int64, FString>(Arrival + 130000, TEXT("Reward recognized"))})
	{
		AdvanceTo(Simulation, Boundary.Key);
		if (!VerifyRoundTripContinuation(*this, Simulation, Boundary.Value))
		{
			return false;
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1ValidationContractTest,
	"AMSim.Phase1.Validation.CommandAndGeometryContracts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1ValidationContractTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	FPhase1Command InvalidName = MakeCommand(1, EPhase1CommandType::CreateAirport);
	InvalidName.MapId = GetPhase1Fixture().MapId;
	TestEqual(
		TEXT("Blank airport name is rejected"),
		Simulation.QueuePhase1Command(InvalidName),
		EPhase1CommandResult::RejectedInvalidName);
	FPhase1Command WrongMap = InvalidName;
	WrongMap.AirportName = TEXT("Riverbend Field");
	WrongMap.MapId = TEXT("Map.DoesNotExist");
	TestEqual(
		TEXT("Unknown map is rejected"),
		Simulation.QueuePhase1Command(WrongMap),
		EPhase1CommandResult::RejectedInvalidCommand);
	FPhase1Command EarlyBuild = MakeCommand(2, EPhase1CommandType::CommitStarterPlan);
	EarlyBuild.Proposal = CreateDefaultStarterPlan();
	TestEqual(
		TEXT("Construction before airport creation is rejected"),
		Simulation.QueuePhase1Command(EarlyBuild),
		EPhase1CommandResult::RejectedInvalidState);

	FStarterPlanProposal Disconnected = CreateDefaultStarterPlan();
	Disconnected.TaxiStart = {1000, 1000};
	const FPhase1Validation DisconnectedResult = ValidateStarterPlan(Disconnected);
	TestFalse(TEXT("Disconnected proposal is invalid"), DisconnectedResult.bValid);
	TestEqual(
		TEXT("Disconnected result is stable"),
		DisconnectedResult.Result,
		EPhase1CommandResult::RejectedDisconnected);
	TestFalse(TEXT("Disconnected cause is present"), DisconnectedResult.Cause.IsEmpty());
	TestFalse(TEXT("Disconnected remedy is present"), DisconnectedResult.Remedy.IsEmpty());

	FPhase1Command InvalidSpeed = MakeCommand(3, EPhase1CommandType::SetSpeed);
	InvalidSpeed.SpeedMultiplier = 16;
	TestEqual(
		TEXT("Unsupported speed is rejected"),
		Simulation.QueuePhase1Command(InvalidSpeed),
		EPhase1CommandResult::RejectedInvalidCommand);
	TestTrue(TEXT("Rejected commands do not mutate state"), !Simulation.GetPhase1State().bInitialized);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1SecondaryLifecycleTest,
	"AMSim.Phase1.Contracts.SecondaryCommandLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1SecondaryLifecycleTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	FPhase1Command Create = MakeCommand(1, EPhase1CommandType::CreateAirport);
	Create.AirportName = TEXT("Riverbend Field");
	Create.MapId = GetPhase1Fixture().MapId;
	FPhase1Command Build = MakeCommand(2, EPhase1CommandType::CommitStarterPlan);
	Build.Proposal = CreateDefaultStarterPlan();
	if (!QueueAndStep(*this, Simulation, Create, TEXT("Create accepted")) ||
		!QueueAndStep(*this, Simulation, Build, TEXT("First build accepted")) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(3, EPhase1CommandType::CancelStarterPlan),
			TEXT("Pre-delivery cancellation accepted")))
	{
		return false;
	}
	TestEqual(TEXT("Cancellation refunds full starter price"), Simulation.GetPhase1State().Credits, static_cast<int64>(5000));
	TestEqual(TEXT("Cancellation resets project"), Simulation.GetPhase1State().Project.Stage, EConstructionStage::None);
	TestEqual(TEXT("Commit and refund remain itemized"), Simulation.GetPhase1State().Transactions.Num(), 2);

	Build.Id = {4};
	if (!QueueAndStep(*this, Simulation, Build, TEXT("Rebuild accepted")))
	{
		return false;
	}
	const int64 ReadyAt =
		Simulation.GetPhase1State().Project.FundedAtGameMilliseconds +
		GetPhase1Fixture().ReadyToOpenAtMilliseconds;
	AdvanceTo(Simulation, ReadyAt);
	TestEqual(
		TEXT("Late cancellation is rejected"),
		Simulation.QueuePhase1Command(MakeCommand(5, EPhase1CommandType::CancelStarterPlan)),
		EPhase1CommandResult::RejectedInvalidState);
	if (!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(6, EPhase1CommandType::OpenAirport),
			TEXT("Open accepted")) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(7, EPhase1CommandType::PinStarterOffer),
			TEXT("Pin accepted")))
	{
		return false;
	}
	TestTrue(TEXT("Pinned offer is authoritative"), Simulation.GetPhase1State().Offer.bPinned);
	if (!VerifyRoundTripContinuation(*this, Simulation, TEXT("Pinned offer")) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(8, EPhase1CommandType::DeclineStarterOffer),
			TEXT("Decline accepted")))
	{
		return false;
	}
	TestEqual(TEXT("Offer records decline"), Simulation.GetPhase1State().Offer.State, EOfferState::Declined);
	if (!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(9, EPhase1CommandType::CloseAirport),
			TEXT("Close accepted")))
	{
		return false;
	}
	TestFalse(TEXT("Airport is closed"), Simulation.GetPhase1State().bAirportOpen);
	TestEqual(TEXT("Closed airfield can be reopened"), Simulation.GetPhase1State().Project.Stage, EConstructionStage::ReadyToOpen);
	for (const FFacilityRecord& Facility : Simulation.GetPhase1State().Facilities)
	{
		TestFalse(TEXT("Closure closes every starter facility"), Facility.bOpen);
	}
	if (!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(10, EPhase1CommandType::OpenAirport),
			TEXT("Reopen accepted")) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(11, EPhase1CommandType::AcceptStarterOffer),
			TEXT("Fresh offer accepted")))
	{
		return false;
	}

	const int64 Now = Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
	const int64 Increment = GetPhase1Fixture().TimetableIncrementMilliseconds;
	FPhase1Command Schedule = MakeCommand(12, EPhase1CommandType::ScheduleStarterFlight);
	Schedule.ScheduledArrivalGameMilliseconds = ((Now + Increment - 1) / Increment) * Increment;
	Schedule.RequestedStandDefinitionId = TEXT("Facility.DoesNotExist");
	TestEqual(
		TEXT("Unknown stand is rejected"),
		Simulation.QueuePhase1Command(Schedule),
		EPhase1CommandResult::RejectedIncompatible);
	Schedule.Id = {13};
	Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
	if (!QueueAndStep(*this, Simulation, Schedule, TEXT("Explicit Stand A1 schedule accepted")))
	{
		return false;
	}
	const FFlightRecord& Flight = Simulation.GetPhase1State().Flight;
	TestTrue(TEXT("Scheduled stand instance is assigned"), Flight.AssignedStand.IsValid());
	TestTrue(
		TEXT("Arrival buffer precedes the exact slot"),
		Flight.StandOccupancyStartGameMilliseconds < Flight.ScheduledArrivalGameMilliseconds);
	TestTrue(
		TEXT("Departure buffer follows the operation"),
		Flight.StandOccupancyEndGameMilliseconds >
			Flight.ScheduledArrivalGameMilliseconds + 130000);
	TestEqual(
		TEXT("Airport cannot close with an active scheduled flight"),
		Simulation.QueuePhase1Command(MakeCommand(14, EPhase1CommandType::CloseAirport)),
		EPhase1CommandResult::RejectedInvalidState);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1MovementSafetyTest,
	"AMSim.Phase1.Operations.MovementServiceAndPhraseSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1MovementSafetyTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation) ||
		!OpenAcceptAndSchedule(*this, Simulation))
	{
		return false;
	}

	EFlightState PreviousState = Simulation.GetPhase1State().Flight.State;
	TSet<EFlightState> ObservedStates;
	const int64 Completion =
		Simulation.GetPhase1State().Flight.ScheduledArrivalGameMilliseconds + 130000;
	while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds < Completion)
	{
		Simulation.Step();
		const FPhase1State& State = Simulation.GetPhase1State();
		if (State.Flight.State != PreviousState)
		{
			TestTrue(
				TEXT("Flight progression never moves backward"),
				static_cast<uint8>(State.Flight.State) > static_cast<uint8>(PreviousState));
			PreviousState = State.Flight.State;
		}
		ObservedStates.Add(State.Flight.State);
		TestTrue(TEXT("Only one aircraft owns movement blocks"), State.Reservations.Num() <= 1);
		if (!State.Reservations.IsEmpty())
		{
			TestEqual(
				TEXT("Reservation belongs to persistent airframe"),
				State.Reservations[0].Owner.Value,
				State.Airframe.Id.Value);
		}
		if (State.Flight.State == EFlightState::Turnaround)
		{
			TestEqual(TEXT("Inspection is active"), State.Flight.Inspection, EServiceTaskState::Active);
			TestEqual(TEXT("Fueling is active"), State.Flight.Fueling, EServiceTaskState::Active);
			TestFalse(TEXT("Reward is not early"), State.Flight.bRewardRecognized);
		}
		if (State.Flight.State == EFlightState::Ready)
		{
			TestEqual(TEXT("Inspection completes before departure"), State.Flight.Inspection, EServiceTaskState::Completed);
			TestEqual(TEXT("Fueling completes before departure"), State.Flight.Fueling, EServiceTaskState::Completed);
		}
	}
	TestTrue(TEXT("Landing observed"), ObservedStates.Contains(EFlightState::Landing));
	TestTrue(TEXT("Taxi-in observed"), ObservedStates.Contains(EFlightState::TaxiIn));
	TestTrue(TEXT("Turnaround observed"), ObservedStates.Contains(EFlightState::Turnaround));
	TestTrue(TEXT("Taxi-out observed"), ObservedStates.Contains(EFlightState::TaxiOut));
	TestTrue(TEXT("Takeoff observed"), ObservedStates.Contains(EFlightState::Takeoff));

	TSet<FName> PhraseIds;
	const TMap<FName, FString> ExpectedCaptions = {
		{TEXT("Radio.ArrivalContact"), TEXT("Riverbend Tower, Riverbend 21, inbound for landing.")},
		{TEXT("Radio.LandingClearance"), TEXT("Riverbend 21, Runway 09 cleared to land.")},
		{TEXT("Radio.TaxiRoute"), TEXT("Riverbend 21, taxi via Taxiway A to Stand A1.")},
		{TEXT("Radio.TaxiDeparture"), TEXT("Riverbend 21, taxi via Taxiway A, hold short of Runway 09.")},
		{TEXT("Radio.TakeoffClearance"), TEXT("Riverbend 21, Runway 09 cleared for takeoff.")}
	};
	for (const FPhraseIntentRecord& Phrase : Simulation.GetPhase1State().PhraseIntents)
	{
		PhraseIds.Add(Phrase.PhraseId);
		TestFalse(TEXT("Every phrase has a caption"), Phrase.Caption.IsEmpty());
		if (const FString* ExpectedCaption = ExpectedCaptions.Find(Phrase.PhraseId))
		{
			TestEqual(
				*FString::Printf(TEXT("Reviewed caption: %s"), *Phrase.PhraseId.ToString()),
				Phrase.Caption,
				*ExpectedCaption);
		}
	}
	for (const FName RequiredPhrase : {
		FName(TEXT("Radio.ArrivalContact")),
		FName(TEXT("Radio.LandingClearance")),
		FName(TEXT("Radio.TaxiRoute")),
		FName(TEXT("Radio.TaxiDeparture")),
		FName(TEXT("Radio.TakeoffClearance"))})
	{
		TestTrue(
			*FString::Printf(TEXT("Required phrase emitted: %s"), *RequiredPhrase.ToString()),
			PhraseIds.Contains(RequiredPhrase));
	}

	const TMap<EFlightState, float> ExpectedHeadings = {
		{EFlightState::Scheduled, 135.0f},
		{EFlightState::Inbound, 135.0f},
		{EFlightState::Approach, 135.0f},
		{EFlightState::Landing, 90.0f},
		{EFlightState::RunwayRoll, 90.0f},
		{EFlightState::TaxiIn, 135.0f},
		{EFlightState::Parked, 180.0f},
		{EFlightState::Turnaround, 180.0f},
		{EFlightState::Ready, 180.0f},
		{EFlightState::TaxiOut, 45.0f},
		{EFlightState::Takeoff, 90.0f},
		{EFlightState::Outbound, 90.0f}
	};
	for (const TPair<EFlightState, float>& Pair : ExpectedHeadings)
	{
		const float Heading = GetPhase1AircraftPresentationHeadingDegrees(Pair.Key);
		TestEqual(TEXT("Aircraft proxy heading follows flight state"), Heading, Pair.Value);
		TestTrue(
			TEXT("Aircraft heading lies on the approved 16-direction grid"),
			FMath::IsNearlyZero(FMath::Fmod(Heading, 22.5f)));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1FlightCatchUpTest,
	"AMSim.Phase1.Movement.CatchUpPreservesIntermediateEffects",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1FlightCatchUpTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation) ||
		!OpenAcceptAndSchedule(*this, Simulation))
	{
		return false;
	}

	FSnapshot CatchUp = Simulation.CreateSnapshot();
	CatchUp.GameTimeMilliseconds =
		CatchUp.Phase1.Flight.ScheduledArrivalGameMilliseconds + 129750;
	CatchUp.Revision =
		static_cast<uint64>(CatchUp.GameTimeMilliseconds / FixedStepMilliseconds);
	FSimulation Restored;
	if (!TestTrue(TEXT("Scheduled catch-up fixture restores"), Restored.RestoreSnapshot(CatchUp)))
	{
		return false;
	}
	Restored.Step();

	const FPhase1State& State = Restored.GetPhase1State();
	TestEqual(TEXT("Catch-up reaches completion"), State.Flight.State, EFlightState::Completed);
	TestEqual(TEXT("Inspection completes during catch-up"), State.Flight.Inspection, EServiceTaskState::Completed);
	TestEqual(TEXT("Fueling completes during catch-up"), State.Flight.Fueling, EServiceTaskState::Completed);
	TestTrue(TEXT("Reward is recognized during catch-up"), State.Flight.bRewardRecognized);

	TSet<FName> PhraseIds;
	for (const FPhraseIntentRecord& Phrase : State.PhraseIntents)
	{
		PhraseIds.Add(Phrase.PhraseId);
	}
	for (const FName RequiredPhrase : {
		FName(TEXT("Radio.ArrivalContact")),
		FName(TEXT("Radio.LandingClearance")),
		FName(TEXT("Radio.TaxiRoute")),
		FName(TEXT("Radio.TaxiDeparture")),
		FName(TEXT("Radio.TakeoffClearance"))})
	{
		TestTrue(
			*FString::Printf(TEXT("Catch-up emits required phrase: %s"), *RequiredPhrase.ToString()),
			PhraseIds.Contains(RequiredPhrase));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1RecoveryTest,
	"AMSim.Phase1.Economy.ZeroCreditRecovery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1RecoveryTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, Simulation) ||
		!QueueAndStep(
			*this,
			Simulation,
			MakeCommand(3, EPhase1CommandType::OpenAirport),
			TEXT("Open airport accepted")))
	{
		return false;
	}

	FSnapshot ZeroCredit = Simulation.CreateSnapshot();
	ZeroCredit.Phase1.Credits = 0;
	FSimulation Restored;
	TestTrue(TEXT("Zero-credit safe airfield restores"), Restored.RestoreSnapshot(ZeroCredit));
	TestTrue(
		TEXT("Recovery command accepted"),
		QueueAndStep(
			*this,
			Restored,
			MakeCommand(4, EPhase1CommandType::RequestRecovery),
			TEXT("Recovery command accepted")));
	TestEqual(TEXT("Recovery grant restores 2,000 Credits"), Restored.GetPhase1State().Credits, static_cast<int64>(2000));
	TestEqual(TEXT("Recovery is recorded once"), Restored.GetPhase1State().RecoveryGrantCount, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase1ReplayTest,
	"AMSim.Phase1.Determinism.S01Replay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase1ReplayTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation First(GetPhase1Fixture().Seed);
	FSimulation Second(GetPhase1Fixture().Seed);
	if (!CreateAndBuildStarterAirfield(*this, First) ||
		!CreateAndBuildStarterAirfield(*this, Second) ||
		!OpenAcceptAndSchedule(*this, First) ||
		!OpenAcceptAndSchedule(*this, Second))
	{
		return false;
	}
	const int64 CompletionAt =
		First.GetPhase1State().Flight.ScheduledArrivalGameMilliseconds + 130000;
	AdvanceTo(First, CompletionAt);
	AdvanceTo(Second, CompletionAt);
	TestEqual(TEXT("Identical S01 checksum"), First.CalculateChecksum(), Second.CalculateChecksum());
	TestEqual(
		TEXT("Identical S01 event count"),
		First.GetPhase1State().Events.Num(),
		Second.GetPhase1State().Events.Num());
	for (int32 Index = 0; Index < First.GetPhase1State().Events.Num(); ++Index)
	{
		TestEqual(
			TEXT("Identical event message"),
			First.GetPhase1State().Events[Index].Message,
			Second.GetPhase1State().Events[Index].Message);
	}
	return true;
}
