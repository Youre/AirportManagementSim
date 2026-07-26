#pragma once

#include "AMSimDeterminism.h"
#include "AMSimPhase1Fixture.h"

namespace AMSim
{
	class AMSIMSIMULATION_API FStarterAirfieldSimulation
	{
	public:
		explicit FStarterAirfieldSimulation(uint64 MasterSeed = 1);

		EPhase1CommandResult QueueCommand(const FPhase1Command& Command, int64 CurrentGameMilliseconds);
		void Step(int64 CurrentGameMilliseconds);
		FPhase1QuerySnapshot CreateQuerySnapshot(uint64 Revision, int64 CurrentGameMilliseconds) const;
		uint64 CalculateChecksum() const;

		const FPhase1State& GetState() const { return State; }
		const TArray<FPhase1Event>& GetEvents() const { return State.Events; }
		bool RestoreState(const FPhase1State& InState);
		bool ApplyExternalEconomyChange(
			FName Category,
			int64 AmountCredits,
			int32 AirportPoints,
			const FString& Explanation,
			int64 CurrentGameMilliseconds);
		void Reset(uint64 MasterSeed);

	private:
		FPhase1Validation ValidateCommand(
			const FPhase1Command& Command,
			int64 CurrentGameMilliseconds) const;
		void ApplyCommand(const FPhase1Command& Command, int64 CurrentGameMilliseconds);
		void AdvanceConstruction(int64 CurrentGameMilliseconds);
		void AdvanceFlight(int64 CurrentGameMilliseconds);
		void ChangeConstructionStage(
			EConstructionStage NewStage,
			int64 CurrentGameMilliseconds,
			const FString& Message);
		void ChangeFlightState(EFlightState NewState, int64 CurrentGameMilliseconds);
		void QueuePhrase(
			FName PhraseId,
			const FString& Speaker,
			const FString& Caption,
			int32 Priority,
			int64 CurrentGameMilliseconds);
		void RecordTransaction(
			FName Category,
			int64 AmountCredits,
			const FString& Explanation,
			int64 CurrentGameMilliseconds);
		void EmitEvent(
			EPhase1EventType Type,
			FCommandId Cause,
			const FString& Message,
			int64 CurrentGameMilliseconds);
		uint64 AllocateDomainId();
		FFacilityId FindFacility(EFacilityType Type) const;

		FPhase1State State;
		TArray<FPhase1Command> PendingCommands;
	};
}
