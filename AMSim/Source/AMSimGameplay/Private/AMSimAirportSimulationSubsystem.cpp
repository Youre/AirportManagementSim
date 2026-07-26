#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"

void UAMSimAirportSimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Simulation = MakeUnique<AMSim::FSimulation>(AMSim::GetPhase1Fixture().Seed);
	NextPhase1CommandId = 1;
	NextPhase2CommandId = 1;
	NextPhase3CommandId = 1;
	NextPhase4CommandId = 1;
	AccumulatedGameMilliseconds = 0.0;
	LastSimulationWorkMilliseconds = 0.0;
	LastSimulationMilliseconds = 0;
	BacklogSteps = 0;
}

void UAMSimAirportSimulationSubsystem::Deinitialize()
{
	Simulation.Reset();
	NextPhase1CommandId = 1;
	NextPhase2CommandId = 1;
	NextPhase3CommandId = 1;
	NextPhase4CommandId = 1;
	AccumulatedGameMilliseconds = 0.0;
	LastSimulationWorkMilliseconds = 0.0;
	LastSimulationMilliseconds = 0;
	BacklogSteps = 0;
	Super::Deinitialize();
}

void UAMSimAirportSimulationSubsystem::Tick(const float DeltaTime)
{
	if (!Simulation || DeltaTime <= 0.0f)
	{
		LastSimulationWorkMilliseconds = 0.0;
		return;
	}

	const AMSim::FPhase1State& State = Simulation->GetPhase1State();
	if (State.bPaused || State.SpeedMultiplier <= 0)
	{
		LastSimulationWorkMilliseconds = 0.0;
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
	const double SimulationStartSeconds = FPlatformTime::Seconds();
	for (int64 StepIndex = 0; StepIndex < StepsToRun; ++StepIndex)
	{
		Simulation->Step();
	}
	LastSimulationWorkMilliseconds =
		(FPlatformTime::Seconds() - SimulationStartSeconds) * 1000.0;
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

AMSim::EPhase2CommandResult UAMSimAirportSimulationSubsystem::SubmitPhase2Command(
	AMSim::FPhase2Command Command)
{
	check(Simulation);
	if (!Command.Id.IsValid())
	{
		Command.Id = AMSim::FCommandId{NextPhase2CommandId++};
	}
	else
	{
		NextPhase2CommandId = FMath::Max(NextPhase2CommandId, Command.Id.Value + 1);
	}

	const AMSim::EPhase2CommandResult Result = Simulation->QueuePhase2Command(Command);
	if (Result == AMSim::EPhase2CommandResult::Accepted)
	{
		Simulation->Step();
		LastSimulationMilliseconds = Simulation->CreateDiagnostics().GameTimeMilliseconds;
	}
	return Result;
}

AMSim::EPhase3CommandResult UAMSimAirportSimulationSubsystem::SubmitPhase3Command(
	AMSim::FPhase3Command Command)
{
	if (!Simulation)
	{
		return AMSim::EPhase3CommandResult::RejectedInvalidState;
	}
	if (!Command.Id.IsValid())
	{
		Command.Id = AMSim::FCommandId{NextPhase3CommandId++};
	}
	else
	{
		NextPhase3CommandId = FMath::Max(
			NextPhase3CommandId,
			Command.Id.Value + 1);
	}
	const AMSim::EPhase3CommandResult Result =
		Simulation->QueuePhase3Command(Command);
	if (Result == AMSim::EPhase3CommandResult::Accepted)
	{
		Simulation->Step();
		LastSimulationMilliseconds =
			Simulation->CreateDiagnostics().GameTimeMilliseconds;
	}
	return Result;
}

AMSim::FPhase2QuerySnapshot UAMSimAirportSimulationSubsystem::GetPhase2Query() const
{
	check(Simulation);
	return Simulation->CreatePhase2QuerySnapshot();
}

AMSim::FPhase3QuerySnapshot UAMSimAirportSimulationSubsystem::GetPhase3Query() const
{
	return GetSimulation().CreatePhase3QuerySnapshot();
}

AMSim::EPhase4CommandResult UAMSimAirportSimulationSubsystem::SubmitPhase4Command(
	AMSim::FPhase4Command Command)
{
	if (!Simulation)
	{
		return AMSim::EPhase4CommandResult::RejectedInvalidState;
	}
	if (!Command.Id.IsValid())
	{
		Command.Id = AMSim::FCommandId{NextPhase4CommandId++};
	}
	else
	{
		NextPhase4CommandId = FMath::Max(
			NextPhase4CommandId,
			Command.Id.Value + 1);
	}
	const AMSim::EPhase4CommandResult Result =
		Simulation->QueuePhase4Command(Command);
	if (Result == AMSim::EPhase4CommandResult::Accepted)
	{
		Simulation->Step();
		LastSimulationMilliseconds =
			Simulation->CreateDiagnostics().GameTimeMilliseconds;
	}
	return Result;
}

AMSim::FPhase4QuerySnapshot UAMSimAirportSimulationSubsystem::GetPhase4Query() const
{
	return GetSimulation().CreatePhase4QuerySnapshot();
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
	const uint64 MaximumPhase2CommandId = Snapshot.Phase2.Events.IsEmpty()
		? 0
		: Snapshot.Phase2.Events.Last().Cause.Value;
	NextPhase2CommandId = FMath::Max(
		NextPhase2CommandId,
		MaximumPhase2CommandId + 1);
	const uint64 MaximumPhase3CommandId = Snapshot.Phase3.Events.IsEmpty()
		? 0
		: Snapshot.Phase3.Events.Last().Cause.Value;
	NextPhase3CommandId = FMath::Max(
		NextPhase3CommandId,
		MaximumPhase3CommandId + 1);
	const uint64 MaximumPhase4CommandId = Snapshot.Phase4.Events.IsEmpty()
		? 0
		: Snapshot.Phase4.Events.Last().Cause.Value;
	NextPhase4CommandId = FMath::Max(
		NextPhase4CommandId,
		MaximumPhase4CommandId + 1);
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
