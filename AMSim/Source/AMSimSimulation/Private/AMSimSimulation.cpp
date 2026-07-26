#include "AMSimSimulation.h"
#include "AMSimSnapshotSerialization.h"

namespace AMSim
{
	namespace
	{
		void HashBytes(uint64& Hash, const void* Data, const SIZE_T Size)
		{
			const uint8* Bytes = static_cast<const uint8*>(Data);
			for (SIZE_T Index = 0; Index < Size; ++Index)
			{
				Hash ^= Bytes[Index];
				Hash *= 1099511628211ull;
			}
		}
	}

	FSimulation::FSimulation(const uint64 InMasterSeed)
		: MasterSeed(InMasterSeed == 0 ? 1 : InMasterSeed)
		, Phase1(MasterSeed)
		, Phase2(MasterSeed)
	{
	}

	ECommandResult FSimulation::QueueCommand(const FCommand& Command)
	{
		if (!Command.Id.IsValid())
		{
			return ECommandResult::RejectedInvalidId;
		}
		if (Command.Type == ECommandType::DestroyEntity && !Entities.Contains(Command.Target))
		{
			return ECommandResult::RejectedEntityMissing;
		}
		PendingCommands.Add(Command);
		return ECommandResult::Accepted;
	}

	EPhase1CommandResult FSimulation::QueuePhase1Command(const FPhase1Command& Command)
	{
		if (Command.Type == EPhase1CommandType::CloseAirport &&
			Phase2.GetState().bInitialized)
		{
			return EPhase1CommandResult::RejectedInvalidState;
		}
		return Phase1.QueueCommand(Command, Clock.GetGameTimeMilliseconds());
	}

	EPhase2CommandResult FSimulation::QueuePhase2Command(const FPhase2Command& Command)
	{
		return Phase2.QueueCommand(
			Command,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState());
	}

	void FSimulation::Step()
	{
		PendingCommands.Sort([](const FCommand& Left, const FCommand& Right)
		{
			return Left.Id.Value < Right.Id.Value;
		});
		for (const FCommand& Command : PendingCommands)
		{
			ApplyCommand(Command);
		}
		PendingCommands.Reset();
		Clock.AdvanceSteps(1);
		Phase1.Step(Clock.GetGameTimeMilliseconds());
		Phase2.Step(Clock.GetGameTimeMilliseconds(), Phase1);
		++Revision;
	}

	void FSimulation::ApplyCommand(const FCommand& Command)
	{
		if (Command.Type == ECommandType::CreateEntity)
		{
			const FEntityId NewEntity{NextEntityId++};
			Entities.Add(NewEntity);
			EmitEvent(Command, NewEntity);
		}
		else if (Command.Type == ECommandType::DestroyEntity)
		{
			Entities.Remove(Command.Target);
			EmitEvent(Command, Command.Target);
		}
		else
		{
			EmitEvent(Command, {});
		}
	}

	void FSimulation::EmitEvent(const FCommand& Command, const FEntityId Entity)
	{
		Events.Add({
			NextEventSequence++,
			Clock.GetGameTimeMilliseconds(),
			Command.Type,
			Command.Id,
			Entity
		});
	}

	FQuerySnapshot FSimulation::CreateQuerySnapshot() const
	{
		return {Revision, Clock.GetGameTimeMilliseconds(), Entities, CalculateChecksum()};
	}

	FPhase1QuerySnapshot FSimulation::CreatePhase1QuerySnapshot() const
	{
		return Phase1.CreateQuerySnapshot(Revision, Clock.GetGameTimeMilliseconds());
	}

	FPhase2QuerySnapshot FSimulation::CreatePhase2QuerySnapshot() const
	{
		return Phase2.CreateQuerySnapshot(Revision, Clock.GetGameTimeMilliseconds());
	}

	FSimulationDiagnostics FSimulation::CreateDiagnostics() const
	{
		return {
			Clock.GetStepIndex(),
			Clock.GetGameTimeMilliseconds(),
			Entities.Num(),
			PendingCommands.Num(),
			Events.Num(),
			CalculateChecksum()
		};
	}

