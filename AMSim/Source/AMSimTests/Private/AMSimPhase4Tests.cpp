#include "AMSimPhase1Fixture.h"
#include "AMSimPhase3Fixture.h"
#include "AMSimPhase4Content.h"
#include "AMSimPhase4Fixture.h"
#include "AMSimPhase4ViewState.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "AMSimWorldPresenter.h"
#include "Misc/AutomationTest.h"

namespace
{
	using namespace AMSim;

	void AdvanceTo(FSimulation& Simulation, const int64 TargetMilliseconds)
	{
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds <
			TargetMilliseconds)
		{
			Simulation.Step();
		}
	}

	bool QueuePhase1(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase1Command Command,
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

	bool QueuePhase4(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase4Command Command,
		const TCHAR* Context)
	{
		if (!Test.TestEqual(
			Context,
			Simulation.QueuePhase4Command(Command),
			EPhase4CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	bool BuildCompletePhase3(
		FAutomationTestBase& Test,
		FSimulation& Simulation)
	{
		uint64 Phase1Id = 1;
		FPhase1Command Create;
		Create.Id = {Phase1Id++};
		Create.Type = EPhase1CommandType::CreateAirport;
		Create.AirportName = TEXT("Riverbend Field");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueuePhase1(Test, Simulation, Create, TEXT("Create airfield")))
		{
			return false;
		}
		FPhase1Command Build;
		Build.Id = {Phase1Id++};
		Build.Type = EPhase1CommandType::CommitStarterPlan;
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueuePhase1(Test, Simulation, Build, TEXT("Commit starter plan")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Simulation.GetPhase1State().Project.FundedAtGameMilliseconds +
				GetPhase1Fixture().ReadyToOpenAtMilliseconds);
		FPhase1Command Open;
		Open.Id = {Phase1Id++};
		Open.Type = EPhase1CommandType::OpenAirport;
		if (!QueuePhase1(Test, Simulation, Open, TEXT("Open airfield")))
		{
			return false;
		}
		FPhase1Command Accept;
		Accept.Id = {Phase1Id++};
		Accept.Type = EPhase1CommandType::AcceptStarterOffer;
		if (!QueuePhase1(Test, Simulation, Accept, TEXT("Accept starter flight")))
		{
			return false;
		}
		FPhase1Command Schedule;
		Schedule.Id = {Phase1Id++};
		Schedule.Type = EPhase1CommandType::ScheduleStarterFlight;
		Schedule.RequestedStandDefinitionId =
			TEXT("Facility.GAStand.Starter");
		const int64 Now =
			Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
		const int64 Increment =
			GetPhase1Fixture().TimetableIncrementMilliseconds;
		Schedule.ScheduledArrivalGameMilliseconds =
			((Now + Increment - 1) / Increment) * Increment;
		if (!QueuePhase1(Test, Simulation, Schedule, TEXT("Schedule starter flight")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Schedule.ScheduledArrivalGameMilliseconds + 130000);

		FPhase2Command InitializeLiving;
		InitializeLiving.Id = {1};
		InitializeLiving.Type =
			EPhase2CommandType::InitializeLivingAirport;
		if (!QueuePhase2(
			Test,
			Simulation,
			InitializeLiving,
			TEXT("Initialize living airport")))
		{
			return false;
		}

		uint64 Phase3Id = 1;
		FPhase3Command InitializeTerminal;
		InitializeTerminal.Id = {Phase3Id++};
		InitializeTerminal.Type =
			EPhase3CommandType::InitializePassengerAirport;
		if (!QueuePhase3(
			Test,
			Simulation,
			InitializeTerminal,
			TEXT("Initialize passenger airport")))
		{
			return false;
		}
		FPhase3Command Fund;
		Fund.Id = {Phase3Id++};
		Fund.Type = EPhase3CommandType::FundTerminal;
		if (!QueuePhase3(Test, Simulation, Fund, TEXT("Fund terminal")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Simulation.GetPhase3State().
					TerminalStageChangedAtGameMilliseconds +
				GetPhase3Fixture().ConstructionStageMilliseconds * 4);
		for (int32 Index = 0;
			Index < Simulation.GetPhase3State().Routes.Num();
			++Index)
		{
			FPhase3Command Connect;
			Connect.Id = {Phase3Id++};
			Connect.Type = EPhase3CommandType::ConnectNextNetwork;
			if (!QueuePhase3(
				Test,
				Simulation,
				Connect,
				TEXT("Connect terminal route")))
			{
				return false;
			}
		}
		FPhase3Command OpenTerminal;
		OpenTerminal.Id = {Phase3Id++};
		OpenTerminal.Type = EPhase3CommandType::OpenTerminal;
		if (!QueuePhase3(
			Test,
			Simulation,
			OpenTerminal,
			TEXT("Open terminal")))
		{
			return false;
		}
		FPhase3Command SchedulePassenger;
		SchedulePassenger.Id = {Phase3Id++};
		SchedulePassenger.Type =
			EPhase3CommandType::SchedulePassengerService;
		if (!QueuePhase3(
			Test,
			Simulation,
			SchedulePassenger,
			TEXT("Schedule passenger service")))
		{
			return false;
		}
		const FPassengerRecord* Accessible =
			Simulation.GetPhase3State().Passengers.FindByPredicate(
				[](const FPassengerRecord& Passenger)
					{
						return Passenger.bRequiresAccessibleRoute;
					});
		if (!Test.TestNotNull(
			TEXT("Accessible passenger exists"),
			Accessible))
		{
			return false;
		}
		FPhase3Command Assist;
		Assist.Id = {Phase3Id++};
		Assist.Type =
			EPhase3CommandType::RequestPassengerAssistance;
		Assist.PassengerId = Accessible->Id;
		if (!QueuePhase3(
			Test,
			Simulation,
			Assist,
			TEXT("Assign accessible assistance")))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Simulation.GetPhase3State().
					Flight.ScheduledAtGameMilliseconds +
				120000);
		return Test.TestEqual(
			TEXT("Domestic passenger service completed"),
			Simulation.GetPhase3State().Flight.State,
			EPhase3FlightState::Completed);
	}

	bool OpenAndPublishRegionalWeek(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& CommandId)
	{
		FPhase4Command Initialize;
		Initialize.Id = {CommandId++};
		Initialize.Type =
			EPhase4CommandType::InitializeRegionalAirport;
		if (!QueuePhase4(
			Test,
			Simulation,
			Initialize,
			TEXT("Initialize regional airport")))
		{
			return false;
		}
		for (int32 Index = 0; Index < 3; ++Index)
		{
			FPhase4Command Accept;
			Accept.Id = {CommandId++};
			Accept.Type =
				EPhase4CommandType::AcceptNextRecurringContract;
			if (!QueuePhase4(
				Test,
				Simulation,
				Accept,
				TEXT("Accept recurring contract")))
			{
				return false;
			}
		}
		FPhase4Command Publish;
		Publish.Id = {CommandId++};
		Publish.Type =
			EPhase4CommandType::PublishSevenDayTimetable;
		return QueuePhase4(
			Test,
			Simulation,
			Publish,
			TEXT("Publish seven-day timetable"));
	}

	const FPhase4FlightRecord* FindDisruptedFlight(
		const FSimulation& Simulation)
	{
		return Simulation.GetPhase4State().Flights.FindByPredicate(
			[](const FPhase4FlightRecord& Flight)
				{
					return Flight.DayIndex == 3 &&
						Flight.bWeatherRestricted;
				});
	}

	bool ConfirmGateChange(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& CommandId)
	{
		const FPhase4FlightRecord* Disrupted =
			FindDisruptedFlight(Simulation);
		if (!Test.TestNotNull(
			TEXT("Weather-disrupted flight exists"),
			Disrupted))
		{
			return false;
		}
		FPhase4Command Review;
		Review.Id = {CommandId++};
		Review.Type =
			EPhase4CommandType::ReassignDisruptedFlight;
		Review.FlightId = Disrupted->Id;
		Review.RequestedGateId = TEXT("R1");
		if (!Test.TestEqual(
			TEXT("Locked-horizon gate change requires explicit warning confirmation"),
			Simulation.QueuePhase4Command(Review),
			EPhase4CommandResult::AcceptedWithWarning))
		{
			return false;
		}
		FPhase4Command Confirm;
		Confirm.Id = {CommandId++};
		Confirm.Type =
			EPhase4CommandType::ConfirmHighRiskGateChange;
		Confirm.FlightId = Disrupted->Id;
		Confirm.RequestedGateId = TEXT("R1");
		Confirm.bConfirmWarning = true;
		return QueuePhase4(
			Test,
			Simulation,
			Confirm,
			TEXT("Confirm remote gate change"));
	}

	bool RespondAndRecover(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		uint64& CommandId)
	{
		const EPhase4CommandType Response[] = {
			EPhase4CommandType::HoldAffectedDepartures,
			EPhase4CommandType::DivertAffectedArrivals,
			EPhase4CommandType::DeployTowTeam,
			EPhase4CommandType::ProtectIncidentArea,
			EPhase4CommandType::ReviewIncidentCause,
			EPhase4CommandType::ApplyRecoveryPlan};
		for (const EPhase4CommandType Type : Response)
		{
			FPhase4Command Command;
			Command.Id = {CommandId++};
			Command.Type = Type;
			if (!QueuePhase4(
				Test,
				Simulation,
				Command,
				TEXT("Apply typed incident response")))
			{
				return false;
			}
		}
		return true;
	}

	bool CompleteRegionalWeek(
		FAutomationTestBase& Test,
		FSimulation& Simulation)
	{
		if (!BuildCompletePhase3(Test, Simulation))
		{
			return false;
		}
		uint64 CommandId = 1;
		if (!OpenAndPublishRegionalWeek(Test, Simulation, CommandId) ||
			!ConfirmGateChange(Test, Simulation, CommandId))
		{
			return false;
		}
		const int64 PublishedAt =
			Simulation.GetPhase4State().PublishedAtGameMilliseconds;
		AdvanceTo(
			Simulation,
			PublishedAt +
				GetPhase4Fixture().OperatingDayMilliseconds * 4);
		if (!Test.TestEqual(
			TEXT("Warned incident activates on day five"),
			Simulation.GetPhase4State().Incident.Lifecycle,
			EPhase4IncidentLifecycle::Alerted) ||
			!RespondAndRecover(Test, Simulation, CommandId))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			PublishedAt +
				GetPhase4Fixture().OperatingDayMilliseconds * 6);
		FPhase4Command Renew;
		Renew.Id = {CommandId++};
		Renew.Type = EPhase4CommandType::AcceptTenantRenewal;
		return QueuePhase4(
			Test,
			Simulation,
			Renew,
			TEXT("Accept tenant renewal"));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase4RegionalWeekTest,
	"AMSim.Phase4.RegionalWeek.CompleteDeterministicFixture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase4RegionalWeekTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation First(GetPhase4Fixture().Seed);
	if (!CompleteRegionalWeek(*this, First))
	{
		return false;
	}
	const FPhase4State& State = First.GetPhase4State();
	TestTrue(TEXT("Seven-day fixture completed"), State.bFixtureCompleted);
	TestTrue(TEXT("Week remains recoverable"), State.bRecoverable);
	TestEqual(TEXT("All 21 flights completed"), State.CompletedFlightCount, 21);
	TestEqual(TEXT("One early flight recorded"), State.EarlyFlightCount, 1);
	TestEqual(TEXT("One late flight recorded"), State.LateFlightCount, 1);
	TestEqual(TEXT("One gate change recorded"), State.GateChangeCount, 1);
	TestEqual(
		TEXT("One weather-restricted flight recorded"),
		State.WeatherRestrictedFlightCount,
		1);
	TestEqual(
		TEXT("Connected passenger cohort exact"),
		State.ConnectedPassengerCount,
		18);
	TestEqual(
		TEXT("Missed cohort exact"),
		State.MissedConnectionPassengerCount,
		6);
	TestEqual(
		TEXT("Missed cohort rebooked"),
		State.RebookedPassengerCount,
		6);
	TestEqual(
		TEXT("Transfer bags reconcile"),
		State.CompletedTransferBagCount,
		32);
	TestEqual(
		TEXT("International passengers reconcile"),
		State.BorderProcessedPassengerCount,
		96);
	TestEqual(TEXT("Rental-car access reconciles"), State.RentalCarPassengerCount, 42);
	TestEqual(TEXT("Rail access reconciles"), State.RailPassengerCount, 58);
	TestEqual(TEXT("One tenant renewal accepted"), State.RenewalAcceptedCount, 1);
	TestEqual(
		TEXT("Incident recovery complete"),
		State.Incident.Lifecycle,
		EPhase4IncidentLifecycle::Recovered);
	TestEqual(TEXT("Incident outcome remains abstract"), State.Incident.Outcome, FString(TEXT("No injuries")));
	TestTrue(TEXT("Incident warning stored"), State.Incident.bWarningShown);
	TestTrue(TEXT("Override decision stored"), State.Incident.bOverrideAcknowledged);
	TestTrue(TEXT("Incident report reviewed"), State.Incident.bReportReviewed);

	FSimulation Second(GetPhase4Fixture().Seed);
	if (!CompleteRegionalWeek(*this, Second))
	{
		return false;
	}
	return TestEqual(
		TEXT("Identical regional weeks produce identical checksums"),
		Second.CalculateChecksum(),
		First.CalculateChecksum());
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase4PersistenceTest,
	"AMSim.Phase4.Persistence.Schema5RoundTripMigrationAndRejection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase4PersistenceTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase4Fixture().Seed);
	if (!CompleteRegionalWeek(*this, Simulation))
	{
		return false;
	}
	TArray<uint8> Bytes;
	FSnapshot Loaded;
	FSimulation Restored;
	if (!TestTrue(
			TEXT("Schema 5 snapshot serializes"),
			SerializeSnapshot(Simulation.CreateSnapshot(), Bytes)) ||
		!TestTrue(
			TEXT("Schema 5 snapshot deserializes"),
			DeserializeSnapshot(Bytes, Loaded)) ||
		!TestTrue(
			TEXT("Schema 5 state restores"),
			Restored.RestoreSnapshot(Loaded)))
	{
		return false;
	}
	TestEqual(
		TEXT("Schema 5 round-trip checksum exact"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());

	FSnapshot Schema4 = Simulation.CreateSnapshot();
	Schema4.SchemaVersion = 4;
	Schema4.Phase4 = {};
	TArray<uint8> LegacyBytes;
	FSnapshot Migrated;
	TestTrue(
		TEXT("Schema 4 fixture serializes"),
		SerializeSnapshot(Schema4, LegacyBytes));
	TestTrue(
		TEXT("Schema 4 fixture migrates"),
		DeserializeSnapshot(LegacyBytes, Migrated));
	TestEqual(
		TEXT("Schema 4 migrates to schema 5"),
		Migrated.SchemaVersion,
		SnapshotSchemaVersion);
	TestFalse(
		TEXT("Migrated schema starts with empty Phase 4"),
		Migrated.Phase4.bInitialized);

	FSnapshot BrokenConnection = Simulation.CreateSnapshot();
	BrokenConnection.Phase4.Connections[0].InboundFlightId = {MAX_uint64};
	FSimulation RejectedConnection;
	TestFalse(
		TEXT("Broken connection flight reference is rejected"),
		RejectedConnection.RestoreSnapshot(BrokenConnection));

	FSnapshot UntraceableIncident = Simulation.CreateSnapshot();
	UntraceableIncident.Phase4.Incident.bWarningShown = false;
	FSimulation RejectedIncident;
	TestFalse(
		TEXT("Untraceable serious incident is rejected"),
		RejectedIncident.RestoreSnapshot(UntraceableIncident));

	FSnapshot Future = Simulation.CreateSnapshot();
	Future.SchemaVersion = SnapshotSchemaVersion + 1;
	TArray<uint8> FutureBytes;
	return TestFalse(
		TEXT("Future schema cannot serialize"),
		SerializeSnapshot(Future, FutureBytes));
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase4TraceableRiskTest,
	"AMSim.Phase4.Incident.NoSeriousIncidentWithoutWarnedOverride",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase4TraceableRiskTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase4Fixture().Seed);
	if (!BuildCompletePhase3(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 1;
	if (!OpenAndPublishRegionalWeek(*this, Simulation, CommandId))
	{
		return false;
	}
	AdvanceTo(
		Simulation,
		Simulation.GetPhase4State().PublishedAtGameMilliseconds +
			GetPhase4Fixture().OperatingDayMilliseconds * 4);
	TestEqual(
		TEXT("No incident materializes without an acknowledged warning"),
		Simulation.GetPhase4State().Incident.Lifecycle,
		EPhase4IncidentLifecycle::None);
	return TestFalse(
		TEXT("Unwarned risk does not close the runway"),
		Simulation.GetPhase4State().Incident.bRunwayClosed);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase4ContentTest,
	"AMSim.Phase4.Content.CatalogAndTwoDimensionalLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase4ContentTest::RunTest(const FString& Parameters)
{
	const FAMSimPhase4CatalogValidation Validation =
		FAMSimPhase4ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(TEXT("Phase 4 catalog validates"), Validation.bValid);
	TestEqual(
		TEXT("All required Phase 4 definitions resolve"),
		Validation.Assets.Num(),
		FAMSimPhase4ContentCatalog::RequiredContentIds().Num());
	return Validation.bValid;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase4PresentationMappingTest,
	"AMSim.Phase4.Presentation.ViewStateAndPaper2DIncidentLayer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase4PresentationMappingTest::RunTest(
	const FString& Parameters)
{
	AMSim::FPhase4QuerySnapshot Query;
	Query.Revision = 91;
	Query.bUnlocked = true;
	Query.bInitialized = true;
	Query.bTimetablePublished = true;
	Query.ContractCount = 3;
	Query.AcceptedContractCount = 3;
	Query.IncidentLifecycle =
		AMSim::EPhase4IncidentLifecycle::Alerted;
	Query.PrimaryStatus = TEXT("Incident response active");
	Query.TimetableSummary = TEXT("21 FLIGHTS · 7 DAYS");
	Query.IncidentHeadline = TEXT("RUNWAY 27 · ALERTED");
	Query.Caption = TEXT("Rescue vehicles entering Runway 27.");
	AMSim::FPhase4State State;
	State.bInitialized = true;
	State.bTimetablePublished = true;
	State.Incident.Lifecycle =
		AMSim::EPhase4IncidentLifecycle::Alerted;

	const AMSim::FPhase4ViewState View =
		AMSim::MakePhase4ViewState(Query, State);
	TestEqual(
		TEXT("Regional view preserves revision gating"),
		View.Revision,
		Query.Revision);
	TestTrue(
		TEXT("Alerted incident switches to response surface"),
		View.bIncidentMode);
	TestTrue(
		TEXT("Initial incident exposes hold action"),
		View.bCanHold);
	TestFalse(
		TEXT("Recovery remains unavailable before cause review"),
		View.bCanRecover);
	TestTrue(
		TEXT("Timetable copy remains query-derived"),
		View.Timetable.Contains(TEXT("21 FLIGHTS")));
	AMSim::FPhase4FlightRecord LockedFlight;
	LockedFlight.bGateChanged = true;
	LockedFlight.bOverrideRecorded = true;
	const AMSim::FPhase4FlightCardViewState LockedCard =
		AMSim::MakePhase4FlightCardViewState(LockedFlight);
	TestTrue(
		TEXT("Locked-horizon risk is localized with text and pattern"),
		LockedCard.bWarning &&
			LockedCard.RiskLabel.Contains(TEXT("LOCK RISK")) &&
			LockedCard.RiskLabel.Contains(TEXT("///")));
	AMSim::FPhase4FlightRecord WeatherFlight;
	WeatherFlight.bWeatherRestricted = true;
	const AMSim::FPhase4FlightCardViewState WeatherCard =
		AMSim::MakePhase4FlightCardViewState(WeatherFlight);
	TestTrue(
		TEXT("Weather conflict keeps a color-independent card pattern"),
		WeatherCard.bWarning &&
			WeatherCard.RiskLabel.Contains(TEXT("WX CONFLICT")) &&
			WeatherCard.RiskLabel.Contains(TEXT("///")));

	const AAMSimWorldPresenter* Presenter =
		GetDefault<AAMSimWorldPresenter>();
	TestTrue(
		TEXT("Phase 4 incident layer reuses cooker-visible Paper2D assets"),
		Presenter->HasRequiredPresentationAssets());
	TestEqual(
		TEXT("Response proxy pool starts inactive"),
		Presenter->GetActivePhase4ResponseProxyCount(),
		0);
	return true;
}
