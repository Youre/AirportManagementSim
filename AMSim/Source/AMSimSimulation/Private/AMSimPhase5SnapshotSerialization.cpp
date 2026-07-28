#include "AMSimPhase5SnapshotSerialization.h"

#include "Serialization/Archive.h"

namespace AMSim
{
	namespace
	{
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

		bool SerializeCount(
			FArchive& Archive,
			int32& Count,
			const int32 Maximum)
		{
			Archive << Count;
			if (Archive.IsLoading() && (Count < 0 || Count > Maximum))
			{
				Archive.SetError();
				return false;
			}
			return !Archive.IsError();
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

		void SerializeNames(FArchive& Archive, TArray<FName>& Names)
		{
			int32 Count = Names.Num();
			if (!SerializeCount(Archive, Count, MaximumSmallRecords))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Names.SetNum(Count);
			}
			for (FName& Name : Names)
			{
				SerializeName(Archive, Name);
			}
		}

		void SerializeIds(FArchive& Archive, TArray<uint64>& Ids)
		{
			int32 Count = Ids.Num();
			if (!SerializeCount(Archive, Count, MaximumHistoryRecords))
			{
				return;
			}
			if (Archive.IsLoading())
			{
				Ids.SetNum(Count);
			}
			for (uint64& Id : Ids)
			{
				Archive << Id;
			}
		}

		void SerializeCargoContract(
			FArchive& Archive,
			FPhase5CargoContractRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			SerializeName(Archive, Record.OperatorId);
			Archive << Record.DisplayName;
			SerializeEnum(Archive, Record.CargoClass);
			SerializeEnum(Archive, Record.Flow);
			SerializeName(Archive, Record.AircraftContentId);
			Archive << Record.RewardCredits;
			Archive << Record.bAccepted;
		}

		void SerializeShipment(
			FArchive& Archive,
			FPhase5ShipmentRecord& Record)
		{
			Archive << Record.Id.Value;
			Archive << Record.ContractId.Value;
			SerializeEnum(Archive, Record.CargoClass);
			SerializeEnum(Archive, Record.Flow);
			SerializeEnum(Archive, Record.State);
			Archive << Record.FlowStep;
			Archive << Record.Pieces;
			Archive << Record.MassKilograms;
			Archive << Record.VolumeLitres;
			Archive << Record.DeadlineGameMilliseconds;
			Archive << Record.bSecurityCleared;
			Archive << Record.bBellyFreight;
			SerializeName(Archive, Record.AircraftContentId);
			Archive << Record.LinkedFlightId.Value;
			Archive << Record.WarehouseZoneId.Value;
			Archive << Record.ActiveServiceTaskId.Value;
			Archive << Record.CurrentLocation;
			Archive << Record.ExceptionCause;
			Archive << Record.Remedy;
			Archive << Record.bRewardRecognized;
		}

		void SerializeZone(
			FArchive& Archive,
			FPhase5WarehouseZoneRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			SerializeEnum(Archive, Record.Type);
			Archive << Record.CapacityLitres;
			Archive << Record.OccupiedLitres;
			Archive << Record.bRoadConnected;
			Archive << Record.bAirsideConnected;
			Archive << Record.bOperational;
		}

		void SerializeTenant(
			FArchive& Archive,
			FPhase5ProviderTenantRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			Archive << Record.DisplayName;
			SerializeName(Archive, Record.ProviderType);
			SerializeEnum(Archive, Record.State);
			SerializeName(Archive, Record.FootprintId);
			Archive << Record.OpeningCostCredits;
			Archive << Record.RentCreditsPerOperatingDay;
			Archive << Record.RevenueSharePercent;
			Archive << Record.SatisfactionPercent;
			Archive << Record.ContractTermOperatingDays;
			Archive << Record.GraceEndsAtGameMilliseconds;
			SerializeName(Archive, Record.RequiredCapability);
			Archive << Record.Requirement;
			Archive << Record.ServiceExpectations;
			Archive << Record.OperatingPattern;
			Archive << Record.SatisfactionDrivers;
			Archive << Record.Cause;
			Archive << Record.Remedy;
		}

		void SerializeContribution(
			FArchive& Archive,
			FPhase5RatingContributionRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ComponentId);
			SerializeName(Archive, Record.SourceGroup);
			Archive << Record.Magnitude;
			Archive << Record.AppliedAtGameMilliseconds;
			Archive << Record.Explanation;
		}

