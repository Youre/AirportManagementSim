#pragma once

#include "AMSimPhase5Fixture.h"

namespace AMSim
{
	class FStarterAirfieldSimulation;
	class FLivingAirportSimulation;

	class AMSIMSIMULATION_API FPhase5Simulation
	{
	public:
		explicit FPhase5Simulation(uint64 MasterSeed = 1);

		EPhase5CommandResult QueueCommand(
			const FPhase5Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		void Step(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			FLivingAirportSimulation& Phase2,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		FPhase5QuerySnapshot CreateQuerySnapshot(
			uint64 Revision,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State) const;
		uint64 CalculateChecksum() const;
		bool RestoreState(
			const FPhase5State& Candidate,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);

		const FPhase5State& GetState() const { return State; }

	private:
		FPhase5Validation ValidateCommand(
			const FPhase5Command& Command,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State) const;
		void ApplyCommand(
			const FPhase5Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			FLivingAirportSimulation& Phase2,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		void Initialize(
			int64 CurrentGameMilliseconds,
			FLivingAirportSimulation& Phase2,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		void RefreshCargo(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			const FPhase2State& Phase2State);
		void RefreshTenants(int64 CurrentGameMilliseconds);
		void RefreshEvents(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void RefreshProgression(
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		void AddRatingContribution(
			FName ComponentId,
			FName SourceGroup,
			int32 Magnitude,
			int64 CurrentGameMilliseconds,
			const FString& Explanation);
		void Emit(
			EPhase5EventType Type,
			const FCommandId& Cause,
			uint64 SubjectId,
			int64 CurrentGameMilliseconds,
			const FString& Message);
		uint64 AllocateId();

		FPhase5State State;
		TArray<FPhase5Command> PendingCommands;
	};
}
