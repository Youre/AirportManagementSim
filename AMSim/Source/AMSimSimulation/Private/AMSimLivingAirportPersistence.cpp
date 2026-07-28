#include "AMSimLivingAirportSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 FnvOffset = 1469598103934665603ull;
		constexpr uint64 FnvPrime = 1099511628211ull;
		constexpr int64 Phase2IdBase = 1ll << 32;

		void HashBytes(uint64& Hash, const void* Data, const SIZE_T Size)
		{
			const uint8* Bytes = static_cast<const uint8*>(Data);
			for (SIZE_T Index = 0; Index < Size; ++Index)
			{
				Hash ^= Bytes[Index];
				Hash *= FnvPrime;
			}
		}

		void HashString(uint64& Hash, const FString& Value)
		{
			FTCHARToUTF8 Utf8(*Value);
			HashBytes(Hash, Utf8.Get(), Utf8.Length());
		}

		template <typename RecordType, typename IdType>
		bool ContainsId(const TArray<RecordType>& Records, const IdType Id)
		{
			return Records.ContainsByPredicate(
				[Id](const RecordType& Record) { return Record.Id == Id; });
		}
	}

	uint64 FLivingAirportSimulation::CalculateChecksum() const
	{
		uint64 Hash = FnvOffset;
		HashBytes(Hash, &State.bInitialized, sizeof(State.bInitialized));
		HashBytes(Hash, &State.MasterSeed, sizeof(State.MasterSeed));
		HashBytes(Hash, &State.RandomStreamState, sizeof(State.RandomStreamState));
		HashBytes(Hash, &State.NextDomainId, sizeof(State.NextDomainId));
		HashBytes(Hash, &State.NextEventSequence, sizeof(State.NextEventSequence));
		HashBytes(Hash, &State.InitializedAtGameMilliseconds, sizeof(int64));
		HashBytes(Hash, &State.LastUpdatedGameMilliseconds, sizeof(int64));
		HashBytes(Hash, &State.CurrentOperatingDay, sizeof(int32));
		HashBytes(Hash, &State.LastEconomyOperatingDay, sizeof(int32));
		HashBytes(Hash, &State.SelectedSpecialization, sizeof(State.SelectedSpecialization));
		HashString(Hash, State.ActiveRunwayDirection.ToString());
		HashBytes(Hash, &State.CompletedFlightCount, sizeof(int32));
		HashBytes(Hash, &State.TotalPhase2RevenueCredits, sizeof(int64));
		HashBytes(Hash, &State.TotalPhase2CostCredits, sizeof(int64));
		for (const FPhase2AircraftRecord& Aircraft : State.Aircraft)
		{
			HashBytes(Hash, &Aircraft.Id.Value, sizeof(uint64));
			HashString(Hash, Aircraft.AircraftContentId.ToString());
			HashString(Hash, Aircraft.TailNumber);
			HashBytes(Hash, &Aircraft.VisitCount, sizeof(int32));
			HashBytes(Hash, &Aircraft.ServiceCount, sizeof(int32));
		}
		for (const FPhase2ContractRecord& Contract : State.Contracts)
		{
			HashBytes(Hash, &Contract.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Contract.bAccepted, sizeof(bool));
		}
		for (const FPhase2FlightRecord& Flight : State.Flights)
		{
			HashBytes(Hash, &Flight.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Flight.State, sizeof(Flight.State));
			HashBytes(Hash, &Flight.ScheduledArrivalGameMilliseconds, sizeof(int64));
			HashBytes(Hash, &Flight.bRewardRecognized, sizeof(bool));
		}
		for (const FPhase2ServiceTaskRecord& Task : State.ServiceTasks)
		{
			HashBytes(Hash, &Task.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Task.State, sizeof(Task.State));
			HashString(Hash, Task.OwnerDomain.ToString());
			HashBytes(Hash, &Task.OwnerId, sizeof(uint64));
			HashString(Hash, Task.OperationId.ToString());
			HashBytes(Hash, &Task.Quantity, sizeof(int32));
			HashBytes(Hash, &Task.AssignedVehicleId.Value, sizeof(uint64));
			HashBytes(Hash, &Task.AssignedTeamId.Value, sizeof(uint64));
		}
		for (const FPhase2VehicleRecord& Vehicle : State.Vehicles)
		{
			HashBytes(Hash, &Vehicle.Id.Value, sizeof(uint64));
			HashBytes(Hash, &Vehicle.State, sizeof(Vehicle.State));
			HashBytes(Hash, &Vehicle.CompletedTaskCount, sizeof(int32));
		}
		for (const FPhase2StaffTeamRecord& Team : State.Teams)
		{
			HashBytes(Hash, &Team.Id.Value, sizeof(uint64));
			HashString(Hash, Team.ZoneId.ToString());
			HashBytes(Hash, &Team.WorkloadPercent, sizeof(int32));
			HashBytes(Hash, &Team.MoralePercent, sizeof(int32));
		}
		HashBytes(Hash, &State.Expansion.Stage, sizeof(State.Expansion.Stage));
		HashBytes(Hash, &State.CurrentWeather.Category, sizeof(State.CurrentWeather.Category));
		HashBytes(Hash, &State.Incident.State, sizeof(State.Incident.State));
		for (const FPhase2RatingRecord& Rating : State.Ratings)
		{
			HashString(Hash, Rating.ComponentId.ToString());
			HashBytes(Hash, &Rating.Value, sizeof(int32));
		}
		for (const FPhase2Event& Event : State.Events)
		{
			HashBytes(Hash, &Event.Sequence, sizeof(uint64));
			HashBytes(Hash, &Event.Type, sizeof(Event.Type));
			HashBytes(Hash, &Event.SubjectId, sizeof(uint64));
		}
		return Hash;
	}

	bool FLivingAirportSimulation::RestoreState(
		const FPhase2State& InState,
		const FPhase1State& Phase1State)
	{
		if (InState.MasterSeed == 0 ||
			InState.NextDomainId < Phase2IdBase ||
			InState.NextEventSequence == 0 ||
			InState.Aircraft.Num() > 10000 ||
			InState.Contracts.Num() > 10000 ||
			InState.Flights.Num() > 100000 ||
			InState.ServiceTasks.Num() > 100000 ||
			InState.Events.Num() > 100000 ||
			InState.CurrentOperatingDay < 0 ||
			InState.RecoveryGrantCount < 0 ||
			InState.RecoveryGrantCount > 2)
		{
			return false;
		}
		if (InState.bInitialized &&
			(!Phase1State.bInitialized || !Phase1State.bAirportOpen))
		{
			return false;
		}

		TSet<uint64> SeenIds;
		auto RegisterId = [&SeenIds, &InState](const uint64 Id)
		{
			return Id >= static_cast<uint64>(Phase2IdBase) &&
				Id < InState.NextDomainId &&
				!SeenIds.Contains(Id) &&
				(SeenIds.Add(Id), true);
		};
		for (const FPhase2AircraftRecord& Aircraft : InState.Aircraft)
		{
			if (!RegisterId(Aircraft.Id.Value) ||
				Aircraft.AircraftContentId.IsNone() ||
				Aircraft.RoleId.IsNone() ||
				Aircraft.TailNumber.IsEmpty())
			{
				return false;
			}
		}
		for (const FPhase2TenantRecord& Tenant : InState.Tenants)
		{
			if (!RegisterId(Tenant.Id.Value) || Tenant.TenantContentId.IsNone())
			{
				return false;
			}
		}
		for (const FPhase2ContractRecord& Contract : InState.Contracts)
		{
			if (!RegisterId(Contract.Id.Value) ||
				!ContainsId(InState.Tenants, Contract.TenantId) ||
				Contract.ContractContentId.IsNone())
			{
				return false;
			}
		}
		for (const FPhase2VehicleRecord& Vehicle : InState.Vehicles)
		{
			if (!RegisterId(Vehicle.Id.Value) || Vehicle.CapabilityId.IsNone())
			{
				return false;
			}
		}
		for (const FPhase2StaffTeamRecord& Team : InState.Teams)
		{
			if (!RegisterId(Team.Id.Value) ||
				Team.RoleId.IsNone() ||
				Team.TeamSize <= 0 ||
				Team.WorkloadPercent < 0 ||
				Team.WorkloadPercent > 100 ||
				Team.MoralePercent < 0 ||
				Team.MoralePercent > 100)
			{
				return false;
			}
		}
		for (const FPhase2ParcelRecord& Parcel : InState.Parcels)
		{
			if (!RegisterId(Parcel.Id.Value) ||
				Parcel.ParcelContentId.IsNone() ||
				Parcel.PurchaseCostCredits < 0)
			{
				return false;
			}
		}
		if (InState.Expansion.Id.IsValid() &&
			(!RegisterId(InState.Expansion.Id.Value) ||
				!ContainsId(InState.Parcels, InState.Expansion.ParcelId)))
		{
			return false;
		}
		for (const FPhase2FlightRecord& Flight : InState.Flights)
		{
			if (!RegisterId(Flight.Id.Value) ||
				!ContainsId(InState.Contracts, Flight.ContractId) ||
				!ContainsId(InState.Aircraft, Flight.AircraftId) ||
				Flight.StandStartGameMilliseconds >
					Flight.ScheduledArrivalGameMilliseconds ||
				Flight.StandEndGameMilliseconds <
					Flight.ScheduledArrivalGameMilliseconds)
			{
				return false;
			}
		}
		for (const FPhase2ServiceTaskRecord& Task : InState.ServiceTasks)
		{
			if (!RegisterId(Task.Id.Value) ||
				(Task.OwnerDomain.IsNone()
					? !ContainsId(InState.Flights, Task.FlightId)
					: Task.OwnerDomain != TEXT("Phase5") ||
						Task.OwnerId == 0 ||
						Task.OperationId.IsNone()) ||
				Task.Quantity <= 0 ||
				(Task.AssignedVehicleId.IsValid() &&
					!ContainsId(InState.Vehicles, Task.AssignedVehicleId)) ||
				(Task.AssignedTeamId.IsValid() &&
					!ContainsId(InState.Teams, Task.AssignedTeamId)))
			{
				return false;
			}
		}
		for (const FPhase2ServiceTaskRecord& Task : InState.ServiceTasks)
		{
			if (Task.PrerequisiteTaskId.IsValid() &&
				!ContainsId(InState.ServiceTasks, Task.PrerequisiteTaskId))
			{
				return false;
			}
		}
		for (const FPhase2VehicleRecord& Vehicle : InState.Vehicles)
		{
			if (Vehicle.AssignedTaskId.IsValid() &&
				!ContainsId(InState.ServiceTasks, Vehicle.AssignedTaskId))
			{
				return false;
			}
		}
		for (const FPhase2StaffTeamRecord& Team : InState.Teams)
		{
			if (Team.AssignedTaskId.IsValid() &&
				!ContainsId(InState.ServiceTasks, Team.AssignedTaskId))
			{
				return false;
			}
		}
		if (InState.Incident.Id.IsValid() && !RegisterId(InState.Incident.Id.Value))
		{
			return false;
		}
		for (const FPhase2AchievementRecord& Achievement : InState.Achievements)
		{
			if (!RegisterId(Achievement.Id.Value) ||
				Achievement.AchievementContentId.IsNone())
			{
				return false;
			}
		}
		for (const FPhase2MovementReservation& Reservation : InState.Reservations)
		{
			if (!ContainsId(InState.Flights, Reservation.FlightId) ||
				Reservation.BlockId.IsNone() ||
				Reservation.StartGameMilliseconds >= Reservation.EndGameMilliseconds)
			{
				return false;
			}
		}
		for (int32 LeftIndex = 0; LeftIndex < InState.Reservations.Num(); ++LeftIndex)
		{
			const FPhase2MovementReservation& Left = InState.Reservations[LeftIndex];
			for (int32 RightIndex = LeftIndex + 1;
				RightIndex < InState.Reservations.Num();
				++RightIndex)
			{
				const FPhase2MovementReservation& Right =
					InState.Reservations[RightIndex];
				if (Left.BlockId == Right.BlockId &&
					Left.FlightId != Right.FlightId &&
					Left.StartGameMilliseconds < Right.EndGameMilliseconds &&
					Right.StartGameMilliseconds < Left.EndGameMilliseconds)
				{
					return false;
				}
			}
		}

		State = InState;
		PendingCommands.Reset();
		return true;
	}
}
