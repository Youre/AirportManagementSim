#pragma once

#include "AMSimPhase2Fixture.h"

namespace AMSim
{
	class FStarterAirfieldSimulation;

	class AMSIMSIMULATION_API FLivingAirportSimulation
	{
	public:
		explicit FLivingAirportSimulation(uint64 MasterSeed = 1);

		EPhase2CommandResult QueueCommand(
			const FPhase2Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State);
		void Step(int64 CurrentGameMilliseconds, FStarterAirfieldSimulation& Phase1);
		FPhase2QuerySnapshot CreateQuerySnapshot(
			uint64 Revision,
			int64 CurrentGameMilliseconds) const;
		uint64 CalculateChecksum() const;

		const FPhase2State& GetState() const { return State; }
		bool RestoreState(const FPhase2State& InState, const FPhase1State& Phase1State);
		void Reset(uint64 MasterSeed);
		void EnsureCargoServiceResources(int64 CurrentGameMilliseconds);
		FServiceTaskId RegisterExternalServiceTask(
			FName OwnerDomain,
			uint64 OwnerId,
			FName OperationId,
			int32 Quantity,
			int64 DurationMilliseconds,
			int64 CurrentGameMilliseconds);
		bool IsServiceTaskComplete(FServiceTaskId TaskId) const;

	private:
		FPhase2Validation ValidateCommand(
			const FPhase2Command& Command,
			int64 CurrentGameMilliseconds,
			const FPhase1State& Phase1State) const;
		void ApplyCommand(
			const FPhase2Command& Command,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void InitializeLivingAirport(int64 CurrentGameMilliseconds);
		void AdvanceOperatingDay(int64 CurrentGameMilliseconds, FStarterAirfieldSimulation& Phase1);
		void AdvanceFlights(int64 CurrentGameMilliseconds, FStarterAirfieldSimulation& Phase1);
		void AdvanceServices(int64 CurrentGameMilliseconds);
		void AdvanceExpansion(int64 CurrentGameMilliseconds);
		void AdvanceIncident(int64 CurrentGameMilliseconds, FStarterAirfieldSimulation& Phase1);
		void RefreshRatings(int64 CurrentGameMilliseconds);
		void CreateFlightsForDay(int32 OperatingDay, int64 CurrentGameMilliseconds);
		void CreateTurnaroundTasks(FPhase2FlightRecord& Flight, int64 CurrentGameMilliseconds);
		bool DispatchTask(FPhase2ServiceTaskRecord& Task, int64 CurrentGameMilliseconds);
		void CompleteFlight(
			FPhase2FlightRecord& Flight,
			int64 CurrentGameMilliseconds,
			FStarterAirfieldSimulation& Phase1);
		void SetFlightState(
			FPhase2FlightRecord& Flight,
			EPhase2FlightState NewState,
			int64 CurrentGameMilliseconds);
		void SetIncidentState(EIncidentState NewState, int64 CurrentGameMilliseconds);
		void EmitEvent(
			EPhase2EventType Type,
			FCommandId Cause,
			uint64 SubjectId,
			const FString& Message,
			int64 CurrentGameMilliseconds);
		uint64 AllocateDomainId();

		FPhase2State State;
		TArray<FPhase2Command> PendingCommands;
	};
}
