#pragma once

#include "AMSimPhase6Fixture.h"

namespace AMSim
{
	class FStarterAirfieldSimulation;
	class FLivingAirportSimulation;

	class AMSIMSIMULATION_API FPhase6Simulation
	{
	public:
		explicit FPhase6Simulation(uint64 MasterSeed = 1);

		EPhase6CommandResult QueueCommand(
			const FPhase6Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State);
		void Step(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			FLivingAirportSimulation& Phase2,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State);
		FPhase6QuerySnapshot CreateQuerySnapshot(
			uint64 Revision,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase5State& Phase5State) const;
		FPhase6CapabilitySignals CreateCapabilitySignals(
			const FPhase1State& Phase1State,
			const FPhase5State& Phase5State) const;
		uint64 CalculateChecksum() const;
		bool RestoreState(
			const FPhase6State& Candidate,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State);

		const FPhase6State& GetState() const { return State; }

	private:
		FPhase6Validation ValidateCommand(
			const FPhase6Command& Command,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State) const;
		void ApplyCommand(
			const FPhase6Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			FLivingAirportSimulation& Phase2,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State);
		void Initialize(
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase5State& Phase5State);
		void CompleteConstruction(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			FLivingAirportSimulation& Phase2);
		void RefreshLargeAircraft(
			int64 CurrentGameMilliseconds,
			FLivingAirportSimulation& Phase2);
		void RefreshPathMetrics(
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State,
			const FPhase5State& Phase5State);
		void RefreshScaleDiagnostics(
			const FPhase2State& Phase2State,
			const FPhase3State& Phase3State,
			const FPhase4State& Phase4State);
		bool IsLargeAircraftCompatible(FString& OutCause) const;
		void CreateLargeAircraftTasks(
			FLargeAircraftOperationRecord& Operation,
			int64 CurrentGameMilliseconds,
			FLivingAirportSimulation& Phase2);
		void RecordMajorOperation(
			const FPhase6Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void Emit(
			EPhase6EventType Type,
			const FCommandId& Cause,
			uint64 SubjectId,
			int64 CurrentGameMilliseconds,
			const FString& Message);
		uint64 AllocateId();

		FPhase6State State;
		TArray<FPhase6Command> PendingCommands;
	};
}
