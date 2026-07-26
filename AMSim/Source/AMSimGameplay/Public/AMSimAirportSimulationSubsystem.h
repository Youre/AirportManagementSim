#pragma once

#include "AMSimSimulation.h"
#include "Subsystems/WorldSubsystem.h"
#include "AMSimAirportSimulationSubsystem.generated.h"

UCLASS()
class AMSIMGAMEPLAY_API UAMSimAirportSimulationSubsystem final : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	AMSim::FSimulation& GetSimulation();
	const AMSim::FSimulation& GetSimulation() const;

private:
	TUniquePtr<AMSim::FSimulation> Simulation;
};
