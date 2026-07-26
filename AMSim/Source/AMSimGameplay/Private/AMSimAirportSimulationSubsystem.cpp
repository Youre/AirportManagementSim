#include "AMSimAirportSimulationSubsystem.h"

void UAMSimAirportSimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Simulation = MakeUnique<AMSim::FSimulation>(1);
}

void UAMSimAirportSimulationSubsystem::Deinitialize()
{
	Simulation.Reset();
	Super::Deinitialize();
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
