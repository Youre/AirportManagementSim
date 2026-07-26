#include "AMSimPhase1Fixture.h"
#include "AMSimPhase2Fixture.h"
#include "AMSimPhase2Content.h"
#include "AMSimPhase2ViewState.h"
#include "AMSimPresentationProxyPool.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "AMSimWorldPresenter.h"
#include "Misc/AutomationTest.h"

namespace
{
	using namespace AMSim;

	FPhase1Command Phase1Command(const uint64 Id, const EPhase1CommandType Type)
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
		const FPhase2Command& Command,
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

	void AdvanceTo(FSimulation& Simulation, const int64 TargetMilliseconds)
	{
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds < TargetMilliseconds)
		{
			Simulation.Step();
		}
	}

	bool BuildPhase1ReadyAirport(FAutomationTestBase& Test, FSimulation& Simulation)
	{
		FPhase1Command Create = Phase1Command(1, EPhase1CommandType::CreateAirport);
		Create.AirportName = TEXT("Riverbend Field");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueuePhase1(Test, Simulation, Create, TEXT("Create Phase 1 airport")))
		{
			return false;
		}

		FPhase1Command Build = Phase1Command(2, EPhase1CommandType::CommitStarterPlan);
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueuePhase1(Test, Simulation, Build, TEXT("Build Phase 1 airfield")))
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
			Phase1Command(3, EPhase1CommandType::OpenAirport),
			TEXT("Open Phase 1 airfield")) ||
			!QueuePhase1(
				Test,
				Simulation,
				Phase1Command(4, EPhase1CommandType::AcceptStarterOffer),
				TEXT("Accept first visit")))
		{
			return false;
		}

		FPhase1Command Schedule =
			Phase1Command(5, EPhase1CommandType::ScheduleStarterFlight);
		Schedule.RequestedStandDefinitionId = TEXT("Facility.GAStand.Starter");
		const int64 Now = Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
		const int64 Increment = GetPhase1Fixture().TimetableIncrementMilliseconds;
		Schedule.ScheduledArrivalGameMilliseconds =
			((Now + Increment - 1) / Increment) * Increment;
		if (!QueuePhase1(Test, Simulation, Schedule, TEXT("Schedule first visit")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Schedule.ScheduledArrivalGameMilliseconds + 130000);
		return Test.TestEqual(
			TEXT("Phase 1 visit completed"),
			Simulation.GetPhase1State().Flight.State,
			EFlightState::Completed);
	}

	bool StartPhase2(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		const EAirportSpecialization Specialization,
		uint64& NextCommandId)
	{
		FPhase2Command Initialize;
		Initialize.Id = {NextCommandId++};
		Initialize.Type = EPhase2CommandType::InitializeLivingAirport;
		if (!QueuePhase2(Test, Simulation, Initialize, TEXT("Initialize living airport")))
		{
			return false;
		}

		FPhase2Command Select;
		Select.Id = {NextCommandId++};
		Select.Type = EPhase2CommandType::SelectSpecialization;
		Select.Specialization = Specialization;
		if (!QueuePhase2(Test, Simulation, Select, TEXT("Select specialization")))
		{
			return false;
		}

		const FPhase2ContractRecord* Contract =
			Simulation.GetPhase2State().Contracts.FindByPredicate(
				[Specialization](const FPhase2ContractRecord& Candidate)
					{
						return Candidate.Specialization == Specialization;
					});
		if (!Test.TestNotNull(TEXT("Specialization contract exists"), Contract))
		{
			return false;
		}
		FPhase2Command Accept;
		Accept.Id = {NextCommandId++};
		Accept.Type = EPhase2CommandType::AcceptContract;
		Accept.ContractId = Contract->Id;
		return QueuePhase2(Test, Simulation, Accept, TEXT("Accept recurring contract"));
	}

	bool PurchaseAndBuildExpansion(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& NextCommandId)
	{
		if (!Test.TestFalse(
			TEXT("Neighboring parcel starts unowned"),
			Simulation.GetPhase2State().Parcels.IsEmpty() ||
				Simulation.GetPhase2State().Parcels[0].bOwned))
		{
			return false;
		}
		const FParcelId ParcelId = Simulation.GetPhase2State().Parcels[0].Id;
		FPhase2Command Purchase;
		Purchase.Id = {NextCommandId++};
		Purchase.Type = EPhase2CommandType::PurchaseParcel;
		Purchase.ParcelId = ParcelId;
		if (!QueuePhase2(Test, Simulation, Purchase, TEXT("Purchase neighboring parcel")))
		{
			return false;
		}
		FPhase2Command Expand;
		Expand.Id = {NextCommandId++};
		Expand.Type = EPhase2CommandType::StartExpansion;
		Expand.ParcelId = ParcelId;
		return QueuePhase2(Test, Simulation, Expand, TEXT("Start apron expansion"));
	}

	bool RoundTrip(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		const TCHAR* Context)
	{
		TArray<uint8> Bytes;
		if (!Test.TestTrue(Context, SerializeSnapshot(Simulation.CreateSnapshot(), Bytes)))
		{
			return false;
		}
		FSnapshot Loaded;
		FSimulation Restored;
		if (!Test.TestTrue(TEXT("Phase 2 snapshot deserializes"), DeserializeSnapshot(Bytes, Loaded)) ||
			!Test.TestTrue(TEXT("Phase 2 snapshot restores"), Restored.RestoreSnapshot(Loaded)) ||
			!Test.TestEqual(
				TEXT("Phase 2 immediate checksum matches"),
				Restored.CalculateChecksum(),
				Simulation.CalculateChecksum()))
		{
			return false;
		}
		for (int32 Step = 0; Step < 100; ++Step)
		{
			Simulation.Step();
			Restored.Step();
		}
		return Test.TestEqual(
			TEXT("Phase 2 continuation checksum matches"),
			Restored.CalculateChecksum(),
			Simulation.CalculateChecksum());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2FoundationTest,
	"AMSim.Phase2.Foundation.DomainAndContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2FoundationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase2Fixture().Seed);
	if (!BuildPhase1ReadyAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 100;
	if (!StartPhase2(
		*this,
		Simulation,
		EAirportSpecialization::FlightSchool,
		CommandId))
	{
		return false;
	}

	const FPhase2State& State = Simulation.GetPhase2State();
	TestEqual(TEXT("Seven persistent Phase 2 airframes"), State.Aircraft.Num(), 7);
	TSet<FName> AircraftRoles;
	for (const FPhase2AircraftRecord& Aircraft : State.Aircraft)
	{
		AircraftRoles.Add(Aircraft.RoleId);
	}
	TestEqual(TEXT("Four fictional aircraft roles"), AircraftRoles.Num(), 4);
	TestEqual(TEXT("Three tenant paths"), State.Tenants.Num(), 3);
	TestEqual(TEXT("Three recurring contract types"), State.Contracts.Num(), 3);
	TestEqual(TEXT("Four service vehicles"), State.Vehicles.Num(), 4);
	TestEqual(TEXT("Four staff roles"), State.Teams.Num(), 4);
	TestEqual(TEXT("Five rating components"), State.Ratings.Num(), 5);
	TestEqual(
		TEXT("Flight-school cadence creates three flights"),
		State.Flights.Num(),
		3);
	TestTrue(TEXT("Approach aid is ready"), State.bApproachAidReady);
	TestEqual(TEXT("Runway has a published direction"), State.ActiveRunwayDirection, FName(TEXT("24")));

	FPhase2Command InvalidRunway;
	InvalidRunway.Id = {CommandId++};
	InvalidRunway.Type = EPhase2CommandType::SetActiveRunway;
	InvalidRunway.RunwayDirection = TEXT("13");
	TestEqual(
		TEXT("Unpublished runway direction rejected"),
		Simulation.QueuePhase2Command(InvalidRunway),
		EPhase2CommandResult::RejectedIncompatible);
	TestEqual(
		TEXT("Phase 1 closure cannot invalidate active Phase 2 operations"),
		Simulation.QueuePhase1Command(Phase1Command(99, EPhase1CommandType::CloseAirport)),
		EPhase1CommandResult::RejectedInvalidState);

	FPhase2Command Assign;
	Assign.Id = {CommandId++};
	Assign.Type = EPhase2CommandType::AssignTeamZone;
	Assign.TeamId = State.Teams[0].Id;
	Assign.ZoneId = TEXT("Zone.PatternAndRunway");
	TestTrue(
		TEXT("Team can be assigned through command"),
		QueuePhase2(*this, Simulation, Assign, TEXT("Assign team zone")));

	const int32 ScheduledFlightIndex =
		Simulation.GetPhase2State().Flights.IndexOfByPredicate(
			[](const FPhase2FlightRecord& Flight)
				{
					return Flight.State == EPhase2FlightState::Scheduled;
				});
	if (!TestTrue(TEXT("A future flight remains available to reschedule"),
		ScheduledFlightIndex != INDEX_NONE))
	{
		return false;
	}
	const FFlightId FlightId =
		Simulation.GetPhase2State().Flights[ScheduledFlightIndex].Id;
	const int64 OriginalArrival =
		Simulation.GetPhase2State().Flights[ScheduledFlightIndex].
			ScheduledArrivalGameMilliseconds;
	FPhase2Command Reschedule;
	Reschedule.Id = {CommandId++};
	Reschedule.Type = EPhase2CommandType::RescheduleFlight;
	Reschedule.FlightId = FlightId;
	Reschedule.RequestedGameTimeMilliseconds = OriginalArrival + 15000;
	if (!QueuePhase2(*this, Simulation, Reschedule, TEXT("Reschedule protected slot")))
	{
		return false;
	}
	TestEqual(
		TEXT("Flight and stand buffer move together"),
		Simulation.GetPhase2State().Flights[ScheduledFlightIndex].
			ScheduledArrivalGameMilliseconds,
		OriginalArrival + 15000);

	const FPhase2ContractRecord* AcceptedContract =
		Simulation.GetPhase2State().Contracts.FindByPredicate(
			[](const FPhase2ContractRecord& Contract)
				{
					return Contract.Specialization ==
						EAirportSpecialization::FlightSchool;
				});
	FPhase2Command Cancel;
	Cancel.Id = {CommandId++};
	Cancel.Type = EPhase2CommandType::CancelContract;
	Cancel.ContractId = AcceptedContract ? AcceptedContract->Id : FContractId{};
	if (!QueuePhase2(*this, Simulation, Cancel, TEXT("Cancel recurring agreement")))
	{
		return false;
	}
	TestFalse(TEXT("Cancelled contract is no longer accepted"), AcceptedContract->bAccepted);
	TestTrue(
		TEXT("Future flights are cancelled with the agreement"),
		Simulation.GetPhase2State().Flights.ContainsByPredicate(
			[](const FPhase2FlightRecord& Flight)
				{
					return Flight.State == EPhase2FlightState::Cancelled;
				}));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2SaveAndMigrationTest,
	"AMSim.Phase2.Persistence.Schema4AndTransientBoundaries",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2SaveAndMigrationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase2Fixture().Seed);
	if (!BuildPhase1ReadyAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 200;
	if (!StartPhase2(
		*this,
		Simulation,
		EAirportSpecialization::GeneralAviation,
		CommandId) ||
		!PurchaseAndBuildExpansion(*this, Simulation, CommandId))
	{
		return false;
	}

	const int64 InitializedAt =
		Simulation.GetPhase2State().InitializedAtGameMilliseconds;
	AdvanceTo(Simulation, InitializedAt + 60000);
	TestTrue(
		TEXT("Flight is in a transient operational state"),
		Simulation.GetPhase2State().Flights.ContainsByPredicate(
			[](const FPhase2FlightRecord& Flight)
				{
					return Flight.State != EPhase2FlightState::Scheduled &&
						Flight.State != EPhase2FlightState::Completed;
				}));
	if (!RoundTrip(*this, Simulation, TEXT("Taxi/service boundary serializes")))
	{
		return false;
	}

	AdvanceTo(Simulation, InitializedAt + 120000);
	TestTrue(
		TEXT("Expansion reaches a transient construction stage"),
		Simulation.GetPhase2State().Expansion.Stage == EExpansionStage::Delivery ||
			Simulation.GetPhase2State().Expansion.Stage == EExpansionStage::Building);
	if (!RoundTrip(*this, Simulation, TEXT("Expansion boundary serializes")))
	{
		return false;
	}

	FSnapshot Schema2 = Simulation.CreateSnapshot();
	Schema2.SchemaVersion = 2;
	Schema2.Phase2 = {};
	TArray<uint8> LegacyBytes;
	FSnapshot Migrated;
	TestTrue(TEXT("Schema 2 fixture serializes"), SerializeSnapshot(Schema2, LegacyBytes));
	TestTrue(TEXT("Schema 2 fixture migrates"), DeserializeSnapshot(LegacyBytes, Migrated));
	TestEqual(
		TEXT("Schema 2 migrates through schema 3 to the current schema"),
		Migrated.SchemaVersion,
		SnapshotSchemaVersion);
	TestFalse(TEXT("Migration does not invent Phase 2 completion"), Migrated.Phase2.bInitialized);

	FSnapshot Duplicate = Simulation.CreateSnapshot();
	Duplicate.Phase2.Aircraft[1].Id = Duplicate.Phase2.Aircraft[0].Id;
	FSimulation Rejected;
	TestFalse(
		TEXT("Duplicate Phase 2 stable ID rejected"),
		Rejected.RestoreSnapshot(Duplicate));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2ConcurrencyAndIncidentTest,
	"AMSim.Phase2.Operations.ConcurrentServicesWeatherIncident",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2ConcurrencyAndIncidentTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase2Fixture().Seed);
	if (!BuildPhase1ReadyAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 300;
	if (!StartPhase2(
		*this,
		Simulation,
		EAirportSpecialization::FlightSchool,
		CommandId))
	{
		return false;
	}
	const int64 InitializedAt =
		Simulation.GetPhase2State().InitializedAtGameMilliseconds;
	AdvanceTo(Simulation, InitializedAt + 95000);

	int32 ActiveFlights = 0;
	TSet<uint64> ActiveAircraft;
	for (const FPhase2FlightRecord& Flight : Simulation.GetPhase2State().Flights)
	{
		if (Flight.State != EPhase2FlightState::Completed &&
			Flight.State != EPhase2FlightState::Scheduled)
		{
			++ActiveFlights;
			TestFalse(
				TEXT("Concurrent flights never double-book an airframe"),
				ActiveAircraft.Contains(Flight.AircraftId.Value));
			ActiveAircraft.Add(Flight.AircraftId.Value);
		}
	}
	TestTrue(TEXT("Multiple flights operate concurrently"), ActiveFlights >= 2);
	TestTrue(
		TEXT("Turnaround service graph exists"),
		Simulation.GetPhase2State().ServiceTasks.Num() >= 2);

	TSet<uint64> BusyVehicles;
	TSet<uint64> BusyTeams;
	for (const FPhase2ServiceTaskRecord& Task : Simulation.GetPhase2State().ServiceTasks)
	{
		if (Task.State == EPhase2ServiceState::Dispatched ||
			Task.State == EPhase2ServiceState::InProgress)
		{
			TestTrue(
				TEXT("Busy vehicle is exclusive"),
				!BusyVehicles.Contains(Task.AssignedVehicleId.Value));
			TestTrue(
				TEXT("Busy team is exclusive"),
				!BusyTeams.Contains(Task.AssignedTeamId.Value));
			BusyVehicles.Add(Task.AssignedVehicleId.Value);
			BusyTeams.Add(Task.AssignedTeamId.Value);
		}
	}

	AdvanceTo(
		Simulation,
		InitializedAt +
			4 * GetPhase2Fixture().OperatingDayMilliseconds +
			95000);
	TestEqual(
		TEXT("Seeded day-five weather is cold and wet"),
		Simulation.GetPhase2State().CurrentWeather.Category,
		EWeatherCategory::ColdWet);
	TestTrue(
		TEXT("Cold/wet turnaround creates a deicing task"),
		Simulation.GetPhase2State().ServiceTasks.ContainsByPredicate(
			[](const FPhase2ServiceTaskRecord& Task)
				{
					return Task.ServiceId == TEXT("Service.Deicing");
				}));

	AdvanceTo(
		Simulation,
		InitializedAt +
			7 * GetPhase2Fixture().OperatingDayMilliseconds +
			1000);
	TestEqual(
		TEXT("Seeded basic incident appears on day eight"),
		Simulation.GetPhase2State().Incident.State,
		EIncidentState::Reported);
	if (!RoundTrip(*this, Simulation, TEXT("Incident-reported boundary serializes")))
	{
		return false;
	}

	FPhase2Command Respond;
	Respond.Id = {CommandId++};
	Respond.Type = EPhase2CommandType::RespondToIncident;
	Respond.IncidentId = Simulation.GetPhase2State().Incident.Id;
	if (!QueuePhase2(*this, Simulation, Respond, TEXT("Dispatch incident response")))
	{
		return false;
	}
	AdvanceTo(
		Simulation,
		Simulation.CreateQuerySnapshot().GameTimeMilliseconds + 95000);
	TestEqual(
		TEXT("Incident reaches cleanup or resolution deterministically"),
		Simulation.GetPhase2State().Incident.State,
		EIncidentState::Resolved);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2FourteenDayGateTest,
	"AMSim.Phase2.Acceptance.FourteenDaySpecializations",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2FourteenDayGateTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	const EAirportSpecialization Specializations[] = {
		EAirportSpecialization::GeneralAviation,
		EAirportSpecialization::FlightSchool,
		EAirportSpecialization::Charter
	};
	for (const EAirportSpecialization Specialization : Specializations)
	{
		FSimulation First(GetPhase2Fixture().Seed);
		FSimulation Second(GetPhase2Fixture().Seed);
		if (!BuildPhase1ReadyAirport(*this, First) ||
			!BuildPhase1ReadyAirport(*this, Second))
		{
			return false;
		}
		uint64 FirstCommandId = 400;
		uint64 SecondCommandId = 400;
		if (!StartPhase2(*this, First, Specialization, FirstCommandId) ||
			!StartPhase2(*this, Second, Specialization, SecondCommandId) ||
			!PurchaseAndBuildExpansion(*this, First, FirstCommandId) ||
			!PurchaseAndBuildExpansion(*this, Second, SecondCommandId))
		{
			return false;
		}
		const int64 FirstTarget =
			First.GetPhase2State().InitializedAtGameMilliseconds +
			GetPhase2Fixture().RequiredOperatingDays *
				GetPhase2Fixture().OperatingDayMilliseconds;
		const int64 SecondTarget =
			Second.GetPhase2State().InitializedAtGameMilliseconds +
			GetPhase2Fixture().RequiredOperatingDays *
				GetPhase2Fixture().OperatingDayMilliseconds;
		AdvanceTo(First, FirstTarget);
		AdvanceTo(Second, SecondTarget);

		const FPhase2State& State = First.GetPhase2State();
		TestTrue(
			*FString::Printf(
				TEXT("%s remains solvent"),
				*SpecializationDisplayName(Specialization)),
			First.GetPhase1State().Credits >= 0);
		TestTrue(
			*FString::Printf(
				TEXT("%s completes fourteen operating days"),
				*SpecializationDisplayName(Specialization)),
			State.CurrentOperatingDay >= GetPhase2Fixture().RequiredOperatingDays);
		TestTrue(
			TEXT("Expected recurring flight evidence completed"),
			State.CompletedFlightCount >=
				GetPhase2Fixture().RequiredOperatingDays *
					Phase2FlightsPerOperatingDay(Specialization));
		TestEqual(
			TEXT("Expansion is operational"),
			State.Expansion.Stage,
			EExpansionStage::Operational);
		TestEqual(
			TEXT("Basic incident is resolved"),
			State.Incident.State,
			EIncidentState::Resolved);
		TestTrue(
			TEXT("Fortnight achievement is evidence-backed"),
			!State.Achievements.IsEmpty() && State.Achievements[0].bEarned);
		TestEqual(
			TEXT("Repeated scenario checksum is deterministic"),
			First.CalculateChecksum(),
			Second.CalculateChecksum());
		if (!RoundTrip(*this, First, TEXT("Fourteen-day state serializes")))
		{
			return false;
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2ContentCatalogTest,
	"AMSim.Phase2.Content.CookerVisibleCatalog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2ContentCatalogTest::RunTest(const FString& Parameters)
{
	const FAMSimPhase2CatalogValidation Validation =
		FAMSimPhase2ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(TEXT("Phase 2 catalog validates"), Validation.bValid);
	TestEqual(
		TEXT("All required Phase 2 definitions resolve"),
		Validation.Assets.Num(),
		FAMSimPhase2ContentCatalog::RequiredContentIds().Num());
	return Validation.bValid;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase2PresentationContractTest,
	"AMSim.Phase2.Presentation.ViewStateAndProxyPolicy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase2PresentationContractTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase2Fixture().Seed);
	if (!BuildPhase1ReadyAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 500;
	if (!StartPhase2(
		*this,
		Simulation,
		EAirportSpecialization::GeneralAviation,
		CommandId))
	{
		return false;
	}
	const FPhase2QuerySnapshot Query = Simulation.CreatePhase2QuerySnapshot();
	const FPhase2ViewState View = MakePhase2ViewState(
		Query,
		Simulation.GetPhase2State(),
		Simulation.GetPhase1State());
	TestEqual(TEXT("View state keeps query revision"), View.Revision, Query.Revision);
	TestTrue(TEXT("Living-airport UI is unlocked"), View.bUnlocked);
	TestTrue(TEXT("Living-airport UI is initialized"), View.bInitialized);
	TestTrue(TEXT("Identity controls remain selectable"), View.bCanSelectIdentity);
	TestFalse(TEXT("Accepted contract disables duplicate acceptance"), View.bCanAcceptContract);
	TestTrue(TEXT("Operations summary is query-backed"), !View.Operations.IsEmpty());
	TestEqual(
		TEXT("Inbound Phase 2 heading is deterministic"),
		AAMSimWorldPresenter::GetPhase2HeadingIndex(EPhase2FlightState::Inbound),
		10);
	TestEqual(
		TEXT("Outbound Phase 2 heading is deterministic"),
		AAMSimWorldPresenter::GetPhase2HeadingIndex(EPhase2FlightState::Outbound),
		2);

	FPresentationProxyPool Pool;
	const FEntityId FirstEntity{Simulation.GetPhase2State().Flights[0].Id.Value};
	const FEntityId SecondEntity{Simulation.GetPhase2State().Flights[1].Id.Value};
	const FPresentationProxyHandle First = Pool.Acquire(FirstEntity);
	const FPresentationProxyHandle Second = Pool.Acquire(SecondEntity);
	TestEqual(TEXT("Two Phase 2 flight proxies are active"), Pool.GetActiveCount(), 2);
	TestTrue(TEXT("Different flights have different proxy slots"), First.Index != Second.Index);
	TestTrue(TEXT("Released Phase 2 proxy returns to pool"), Pool.Release(FirstEntity));
	const FEntityId Replacement{Simulation.GetPhase2State().Aircraft[3].Id.Value};
	const FPresentationProxyHandle Reused = Pool.Acquire(Replacement);
	TestEqual(TEXT("Released proxy slot is reused"), Reused.Index, First.Index);
	TestTrue(TEXT("Reused proxy generation advances"), Reused.Generation > First.Generation);
	return true;
}