	FSnapshot FSimulation::CreateSnapshot() const
	{
		FSnapshot Snapshot;
		Snapshot.SchemaVersion = SnapshotSchemaVersion;
		Snapshot.MasterSeed = MasterSeed;
		Snapshot.NextEntityId = NextEntityId;
		Snapshot.NextEventSequence = NextEventSequence;
		Snapshot.Revision = Revision;
		Snapshot.GameTimeMilliseconds = Clock.GetGameTimeMilliseconds();
		Snapshot.Entities = Entities;
		Snapshot.Phase1 = Phase1.GetState();
		Snapshot.Phase2 = Phase2.GetState();
		return Snapshot;
	}

	bool FSimulation::RestoreSnapshot(const FSnapshot& Snapshot)
	{
		FSnapshot Candidate = Snapshot;
		if (!MigrateSnapshotToCurrent(Candidate) ||
			Candidate.MasterSeed == 0 ||
			Candidate.NextEntityId == 0 ||
			Candidate.NextEventSequence == 0)
		{
			return false;
		}
		FSimulationClock RestoredClock;
		if (!RestoredClock.Restore(Candidate.GameTimeMilliseconds, Candidate.Revision))
		{
			return false;
		}
		TArray<FEntityId> RestoredEntities = Candidate.Entities;
		RestoredEntities.Sort([](const FEntityId Left, const FEntityId Right) { return Left.Value < Right.Value; });
		uint64 PreviousId = 0;
		for (const FEntityId Entity : RestoredEntities)
		{
			if (!Entity.IsValid() || Entity.Value == PreviousId || Entity.Value >= Candidate.NextEntityId)
			{
				return false;
			}
			PreviousId = Entity.Value;
		}

		FStarterAirfieldSimulation RestoredPhase1(Candidate.MasterSeed);
		if (!RestoredPhase1.RestoreState(Candidate.Phase1))
		{
			return false;
		}
		FLivingAirportSimulation RestoredPhase2(Candidate.MasterSeed);
		if (!RestoredPhase2.RestoreState(Candidate.Phase2, Candidate.Phase1))
		{
			return false;
		}

		MasterSeed = Candidate.MasterSeed;
		NextEntityId = Candidate.NextEntityId;
		NextEventSequence = Candidate.NextEventSequence;
		Revision = Candidate.Revision;
		Clock = RestoredClock;
		Entities = MoveTemp(RestoredEntities);
		Phase1 = MoveTemp(RestoredPhase1);
		Phase2 = MoveTemp(RestoredPhase2);
		PendingCommands.Reset();
		Events.Reset();
		return true;
	}

	uint64 FSimulation::CalculateChecksum() const
	{
		uint64 Hash = 1469598103934665603ull;
		const int64 Time = Clock.GetGameTimeMilliseconds();
		HashBytes(Hash, &MasterSeed, sizeof(MasterSeed));
		HashBytes(Hash, &NextEntityId, sizeof(NextEntityId));
		HashBytes(Hash, &Revision, sizeof(Revision));
		HashBytes(Hash, &Time, sizeof(Time));
		for (const FEntityId Entity : Entities)
		{
			HashBytes(Hash, &Entity.Value, sizeof(Entity.Value));
		}
		const uint64 Phase1Checksum = Phase1.CalculateChecksum();
		HashBytes(Hash, &Phase1Checksum, sizeof(Phase1Checksum));
		const uint64 Phase2Checksum = Phase2.CalculateChecksum();
		HashBytes(Hash, &Phase2Checksum, sizeof(Phase2Checksum));
		return Hash;
	}

	FReplayResult Replay(const uint64 MasterSeed, const TArray<FCommand>& Commands, const int32 StepCount)
	{
		FSimulation Simulation(MasterSeed);
		for (const FCommand& Command : Commands)
		{
			Simulation.QueueCommand(Command);
		}
		for (int32 Step = 0; Step < StepCount; ++Step)
		{
			Simulation.Step();
		}
		return {Simulation.CalculateChecksum(), Simulation.GetEvents()};
	}
}
