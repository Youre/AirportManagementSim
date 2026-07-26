#include "AMSimSnapshotSerialization.h"
#include "Misc/Crc.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

namespace AMSim
{
	namespace
	{
		constexpr uint32 SnapshotMagic = 0x414D5331;
		constexpr int32 MaximumSmallRecords = 10000;
		constexpr int32 MaximumHistoryRecords = 100000;

		template <typename EnumType>
		void SerializeEnum(FArchive& Archive, EnumType& Value)
		{
			uint8 Raw = static_cast<uint8>(Value);
			Archive << Raw;
			if (Archive.IsLoading())
			{
				Value = static_cast<EnumType>(Raw);
			}
		}

		void SerializeName(FArchive& Archive, FName& Value)
		{
			FString Text = Value.ToString();
			Archive << Text;
			if (Archive.IsLoading())
			{
				Value = FName(*Text);
			}
		}

		bool SerializeCount(FArchive& Archive, int32& Count, const int32 Maximum)
		{
			Archive << Count;
			if (Archive.IsLoading() && (Count < 0 || Count > Maximum))
			{
				Archive.SetError();
				return false;
			}
			return !Archive.IsError();
		}

		void SerializePoint(FArchive& Archive, FPhase1Point& Point)
		{
			Archive << Point.X;
			Archive << Point.Y;
		}

		void SerializeProposal(FArchive& Archive, FStarterPlanProposal& Proposal)
		{
			SerializePoint(Archive, Proposal.RunwayStart);
			SerializePoint(Archive, Proposal.RunwayEnd);
			Archive << Proposal.RunwayWidthCentimeters;
			SerializePoint(Archive, Proposal.TaxiStart);
			SerializePoint(Archive, Proposal.TaxiEnd);
			SerializePoint(Archive, Proposal.StandCenter);
			SerializePoint(Archive, Proposal.AccessStart);
			SerializePoint(Archive, Proposal.AccessEnd);
			SerializePoint(Archive, Proposal.OperationsHutCenter);
		}

		void SerializeFacility(FArchive& Archive, FFacilityRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.DefinitionId);
			SerializeEnum(Archive, Record.Type);
			Archive << Record.bBuilt;
			Archive << Record.bOpen;
			Archive << Record.Status;
		}