		void SerializePath(
			FArchive& Archive,
			FPhase5PathEvidenceRecord& Record)
		{
			SerializeEnum(Archive, Record.Path);
			SerializeEnum(Archive, Record.Band);
			Archive << Record.AirportPoints;
			Archive << Record.OperatingDays;
			Archive << Record.SafetyRating;
			Archive << Record.ReliabilityRating;
			Archive << Record.CompletedOperations;
			Archive << Record.DistinctRolesOrClasses;
			Archive << Record.CompletedPassengers;
			Archive << Record.RegionalPathCount;
			Archive << Record.bPrimaryTenantActive;
			Archive << Record.bSecondaryProviderActive;
			Archive << Record.bSignatureFacilityOperational;
			Archive << Record.bSharedResourceDayCompleted;
			SerializeNames(Archive, Record.FacilityIds);
			Archive << Record.CurrentEvidence;
			Archive << Record.NextRequirement;
		}

		void SerializeObjective(
			FArchive& Archive,
			FPhase5ObjectiveRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			SerializeEnum(Archive, Record.Path);
			Archive << Record.Text;
			Archive << Record.Current;
			Archive << Record.Target;
			Archive << Record.RewardAirportPoints;
			Archive << Record.bCompleted;
		}

		void SerializeAchievement(
			FArchive& Archive,
			FPhase5AchievementRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			SerializeEnum(Archive, Record.Path);
			Archive << Record.DisplayName;
			Archive << Record.bEarned;
			Archive << Record.EarnedAtGameMilliseconds;
		}

		void SerializeSpecialEvent(
			FArchive& Archive,
			FPhase5SpecialEventRecord& Record)
		{
			Archive << Record.Id.Value;
			SerializeName(Archive, Record.ContentId);
			Archive << Record.DisplayName;
			SerializeEnum(Archive, Record.Family);
			SerializeEnum(Archive, Record.State);
			Archive << Record.NoticeEndsAtGameMilliseconds;
			Archive << Record.ActiveEndsAtGameMilliseconds;
			Archive << Record.CooldownEndsAtGameMilliseconds;
			Archive << Record.ExpectedDemand;
			Archive << Record.SatisfiedDemand;
			Archive << Record.RewardCredits;
			SerializeName(Archive, Record.IntegrationDomain);
			SerializeIds(Archive, Record.LinkedEntityIds);
			Archive << Record.Preview;
			Archive << Record.SuccessSummary;
			Archive << Record.bRewardRecognized;
		}

		void SerializeEvent(FArchive& Archive, FPhase5Event& Record)
		{
			Archive << Record.Sequence;
			Archive << Record.GameTimeMilliseconds;
			SerializeEnum(Archive, Record.Type);
			Archive << Record.Cause.Value;
			Archive << Record.SubjectId;
			Archive << Record.Message;
		}
	}

	void SerializePhase5State(FArchive& Archive, FPhase5State& State)
	{
		Archive << State.bInitialized;
		Archive << State.bFixtureCompleted;
		Archive << State.MasterSeed;
		Archive << State.RandomStreamState;
		Archive << State.NextDomainId;
		Archive << State.NextEventSequence;
		Archive << State.InitializedAtGameMilliseconds;
		Archive << State.LastUpdatedGameMilliseconds;
		Archive << State.CurrentOperatingDay;
		SerializeRecords(
			Archive,
			State.CargoContracts,
			MaximumSmallRecords,
			SerializeCargoContract);
		SerializeRecords(
			Archive,
			State.Shipments,
			MaximumHistoryRecords,
			SerializeShipment);
		SerializeRecords(
			Archive,
			State.WarehouseZones,
			MaximumSmallRecords,
			SerializeZone);
		SerializeRecords(
			Archive,
			State.ProviderTenants,
			MaximumSmallRecords,
			SerializeTenant);
		SerializeRecords(
			Archive,
			State.RatingContributions,
			MaximumHistoryRecords,
			SerializeContribution);
		SerializeRecords(
			Archive,
			State.Paths,
			MaximumSmallRecords,
			SerializePath);
		SerializeRecords(
			Archive,
			State.Objectives,
			MaximumSmallRecords,
			SerializeObjective);
		SerializeRecords(
			Archive,
			State.Achievements,
			MaximumSmallRecords,
			SerializeAchievement);
		SerializeRecords(
			Archive,
			State.SpecialEvents,
			MaximumSmallRecords,
			SerializeSpecialEvent);
		SerializeRecords(
			Archive,
			State.Events,
			MaximumHistoryRecords,
			SerializeEvent);
		Archive << State.CompletedShipmentCount;
		Archive << State.CompletedCargoClassCount;
		Archive << State.CompletedCargoFlowCount;
		Archive << State.AdvancedPathCount;
		Archive << State.OverallRating;
		Archive << State.TotalCargoRevenueCredits;
		Archive << State.TotalEventRevenueCredits;
	}
}
