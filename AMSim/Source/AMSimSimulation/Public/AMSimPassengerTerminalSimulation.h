#pragma once

#include "AMSimPhase3Fixture.h"

namespace AMSim
{
	class FStarterAirfieldSimulation;

	class AMSIMSIMULATION_API FPassengerTerminalSimulation
	{
	public:
		explicit FPassengerTerminalSimulation(uint64 MasterSeed = 1);

		EPhase3CommandResult QueueCommand(
			const FPhase3Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State);
		void Step(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1,
			const FPhase2State& Phase2State);
		FPhase3QuerySnapshot CreateQuerySnapshot(
			uint64 Revision,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State) const;
		uint64 CalculateChecksum() const;

		const FPhase3State& GetState() const { return State; }
		bool RestoreState(
			const FPhase3State& InState,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State);
		void Reset(uint64 MasterSeed);
		void PrepareLayoutMigration(bool bPassengerInitialized);
		bool ValidateSecurityTopology(
			const FPhase3State& Candidate,
			bool& bHasAccessibleRoute,
			FString& Failure) const;
		bool ValidateReconciliation(
			const FPhase3State& Candidate,
			FString& Failure) const;
		bool ValidateTerminalLayout(
			const FTerminalLayoutState& Candidate,
			FString& Failure) const;

	private:
		FPhase3Validation ValidateCommand(
			const FPhase3Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State,
			const FPhase2State& Phase2State) const;
		void ApplyCommand(
			const FPhase3Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void InitializePassengerAirport(int64 CurrentGameMilliseconds);
		void AdvanceConstruction(int64 CurrentGameMilliseconds);
		void AdvanceTerminalConstruction(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void AdvanceGATerminalVisitors(
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State);
		void AdvancePassengers(int64 CurrentGameMilliseconds);
		void AdvanceBags(int64 CurrentGameMilliseconds);
		void AdvanceFlight(
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void CreatePassengerService(int64 CurrentGameMilliseconds);
		void CreateCohort(
			EPassengerDirection Direction,
			int32 PassengerCount,
			int32 BagCount,
			int64 CurrentGameMilliseconds);
		void RefreshDerivedValidation();
		void RefreshTerminalReadiness();
		void SeedStarterTerminalLayout(bool bCompleted);
		void ExpandPassengerTerminalLayout(bool bCompleted);
		int64 QuoteTerminalEdit(const FPhase3Command& Command) const;
		bool ValidateTerminalEdit(
			const FPhase3Command& Command,
			FPhase3Validation& Result) const;
		void ApplyTerminalEdit(
			const FPhase3Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void SetTerminalStage(
			ETerminalConstructionStage Stage,
			int64 CurrentGameMilliseconds);
		void SetPassengerState(
			FPassengerRecord& Passenger,
			EPassengerJourneyState JourneyState,
			int64 CurrentGameMilliseconds);
		void SetBagState(
			FBagRecord& Bag,
			EBagJourneyState JourneyState,
			int64 CurrentGameMilliseconds);
		void EmitEvent(
			EPhase3EventType Type,
			FCommandId Cause,
			uint64 SubjectId,
			const FString& Message,
			int64 CurrentGameMilliseconds);
		uint64 AllocateDomainId();

		FPhase3State State;
		TArray<FPhase3Command> PendingCommands;
	};
}