		void SerializeTeam(FArchive& Archive, FStaffTeamRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.RoleId);
			Archive << Record.TeamSize;
			Archive << Record.bAvailable;
			Archive << Record.CurrentTask;
		}

		void SerializeTransaction(FArchive& Archive, FTransactionRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.GameTimeMilliseconds;
			SerializeName(Archive, Record.Category);
			Archive << Record.AmountCredits;
			Archive << Record.Explanation;
		}

		void SerializeRating(FArchive& Archive, FRatingContribution& Record)
		{
			SerializeName(Archive, Record.Component);
			Archive << Record.Value;
			Archive << Record.Reason;
		}

		void SerializeObjective(FArchive& Archive, FObjectiveRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ObjectiveId);
			Archive << Record.Text;
			Archive << Record.bCompleted;
		}

		void SerializePhrase(FArchive& Archive, FPhraseIntentRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.GameTimeMilliseconds;
			SerializeName(Archive, Record.PhraseId);
			Archive << Record.Speaker;
			Archive << Record.CallSign;
			Archive << Record.Caption;
			Archive << Record.Priority;
			Archive << Record.DeduplicationKey;
		}

		void SerializePhase1Event(FArchive& Archive, FPhase1Event& Record)
		{
			Archive << Record.Sequence;
			Archive << Record.GameTimeMilliseconds;
			SerializeEnum(Archive, Record.Type);
			Archive << Record.Cause.Value;
			Archive << Record.Message;
		}

		void SerializeReservation(FArchive& Archive, FMovementReservation& Record)
		{
			SerializeName(Archive, Record.BlockId);
			Archive << Record.Owner.Value;
			Archive << Record.StartGameMilliseconds;
			Archive << Record.EndGameMilliseconds;
		}

		template <typename RecordType, typename Serializer>
		void SerializeRecords(
			FArchive& Archive,
			TArray<RecordType>& Records,
			const int32 Maximum,
			Serializer&& SerializeRecord)
		{
			int32 Count = Records.Num();
			if (!SerializeCount(Archive, Count, Maximum))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Records.SetNum(Count);
			}
			for (RecordType& Record : Records)
			{
				SerializeRecord(Archive, Record);
				if (Archive.IsError())
				{
					return;
				}
			}
		}

		void SerializePhase1State(FArchive& Archive, FPhase1State& State)
		{
			Archive << State.bInitialized;
			Archive << State.bPaused;
			Archive << State.bAirportOpen;
			Archive << State.AirportId.Value;
			SerializeName(Archive, State.MapId);
			Archive << State.AirportName;
			Archive << State.MasterSeed;
			Archive << State.RandomStreamState;
			Archive << State.SpeedMultiplier;
			Archive << State.Credits;
			Archive << State.AirportPoints;
			Archive << State.LastUpdatedGameMilliseconds;
			Archive << State.NextDomainId;
			Archive << State.NextPhase1EventSequence;

			Archive << State.Project.Id.Value;
			SerializeEnum(Archive, State.Project.Stage);
			SerializeProposal(Archive, State.Project.Proposal);
			Archive << State.Project.QuotedCost;
			Archive << State.Project.FundedAtGameMilliseconds;
			Archive << State.Project.StageChangedAtGameMilliseconds;
			Archive << State.Project.bDeliveryArrived;
			Archive << State.Project.bInspectionPassed;

			SerializeRecords(Archive, State.Facilities, MaximumSmallRecords, SerializeFacility);
			SerializeRecords(Archive, State.Teams, MaximumSmallRecords, SerializeTeam);

			Archive << State.Offer.ContractId.Value;
			SerializeEnum(Archive, State.Offer.State);
			SerializeName(Archive, State.Offer.AircraftContentId);
			SerializeName(Archive, State.Offer.OperatorContentId);
			Archive << State.Offer.CompatibilitySummary;
			Archive << State.Offer.RewardCredits;
			Archive << State.Offer.bPinned;

			Archive << State.Airframe.Id.Value;
			SerializeName(Archive, State.Airframe.AircraftContentId);
			SerializeName(Archive, State.Airframe.OperatorContentId);
			Archive << State.Airframe.TailNumber;
			Archive << State.Airframe.VisitCount;
			Archive << State.Airframe.HistorySummary;

			Archive << State.Flight.Id.Value;
			Archive << State.Flight.ContractId.Value;
			Archive << State.Flight.AirframeId.Value;
			SerializeEnum(Archive, State.Flight.State);
			Archive << State.Flight.ScheduledArrivalGameMilliseconds;
			Archive << State.Flight.StandOccupancyStartGameMilliseconds;
			Archive << State.Flight.StandOccupancyEndGameMilliseconds;
			Archive << State.Flight.StateChangedAtGameMilliseconds;
			Archive << State.Flight.AssignedRunway.Value;
			Archive << State.Flight.AssignedStand.Value;
			SerializeEnum(Archive, State.Flight.Inspection);
			SerializeEnum(Archive, State.Flight.Fueling);
			Archive << State.Flight.Blocker;
			Archive << State.Flight.bRewardRecognized;

			SerializeRecords(Archive, State.Reservations, MaximumSmallRecords, SerializeReservation);
			SerializeRecords(
				Archive,
				State.Transactions,
				MaximumHistoryRecords,
				SerializeTransaction);
			SerializeRecords(
				Archive,
				State.RatingContributions,
				MaximumHistoryRecords,
				SerializeRating);
			SerializeRecords(
				Archive,
				State.Objectives,
				MaximumHistoryRecords,
				SerializeObjective);
			SerializeRecords(
				Archive,
				State.PhraseIntents,
				MaximumHistoryRecords,
				SerializePhrase);
			SerializeRecords(
				Archive,
				State.Events,
				MaximumHistoryRecords,
				SerializePhase1Event);
			Archive << State.RecoveryGrantCount;
		}

		void SerializePhase2Aircraft(FArchive& Archive, FPhase2AircraftRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.AircraftContentId);
			SerializeName(Archive, Record.RoleId);
			SerializeName(Archive, Record.OperatorContentId);
			Archive << Record.TailNumber;
			Archive << Record.VisitCount;
			Archive << Record.ServiceCount;
			Archive << Record.LastVisitGameMilliseconds;
			Archive << Record.HistorySummary;
		}

		void SerializePhase2Contract(FArchive& Archive, FPhase2ContractRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.TenantId.Value;
			SerializeEnum(Archive, Record.Specialization);
			SerializeName(Archive, Record.ContractContentId);
			SerializeName(Archive, Record.AircraftRoleId);
			Archive << Record.FlightsPerOperatingDay;
			Archive << Record.RewardPerFlightCredits;
			Archive << Record.CancellationCostCredits;
			Archive << Record.bAccepted;
			Archive << Record.CompatibilitySummary;
		}

		void SerializePhase2Flight(FArchive& Archive, FPhase2FlightRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.ContractId.Value;
			Archive << Record.AircraftId.Value;
			SerializeEnum(Archive, Record.State);
			Archive << Record.OperatingDay;
			Archive << Record.ScheduledArrivalGameMilliseconds;
			Archive << Record.StateChangedAtGameMilliseconds;
			Archive << Record.StandStartGameMilliseconds;
			Archive << Record.StandEndGameMilliseconds;
			SerializeName(Archive, Record.AssignedRunwayDirection);
			SerializeName(Archive, Record.AssignedStandId);
			Archive << Record.bRequiresDeicing;
			Archive << Record.bRewardRecognized;
			Archive << Record.Blocker;
		}

		void SerializePhase2Reservation(
			FArchive& Archive,
			FPhase2MovementReservation& Record)
		{
			SerializeName(Archive, Record.BlockId);
			Archive << Record.FlightId.Value;
			Archive << Record.StartGameMilliseconds;
			Archive << Record.EndGameMilliseconds;
			Archive << Record.bTow;
		}

		void SerializePhase2Service(FArchive& Archive, FPhase2ServiceTaskRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.FlightId.Value;
			SerializeName(Archive, Record.ServiceId);
			SerializeEnum(Archive, Record.State);
			Archive << Record.PrerequisiteTaskId.Value;
			Archive << Record.AssignedVehicleId.Value;
			Archive << Record.AssignedTeamId.Value;
			Archive << Record.StateChangedAtGameMilliseconds;
			Archive << Record.DurationMilliseconds;
			Archive << Record.Blocker;
		}

		void SerializePhase2Vehicle(FArchive& Archive, FPhase2VehicleRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.VehicleContentId);
			SerializeName(Archive, Record.CapabilityId);
			SerializeName(Archive, Record.HomeDepotId);
			SerializeEnum(Archive, Record.State);
			Archive << Record.AssignedTaskId.Value;
			Archive << Record.StateChangedAtGameMilliseconds;
			Archive << Record.CompletedTaskCount;
		}

		void SerializePhase2Team(FArchive& Archive, FPhase2StaffTeamRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.RoleId);
			SerializeName(Archive, Record.ZoneId);
			Archive << Record.TeamSize;
			Archive << Record.WorkloadPercent;
			Archive << Record.MoralePercent;
			Archive << Record.bOnShift;
			Archive << Record.AssignedTaskId.Value;
		}

		void SerializePhase2Tenant(FArchive& Archive, FPhase2TenantRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.TenantContentId);
			SerializeEnum(Archive, Record.Specialization);
			Archive << Record.DisplayName;
			Archive << Record.Requirements;
			Archive << Record.EvidencePoints;
			Archive << Record.bActive;
		}

		void SerializePhase2Parcel(FArchive& Archive, FPhase2ParcelRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ParcelContentId);
			Archive << Record.PurchaseCostCredits;
			Archive << Record.bOwned;
		}

		void SerializePhase2Expansion(
			FArchive& Archive,
			FPhase2ExpansionProjectRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.ParcelId.Value;
			SerializeEnum(Archive, Record.Stage);
			Archive << Record.FundedAtGameMilliseconds;
			Archive << Record.StageChangedAtGameMilliseconds;
			Archive << Record.QuotedCostCredits;
			Archive << Record.bDeliveryArrived;
			Archive << Record.bInspectionPassed;
			Archive << Record.ClosureSummary;
		}

		void SerializePhase2Weather(FArchive& Archive, FPhase2WeatherRecord& Record)
		{
			Archive << Record.OperatingDay;
			SerializeEnum(Archive, Record.Category);
			Archive << Record.WindDirectionDegrees;
			Archive << Record.WindSpeedKnots;
			Archive << Record.VisibilityMeters;
			Archive << Record.TemperatureCelsius;
			Archive << Record.OperationalSummary;
		}

		void SerializePhase2Incident(FArchive& Archive, FPhase2IncidentRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeEnum(Archive, Record.Type);
			SerializeEnum(Archive, Record.State);
			Archive << Record.ReportedAtGameMilliseconds;
			Archive << Record.StateChangedAtGameMilliseconds;
			Archive << Record.ResponseTeamId.Value;
			Archive << Record.Cause;
			Archive << Record.Remedy;
			Archive << Record.CostCredits;
		}

		void SerializePhase2Rating(FArchive& Archive, FPhase2RatingRecord& Record)
		{
			SerializeName(Archive, Record.ComponentId);
			Archive << Record.Value;
			Archive << Record.RecentCause;
			Archive << Record.Remedy;
		}

		void SerializePhase2Achievement(
			FArchive& Archive,
			FPhase2AchievementRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.AchievementContentId);
			Archive << Record.DisplayName;
			Archive << Record.bEarned;
			Archive << Record.EarnedAtGameMilliseconds;
		}

		void SerializePhase2Event(FArchive& Archive, FPhase2Event& Record)
		{
			Archive << Record.Sequence;
			Archive << Record.GameTimeMilliseconds;
			SerializeEnum(Archive, Record.Type);
			Archive << Record.Cause.Value;
			Archive << Record.SubjectId;
			Archive << Record.Message;
		}

		void SerializePhase2State(FArchive& Archive, FPhase2State& State)
		{
			Archive << State.bInitialized;
			Archive << State.MasterSeed;
			Archive << State.RandomStreamState;
			Archive << State.NextDomainId;
			Archive << State.NextEventSequence;
			Archive << State.InitializedAtGameMilliseconds;
			Archive << State.LastUpdatedGameMilliseconds;
			Archive << State.CurrentOperatingDay;
			Archive << State.LastEconomyOperatingDay;
			SerializeEnum(Archive, State.SelectedSpecialization);
			SerializeName(Archive, State.ActiveRunwayDirection);
			Archive << State.ActiveRunwayReason;
			Archive << State.bApproachAidReady;
			Archive << State.bAutomaticDispatch;

			SerializeRecords(
				Archive,
				State.Aircraft,
				MaximumSmallRecords,
				SerializePhase2Aircraft);
			SerializeRecords(
				Archive,
				State.Contracts,
				MaximumSmallRecords,
				SerializePhase2Contract);
			SerializeRecords(
				Archive,
				State.Flights,
				MaximumHistoryRecords,
				SerializePhase2Flight);
			SerializeRecords(
				Archive,
				State.Reservations,
				MaximumHistoryRecords,
				SerializePhase2Reservation);
			SerializeRecords(
				Archive,
				State.ServiceTasks,
				MaximumHistoryRecords,
				SerializePhase2Service);
			SerializeRecords(
				Archive,
				State.Vehicles,
				MaximumSmallRecords,
				SerializePhase2Vehicle);
			SerializeRecords(
				Archive,
				State.Teams,
				MaximumSmallRecords,
				SerializePhase2Team);
			SerializeRecords(
				Archive,
				State.Tenants,
				MaximumSmallRecords,
				SerializePhase2Tenant);
			SerializeRecords(
				Archive,
				State.Parcels,
				MaximumSmallRecords,
				SerializePhase2Parcel);
			SerializePhase2Expansion(Archive, State.Expansion);
			SerializePhase2Weather(Archive, State.CurrentWeather);
			SerializeRecords(
				Archive,
				State.Forecast,
				MaximumSmallRecords,
				SerializePhase2Weather);
			SerializePhase2Incident(Archive, State.Incident);
			SerializeRecords(
				Archive,
				State.Ratings,
				MaximumSmallRecords,
				SerializePhase2Rating);
			SerializeRecords(
				Archive,
				State.Achievements,
				MaximumSmallRecords,
				SerializePhase2Achievement);
			SerializeRecords(
				Archive,
				State.Events,
				MaximumHistoryRecords,
				SerializePhase2Event);
			Archive << State.CompletedFlightCount;
			Archive << State.RecoveryGrantCount;
			Archive << State.TotalPhase2RevenueCredits;
			Archive << State.TotalPhase2CostCredits;
		}

		void SerializeBody(FArchive& Archive, FSnapshot& Snapshot)
		{
			Archive << Snapshot.SchemaVersion;
			Archive << Snapshot.MasterSeed;
			Archive << Snapshot.NextEntityId;
			Archive << Snapshot.NextEventSequence;
			Archive << Snapshot.Revision;
			Archive << Snapshot.GameTimeMilliseconds;
			int32 EntityCount = Snapshot.Entities.Num();
			if (!SerializeCount(Archive, EntityCount, 1000000))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Snapshot.Entities.SetNum(EntityCount);
			}
			for (FEntityId& Entity : Snapshot.Entities)
			{
				Archive << Entity.Value;
			}
			if (Snapshot.SchemaVersion >= 2)
			{
				SerializePhase1State(Archive, Snapshot.Phase1);
			}
			if (Snapshot.SchemaVersion >= 3)
			{
				SerializePhase2State(Archive, Snapshot.Phase2);
			}
		}
	}

	bool SerializeSnapshot(const FSnapshot& Snapshot, TArray<uint8>& Output)
	{
		if (Snapshot.SchemaVersion < MinimumSupportedSnapshotSchemaVersion ||
			Snapshot.SchemaVersion > SnapshotSchemaVersion)
		{
			return false;
		}

		TArray<uint8> Body;
		FMemoryWriter BodyWriter(Body, true);
		FSnapshot MutableSnapshot = Snapshot;
		SerializeBody(BodyWriter, MutableSnapshot);
		if (BodyWriter.IsError())
		{
			return false;
		}

		Output.Reset();
		FMemoryWriter Writer(Output, true);
		uint32 Magic = SnapshotMagic;
		uint32 BodySize = Body.Num();
		uint32 Checksum = FCrc::MemCrc32(Body.GetData(), Body.Num());
		Writer << Magic;
		Writer << BodySize;
		Writer << Checksum;
		Writer.Serialize(Body.GetData(), Body.Num());
		return !Writer.IsError();
	}

	bool DeserializeSnapshot(const TArray<uint8>& Input, FSnapshot& Output)
	{
		FMemoryReader Reader(Input, true);
		uint32 Magic = 0;
		uint32 BodySize = 0;
		uint32 ExpectedChecksum = 0;
		Reader << Magic;
		Reader << BodySize;
		Reader << ExpectedChecksum;
		if (Reader.IsError() ||
			Magic != SnapshotMagic ||
			BodySize != static_cast<uint32>(Reader.TotalSize() - Reader.Tell()))
		{
			return false;
		}

		TArray<uint8> Body;
		Body.SetNumUninitialized(BodySize);
		Reader.Serialize(Body.GetData(), BodySize);
		if (Reader.IsError() || FCrc::MemCrc32(Body.GetData(), Body.Num()) != ExpectedChecksum)
		{
			return false;
		}

		FMemoryReader BodyReader(Body, true);
		FSnapshot Candidate;
		SerializeBody(BodyReader, Candidate);
		if (BodyReader.IsError() ||
			BodyReader.Tell() != BodyReader.TotalSize() ||
			!MigrateSnapshotToCurrent(Candidate))
		{
			return false;
		}
		Output = MoveTemp(Candidate);
		return true;
	}

	bool MigrateSnapshotToCurrent(FSnapshot& Snapshot)
	{
		if (Snapshot.SchemaVersion < MinimumSupportedSnapshotSchemaVersion ||
			Snapshot.SchemaVersion > SnapshotSchemaVersion)
		{
			return false;
		}
		if (Snapshot.SchemaVersion == 1)
		{
			Snapshot.Phase1 = {};
			Snapshot.Phase1.MasterSeed = Snapshot.MasterSeed == 0 ? 1 : Snapshot.MasterSeed;
			Snapshot.Phase1.RandomStreamState = FDeterministicStream::SeedNamedStream(
				Snapshot.Phase1.MasterSeed,
				TEXT("Phase1.StarterAirfield"));
			Snapshot.SchemaVersion = 2;
		}
		if (Snapshot.SchemaVersion == 2)
		{
			FLivingAirportSimulation EmptyPhase2(
				Snapshot.MasterSeed == 0 ? 1 : Snapshot.MasterSeed);
			Snapshot.Phase2 = EmptyPhase2.GetState();
			Snapshot.SchemaVersion = 3;
		}
		return Snapshot.SchemaVersion == SnapshotSchemaVersion;
	}
}
