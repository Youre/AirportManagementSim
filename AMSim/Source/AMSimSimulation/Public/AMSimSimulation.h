#pragma once

#include "AMSimDeterminism.h"
#include "AMSimLivingAirportSimulation.h"
#include "AMSimPassengerTerminalSimulation.h"
#include "AMSimRegionalScheduledSimulation.h"
#include "AMSimPhase5Simulation.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	struct FSnapshot
	{
		uint32 SchemaVersion = SnapshotSchemaVersion;
		uint64 MasterSeed = 1;
		uint64 NextEntityId = 1;
		uint64 NextEventSequence = 1;
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		TArray<FEntityId> Entities;
		FPhase1State Phase1;
		FPhase2State Phase2;
		FPhase3State Phase3;
		FPhase4State Phase4;
		FPhase5State Phase5;
	};

	class AMSIMSIMULATION_API FSimulation
	{
	public:
		explicit FSimulation(uint64 MasterSeed = 1);

		ECommandResult QueueCommand(const FCommand& Command);
		void Step();
		FQuerySnapshot CreateQuerySnapshot() const;
		FPhase1QuerySnapshot CreatePhase1QuerySnapshot() const;
		FPhase2QuerySnapshot CreatePhase2QuerySnapshot() const;
		FPhase3QuerySnapshot CreatePhase3QuerySnapshot() const;
		FPhase4QuerySnapshot CreatePhase4QuerySnapshot() const;
		FPhase5QuerySnapshot CreatePhase5QuerySnapshot() const;
		FSimulationDiagnostics CreateDiagnostics() const;
		FSnapshot CreateSnapshot() const;
		bool RestoreSnapshot(const FSnapshot& Snapshot);
		EPhase1CommandResult QueuePhase1Command(const FPhase1Command& Command);
		EPhase2CommandResult QueuePhase2Command(const FPhase2Command& Command);
		EPhase3CommandResult QueuePhase3Command(const FPhase3Command& Command);
		EPhase4CommandResult QueuePhase4Command(const FPhase4Command& Command);
		EPhase5CommandResult QueuePhase5Command(const FPhase5Command& Command);

		const TArray<FEvent>& GetEvents() const { return Events; }
		const FPhase1State& GetPhase1State() const { return Phase1.GetState(); }
		const FPhase2State& GetPhase2State() const { return Phase2.GetState(); }
		const FPhase3State& GetPhase3State() const { return Phase3.GetState(); }
		const FPhase4State& GetPhase4State() const { return Phase4.GetState(); }
		const FPhase5State& GetPhase5State() const { return Phase5.GetState(); }
		uint64 CalculateChecksum() const;

	private:
		void ApplyCommand(const FCommand& Command);
		void EmitEvent(const FCommand& Command, FEntityId Entity);

		uint64 MasterSeed;
		uint64 NextEntityId = 1;
		uint64 NextEventSequence = 1;
		uint64 Revision = 0;
		FSimulationClock Clock;
		TArray<FEntityId> Entities;
		TArray<FCommand> PendingCommands;
		TArray<FEvent> Events;
		FStarterAirfieldSimulation Phase1;
		FLivingAirportSimulation Phase2;
		FPassengerTerminalSimulation Phase3;
		FRegionalScheduledSimulation Phase4;
		FPhase5Simulation Phase5;
	};

	struct FReplayResult
	{
		uint64 Checksum = 0;
		TArray<FEvent> Events;
	};

	AMSIMSIMULATION_API FReplayResult Replay(uint64 MasterSeed, const TArray<FCommand>& Commands, int32 StepCount);
}
