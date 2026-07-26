#pragma once

#include "AMSimPhase4Fixture.h"

namespace AMSim
{
	class FStarterAirfieldSimulation;

	class AMSIMSIMULATION_API FRegionalScheduledSimulation
	{
	public:
		explicit FRegionalScheduledSimulation(uint64 MasterSeed = 1);

		EPhase4CommandResult QueueCommand(
			const FPhase4Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State);
		void Step(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State);
		FPhase4QuerySnapshot CreateQuerySnapshot(
			uint64 Revision,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State) const;
		uint64 CalculateChecksum() const;
		bool RestoreState(
			const FPhase4State& Candidate,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State);

		const FPhase4State& GetState() const { return State; }

	private:
		FPhase4Validation ValidateCommand(
			const FPhase4Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State) const;
		void ApplyCommand(
			const FPhase4Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void InitializeFixture(int64 CurrentGameMilliseconds);
		void AdvanceOperatingDay(
			int32 DayIndex,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void UpdateIncident(int64 CurrentGameMilliseconds);
		bool ValidateState(
			const FPhase4State& Candidate,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State) const;
		uint64 AllocateId();
		void Emit(
			EPhase4EventType Type,
			const FCommandId& Cause,
			uint64 SubjectId,
			int64 CurrentGameMilliseconds,
			const FString& Message);

		FPhase4State State;
		TArray<FPhase4Command> PendingCommands;
	};
}
