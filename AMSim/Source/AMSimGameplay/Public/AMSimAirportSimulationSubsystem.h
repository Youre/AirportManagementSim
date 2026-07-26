#pragma once

#include "AMSimSimulation.h"
#include "Subsystems/WorldSubsystem.h"
#include "AMSimAirportSimulationSubsystem.generated.h"

UCLASS()
class AMSIMGAMEPLAY_API UAMSimAirportSimulationSubsystem final : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	AMSim::FSimulation& GetSimulation();
	const AMSim::FSimulation& GetSimulation() const;
	AMSim::EPhase1CommandResult SubmitPhase1Command(AMSim::FPhase1Command Command);
	AMSim::EPhase2CommandResult SubmitPhase2Command(AMSim::FPhase2Command Command);
	AMSim::EPhase3CommandResult SubmitPhase3Command(AMSim::FPhase3Command Command);
	AMSim::FPhase1QuerySnapshot GetPhase1Query() const;
	AMSim::FPhase2QuerySnapshot GetPhase2Query() const;
	AMSim::FPhase3QuerySnapshot GetPhase3Query() const;
	AMSim::FSnapshot CreateSnapshot() const;
	bool RestoreSnapshot(const AMSim::FSnapshot& Snapshot);
	int64 GetRecommendedStarterArrivalTime() const;
	int64 GetLastSimulationMilliseconds() const { return LastSimulationMilliseconds; }
	int32 GetBacklogSteps() const { return BacklogSteps; }
	double GetLastSimulationWorkMilliseconds() const { return LastSimulationWorkMilliseconds; }

private:
	TUniquePtr<AMSim::FSimulation> Simulation;
	uint64 NextPhase1CommandId = 1;
	uint64 NextPhase2CommandId = 1;
	uint64 NextPhase3CommandId = 1;
	double AccumulatedGameMilliseconds = 0.0;
	double LastSimulationWorkMilliseconds = 0.0;
	int64 LastSimulationMilliseconds = 0;
	int32 BacklogSteps = 0;
};
