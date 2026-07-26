#include "AMSimPhase1Fixture.h"
#include "AMSimPhase2Fixture.h"
#include "AMSimPhase3Content.h"
#include "AMSimPhase3Fixture.h"
#include "AMSimPhase3ViewState.h"
#include "AMSimPresentationProxyPool.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "AMSimWorldPresenter.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"

namespace
{
	using namespace AMSim;

	FPhase1Command MakePhase1Command(
		const uint64 Id,
		const EPhase1CommandType Type)
	{
		FPhase1Command Command;
		Command.Id = {Id};
		Command.Type = Type;
		return Command;
	}

	bool QueuePhase1(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		const FPhase1Command& Command,
		const TCHAR* Context)
	{
		if (!Test.TestEqual(
			Context,
			Simulation.QueuePhase1Command(Command),
			EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	bool QueuePhase2(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase2Command Command,
		const TCHAR* Context)
	{
		if (!Test.TestEqual(
			Context,
			Simulation.QueuePhase2Command(Command),
			EPhase2CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	bool QueuePhase3(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase3Command Command,
		const TCHAR* Context)
	{
		if (!Test.TestEqual(
			Context,
			Simulation.QueuePhase3Command(Command),
			EPhase3CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	void AdvanceTo(FSimulation& Simulation, const int64 TargetMilliseconds)
	{
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds <
			TargetMilliseconds)
		{
			Simulation.Step();
		}
	}

	bool BuildLivingAirport(
		FAutomationTestBase& Test,
		FSimulation& Simulation)
	{
		FPhase1Command Create =
			MakePhase1Command(1, EPhase1CommandType::CreateAirport);
		Create.AirportName = TEXT("Riverbend Field");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueuePhase1(Test, Simulation, Create, TEXT("Create airport")))
		{
			return false;
		}
		FPhase1Command Build =
			MakePhase1Command(2, EPhase1CommandType::CommitStarterPlan);
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueuePhase1(Test, Simulation, Build, TEXT("Build starter airfield")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Simulation.GetPhase1State().Project.FundedAtGameMilliseconds +
				GetPhase1Fixture().ReadyToOpenAtMilliseconds);
		if (!QueuePhase1(
				Test,
				Simulation,
				MakePhase1Command(3, EPhase1CommandType::OpenAirport),
				TEXT("Open airport")) ||
			!QueuePhase1(
				Test,
				Simulation,
				MakePhase1Command(4, EPhase1CommandType::AcceptStarterOffer),
				TEXT("Accept starter visit")))
		{
			return false;
		}
		FPhase1Command Schedule =
			MakePhase1Command(5, EPhase1CommandType::ScheduleStarterFlight);
		Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
		const int64 Now =
			Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
		const int64 Increment =
			GetPhase1Fixture().TimetableIncrementMilliseconds;
		Schedule.ScheduledArrivalGameMilliseconds =
			((Now + Increment - 1) / Increment) * Increment;
		if (!QueuePhase1(Test, Simulation, Schedule, TEXT("Schedule starter visit")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Schedule.ScheduledArrivalGameMilliseconds + 130000);
		if (!Test.TestEqual(
			TEXT("Starter visit completed"),
			Simulation.GetPhase1State().Flight.State,
			EFlightState::Completed))
		{
			return false;
		}
		FPhase2Command Initialize;
		Initialize.Id = {1};
		Initialize.Type = EPhase2CommandType::InitializeLivingAirport;
		return QueuePhase2(
			Test,
			Simulation,
			Initialize,
			TEXT("Initialize living airport"));
	}

	bool PrepareConnectedTerminal(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& NextCommandId)
	{
		FPhase3Command Initialize;
		Initialize.Id = {NextCommandId++};
		Initialize.Type = EPhase3CommandType::InitializePassengerAirport;
		if (!QueuePhase3(
			Test,
			Simulation,
			Initialize,
			TEXT("Initialize passenger airport")))
		{
			return false;
		}
		FPhase3Command Fund;
		Fund.Id = {NextCommandId++};
		Fund.Type = EPhase3CommandType::FundTerminal;
		if (!QueuePhase3(Test, Simulation, Fund, TEXT("Fund terminal")))
		{
			return false;
		}
		const int64 ConstructionStarted =
			Simulation.GetPhase3State().TerminalStageChangedAtGameMilliseconds;
		AdvanceTo(
			Simulation,
			ConstructionStarted +
				GetPhase3Fixture().ConstructionStageMilliseconds * 4);
		if (!Test.TestEqual(
			TEXT("Terminal shell reaches network stage"),
			Simulation.GetPhase3State().TerminalStage,
			ETerminalConstructionStage::ShellReady))
		{
			return false;
		}
		for (int32 Index = 0;
			Index < Simulation.GetPhase3State().Routes.Num();
			++Index)
		{
			FPhase3Command Connect;
			Connect.Id = {NextCommandId++};
			Connect.Type = EPhase3CommandType::ConnectNextNetwork;
			if (!QueuePhase3(
				Test,
				Simulation,
				Connect,
				TEXT("Connect terminal network")))
			{
				return false;
			}
		}
		return Test.TestTrue(
			TEXT("Connected topology is secure and accessible"),
			Simulation.GetPhase3State().bSecurityIntegrityValid &&
				Simulation.GetPhase3State().bAccessibleRouteValid);
	}

	bool OpenAndSchedule(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& NextCommandId)
	{
		FPhase3Command Open;
		Open.Id = {NextCommandId++};
		Open.Type = EPhase3CommandType::OpenTerminal;
		if (!QueuePhase3(Test, Simulation, Open, TEXT("Open terminal")))
		{
			return false;
		}
		FPhase3Command Schedule;
		Schedule.Id = {NextCommandId++};
		Schedule.Type = EPhase3CommandType::SchedulePassengerService;
		return QueuePhase3(
			Test,
			Simulation,
			Schedule,
			TEXT("Schedule passenger service"));
	}

	bool VerifyBoundaryContinuation(
		FAutomationTestBase& Test,
		const FSimulation& Source,
		const TCHAR* Context)
	{
		TArray<uint8> Bytes;
		FSnapshot Snapshot;
		if (!Test.TestTrue(
			Context,
			SerializeSnapshot(Source.CreateSnapshot(), Bytes)) ||
			!Test.TestTrue(
				TEXT("Boundary snapshot deserializes"),
				DeserializeSnapshot(Bytes, Snapshot)))
		{
			return false;
		}
		FSimulation Left;
		FSimulation Right;
		if (!Test.TestTrue(
			TEXT("Boundary snapshot restores left"),
			Left.RestoreSnapshot(Snapshot)) ||
			!Test.TestTrue(
				TEXT("Boundary snapshot restores right"),
				Right.RestoreSnapshot(Snapshot)) ||
			!Test.TestEqual(
				TEXT("Boundary immediate checksum matches source"),
				Left.CalculateChecksum(),
				Source.CalculateChecksum()))
		{
			return false;
		}
		for (int32 Index = 0; Index < 100; ++Index)
		{
			Left.Step();
			Right.Step();
		}
		return Test.TestEqual(
			TEXT("Boundary continuation remains deterministic"),
			Left.CalculateChecksum(),
			Right.CalculateChecksum());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3LifecycleTest,
	"AMSim.Phase3.PassengerAirport.CompleteDomesticJourney",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3LifecycleTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase3Fixture().Seed);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 100;
	if (!PrepareConnectedTerminal(*this, Simulation, CommandId) ||
		!OpenAndSchedule(*this, Simulation, CommandId))
	{
		return false;
	}
	const FPassengerRecord* Accessible =
		Simulation.GetPhase3State().Passengers.FindByPredicate(
			[](const FPassengerRecord& Passenger)
				{
					return Passenger.bRequiresAccessibleRoute;
				});
	TestNotNull(TEXT("Maya's party has an accessible-route passenger"), Accessible);
	if (Accessible)
	{
		FPhase3Command Assist;
		Assist.Id = {CommandId++};
		Assist.Type = EPhase3CommandType::RequestPassengerAssistance;
		Assist.PassengerId = Accessible->Id;
		if (!QueuePhase3(
			*this,
			Simulation,
			Assist,
			TEXT("Assign accessible-route assistance")))
		{
			return false;
		}
	}
	const int64 JourneyStarted =
		Simulation.GetPhase3State().Flight.ScheduledAtGameMilliseconds;
	AdvanceTo(Simulation, JourneyStarted + 120000);
	const FPhase3State& State = Simulation.GetPhase3State();
	TestEqual(
		TEXT("Passenger service completes"),
		State.Flight.State,
		EPhase3FlightState::Completed);
	TestEqual(TEXT("All passengers complete"), State.CompletedPassengerCount, 52);
	TestEqual(TEXT("All bags complete"), State.CompletedBagCount, 34);
	TestTrue(TEXT("Passenger reconciliation exact"), State.Flight.bPassengerReconciled);
	TestTrue(TEXT("Bag reconciliation exact"), State.Flight.bBagReconciled);
	TestEqual(TEXT("One reconciliation gate passes"), State.ReconciliationPassCount, 1);
	TestEqual(
		TEXT("Passenger reward recognized once"),
		State.TotalPassengerRevenueCredits,
		GetPhase3Fixture().PassengerFlightRewardCredits);
	TestTrue(
		TEXT("Accessible passenger used controlled accessible route"),
		Accessible && Accessible->bUsedAccessibleRoute);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3TopologyTest,
	"AMSim.Phase3.Security.ControlledAndAccessibleTopology",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3TopologyTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Source(GetPhase3Fixture().Seed);
	if (!BuildLivingAirport(*this, Source))
	{
		return false;
	}
	uint64 CommandId = 200;
	if (!PrepareConnectedTerminal(*this, Source, CommandId))
	{
		return false;
	}

	FSnapshot BypassSnapshot = Source.CreateSnapshot();
	FTerminalRouteRecord* SecurityRoute =
		BypassSnapshot.Phase3.Routes.FindByPredicate(
			[](const FTerminalRouteRecord& Route)
				{
					return Route.Kind == ETerminalRouteKind::SecurityControlled;
				});
	if (!TestNotNull(TEXT("Controlled security route exists"), SecurityRoute))
	{
		return false;
	}
	SecurityRoute->bIntroducesSecurityBypass = true;
	FSimulation Bypass;
	if (!TestTrue(
		TEXT("Pre-open invalid topology can be loaded for validation"),
		Bypass.RestoreSnapshot(BypassSnapshot)))
	{
		return false;
	}
	FPhase3Command OpenBypass;
	OpenBypass.Id = {CommandId++};
	OpenBypass.Type = EPhase3CommandType::OpenTerminal;
	TestEqual(
		TEXT("Security bypass blocks terminal opening"),
		Bypass.QueuePhase3Command(OpenBypass),
		EPhase3CommandResult::RejectedSecurityBypass);

	FSnapshot AccessibleSnapshot = Source.CreateSnapshot();
	for (FTerminalRouteRecord& Route : AccessibleSnapshot.Phase3.Routes)
	{
		Route.bAccessible = false;
	}
	FSimulation Inaccessible;
	if (!TestTrue(
		TEXT("Pre-open inaccessible topology loads for validation"),
		Inaccessible.RestoreSnapshot(AccessibleSnapshot)))
	{
		return false;
	}
	FPhase3Command OpenInaccessible;
	OpenInaccessible.Id = {CommandId++};
	OpenInaccessible.Type = EPhase3CommandType::OpenTerminal;
	return TestEqual(
		TEXT("Missing accessible route blocks terminal opening"),
		Inaccessible.QueuePhase3Command(OpenInaccessible),
		EPhase3CommandResult::RejectedNoAccessibleRoute);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3PersistenceTest,
	"AMSim.Phase3.Persistence.Schema4RoundTripAndMigration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3PersistenceTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase3Fixture().Seed);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 300;
	if (!PrepareConnectedTerminal(*this, Simulation, CommandId) ||
		!OpenAndSchedule(*this, Simulation, CommandId))
	{
		return false;
	}
	AdvanceTo(
		Simulation,
		Simulation.GetPhase3State().Flight.ScheduledAtGameMilliseconds + 17000);
	TArray<uint8> Bytes;
	FSnapshot Loaded;
	FSimulation Restored;
	if (!TestTrue(
			TEXT("Schema 4 snapshot serializes"),
			SerializeSnapshot(Simulation.CreateSnapshot(), Bytes)) ||
		!TestTrue(
			TEXT("Schema 4 snapshot deserializes"),
			DeserializeSnapshot(Bytes, Loaded)) ||
		!TestTrue(
			TEXT("Schema 4 transient terminal state restores"),
			Restored.RestoreSnapshot(Loaded)))
	{
		return false;
	}
	TestEqual(
		TEXT("Immediate Phase 3 checksum matches"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());
	for (int32 Index = 0; Index < 500; ++Index)
	{
		Simulation.Step();
		Restored.Step();
	}
	TestEqual(
		TEXT("Post-load continuation remains deterministic"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());

	FSnapshot Schema3 = Simulation.CreateSnapshot();
	Schema3.SchemaVersion = 3;
	Schema3.Phase3 = {};
	TArray<uint8> LegacyBytes;
	FSnapshot Migrated;
	TestTrue(
		TEXT("Schema 3 fixture serializes"),
		SerializeSnapshot(Schema3, LegacyBytes));
	TestTrue(
		TEXT("Schema 3 fixture migrates"),
		DeserializeSnapshot(LegacyBytes, Migrated));
	TestEqual(
		TEXT("Schema 3 migrates to schema 4"),
		Migrated.SchemaVersion,
		SnapshotSchemaVersion);
	TestFalse(
		TEXT("Migrated schema starts with empty Phase 3 domain"),
		Migrated.Phase3.bInitialized);
	FSnapshot BrokenReference = Simulation.CreateSnapshot();
	if (!BrokenReference.Phase3.Passengers.IsEmpty())
	{
		BrokenReference.Phase3.Passengers[0].PartyId = {MAX_uint64};
	}
	FSimulation RejectedReference;
	TestFalse(
		TEXT("Broken passenger-party ownership is rejected"),
		RejectedReference.RestoreSnapshot(BrokenReference));
	FSnapshot Future = Simulation.CreateSnapshot();
	Future.SchemaVersion = SnapshotSchemaVersion + 1;
	TArray<uint8> FutureBytes;
	return TestFalse(
		TEXT("Unsupported future schema cannot serialize"),
		SerializeSnapshot(Future, FutureBytes));
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3ScaleTest,
	"AMSim.Phase3.Scale.Logical10000AndVisible2000",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3ScaleTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Foundation(GetPhase3Fixture().Seed);
	if (!BuildLivingAirport(*this, Foundation))
	{
		return false;
	}
	FSnapshot Snapshot = Foundation.CreateSnapshot();
	Snapshot.Phase3 = BuildPhase3LogicalScaleFixture(
		GetPhase3Fixture().LogicalScalePassengerCount);
	FSimulation Scaled;
	const double RestoreStarted = FPlatformTime::Seconds();
	if (!TestTrue(
		TEXT("10k logical passenger fixture restores"),
		Scaled.RestoreSnapshot(Snapshot)))
	{
		return false;
	}
	const double RestoreMilliseconds =
		(FPlatformTime::Seconds() - RestoreStarted) * 1000.0;
	const FPhase3QuerySnapshot Query = Scaled.CreatePhase3QuerySnapshot();
	TestEqual(
		TEXT("Logical passenger count is 10k"),
		Query.PassengerCount,
		10000);
	TestEqual(
		TEXT("Visible proxy target is bounded at 2k"),
		Query.VisiblePassengerTarget,
		2000);
	TestTrue(
		TEXT("Logical fixture restore remains bounded"),
		RestoreMilliseconds < 1000.0);

	FPresentationProxyPool Pool;
	TArray<FEntityId> Entities;
	Entities.Reserve(GetPhase3Fixture().VisibleScaleProxyCount);
	for (int32 Index = 0;
		Index < GetPhase3Fixture().VisibleScaleProxyCount;
		++Index)
	{
		const FEntityId Entity{static_cast<uint64>(Index + 1)};
		Entities.Add(Entity);
		Pool.Acquire(Entity);
	}
	TestEqual(TEXT("2k presentation proxies active"), Pool.GetActiveCount(), 2000);
	for (int32 Index = 0; Index < Entities.Num(); Index += 2)
	{
		Pool.Release(Entities[Index]);
	}
	for (int32 Index = 0; Index < 1000; ++Index)
	{
		Pool.Acquire(FEntityId{static_cast<uint64>(3000 + Index)});
	}
	return TestEqual(
		TEXT("Released proxy slots are reused without growing active count"),
		Pool.GetActiveCount(),
		2000);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3BoundaryMatrixTest,
	"AMSim.Phase3.Persistence.BoundaryContinuationMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3BoundaryMatrixTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase3Fixture().Seed);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 400;
	FPhase3Command Initialize;
	Initialize.Id = {CommandId++};
	Initialize.Type = EPhase3CommandType::InitializePassengerAirport;
	FPhase3Command Fund;
	Fund.Id = {CommandId++};
	Fund.Type = EPhase3CommandType::FundTerminal;
	if (!QueuePhase3(*this, Simulation, Initialize, TEXT("Initialize terminal")) ||
		!QueuePhase3(*this, Simulation, Fund, TEXT("Fund terminal")))
	{
		return false;
	}
	AdvanceTo(
		Simulation,
		Simulation.GetPhase3State().TerminalStageChangedAtGameMilliseconds +
			GetPhase3Fixture().ConstructionStageMilliseconds + 100);
	if (!TestEqual(
		TEXT("Construction boundary reached"),
		Simulation.GetPhase3State().TerminalStage,
		ETerminalConstructionStage::Building) ||
		!VerifyBoundaryContinuation(
			*this,
			Simulation,
			TEXT("Construction boundary serializes")))
	{
		return false;
	}
	const int64 ConstructionStarted =
		Simulation.GetPhase3State().TerminalStageChangedAtGameMilliseconds;
	AdvanceTo(
		Simulation,
		ConstructionStarted +
			GetPhase3Fixture().ConstructionStageMilliseconds * 3);
	for (int32 Index = 0;
		Index < Simulation.GetPhase3State().Routes.Num();
		++Index)
	{
		FPhase3Command Connect;
		Connect.Id = {CommandId++};
		Connect.Type = EPhase3CommandType::ConnectNextNetwork;
		if (!QueuePhase3(*this, Simulation, Connect, TEXT("Connect route")))
		{
			return false;
		}
	}
	if (!OpenAndSchedule(*this, Simulation, CommandId))
	{
		return false;
	}

	bool bSecurity = false;
	bool bBoarding = false;
	bool bBaggage = false;
	bool bReclaim = false;
	bool bLandside = false;
	for (int32 Step = 0; Step < 500 &&
		!(bSecurity && bBoarding && bBaggage && bReclaim && bLandside);
		++Step)
	{
		Simulation.Step();
		const FPhase3State& State = Simulation.GetPhase3State();
		auto CaptureOnce =
			[this, &Simulation](bool& bCaptured, const bool bCondition, const TCHAR* Label)
			{
				if (!bCaptured && bCondition)
				{
					bCaptured = VerifyBoundaryContinuation(
						*this,
						Simulation,
						Label);
				}
			};
		CaptureOnce(
			bSecurity,
			State.Passengers.ContainsByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.JourneyState ==
							EPassengerJourneyState::SecurityQueue;
					}),
			TEXT("Security queue boundary serializes"));
		CaptureOnce(
			bBoarding,
			State.Passengers.ContainsByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.JourneyState ==
							EPassengerJourneyState::Boarding;
					}),
			TEXT("Boarding boundary serializes"));
		CaptureOnce(
			bBaggage,
			State.Bags.ContainsByPredicate(
				[](const FBagRecord& Bag)
					{
						return Bag.JourneyState == EBagJourneyState::MakeUp;
					}),
			TEXT("Baggage make-up boundary serializes"));
		CaptureOnce(
			bReclaim,
			State.Passengers.ContainsByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.JourneyState ==
							EPassengerJourneyState::BaggageReclaim;
					}),
			TEXT("Baggage reclaim boundary serializes"));
		CaptureOnce(
			bLandside,
			State.Passengers.ContainsByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.JourneyState ==
							EPassengerJourneyState::GroundTransport;
					}),
			TEXT("Landside exit boundary serializes"));
	}
	TestTrue(TEXT("Security boundary captured"), bSecurity);
	TestTrue(TEXT("Boarding boundary captured"), bBoarding);
	TestTrue(TEXT("Baggage boundary captured"), bBaggage);
	TestTrue(TEXT("Reclaim boundary captured"), bReclaim);
	TestTrue(TEXT("Landside boundary captured"), bLandside);
	return bSecurity && bBoarding && bBaggage && bReclaim && bLandside;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3PresentationTest,
	"AMSim.Phase3.Presentation.ViewStateAndWorldContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3PresentationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FPhase3State State = BuildPhase3LogicalScaleFixture(4);
	FPhase3QuerySnapshot Query;
	Query.Revision = 77;
	Query.bUnlocked = true;
	Query.bInitialized = true;
	Query.bTerminalOpen = true;
	Query.TerminalStage = ETerminalConstructionStage::Operational;
	Query.FlightState = EPhase3FlightState::AtGate;
	Query.RequiredConnectionCount = 7;
	Query.ConnectedCount = 7;
	Query.DepartingPassengerCount = 4;
	Query.PrimaryStatus = TEXT("RB 304 · At Gate A1");
	Query.FlightCode = TEXT("RB 304");
	Query.FlightStatus = TEXT("At Gate A1");
	Query.FeaturedPartyName = TEXT("Maya's party");
	Query.FeaturedPartyMembers = 4;
	Query.FeaturedStep = TEXT("Security queue");
	Query.FeaturedNeeds = TEXT("Accessible route · Food · Restroom");
	Query.FeaturedRoute =
		TEXT("Entrance -> Bag drop -> Security -> Gate A1 · accessible");
	Query.FeaturedTimeConfidencePercent = 86;
	Query.bSecurityIntegrityValid = true;
	const FPhase3ViewState View = MakePhase3ViewState(Query, State);
	TestEqual(TEXT("View state preserves revision"), View.Revision, uint64(77));
	TestTrue(TEXT("Operational terminal is shown"), View.bShowTerminal);
	TestTrue(TEXT("Named party remains visible"), View.FeaturedParty.Contains(TEXT("MAYA")));
	TestTrue(TEXT("Accessible route remains explicit"), View.FeaturedRoute.Contains(TEXT("accessible")));
	TestTrue(TEXT("Security status is color-independent text"), View.Security.Contains(TEXT("CONTROLLED")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase3ContentTest,
	"AMSim.Phase3.Content.CatalogAndTwoDimensionalLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase3ContentTest::RunTest(const FString& Parameters)
{
	const FAMSimPhase3CatalogValidation Validation =
		FAMSimPhase3ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(TEXT("Phase 3 catalog validates"), Validation.bValid);
	TestEqual(
		TEXT("All required Phase 3 definitions resolve"),
		Validation.Assets.Num(),
		FAMSimPhase3ContentCatalog::RequiredContentIds().Num());
	return Validation.bValid;
}
