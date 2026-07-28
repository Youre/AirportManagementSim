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
		, Phase3(MasterSeed)
		, Phase4(MasterSeed)
		, Phase5(MasterSeed)
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
			(Phase2.GetState().bInitialized ||
				Phase3.GetState().bInitialized ||
				Phase4.GetState().bInitialized ||
				Phase5.GetState().bInitialized))
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

	EPhase3CommandResult FSimulation::QueuePhase3Command(const FPhase3Command& Command)
	{
		return Phase3.QueueCommand(
			Command,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState());
	}

	EPhase4CommandResult FSimulation::QueuePhase4Command(
		const FPhase4Command& Command)
	{
		return Phase4.QueueCommand(
			Command,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3.GetState());
	}

	EPhase5CommandResult FSimulation::QueuePhase5Command(
		const FPhase5Command& Command)
	{
		return Phase5.QueueCommand(
			Command,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3.GetState(),
			Phase4.GetState());
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
		Phase3.Step(Clock.GetGameTimeMilliseconds(), Phase1, Phase2.GetState());
		Phase4.Step(
			Clock.GetGameTimeMilliseconds(),
			Phase1,
			Phase2.GetState(),
			Phase3.GetState());
		Phase5.Step(
			Clock.GetGameTimeMilliseconds(),
			Phase1,
			Phase2,
			Phase3.GetState(),
			Phase4.GetState());
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

	FPhase3QuerySnapshot FSimulation::CreatePhase3QuerySnapshot() const
	{
		return Phase3.CreateQuerySnapshot(
			Revision,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState());
	}

	FPhase4QuerySnapshot FSimulation::CreatePhase4QuerySnapshot() const
	{
		return Phase4.CreateQuerySnapshot(
			Revision,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3.GetState());
	}

	FPhase5QuerySnapshot FSimulation::CreatePhase5QuerySnapshot() const
	{
		return Phase5.CreateQuerySnapshot(
			Revision,
			Clock.GetGameTimeMilliseconds(),
			Phase1.GetState(),
			Phase2.GetState(),
			Phase3.GetState(),
			Phase4.GetState());
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
		Snapshot.Phase3 = Phase3.GetState();
		Snapshot.Phase4 = Phase4.GetState();
		Snapshot.Phase5 = Phase5.GetState();
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
		FPassengerTerminalSimulation RestoredPhase3(Candidate.MasterSeed);
		if (!RestoredPhase3.RestoreState(
			Candidate.Phase3,
			Candidate.Phase1,
			Candidate.Phase2))
		{
			return false;
		}
		FRegionalScheduledSimulation RestoredPhase4(Candidate.MasterSeed);
		if (!RestoredPhase4.RestoreState(
			Candidate.Phase4,
			Candidate.Phase1,
			Candidate.Phase2,
			Candidate.Phase3))
		{
			return false;
		}
		FPhase5Simulation RestoredPhase5(Candidate.MasterSeed);
		if (!RestoredPhase5.RestoreState(
			Candidate.Phase5,
			Candidate.Phase1,
			Candidate.Phase2,
			Candidate.Phase3,
			Candidate.Phase4))
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
		Phase3 = MoveTemp(RestoredPhase3);
		Phase4 = MoveTemp(RestoredPhase4);
		Phase5 = MoveTemp(RestoredPhase5);
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
		const uint64 Phase3Checksum = Phase3.CalculateChecksum();
		HashBytes(Hash, &Phase3Checksum, sizeof(Phase3Checksum));
		const uint64 Phase4Checksum = Phase4.CalculateChecksum();
		HashBytes(Hash, &Phase4Checksum, sizeof(Phase4Checksum));
		const uint64 Phase5Checksum = Phase5.CalculateChecksum();
		HashBytes(Hash, &Phase5Checksum, sizeof(Phase5Checksum));
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
