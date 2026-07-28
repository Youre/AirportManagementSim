#include "AMSimPhase1Fixture.h"
#include "AMSimPhase3Fixture.h"
#include "AMSimPhase5Fixture.h"
#include "AMSimPresentationProxyPool.h"
#include "AMSimPhase6Content.h"
#include "AMSimPhase6Fixture.h"
#include "AMSimPhase6Simulation.h"
#include "AMSimPhase6ViewState.h"
#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	using namespace AMSim;

	FPhase5State MakePhase6ReadyState()
	{
		FPhase5State State;
		State.bInitialized = true;
		State.MasterSeed = GetPhase6Fixture().Seed;
		State.AdvancedPathCount = 1;
		FPhase5PathEvidenceRecord Passenger =
			MakeAdvancedPathEvidenceFixture(
				ESpecializationPath::Passenger);
		Passenger.EarnedBand = ECapabilityBand::Advanced;
		Passenger.OperationalBand = ECapabilityBand::Advanced;
		State.Paths.Add(Passenger);
		return State;
	}

	bool InitializePhase1ForPhase6(
		FAutomationTestBase& Test,
		FStarterAirfieldSimulation& Phase1)
	{
		FPhase1Command Create;
		Create.Id = {1};
		Create.Type = EPhase1CommandType::CreateAirport;
		Create.AirportName = TEXT("Phase 6 Test Airport");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!Test.TestEqual(
			TEXT("Phase 1 initializes for Phase 6"),
			Phase1.QueueCommand(Create, 0),
			EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Phase1.Step(FixedStepMilliseconds);
		return Phase1.ApplyExternalEconomyChange(
			TEXT("Phase6.TestCapital"),
			200000,
			GetPhase6Fixture().MajorAirportPoints,
			TEXT("Prepared Major-capability test capital."),
			FixedStepMilliseconds);
	}

	bool QueuePhase6(
		FAutomationTestBase& Test,
		FPhase6Simulation& Phase6,
		FStarterAirfieldSimulation& Phase1,
		FLivingAirportSimulation& Phase2,
		const FPhase3State& Phase3,
		const FPhase4State& Phase4,
		const FPhase5State& Phase5,
		FPhase6Command Command,
		const int64 CurrentGameMilliseconds,
		const TCHAR* Label)
	{
		if (!Test.TestEqual(
			Label,
			Phase6.QueueCommand(
				Command,
				CurrentGameMilliseconds,
				Phase1.GetState(),
				Phase2.GetState(),
				Phase3,
				Phase4,
				Phase5),
			EPhase6CommandResult::Accepted))
		{
			return false;
		}
		Phase6.Step(
			CurrentGameMilliseconds,
			Phase1,
			Phase2,
			Phase3,
			Phase4,
			Phase5);
		return true;
	}

	bool QueueSimulationPhase1(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase1Command Command)
	{
		if (!Test.TestEqual(
			TEXT("Phase 1 setup command accepted"),
			Simulation.QueuePhase1Command(Command),
			EPhase1CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	void AdvanceSimulationTo(
		FSimulation& Simulation,
		const int64 TargetMilliseconds)
	{
		while (Simulation.CreateQuerySnapshot().GameTimeMilliseconds <
			TargetMilliseconds)
		{
			Simulation.Step();
		}
	}

	bool BuildLivingAirportForPhase6(
		FAutomationTestBase& Test,
		FSimulation& Simulation)
	{
		FPhase1Command Create;
		Create.Id = {1};
		Create.Type = EPhase1CommandType::CreateAirport;
		Create.AirportName = TEXT("Phase 6 Integration Airport");
		Create.MapId = GetPhase1Fixture().MapId;
		if (!QueueSimulationPhase1(Test, Simulation, Create))
		{
			return false;
		}
		FPhase1Command Build;
		Build.Id = {2};
		Build.Type = EPhase1CommandType::CommitStarterPlan;
		Build.Proposal = CreateDefaultStarterPlan();
		if (!QueueSimulationPhase1(Test, Simulation, Build))
		{
			return false;
		}
		AdvanceSimulationTo(
			Simulation,
			Simulation.GetPhase1State().Project.FundedAtGameMilliseconds +
				GetPhase1Fixture().ReadyToOpenAtMilliseconds);
		FPhase1Command Open;
		Open.Id = {3};
		Open.Type = EPhase1CommandType::OpenAirport;
		if (!QueueSimulationPhase1(Test, Simulation, Open))
		{
			return false;
		}
		FPhase1Command Accept;
		Accept.Id = {4};
		Accept.Type = EPhase1CommandType::AcceptStarterOffer;
		if (!QueueSimulationPhase1(Test, Simulation, Accept))
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
		if (!QueueSimulationPhase1(Test, Simulation, Schedule))
		{
			return false;
		}
		AdvanceSimulationTo(
			Simulation,
			Schedule.ScheduledArrivalGameMilliseconds + 130000);
		FPhase2Command Initialize;
		Initialize.Id = {1};
		Initialize.Type = EPhase2CommandType::InitializeLivingAirport;
		if (!Test.TestEqual(
			TEXT("Living airport initializes for large-aircraft dispatch"),
			Simulation.QueuePhase2Command(Initialize),
			EPhase2CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}

	bool QueueSimulationPhase6(
		FAutomationTestBase& Test,
		FSimulation& Simulation,
		FPhase6Command Command,
		const TCHAR* Label)
	{
		if (!Test.TestEqual(
			Label,
			Simulation.QueuePhase6Command(Command),
			EPhase6CommandResult::Accepted))
		{
			return false;
		}
		Simulation.Step();
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6ContentCatalogTest,
	"AMSim.Phase6.Content.CookerVisiblePrimaryAssetsAnd2DLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6ContentCatalogTest::RunTest(const FString& Parameters)
{
	const FAMSimPhase6CatalogValidation Validation =
		FAMSimPhase6ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Validation.Errors)
	{
		AddError(Error);
	}
	TestTrue(
		TEXT("Phase 6 Primary Asset catalog validates"),
		Validation.bValid);
	TestEqual(
		TEXT("All required Phase 6 definitions are loaded"),
		Validation.Assets.Num(),
		FAMSimPhase6ContentCatalog::RequiredContentIds().Num());
	bool bAllAircraftPresentationAssetsExist = true;
	for (int32 Index = 0; Index < 16; ++Index)
	{
		const FString TexturePackage = FString::Printf(
			TEXT("/Game/Phase6/Presentation/Textures/Aircraft/"
				"T_RiverbendLongreach787_9_H%02d"),
			Index);
		const FString SpritePackage = FString::Printf(
			TEXT("/Game/Phase6/Presentation/Sprites/Aircraft/"
				"S_RiverbendLongreach787_9_H%02d"),
			Index);
		if (!FPackageName::DoesPackageExist(TexturePackage) ||
			!FPackageName::DoesPackageExist(SpritePackage))
		{
			AddError(FString::Printf(
				TEXT("Phase 6 aircraft heading %02d is not cooker-visible."),
				Index));
			bAllAircraftPresentationAssetsExist = false;
		}
	}
	TestTrue(
		TEXT("All 16 approved 787 texture/sprite heading pairs exist"),
		bAllAircraftPresentationAssetsExist);
	return Validation.bValid && bAllAircraftPresentationAssetsExist;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6ViewStateTest,
	"AMSim.Phase6.Presentation.ConciseRevisionedMajorSurface",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6ViewStateTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FPhase6QuerySnapshot Query;
	Query.Revision = 88;
	Query.bUnlocked = true;
	Query.bInitialized = true;
	Query.PrimaryStatus = TEXT("2 / 6 Major paths");
	Query.RunwaySummary = TEXT("2 operational runways");
	Query.CapacitySummary = TEXT("8 operational facilities");
	Query.LargeAircraftSummary = TEXT("RL 602 at Stand H1");
	Query.IncidentSummary = TEXT("No active incident");
	Query.ProgressionSummary = TEXT("Major evidence is permanent");

	FPhase6State State;
	State.bInitialized = true;
	State.Facilities = MakeMajorFacilityDefinitions();
	FPhase6RunwayRecord Runway;
	Runway.DisplayName = TEXT("Runway 09L/27R");
	Runway.PrimaryEndId = TEXT("09L");
	Runway.ReciprocalEndId = TEXT("27R");
	Runway.DeclaredLengthMeters = 3000;
	Runway.WidthMeters = 45;
	Runway.bOperational = true;
	State.Runways.Add(Runway);
	Runway.DisplayName = TEXT("Runway 09R/27L");
	Runway.PrimaryEndId = TEXT("09R");
	Runway.ReciprocalEndId = TEXT("27L");
	State.Runways.Add(Runway);
	for (int32 Index = 0; Index < 6; ++Index)
	{
		FPhase6PathMetricRecord Metric;
		Metric.Path = static_cast<ESpecializationPath>(Index);
		Metric.CompletedOperations = Index * 4;
		State.PathMetrics.Add(Metric);
	}
	const FPhase6ViewState View = MakePhase6ViewState(
		Query,
		State,
		ESpecializationPath::Cargo);
	TestEqual(TEXT("Revision maps exactly"), View.Revision, Query.Revision);
	TestEqual(TEXT("Two runway cards"), View.RunwayCards.Num(), 2);
	TestEqual(TEXT("Twelve capacity cards"), View.FacilityCards.Num(), 12);
	TestEqual(TEXT("Six equal-weight path cards"), View.CapabilityCards.Num(), 6);
	TestEqual(
		TEXT("Selected path remains concise"),
		View.SelectedPathHeadline,
		FString(TEXT("CARGO")));
	TestTrue(
		TEXT("Major surface exposes the next evidence action"),
		!View.NextEvidenceLabel.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6RunwayNumberingTest,
	"AMSim.Phase6.Runways.ReciprocalAndParallelNumbering",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6RunwayNumberingTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	TestEqual(
		TEXT("090 left is 09L"),
		RunwayEndLabel(90, ERunwaySide::Left),
		FString(TEXT("09L")));
	TestEqual(
		TEXT("Reciprocal left becomes right"),
		RunwayEndLabel(
			270,
			ReciprocalRunwaySide(ERunwaySide::Left)),
		FString(TEXT("27R")));
	TestEqual(
		TEXT("360 normalizes to runway 36"),
		RunwayEndLabel(0, ERunwaySide::Center),
		FString(TEXT("36C")));
	TestEqual(
		TEXT("Right reciprocal becomes left"),
		ReciprocalRunwaySide(ERunwaySide::Right),
		ERunwaySide::Left);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6MajorPathIndependenceTest,
	"AMSim.Phase6.Progression.SixIndependentMajorPaths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6MajorPathIndependenceTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	for (int32 Index = 0; Index < 6; ++Index)
	{
		const ESpecializationPath Path =
			static_cast<ESpecializationPath>(Index);
		FPhase5PathEvidenceRecord Evidence =
			MakeMajorPathEvidenceFixture(Path);
		TestTrue(
			*FString::Printf(
				TEXT("Path %d reaches Major with exact evidence"),
				Index),
			MeetsMajorRequirements(Evidence));

		FPhase5PathEvidenceRecord BelowThreshold = Evidence;
		BelowThreshold.QualifyingOperatingDays =
			GetPhase6Fixture().MajorOperatingDays - 1;
		TestFalse(
			TEXT("A missing qualifying day blocks Major"),
			MeetsMajorRequirements(BelowThreshold));

		if (Path == ESpecializationPath::Cargo)
		{
			TestFalse(
				TEXT("Cargo Major does not require a passenger concourse"),
				Evidence.FacilityIds.Contains(
					TEXT("Facility.Major.Passenger.Concourse")));
		}
		else if (Path == ESpecializationPath::Passenger)
		{
			TestFalse(
				TEXT("Passenger Major does not require a cargo hub"),
				Evidence.FacilityIds.Contains(
					TEXT("Facility.Major.Cargo.Hub")));
		}
		else if (Path != ESpecializationPath::Mixed)
		{
			TestFalse(
				TEXT("Independent non-passenger path has no concourse"),
				Evidence.FacilityIds.Contains(
					TEXT("Facility.Major.Passenger.Concourse")));
			TestFalse(
				TEXT("Independent non-cargo path has no cargo hub"),
				Evidence.FacilityIds.Contains(
					TEXT("Facility.Major.Cargo.Hub")));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6LargeAircraftIntegrationTest,
	"AMSim.Phase6.LargeAircraft.CompatibilityDispatchReconciliationAndPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6LargeAircraftIntegrationTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase6Fixture().Seed);
	if (!BuildLivingAirportForPhase6(*this, Simulation))
	{
		return false;
	}
	FPhase5Command InitializeBreadth;
	InitializeBreadth.Id = {1};
	InitializeBreadth.Type = EPhase5CommandType::InitializeBreadth;
	TestEqual(
		TEXT("Phase 5 baseline initializes"),
		Simulation.QueuePhase5Command(InitializeBreadth),
		EPhase5CommandResult::Accepted);
	Simulation.Step();

	FSnapshot Prepared = Simulation.CreateSnapshot();
	Prepared.Phase1.Credits += 200000;
	Prepared.Phase1.AirportPoints =
		GetPhase6Fixture().MajorAirportPoints;
	FPhase5PathEvidenceRecord* Passenger =
		Prepared.Phase5.Paths.FindByPredicate(
			[](const FPhase5PathEvidenceRecord& Path)
			{
				return Path.Path == ESpecializationPath::Passenger;
			});
	if (!TestNotNull(
		TEXT("Passenger capability path exists"),
		Passenger))
	{
		return false;
	}
	Passenger->Band = ECapabilityBand::Advanced;
	Passenger->EarnedBand = ECapabilityBand::Advanced;
	Passenger->OperationalBand = ECapabilityBand::Advanced;
	Prepared.Phase5.AdvancedPathCount = 1;
	if (!TestTrue(
		TEXT("Prepared Advanced save restores"),
		Simulation.RestoreSnapshot(Prepared)))
	{
		return false;
	}

	uint64 CommandId = 1;
	FPhase6Command Initialize;
	Initialize.Id = {CommandId++};
	Initialize.Type = EPhase6CommandType::InitializeMajorCapability;
	if (!QueueSimulationPhase6(
		*this,
		Simulation,
		Initialize,
		TEXT("Major capability initializes from earned Advanced")))
	{
		return false;
	}

	const EMajorFacilityType RequiredFacilities[] = {
		EMajorFacilityType::LargeStand,
		EMajorFacilityType::TerminalConcourse,
		EMajorFacilityType::BaggageHall,
		EMajorFacilityType::ServiceDepot,
		EMajorFacilityType::EmergencyStation};
	for (const EMajorFacilityType Type : RequiredFacilities)
	{
		const FMajorFacilityRecord* Facility =
			Simulation.GetPhase6State().Facilities.FindByPredicate(
				[Type](const FMajorFacilityRecord& Candidate)
				{
					return Candidate.Type == Type;
				});
		if (!TestNotNull(
			TEXT("Required large-aircraft facility exists"),
			Facility))
		{
			return false;
		}
		FPhase6Command Construct;
		Construct.Id = {CommandId++};
		Construct.Type = EPhase6CommandType::ConstructFacility;
		Construct.FacilityId = Facility->Id;
		if (!QueueSimulationPhase6(
			*this,
			Simulation,
			Construct,
			TEXT("Large-aircraft facility construction starts")))
		{
			return false;
		}
	}
	const int64 ConstructionTarget =
		Simulation.CreateQuerySnapshot().GameTimeMilliseconds +
		GetPhase6Fixture().FacilityConstructionMilliseconds +
		FixedStepMilliseconds;
	AdvanceSimulationTo(Simulation, ConstructionTarget);
	for (const EMajorFacilityType Type : RequiredFacilities)
	{
		TestTrue(
			TEXT("Required facility becomes staffed and operational"),
			Simulation.GetPhase6State().Facilities.ContainsByPredicate(
				[Type](const FMajorFacilityRecord& Facility)
				{
					return Facility.Type == Type &&
						Facility.State ==
							EMajorFacilityState::Operational &&
						Facility.bStaffed;
				}));
	}

	FPhase6Command AcceptLarge;
	AcceptLarge.Id = {CommandId++};
	AcceptLarge.Type =
		EPhase6CommandType::AcceptLargeAircraftOffer;
	if (!QueueSimulationPhase6(
		*this,
		Simulation,
		AcceptLarge,
		TEXT("Compatible 787-9 operation is accepted")))
	{
		return false;
	}
	AdvanceSimulationTo(
		Simulation,
		Simulation.GetPhase6State().LargeAircraftOperations[0]
				.ScheduledArrivalGameMilliseconds +
			FixedStepMilliseconds);
	const FLargeAircraftOperationId OperationId =
		Simulation.GetPhase6State().LargeAircraftOperations[0].Id;
	TestEqual(
		TEXT("Large aircraft arrives at the compatible stand"),
		Simulation.GetPhase6State().LargeAircraftOperations[0].State,
		ELargeAircraftOperationState::Arrived);

	FPhase6Command Start;
	Start.Id = {CommandId++};
	Start.Type =
		EPhase6CommandType::StartLargeAircraftTurnaround;
	Start.LargeAircraftOperationId = OperationId;
	if (!QueueSimulationPhase6(
		*this,
		Simulation,
		Start,
		TEXT("Turnaround requests ordinary dispatch tasks")))
	{
		return false;
	}
	TestEqual(
		TEXT("Eleven dependency-aware services are requested"),
		Simulation.GetPhase6State().LargeAircraftOperations[0]
			.RequiredServiceTaskCount,
		11);
	TestTrue(
		TEXT("Tasks are owned by Phase 6 in the ordinary dispatcher"),
		Simulation.GetPhase2State().ServiceTasks.ContainsByPredicate(
			[OperationId](const FPhase2ServiceTaskRecord& Task)
			{
				return Task.OwnerDomain == TEXT("Phase6") &&
					Task.OwnerId == OperationId.Value;
			}));

	TArray<uint8> MidTurnaroundBytes;
	TestTrue(
		TEXT("Mid-turnaround schema 7 snapshot serializes"),
		SerializeSnapshot(
			Simulation.CreateSnapshot(),
			MidTurnaroundBytes));
	FSnapshot MidTurnaround;
	TestTrue(
		TEXT("Mid-turnaround schema 7 snapshot deserializes"),
		DeserializeSnapshot(MidTurnaroundBytes, MidTurnaround));
	FSimulation Restored(GetPhase6Fixture().Seed);
	if (!TestTrue(
		TEXT("Mid-turnaround Phase 6 tasks restore"),
		Restored.RestoreSnapshot(MidTurnaround)))
	{
		return false;
	}

	for (int32 StepIndex = 0; StepIndex < 1200; ++StepIndex)
	{
		Restored.Step();
		const FLargeAircraftOperationRecord& Operation =
			Restored.GetPhase6State().LargeAircraftOperations[0];
		if (Operation.CompletedServiceTaskCount ==
			Operation.RequiredServiceTaskCount)
		{
			break;
		}
	}
	const FLargeAircraftOperationRecord& Ready =
		Restored.GetPhase6State().LargeAircraftOperations[0];
	TestEqual(
		TEXT("All ordinary dispatch tasks complete"),
		Ready.CompletedServiceTaskCount,
		Ready.RequiredServiceTaskCount);
	TestTrue(
		TEXT("Passenger and bag reconciliation complete"),
		Ready.bPassengerReconciled && Ready.bBagReconciled);
	TestEqual(
		TEXT("Every accepted bag is loaded"),
		Ready.LoadedBagCount,
		Ready.AcceptedBagCount);

	for (int32 AdvanceIndex = 0; AdvanceIndex < 3; ++AdvanceIndex)
	{
		FPhase6Command Advance;
		Advance.Id = {CommandId++};
		Advance.Type =
			EPhase6CommandType::AdvanceLargeAircraftOperation;
		Advance.LargeAircraftOperationId = OperationId;
		if (!QueueSimulationPhase6(
			*this,
			Restored,
			Advance,
			TEXT("Reconciled large-aircraft operation advances")))
		{
			return false;
		}
	}
	TestEqual(
		TEXT("Large-aircraft operation completes"),
		Restored.GetPhase6State().LargeAircraftOperations[0].State,
		ELargeAircraftOperationState::Completed);
	TestEqual(
		TEXT("Completion is recognized exactly once"),
		Restored.GetPhase6State().CompletedLargeAircraftCount,
		1);
	TestEqual(
		TEXT("Major revenue is recorded"),
		Restored.GetPhase6State().TotalMajorRevenueCredits,
		15000ll);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6IncidentContinuityTest,
	"AMSim.Phase6.Incident.WarningContinuityRecoveryAndRestore",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6IncidentContinuityTest::RunTest(
	const FString& Parameters)
{
	using namespace AMSim;
	const uint64 Seed = GetPhase6Fixture().Seed;
	FStarterAirfieldSimulation Phase1(Seed);
	FLivingAirportSimulation Phase2(Seed);
	FPhase6Simulation Phase6(Seed);
	FPhase3State Phase3;
	FPhase4State Phase4;
	const FPhase5State Phase5 = MakePhase6ReadyState();
	if (!InitializePhase1ForPhase6(*this, Phase1))
	{
		return false;
	}

	uint64 CommandId = 1;
	int64 Time = FixedStepMilliseconds;
	FPhase6Command Initialize;
	Initialize.Id = {CommandId++};
	Initialize.Type =
		EPhase6CommandType::InitializeMajorCapability;
	if (!QueuePhase6(
		*this,
		Phase6,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		Initialize,
		Time,
		TEXT("Major capability initializes")))
	{
		return false;
	}

	const FMajorFacilityRecord* ParallelProject =
		Phase6.GetState().Facilities.FindByPredicate(
			[](const FMajorFacilityRecord& Facility)
			{
				return Facility.Type ==
					EMajorFacilityType::ParallelRunway;
			});
	if (!TestNotNull(
		TEXT("Parallel runway project exists"),
		ParallelProject))
	{
		return false;
	}
	FPhase6Command ConstructParallel;
	ConstructParallel.Id = {CommandId++};
	ConstructParallel.Type =
		EPhase6CommandType::ConstructFacility;
	ConstructParallel.FacilityId = ParallelProject->Id;
	if (!QueuePhase6(
		*this,
		Phase6,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		ConstructParallel,
		Time += FixedStepMilliseconds,
		TEXT("Parallel runway construction starts")))
	{
		return false;
	}
	Time += GetPhase6Fixture().FacilityConstructionMilliseconds;
	Phase6.Step(Time, Phase1, Phase2, Phase3, Phase4, Phase5);
	FPhase6Command ActivateParallel;
	ActivateParallel.Id = {CommandId++};
	ActivateParallel.Type =
		EPhase6CommandType::ActivateParallelRunways;
	if (!QueuePhase6(
		*this,
		Phase6,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		ActivateParallel,
		Time += FixedStepMilliseconds,
		TEXT("Parallel runway activates")))
	{
		return false;
	}

	FPhase6Command Risk;
	Risk.Id = {CommandId++};
	Risk.Type = EPhase6CommandType::AcknowledgeSeriousRisk;
	Risk.Path = ESpecializationPath::Passenger;
	TestEqual(
		TEXT("Risk first returns a visible confirmation warning"),
		Phase6.QueueCommand(
			Risk,
			Time,
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3,
			Phase4,
			Phase5),
		EPhase6CommandResult::AcceptedWithWarning);
	TestEqual(
		TEXT("Unconfirmed warning does not mutate the incident"),
		Phase6.GetState().SeriousIncident.Lifecycle,
		ESeriousIncidentLifecycle::None);
	Risk.bConfirmWarning = true;
	if (!QueuePhase6(
		*this,
		Phase6,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		Risk,
		Time += FixedStepMilliseconds,
		TEXT("Disclosed risk confirmation accepted")))
	{
		return false;
	}

	FPhase6Command Materialize;
	Materialize.Id = {CommandId++};
	Materialize.Type =
		EPhase6CommandType::MaterializeSeriousIncident;
	if (!QueuePhase6(
		*this,
		Phase6,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		Materialize,
		Time += FixedStepMilliseconds,
		TEXT("Warned incident materializes")))
	{
		return false;
	}
	TestTrue(
		TEXT("Incident stores abstract human outcome"),
		Phase6.GetState().SeriousIncident.HumanOutcome !=
			EAbstractHumanOutcome::NotDetermined);
	TestTrue(
		TEXT("Aircraft loss remains traceably warned"),
		Phase6.GetState().SeriousIncident.bAircraftLost &&
			Phase6.GetState().SeriousIncident.bWarningShown &&
			Phase6.GetState().SeriousIncident.bRiskAcknowledged);

	const FPhase6State MidIncident = Phase6.GetState();
	FPhase6Simulation Restored(Seed);
	TestTrue(
		TEXT("Mid-incident state restores"),
		Restored.RestoreState(
			MidIncident,
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3,
			Phase4,
			Phase5));
	TestEqual(
		TEXT("Restore preserves deterministic checksum"),
		Restored.CalculateChecksum(),
		Phase6.CalculateChecksum());

	const EPhase6CommandType RecoveryFlow[] = {
		EPhase6CommandType::DispatchEmergencyResponse,
		EPhase6CommandType::ProtectIncidentArea,
		EPhase6CommandType::StabilizeIncident,
		EPhase6CommandType::InvestigateIncident,
		EPhase6CommandType::ApplyIncidentRecovery,
		EPhase6CommandType::BeginIncidentRepair};
	for (const EPhase6CommandType Type : RecoveryFlow)
	{
		FPhase6Command Command;
		Command.Id = {CommandId++};
		Command.Type = Type;
		if (!QueuePhase6(
			*this,
			Restored,
			Phase1,
			Phase2,
			Phase3,
			Phase4,
			Phase5,
			Command,
			Time += FixedStepMilliseconds,
			TEXT("Incident recovery command accepted")))
		{
			return false;
		}
	}
	TestTrue(
		TEXT("Parallel safe operation remains available"),
		Restored.GetState().SeriousIncident.bOtherOperationsContinue);

	Time += GetPhase6Fixture().SeriousIncidentRepairMilliseconds;
	FPhase6Command CompleteRepair;
	CompleteRepair.Id = {CommandId++};
	CompleteRepair.Type =
		EPhase6CommandType::CompleteIncidentRepair;
	if (!QueuePhase6(
		*this,
		Restored,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		CompleteRepair,
		Time,
		TEXT("Bounded repair completes")))
	{
		return false;
	}
	FPhase6Command Reopen;
	Reopen.Id = {CommandId++};
	Reopen.Type = EPhase6CommandType::ReopenIncidentArea;
	if (!QueuePhase6(
		*this,
		Restored,
		Phase1,
		Phase2,
		Phase3,
		Phase4,
		Phase5,
		Reopen,
		Time += FixedStepMilliseconds,
		TEXT("Incident area reopens")))
	{
		return false;
	}
	TestEqual(
		TEXT("Incident reaches recovered lifecycle"),
		Restored.GetState().SeriousIncident.Lifecycle,
		ESeriousIncidentLifecycle::Recovered);
	TestEqual(
		TEXT("Recovery counted exactly once"),
		Restored.GetState().RecoveredSeriousIncidentCount,
		1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6SchemaMigrationTest,
	"AMSim.Phase6.Persistence.Schema7RoundTripAndSchema6Migration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6SchemaMigrationTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	FSimulation Simulation(GetPhase6Fixture().Seed);
	const FSnapshot Current = Simulation.CreateSnapshot();
	TArray<uint8> Bytes;
	TestTrue(
		TEXT("Schema 7 serializes"),
		SerializeSnapshot(Current, Bytes));
	FSnapshot RoundTrip;
	TestTrue(
		TEXT("Schema 7 deserializes"),
		DeserializeSnapshot(Bytes, RoundTrip));
	TestEqual(
		TEXT("Schema 7 remains current"),
		RoundTrip.SchemaVersion,
		SnapshotSchemaVersion);
	TestEqual(
		TEXT("Empty Phase 6 state checksum survives round trip"),
		RoundTrip.Phase6.NextDomainId,
		Current.Phase6.NextDomainId);

	FSnapshot Legacy = Current;
	Legacy.SchemaVersion = 6;
	Legacy.Phase5.Paths.Add(
		MakeAdvancedPathEvidenceFixture(
			ESpecializationPath::Cargo));
	Legacy.Phase5.Paths.Last().EarnedBand =
		ECapabilityBand::Unavailable;
	Legacy.Phase5.Paths.Last().OperationalBand =
		ECapabilityBand::Unavailable;
	TArray<uint8> LegacyBytes;
	TestTrue(
		TEXT("Schema 6 remains serializable for migration coverage"),
		SerializeSnapshot(Legacy, LegacyBytes));
	FSnapshot Migrated;
	TestTrue(
		TEXT("Schema 6 migrates through deserialization"),
		DeserializeSnapshot(LegacyBytes, Migrated));
	TestEqual(
		TEXT("Migrated snapshot reaches schema 7"),
		Migrated.SchemaVersion,
		SnapshotSchemaVersion);
	TestEqual(
		TEXT("Legacy band seeds permanent earned band"),
		Migrated.Phase5.Paths.Last().EarnedBand,
		Migrated.Phase5.Paths.Last().Band);
	TestEqual(
		TEXT("Legacy band seeds operational band"),
		Migrated.Phase5.Paths.Last().OperationalBand,
		Migrated.Phase5.Paths.Last().Band);
	TestFalse(
		TEXT("Schema 6 migration initializes an empty Phase 6 domain"),
		Migrated.Phase6.bInitialized);

	FSnapshot Future = Current;
	Future.SchemaVersion = SnapshotSchemaVersion + 1;
	TestFalse(
		TEXT("Unsupported future schema is rejected"),
		SerializeSnapshot(Future, Bytes));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimPhase6ScaleContractTest,
	"AMSim.Phase6.Scale.MaximumScaleContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimPhase6ScaleContractTest::RunTest(const FString& Parameters)
{
	using namespace AMSim;
	const FPhase6Fixture& Fixture = GetPhase6Fixture();
	FSimulation Simulation(Fixture.Seed);
	if (!BuildLivingAirportForPhase6(*this, Simulation))
	{
		return false;
	}
	FPhase5Command InitializeBreadth;
	InitializeBreadth.Id = {1};
	InitializeBreadth.Type = EPhase5CommandType::InitializeBreadth;
	TestEqual(
		TEXT("Phase 5 initializes for max-scale fixture"),
		Simulation.QueuePhase5Command(InitializeBreadth),
		EPhase5CommandResult::Accepted);
	Simulation.Step();

	FSnapshot Snapshot = Simulation.CreateSnapshot();
	Snapshot.Phase1.Credits += 200000;
	Snapshot.Phase1.AirportPoints = Fixture.MajorAirportPoints;
	FPhase5PathEvidenceRecord* Passenger =
		Snapshot.Phase5.Paths.FindByPredicate(
			[](const FPhase5PathEvidenceRecord& Path)
			{
				return Path.Path == ESpecializationPath::Passenger;
			});
	if (!TestNotNull(TEXT("Passenger path exists"), Passenger))
	{
		return false;
	}
	Passenger->Band = ECapabilityBand::Advanced;
	Passenger->EarnedBand = ECapabilityBand::Advanced;
	Passenger->OperationalBand = ECapabilityBand::Advanced;
	Snapshot.Phase5.AdvancedPathCount = 1;
	const int32 SupportAgentCount =
		Snapshot.Phase2.Teams.Num() +
		Snapshot.Phase2.Vehicles.Num();
	Snapshot.Phase3 = BuildPhase3LogicalScaleFixture(
		Fixture.LogicalAgentTarget - SupportAgentCount);
	if (!TestTrue(
		TEXT("Prepared maximum-scale save restores"),
		Simulation.RestoreSnapshot(Snapshot)))
	{
		return false;
	}

	FPhase6Command Initialize;
	Initialize.Id = {1};
	Initialize.Type = EPhase6CommandType::InitializeMajorCapability;
	const double Started = FPlatformTime::Seconds();
	if (!QueueSimulationPhase6(
		*this,
		Simulation,
		Initialize,
		TEXT("Phase 6 initializes at maximum logical scale")))
	{
		return false;
	}
	const double StepMilliseconds =
		(FPlatformTime::Seconds() - Started) * 1000.0;
	const FPhase6ScaleDiagnostics& Scale =
		Simulation.GetPhase6State().ScaleDiagnostics;
	TestEqual(
		TEXT("Logical-agent fixture reaches 10,000"),
		Scale.LogicalAgentCount,
		Fixture.LogicalAgentTarget);
	TestEqual(
		TEXT("Visible-agent work remains bounded at 2,000"),
		Scale.VisibleAgentCount,
		Fixture.VisibleAgentTarget);
	TestFalse(
		TEXT("Target-scale fixture does not create a backlog warning"),
		Scale.bBacklogWarning);
	TestTrue(
		TEXT("Maximum-scale simulation step remains bounded"),
		StepMilliseconds < 1000.0);

	FPresentationProxyPool Pool;
	TArray<FEntityId> Entities;
	Entities.Reserve(Fixture.VisibleAgentTarget);
	for (int32 Index = 0; Index < Fixture.VisibleAgentTarget; ++Index)
	{
		const FEntityId Entity{static_cast<uint64>(Index + 1)};
		Entities.Add(Entity);
		Pool.Acquire(Entity);
	}
	for (int32 Index = 0; Index < Entities.Num(); Index += 2)
	{
		Pool.Release(Entities[Index]);
	}
	for (int32 Index = 0; Index < Fixture.VisibleAgentTarget / 2; ++Index)
	{
		Pool.Acquire(FEntityId{static_cast<uint64>(3000 + Index)});
	}
	TestEqual(
		TEXT("Released presentation slots are reused at the 2,000 cap"),
		Pool.GetActiveCount(),
		Fixture.VisibleAgentTarget);
	TestEqual(
		TEXT("On-map aircraft target remains 150"),
		Fixture.OnMapAircraftTarget,
		150);
	TestEqual(
		TEXT("Vehicle target remains 500"),
		Fixture.VehicleTarget,
		500);
	return true;
}

#endif
