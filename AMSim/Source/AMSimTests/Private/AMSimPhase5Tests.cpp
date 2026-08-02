#include "AMSimPhase1Fixture.h"
#include "AMSimPhase5Content.h"
#include "AMSimPhase5Fixture.h"
#include "AMSimPhase5Simulation.h"
#include "AMSimPhase5ViewState.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	using namespace AMSim;

	bool QueuePhase1(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase1Command Command)
	{
		if (!Test.TestEqual(
			TEXT("Phase 1 command accepted"),
			Simulation.QueuePhase1Command(Command),
			EPhase1CommandResult::Accepted))
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
		FPhase1Command Create;
		Create.Id = {1};
		Create.Type = EPhase1CommandType::CreateAirport;
		Create.AirportName = TEXT("Phase 5 Test Field");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueuePhase1(Test, Simulation, Create))
		{
			return false;
		}
		FPhase1Command Build;
		Build.Id = {2};
		Build.Type = EPhase1CommandType::CommitStarterPlan;
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueuePhase1(Test, Simulation, Build))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Simulation.GetPhase1State().Project.FundedAtGameMilliseconds +
				GetPhase1Fixture().ReadyToOpenAtMilliseconds);
		FPhase1Command Open;
		Open.Id = {3};
		Open.Type = EPhase1CommandType::OpenAirport;
		if (!QueuePhase1(Test, Simulation, Open))
		{
			return false;
		}
		FPhase1Command Accept;
		Accept.Id = {4};
		Accept.Type = EPhase1CommandType::AcceptStarterOffer;
		if (!QueuePhase1(Test, Simulation, Accept))
		{
			return false;
		}
		FPhase1Command Schedule;
		Schedule.Id = {5};
		Schedule.Type = EPhase1CommandType::ScheduleStarterFlight;
		Schedule.RequestedStandDefinitionId =
			TEXT("Facility.GAStand.Starter");
		const int64 Now =
			Simulation.CreateQuerySnapshot().GameTimeMilliseconds;
		const int64 Increment =
			GetPhase1Fixture().TimetableIncrementMilliseconds;
		Schedule.ScheduledArrivalGameMilliseconds =
			((Now + Increment - 1) / Increment) * Increment;
		if (!QueuePhase1(Test, Simulation, Schedule))
		{
			return false;
		}
		AdvanceTo(
			Simulation,
			Schedule.ScheduledArrivalGameMilliseconds +
				GetPhase1Fixture().FlightCompletedOffsetMilliseconds);
		FPhase2Command Initialize;
		Initialize.Id = {1};
		Initialize.Type = EPhase2CommandType::InitializeLivingAirport;
		if (!Test.TestEqual(
			TEXT("Living airport initializes"),
			Simulation.QueuePhase2Command(Initialize),
			EPhase2CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	bool QueuePhase5(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase5Command Command)
	{
		if (!Test.TestEqual(
			TEXT("Phase 5 command accepted"),
			Simulation.QueuePhase5Command(Command),
			EPhase5CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	void AdvanceSteps(FSimulation& Simulation, const int32 Count)
	{
		for (int32 Index = 0; Index < Count; ++Index)
		{
			Simulation.Step();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase5AdvancedPathIndependenceTest,
	"AMSim.Phase5.Progression.SixIndependentAdvancedPaths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase5AdvancedPathIndependenceTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	for (int32 Index = 0; Index < 6; ++Index)
	{
		const ESpecializationPath Path =
			static_cast<ESpecializationPath>(Index);
		const FPhase5PathEvidenceRecord Evidence =
			MakeAdvancedPathEvidenceFixture(Path);
		TestTrue(
			*FString::Printf(
				TEXT("%s reaches Advanced"),
				*SpecializationPathDisplayName(Path)),
			MeetsAdvancedRequirements(Evidence));
		if (Path == ESpecializationPath::Cargo)
		{
			TestFalse(
				TEXT("Cargo fixture has no passenger terminal"),
				Evidence.FacilityIds.Contains(TEXT("Facility.Terminal.Domestic")));
		}
		else if (Path == ESpecializationPath::Passenger)
		{
			TestFalse(
				TEXT("Passenger fixture has no cargo warehouse"),
				Evidence.FacilityIds.Contains(TEXT("Facility.Cargo.Warehouse")));
		}
		else if (Path != ESpecializationPath::Mixed)
		{
			TestFalse(
				TEXT("GA, school, and charter fixtures have no terminal"),
				Evidence.FacilityIds.Contains(TEXT("Facility.Terminal.Domestic")));
			TestFalse(
				TEXT("GA, school, and charter fixtures have no warehouse"),
				Evidence.FacilityIds.Contains(TEXT("Facility.Cargo.Warehouse")));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase5CargoContinuityTest,
	"AMSim.Phase5.Cargo.AllClassesFlowsDispatchAndPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase5CargoContinuityTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase5Fixture().Seed);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 1;
	FPhase5Command Initialize;
	Initialize.Id = {CommandId++};
	Initialize.Type = EPhase5CommandType::InitializeBreadth;
	if (!QueuePhase5(*this, Simulation, Initialize))
	{
		return false;
	}
	TestEqual(
		TEXT("Warehouse has four compatible zones"),
		Simulation.GetPhase5State().WarehouseZones.Num(),
		4);
	TestTrue(
		TEXT("Ordinary cargo vehicle exists"),
		Simulation.GetPhase2State().Vehicles.ContainsByPredicate(
			[](const FPhase2VehicleRecord& Vehicle)
			{
				return Vehicle.CapabilityId == TEXT("Service.CargoHandling");
			}));

	for (int32 Index = 0; Index < 5; ++Index)
	{
		FPhase5Command Accept;
		Accept.Id = {CommandId++};
		Accept.Type = EPhase5CommandType::AcceptNextCargoContract;
		if (!QueuePhase5(*this, Simulation, Accept))
		{
			return false;
		}
	}

	for (int32 Round = 0; Round < 18; ++Round)
	{
		TArray<FCargoShipmentId> ReadyShipments;
		for (const FPhase5ShipmentRecord& Shipment :
			Simulation.GetPhase5State().Shipments)
		{
			if (Shipment.State != ECargoState::Offered &&
				Shipment.State != ECargoState::Completed &&
				Shipment.State != ECargoState::Exception &&
				!Shipment.ActiveServiceTaskId.IsValid())
			{
				ReadyShipments.Add(Shipment.Id);
			}
		}
		for (const FCargoShipmentId ShipmentId : ReadyShipments)
		{
			FPhase5Command Advance;
			Advance.Id = {CommandId++};
			Advance.Type = EPhase5CommandType::AdvanceNextCargoShipment;
			Advance.ShipmentId = ShipmentId;
			TestEqual(
				TEXT("Cargo stage command queues"),
				Simulation.QueuePhase5Command(Advance),
				EPhase5CommandResult::Accepted);
		}
		Simulation.Step();
		AdvanceSteps(Simulation, 35);
	}

	for (int32 RecurringIndex = 0; RecurringIndex < 7; ++RecurringIndex)
	{
		FPhase5Command Accept;
		Accept.Id = {CommandId++};
		Accept.Type = EPhase5CommandType::AcceptNextCargoContract;
		if (!QueuePhase5(*this, Simulation, Accept))
		{
			return false;
		}
		for (int32 Round = 0; Round < 18; ++Round)
		{
			const FPhase5ShipmentRecord* Ready =
				Simulation.GetPhase5State().Shipments.FindByPredicate(
					[](const FPhase5ShipmentRecord& Shipment)
					{
						return Shipment.State != ECargoState::Offered &&
							Shipment.State != ECargoState::Completed &&
							Shipment.State != ECargoState::Exception &&
							!Shipment.ActiveServiceTaskId.IsValid();
					});
			if (Ready)
			{
				FPhase5Command Advance;
				Advance.Id = {CommandId++};
				Advance.Type =
					EPhase5CommandType::AdvanceNextCargoShipment;
				Advance.ShipmentId = Ready->Id;
				if (!QueuePhase5(*this, Simulation, Advance))
				{
					return false;
				}
			}
			AdvanceSteps(Simulation, 35);
		}
	}

	const FPhase5State& State = Simulation.GetPhase5State();
	TestEqual(
		TEXT("Recurring cargo reaches the Advanced shipment threshold"),
		State.CompletedShipmentCount,
		12);
	TestEqual(TEXT("All cargo classes complete"), State.CompletedCargoClassCount, 4);
	TestEqual(TEXT("All cargo flows complete"), State.CompletedCargoFlowCount, 3);
	TestTrue(
		TEXT("Belly freight completes through the same continuity model"),
		State.Shipments.ContainsByPredicate(
			[](const FPhase5ShipmentRecord& Shipment)
			{
				return Shipment.bBellyFreight &&
					Shipment.State == ECargoState::Completed;
			}));
	TestTrue(
		TEXT("Cargo tasks are owned by Phase 5 through ordinary dispatch"),
		Simulation.GetPhase2State().ServiceTasks.ContainsByPredicate(
			[](const FPhase2ServiceTaskRecord& Task)
			{
				return Task.OwnerDomain == TEXT("Phase5") &&
					Task.OwnerId != 0 &&
					Task.Quantity > 0 &&
					Task.State == EPhase2ServiceState::Completed;
			}));

	TArray<uint8> Bytes;
	TestTrue(
		TEXT("Schema 6 serializes"),
		SerializeSnapshot(Simulation.CreateSnapshot(), Bytes));
	FSnapshot Loaded;
	TestTrue(TEXT("Schema 6 deserializes"), DeserializeSnapshot(Bytes, Loaded));
	FSimulation Restored;
	TestTrue(TEXT("Schema 6 restores"), Restored.RestoreSnapshot(Loaded));
	TestEqual(
		TEXT("Cargo checksum survives round trip"),
		Restored.CalculateChecksum(),
		Simulation.CalculateChecksum());

	FSnapshot Corrupt = Simulation.CreateSnapshot();
	Corrupt.Phase5.WarehouseZones[0].OccupiedLitres = 1;
	FSimulation Rejected;
	TestFalse(
		TEXT("Cargo occupancy corruption is rejected"),
		Rejected.RestoreSnapshot(Corrupt));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase5TenantAndEventTest,
	"AMSim.Phase5.Breadth.TenantRecoveryAndEightEventFamilies",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase5TenantAndEventTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase5Fixture().Seed + 1);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	uint64 CommandId = 1;
	FPhase5Command Initialize;
	Initialize.Id = {CommandId++};
	Initialize.Type = EPhase5CommandType::InitializeBreadth;
	if (!QueuePhase5(*this, Simulation, Initialize))
	{
		return false;
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		FPhase5Command AcceptTenant;
		AcceptTenant.Id = {CommandId++};
		AcceptTenant.Type = EPhase5CommandType::AcceptNextProviderTenant;
		if (!QueuePhase5(*this, Simulation, AcceptTenant))
		{
			return false;
		}
	}
	TestEqual(
		TEXT("All provider and concession tenants are active"),
		Simulation.CreatePhase5QuerySnapshot().ActiveProviderCount,
		6);
	for (const FPhase5ProviderTenantRecord& Tenant :
		Simulation.GetPhase5State().ProviderTenants)
	{
		TestTrue(TEXT("Tenant exposes footprint"), !Tenant.FootprintId.IsNone());
		TestTrue(
			TEXT("Tenant exposes required capability"),
			!Tenant.RequiredCapability.IsNone());
		TestTrue(
			TEXT("Tenant exposes contract term"),
			Tenant.ContractTermOperatingDays > 0);
		TestTrue(
			TEXT("Tenant exposes service expectations"),
			!Tenant.ServiceExpectations.IsEmpty());
		TestTrue(
			TEXT("Tenant exposes operating pattern"),
			!Tenant.OperatingPattern.IsEmpty());
		TestTrue(
			TEXT("Tenant exposes satisfaction drivers"),
			!Tenant.SatisfactionDrivers.IsEmpty());
	}
	FPhase5Command Lose;
	Lose.Id = {CommandId++};
	Lose.Type = EPhase5CommandType::TriggerTenantRequirementLoss;
	QueuePhase5(*this, Simulation, Lose);
	TestEqual(
		TEXT("Requirement loss starts grace"),
		Simulation.CreatePhase5QuerySnapshot().GraceProviderCount,
		1);
	FPhase5Command Recover;
	Recover.Id = {CommandId++};
	Recover.Type = EPhase5CommandType::RecoverTenant;
	QueuePhase5(*this, Simulation, Recover);
	TestEqual(
		TEXT("Tenant recovers without termination"),
		Simulation.CreatePhase5QuerySnapshot().GraceProviderCount,
		0);

	for (int32 Index = 0; Index < 5; ++Index)
	{
		FPhase5Command AcceptCargo;
		AcceptCargo.Id = {CommandId++};
		AcceptCargo.Type = EPhase5CommandType::AcceptNextCargoContract;
		QueuePhase5(*this, Simulation, AcceptCargo);
	}

	int32 ExercisedFamilies = 0;
	for (int32 EventIndex = 0; EventIndex < 8; ++EventIndex)
	{
		FPhase5Command Accept;
		Accept.Id = {CommandId++};
		Accept.Type = EPhase5CommandType::AcceptNextEvent;
		if (!QueuePhase5(*this, Simulation, Accept))
		{
			return false;
		}
		const FPhase5SpecialEventRecord* Active =
			Simulation.GetPhase5State().SpecialEvents.FindByPredicate(
				[](const FPhase5SpecialEventRecord& Event)
				{
					return Event.State == ESpecialEventState::Preparing;
				});
		if (!TestNotNull(TEXT("Event enters preparation"), Active))
		{
			return false;
		}
		const FSpecialEventId ActiveId = Active->Id;
		TestTrue(
			TEXT("Event references ordinary-system entities"),
			!Active->LinkedEntityIds.IsEmpty());
		for (int32 Stage = 0; Stage < 4; ++Stage)
		{
			FPhase5Command Advance;
			Advance.Id = {CommandId++};
			Advance.Type = EPhase5CommandType::AdvanceActiveEvent;
			Advance.EventId = ActiveId;
			if (!QueuePhase5(*this, Simulation, Advance))
			{
				return false;
			}
			if (Stage == 1)
			{
				const FPhase5SpecialEventRecord* Partial =
					Simulation.GetPhase5State().SpecialEvents.FindByPredicate(
						[ActiveId](const FPhase5SpecialEventRecord& Event)
						{
							return Event.Id == ActiveId &&
								Event.State ==
									ESpecialEventState::PartialSuccess;
						});
				TestNotNull(
					TEXT("Event records a partial-success outcome"),
					Partial);
			}
		}
		++ExercisedFamilies;
	}
	TestEqual(TEXT("All eight event families exercised"), ExercisedFamilies, 8);

	FPhase5State EventState = Simulation.GetPhase5State();
	EventState.SpecialEvents[0].State = ESpecialEventState::Offered;
	FPhase5Simulation DeclineSimulation(EventState.MasterSeed);
	TestTrue(
		TEXT("Event state restores for decline proof"),
		DeclineSimulation.RestoreState(
			EventState,
			Simulation.GetPhase1State(),
			Simulation.GetPhase2State(),
			Simulation.GetPhase3State(),
			Simulation.GetPhase4State()));
	FPhase5Command Decline;
	Decline.Id = {999};
	Decline.Type = EPhase5CommandType::DeclineNextEvent;
	TestEqual(
		TEXT("Decline is accepted without penalty"),
		DeclineSimulation.QueueCommand(
			Decline,
			Simulation.CreateDiagnostics().GameTimeMilliseconds,
			Simulation.GetPhase1State(),
			Simulation.GetPhase2State(),
			Simulation.GetPhase3State(),
			Simulation.GetPhase4State()),
		EPhase5CommandResult::Accepted);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase5ContentCatalogTest,
	"AMSim.Phase5.Content.CookerVisiblePrimaryAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase5ContentCatalogTest::RunTest(const FString& Parameters)
{
	const FAMSimPhase5CatalogValidation Validation =
		FAMSimPhase5ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(TEXT("Phase 5 catalog validates"), Validation.bValid);
	TestEqual(
		TEXT("Every required Phase 5 Primary Asset resolves"),
		Validation.Assets.Num(),
		FAMSimPhase5ContentCatalog::RequiredContentIds().Num());
	return Validation.bValid;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase5PresentationMappingTest,
	"AMSim.Phase5.Presentation.ViewStateIsConciseAndRevisionGated",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase5PresentationMappingTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase5Fixture().Seed + 2);
	if (!BuildLivingAirport(*this, Simulation))
	{
		return false;
	}
	FPhase5Command Initialize;
	Initialize.Id = {1};
	Initialize.Type = EPhase5CommandType::InitializeBreadth;
	if (!QueuePhase5(*this, Simulation, Initialize))
	{
		return false;
	}
	const FPhase5QuerySnapshot Query =
		Simulation.CreatePhase5QuerySnapshot();
	const FPhase5ViewState View =
		MakePhase5ViewState(Query, Simulation.GetPhase5State());
	TestEqual(TEXT("View retains query revision"), View.Revision, Query.Revision);
	TestEqual(TEXT("Five cargo cards map"), View.ContractCards.Num(), 5);
	TestEqual(TEXT("Four compatible zones map"), View.ZoneCards.Num(), 4);
	TestEqual(TEXT("Six provider cards map"), View.ProviderCards.Num(), 6);
	TestEqual(TEXT("Eight event cards map"), View.EventCards.Num(), 8);
	TestEqual(TEXT("Six capability cards map"), View.CapabilityCards.Num(), 6);
	TestTrue(
		TEXT("Capability copy keeps Major visibly future-locked"),
		View.Progression.Contains(TEXT("Major")) &&
			View.Progression.Contains(TEXT("locked")));
	return true;
}

#endif
