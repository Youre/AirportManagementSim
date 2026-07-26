#pragma once

#include "AMSimDeterminism.h"
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
	};

	class AMSIMSIMULATION_API FSimulation
	{
	public:
		explicit FSimulation(uint64 MasterSeed = 1);

		ECommandResult QueueCommand(const FCommand& Command);
		void Step();
		FQuerySnapshot CreateQuerySnapshot() const;
		FPhase1QuerySnapshot CreatePhase1QuerySnapshot() const;
		FSimulationDiagnostics CreateDiagnostics() const;
		FSnapshot CreateSnapshot() const;
		bool RestoreSnapshot(const FSnapshot& Snapshot);
		EPhase1CommandResult QueuePhase1Command(const FPhase1Command& Command);

		const TArray<FEvent>& GetEvents() const { return Events; }
		const FPhase1State& GetPhase1State() const { return Phase1.GetState(); }
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
	};

	struct FReplayResult
	{
		uint64 Checksum = 0;
		TArray<FEvent> Events;
	};

	AMSIMSIMULATION_API FReplayResult Replay(uint64 MasterSeed, const TArray<FCommand>& Commands, int32 StepCount);
}
