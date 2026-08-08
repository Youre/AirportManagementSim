#pragma once

#include "AMSimPhase2Types.h"

namespace AMSim
{
#define AMSIM_DECLARE_TERMINAL_ID(Name) \
	struct Name \
	{ \
		uint64 Value = 0; \
		bool IsValid() const { return Value != 0; } \
		auto operator<=>(const Name&) const = default; \
	}

	AMSIM_DECLARE_TERMINAL_ID(FTerminalElementId);
	AMSIM_DECLARE_TERMINAL_ID(FTerminalConstructionJobId);
	AMSIM_DECLARE_TERMINAL_ID(FTerminalEditTransactionId);
	AMSIM_DECLARE_TERMINAL_ID(FTerminalVisitorId);

#undef AMSIM_DECLARE_TERMINAL_ID

	struct FTerminalCellCoord
	{
		int32 X = 0;
		int32 Y = 0;

		auto operator<=>(const FTerminalCellCoord&) const = default;
	};

	enum class ETerminalFloorKind : uint8
	{
		Public,
		StaffService,
		EntranceThreshold,
		Restroom,
		Sterile,
		Baggage
	};

	enum class ETerminalEdgeKind : uint8
	{
		ExteriorWall,
		InteriorWall,
		GlassWall,
		EntranceDoor,
		StandardDoor,
		StaffDoor,
		AirsideGateDoor
	};

	enum class ETerminalObjectKind : uint8
	{
		SeatGroup2,
		SeatGroup4,
		SeatGroup6,
		InformationDesk,
		Restroom,
		StaffDesk,
		Storage,
		Vending,
		Water,
		WasteRecycling,
		Noticeboard,
		Signage,
		Plant
	};

	enum class ETerminalConstructionWorkKind : uint8
	{
		BuildFloor,
		BuildEdge,
		PlaceObject,
		RotateObject,
		DemolishElement
	};

	enum class ETerminalConstructionWorkStage : uint8
	{
		Queued,
		Delivery,
		Foundation,
		Framing,
		ActiveWork,
		Inspection,
		Completed,
		Cancelled
	};

	enum class ETerminalVisitorActivity : uint8
	{
		Entering,
		Information,
		Waiting,
		Restroom,
		AirsideGate,
		Completed
	};

	struct FTerminalFloorCellRecord
	{
		FTerminalElementId Id;
		FTerminalCellCoord Cell;
		ETerminalFloorKind Kind = ETerminalFloorKind::Public;
		bool bBuilt = false;
		bool bLocallyClosed = false;
	};

	struct FTerminalEdgeRecord
	{
		FTerminalElementId Id;
		FTerminalCellCoord From;
		FTerminalCellCoord To;
		ETerminalEdgeKind Kind = ETerminalEdgeKind::ExteriorWall;
		bool bBuilt = false;
		bool bLocallyClosed = false;
	};

	struct FTerminalPlacedObjectRecord
	{
		FTerminalElementId Id;
		FName DefinitionId;
		ETerminalObjectKind Kind = ETerminalObjectKind::SeatGroup2;
		FTerminalCellCoord Anchor;
		int32 QuarterTurns = 0;
		int32 FootprintWidth = 1;
		int32 FootprintHeight = 1;
		int32 ClearanceCells = 0;
		TArray<FTerminalCellCoord> InteractionPorts;
		bool bBuilt = false;
		bool bOperational = false;
		bool bLocallyClosed = false;
	};

	struct FTerminalConstructionJobRecord
	{
		FTerminalConstructionJobId Id;
		FTerminalEditTransactionId TransactionId;
		FTerminalElementId ElementId;
		ETerminalConstructionWorkKind WorkKind =
			ETerminalConstructionWorkKind::BuildFloor;
		ETerminalConstructionWorkStage Stage =
			ETerminalConstructionWorkStage::Queued;
		FTerminalCellCoord WorkCell;
		FStaffTeamId AssignedTeamId;
		int64 StageChangedAtGameMilliseconds = 0;
		int32 ProgressPercent = 0;
	};

	struct FTerminalEditTransactionRecord
	{
		FTerminalEditTransactionId Id;
		FCommandId Cause;
		TArray<FTerminalElementId> ElementIds;
		int64 CostCredits = 0;
		int64 SalvageCredits = 0;
		int64 CreatedAtGameMilliseconds = 0;
		int32 PreviousQuarterTurns = 0;
		bool bRefundable = true;
		bool bCompleted = false;
		bool bUndone = false;
	};

	struct FTerminalVisitorRecord
	{
		FTerminalVisitorId Id;
		FFlightId FlightId;
		ETerminalVisitorActivity Activity = ETerminalVisitorActivity::Entering;
		FTerminalCellCoord Cell;
		int64 ActivityChangedAtGameMilliseconds = 0;
		bool bActive = false;
		bool bRequiresAccessibleRoute = false;
	};

	struct FTerminalSpatialRoomRecord
	{
		uint64 StableId = 0;
		ETerminalFloorKind Function = ETerminalFloorKind::Public;
		TArray<FTerminalCellCoord> Cells;
		bool bOperational = false;
	};

	struct FTerminalLayoutState
	{
		bool bSeeded = false;
		uint64 Revision = 0;
		int32 MinimumX = 0;
		int32 MinimumY = 0;
		int32 MaximumX = 17;
		int32 MaximumY = 11;
		TArray<FTerminalFloorCellRecord> FloorCells;
		TArray<FTerminalEdgeRecord> Edges;
		TArray<FTerminalPlacedObjectRecord> Objects;
		TArray<FTerminalConstructionJobRecord> ConstructionJobs;
		TArray<FTerminalEditTransactionRecord> EditTransactions;
		TArray<FTerminalVisitorRecord> Visitors;
		bool bEntranceConnected = false;
		bool bInformationPointAvailable = false;
		bool bSeatingAvailable = false;
		bool bAccessibleRestroomRoute = false;
		bool bStaffSpaceAvailable = false;
		int32 ValidAirsideGateCount = 0;
		bool bReady = false;
	};

	struct FTerminalLayoutQuerySnapshot
	{
		uint64 Revision = 0;
		TArray<FTerminalFloorCellRecord> FloorCells;
		TArray<FTerminalEdgeRecord> Edges;
		TArray<FTerminalPlacedObjectRecord> Objects;
		TArray<FTerminalConstructionJobRecord> ConstructionJobs;
		TArray<FTerminalVisitorRecord> Visitors;
		TArray<FTerminalSpatialRoomRecord> Rooms;
		bool bReady = false;
		int32 ValidAirsideGateCount = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FTerminalCellCoord& Cell)
	{
		return HashCombine(::GetTypeHash(Cell.X), ::GetTypeHash(Cell.Y));
	}
}
