#include "AMSimTerminalLayoutSerialization.h"

#include "Serialization/Archive.h"

namespace AMSim
{
	namespace
	{
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

		template <typename RecordType, typename SerializerType>
		void SerializeRecords(
			FArchive& Archive,
			TArray<RecordType>& Records,
			const int32 MaximumCount,
			SerializerType Serializer)
		{
			int32 Count = Records.Num();
			Archive << Count;
			if (Count < 0 || Count > MaximumCount)
			{
				Archive.SetError();
				return;
			}
			if (Archive.IsLoading())
			{
				Records.SetNum(Count);
			}
			for (RecordType& Record : Records)
			{
				Serializer(Archive, Record);
			}
		}

		void SerializeCell(FArchive& Archive, FTerminalCellCoord& Cell)
		{
			Archive << Cell.X;
			Archive << Cell.Y;
		}

		void SerializeElementIds(
			FArchive& Archive,
			TArray<FTerminalElementId>& Ids)
		{
			SerializeRecords(
				Archive,
				Ids,
				65536,
				[](FArchive& Inner, FTerminalElementId& Id) { Inner << Id.Value; });
		}
	}

	void SerializeTerminalLayoutState(
		FArchive& Archive,
		FTerminalLayoutState& State)
	{
		Archive << State.bSeeded;
		Archive << State.Revision;
		Archive << State.MinimumX;
		Archive << State.MinimumY;
		Archive << State.MaximumX;
		Archive << State.MaximumY;
		SerializeRecords(
			Archive,
			State.FloorCells,
			65536,
			[](FArchive& Inner, FTerminalFloorCellRecord& Record)
			{
				Inner << Record.Id.Value;
				SerializeCell(Inner, Record.Cell);
				SerializeEnum(Inner, Record.Kind);
				Inner << Record.bBuilt;
				Inner << Record.bLocallyClosed;
			});
		SerializeRecords(
			Archive,
			State.Edges,
			65536,
			[](FArchive& Inner, FTerminalEdgeRecord& Record)
			{
				Inner << Record.Id.Value;
				SerializeCell(Inner, Record.From);
				SerializeCell(Inner, Record.To);
				SerializeEnum(Inner, Record.Kind);
				Inner << Record.bBuilt;
				Inner << Record.bLocallyClosed;
			});
		SerializeRecords(
			Archive,
			State.Objects,
			16384,
			[](FArchive& Inner, FTerminalPlacedObjectRecord& Record)
			{
				Inner << Record.Id.Value;
				SerializeName(Inner, Record.DefinitionId);
				SerializeEnum(Inner, Record.Kind);
				SerializeCell(Inner, Record.Anchor);
				Inner << Record.QuarterTurns;
				Inner << Record.FootprintWidth;
				Inner << Record.FootprintHeight;
				Inner << Record.ClearanceCells;
				SerializeRecords(
					Inner,
					Record.InteractionPorts,
					64,
					[](FArchive& PortArchive, FTerminalCellCoord& Port)
					{
						SerializeCell(PortArchive, Port);
					});
				Inner << Record.bBuilt;
				Inner << Record.bOperational;
				Inner << Record.bLocallyClosed;
			});
		SerializeRecords(
			Archive,
			State.ConstructionJobs,
			65536,
			[](FArchive& Inner, FTerminalConstructionJobRecord& Record)
			{
				Inner << Record.Id.Value;
				Inner << Record.TransactionId.Value;
				Inner << Record.ElementId.Value;
				SerializeEnum(Inner, Record.WorkKind);
				SerializeEnum(Inner, Record.Stage);
				SerializeCell(Inner, Record.WorkCell);
				Inner << Record.AssignedTeamId.Value;
				Inner << Record.StageChangedAtGameMilliseconds;
				Inner << Record.ProgressPercent;
			});
		SerializeRecords(
			Archive,
			State.EditTransactions,
			65536,
			[](FArchive& Inner, FTerminalEditTransactionRecord& Record)
			{
				Inner << Record.Id.Value;
				Inner << Record.Cause.Value;
				SerializeElementIds(Inner, Record.ElementIds);
				Inner << Record.CostCredits;
				Inner << Record.SalvageCredits;
				Inner << Record.CreatedAtGameMilliseconds;
				Inner << Record.PreviousQuarterTurns;
				Inner << Record.bRefundable;
				Inner << Record.bCompleted;
				Inner << Record.bUndone;
			});
		SerializeRecords(
			Archive,
			State.Visitors,
			1024,
			[](FArchive& Inner, FTerminalVisitorRecord& Record)
			{
				Inner << Record.Id.Value;
				Inner << Record.FlightId.Value;
				SerializeEnum(Inner, Record.Activity);
				SerializeCell(Inner, Record.Cell);
				Inner << Record.ActivityChangedAtGameMilliseconds;
				Inner << Record.bActive;
				Inner << Record.bRequiresAccessibleRoute;
			});
		Archive << State.bEntranceConnected;
		Archive << State.bInformationPointAvailable;
		Archive << State.bSeatingAvailable;
		Archive << State.bAccessibleRestroomRoute;
		Archive << State.bStaffSpaceAvailable;
		Archive << State.ValidAirsideGateCount;
		Archive << State.bReady;
	}
}
