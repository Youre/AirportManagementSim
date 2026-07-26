#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "Engine/World.h"

void UAMSimAirportSimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Simulation = MakeUnique<AMSim::FSimulation>(AMSim::GetPhase1Fixture().Seed);
	NextPhase1CommandId = 1;
	AccumulatedGameMilliseconds = 0.0;
	LastSimulationMilliseconds = 0;
	BacklogSteps = 0;
}

void UAMSimAirportSimulationSubsystem::Deinitialize()
{
	Simulation.Reset();
	AccumulatedGameMilliseconds = 0.0;
	LastSimulationMilliseconds = 0;
	BacklogSteps = 0;
	Super::Deinitialize();
}

void UAMSimAirportSimulationSubsystem::Tick(const float DeltaTime)
{
	if (!Simulation || DeltaTime <= 0.0f)
	{
		return;
	}

	const AMSim::FPhase1State& State = Simulation->GetPhase1State();
	if (State.bPaused || State.SpeedMultiplier <= 0)
	{
		BacklogSteps = 0;
		return;
	}

	constexpr double GameMillisecondsPerRealSecondAtOneX = 60000.0;
	AccumulatedGameMilliseconds +=
		static_cast<double>(DeltaTime) *
		GameMillisecondsPerRealSecondAtOneX *
		static_cast<double>(State.SpeedMultiplier);

	const int64 AvailableSteps = FMath::FloorToInt64(
		AccumulatedGameMilliseconds / static_cast<double>(AMSim::FixedStepMilliseconds));
	constexpr int64 MaximumStepsPerFrame = 4096;
	const int64 StepsToRun = FMath::Min(AvailableSteps, MaximumStepsPerFrame);
	for (int64 StepIndex = 0; StepIndex < StepsToRun; ++StepIndex)
	{
		Simulation->Step();
	}
	AccumulatedGameMilliseconds -=
		static_cast<double>(StepsToRun * AMSim::FixedStepMilliseconds);
	BacklogSteps = static_cast<int32>(FMath::Min<int64>(
		AvailableSteps - StepsToRun,
		TNumericLimits<int32>::Max()));
	LastSimulationMilliseconds = Simulation->CreateDiagnostics().GameTimeMilliseconds;
}

bool UAMSimAirportSimulationSubsystem::IsTickable() const
{
	return Simulation.IsValid() && GetWorld() != nullptr && !GetWorld()->IsPreviewWorld();
}

TStatId UAMSimAirportSimulationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAMSimAirportSimulationSubsystem, STATGROUP_Tickables);
}

AMSim::FSimulation& UAMSimAirportSimulationSubsystem::GetSimulation()
{
	check(Simulation);
	return *Simulation;
}

const AMSim::FSimulation& UAMSimAirportSimulationSubsystem::GetSimulation() const
{
	check(Simulation);
	return *Simulation;
}

AMSim::EPhase1CommandResult UAMSimAirportSimulationSubsystem::SubmitPhase1Command(
	AMSim::FPhase1Command Command)
{
	check(Simulation);
	if (!Command.Id.IsValid())
	{
		Command.Id = AMSim::FCommandId{NextPhase1CommandId++};
	}
	else
	{
		NextPhase1CommandId = FMath::Max(NextPhase1CommandId, Command.Id.Value + 1);
	}

	const AMSim::EPhase1CommandResult Result = Simulation->QueuePhase1Command(Command);
	if (Result == AMSim::EPhase1CommandResult::Accepted)
	{
		Simulation->Step();
		LastSimulationMilliseconds = Simulation->CreateDiagnostics().GameTimeMilliseconds;
	}
	return Result;
}

AMSim::FPhase1QuerySnapshot UAMSimAirportSimulationSubsystem::GetPhase1Query() const
{
	check(Simulation);
	return Simulation->CreatePhase1QuerySnapshot();
}

AMSim::FSnapshot UAMSimAirportSimulationSubsystem::CreateSnapshot() const
{
	check(Simulation);
	return Simulation->CreateSnapshot();
}

bool UAMSimAirportSimulationSubsystem::RestoreSnapshot(const AMSim::FSnapshot& Snapshot)
{
	check(Simulation);
	if (!Simulation->RestoreSnapshot(Snapshot))
	{
		return false;
	}
	AccumulatedGameMilliseconds = 0.0;
	LastSimulationMilliseconds = Simulation->CreateDiagnostics().GameTimeMilliseconds;
	const uint64 MaximumCommandId = Snapshot.Phase1.Events.IsEmpty()
		? 0
		: Snapshot.Phase1.Events.Last().Cause.Value;
	NextPhase1CommandId = FMath::Max(NextPhase1CommandId, MaximumCommandId + 1);
	return true;
}

int64 UAMSimAirportSimulationSubsystem::GetRecommendedStarterArrivalTime() const
{
	check(Simulation);
	const int64 CurrentGameMilliseconds =
		Simulation->CreateDiagnostics().GameTimeMilliseconds;
	const int64 Increment = AMSim::GetPhase1Fixture().TimetableIncrementMilliseconds;
	return ((CurrentGameMilliseconds / Increment) + 1) * Increment;
}
