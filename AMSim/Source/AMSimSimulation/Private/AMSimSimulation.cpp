#include "AMSimSimulation.h"

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
		return {
			SnapshotSchemaVersion,
			MasterSeed,
			NextEntityId,
			NextEventSequence,
			Revision,
			Clock.GetGameTimeMilliseconds(),
			Entities
		};
	}

	bool FSimulation::RestoreSnapshot(const FSnapshot& Snapshot)
	{
		if (Snapshot.SchemaVersion != SnapshotSchemaVersion ||
			Snapshot.MasterSeed == 0 ||
			Snapshot.NextEntityId == 0 ||
			Snapshot.NextEventSequence == 0)
		{
			return false;
		}
		FSimulationClock RestoredClock;
		if (!RestoredClock.Restore(Snapshot.GameTimeMilliseconds, Snapshot.Revision))
		{
			return false;
		}
		TArray<FEntityId> RestoredEntities = Snapshot.Entities;
		RestoredEntities.Sort([](const FEntityId Left, const FEntityId Right) { return Left.Value < Right.Value; });
		uint64 PreviousId = 0;
		for (const FEntityId Entity : RestoredEntities)
		{
			if (!Entity.IsValid() || Entity.Value == PreviousId || Entity.Value >= Snapshot.NextEntityId)
			{
				return false;
			}
			PreviousId = Entity.Value;
		}

		MasterSeed = Snapshot.MasterSeed;
		NextEntityId = Snapshot.NextEntityId;
		NextEventSequence = Snapshot.NextEventSequence;
		Revision = Snapshot.Revision;
		Clock = RestoredClock;
		Entities = MoveTemp(RestoredEntities);
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
