#include "AMSimPassengerTerminalSimulation.h"

#include "Algo/Reverse.h"
#include "AMSimStarterAirfieldSimulation.h"

namespace AMSim
{
	namespace
	{
		constexpr int32 MaximumTerminalCoordinate = 128;
		constexpr int64 TerminalWorkStageMilliseconds = 6000;

		bool IsInsideCoordinateLimit(const FTerminalCellCoord Cell)
		{
			return FMath::Abs(Cell.X) <= MaximumTerminalCoordinate &&
				FMath::Abs(Cell.Y) <= MaximumTerminalCoordinate;
		}

		int32 ManhattanLength(
			const FTerminalCellCoord From,
			const FTerminalCellCoord To)
		{
			return FMath::Abs(To.X - From.X) + FMath::Abs(To.Y - From.Y);
		}

		bool IsAxisAligned(
			const FTerminalCellCoord From,
			const FTerminalCellCoord To)
		{
			return (From.X == To.X) != (From.Y == To.Y);
		}

		bool SameUndirectedEdge(
			const FTerminalEdgeRecord& Edge,
			const FTerminalCellCoord From,
			const FTerminalCellCoord To)
		{
			return (Edge.From == From && Edge.To == To) ||
				(Edge.From == To && Edge.To == From);
		}

		bool ContainsFloor(
			const FTerminalLayoutState& Layout,
			const FTerminalCellCoord Cell,
			const bool bRequireWalkable = false)
		{
			return Layout.FloorCells.ContainsByPredicate(
				[Cell, bRequireWalkable](const FTerminalFloorCellRecord& Record)
				{
					return Record.Cell == Cell &&
						(!bRequireWalkable ||
							(Record.bBuilt && !Record.bLocallyClosed));
				});
		}

		bool ContainsElement(
			const FTerminalLayoutState& Layout,
			const FTerminalElementId Id)
		{
			return Layout.FloorCells.ContainsByPredicate(
				[Id](const FTerminalFloorCellRecord& Record) { return Record.Id == Id; }) ||
				Layout.Edges.ContainsByPredicate(
					[Id](const FTerminalEdgeRecord& Record) { return Record.Id == Id; }) ||
				Layout.Objects.ContainsByPredicate(
					[Id](const FTerminalPlacedObjectRecord& Record) { return Record.Id == Id; });
		}

		void GetObjectFootprint(
			const ETerminalObjectKind Kind,
			int32& Width,
			int32& Height,
			int32& Clearance)
		{
			Width = 1;
			Height = 1;
			Clearance = 0;
			switch (Kind)
			{
			case ETerminalObjectKind::SeatGroup2: Width = 2; break;
			case ETerminalObjectKind::SeatGroup4: Width = 2; Height = 2; break;
			case ETerminalObjectKind::SeatGroup6: Width = 3; Height = 2; break;
			case ETerminalObjectKind::InformationDesk:
			case ETerminalObjectKind::StaffDesk: Width = 2; Clearance = 1; break;
			case ETerminalObjectKind::Restroom: Width = 3; Height = 3; break;
			case ETerminalObjectKind::Storage: Width = 2; Height = 2; break;
			default: break;
			}
		}

		FName ObjectDefinition(const ETerminalObjectKind Kind)
		{
			switch (Kind)
			{
			case ETerminalObjectKind::SeatGroup2: return TEXT("Terminal.Object.Seating.2");
			case ETerminalObjectKind::SeatGroup4: return TEXT("Terminal.Object.Seating.4");
			case ETerminalObjectKind::SeatGroup6: return TEXT("Terminal.Object.Seating.6");
			case ETerminalObjectKind::InformationDesk: return TEXT("Terminal.Object.InformationDesk");
			case ETerminalObjectKind::Restroom: return TEXT("Terminal.Object.Restroom.Accessible");
			case ETerminalObjectKind::StaffDesk: return TEXT("Terminal.Object.StaffDesk");
			case ETerminalObjectKind::Storage: return TEXT("Terminal.Object.Storage");
			case ETerminalObjectKind::Vending: return TEXT("Terminal.Object.Vending");
			case ETerminalObjectKind::Water: return TEXT("Terminal.Object.Water");
			case ETerminalObjectKind::WasteRecycling: return TEXT("Terminal.Object.WasteRecycling");
			case ETerminalObjectKind::Noticeboard: return TEXT("Terminal.Object.Noticeboard");
			case ETerminalObjectKind::Signage: return TEXT("Terminal.Object.Signage");
			case ETerminalObjectKind::Plant: return TEXT("Terminal.Object.Plant");
			default: return TEXT("Terminal.Object.Unknown");
			}
		}

		int64 ObjectCost(const ETerminalObjectKind Kind)
		{
			switch (Kind)
			{
			case ETerminalObjectKind::InformationDesk: return 425;
			case ETerminalObjectKind::Restroom: return 850;
			case ETerminalObjectKind::StaffDesk: return 300;
			case ETerminalObjectKind::Storage: return 275;
			case ETerminalObjectKind::SeatGroup6: return 240;
			case ETerminalObjectKind::SeatGroup4: return 180;
			case ETerminalObjectKind::SeatGroup2: return 110;
			default: return 90;
			}
		}

		int64 ElementSalvageCredits(
			const FTerminalLayoutState& Layout,
			const FTerminalElementId ElementId)
		{
			if (Layout.FloorCells.ContainsByPredicate(
				[ElementId](const FTerminalFloorCellRecord& Cell) { return Cell.Id == ElementId; }))
			{
				return 25 / 4;
			}
			if (const FTerminalEdgeRecord* Edge = Layout.Edges.FindByPredicate(
				[ElementId](const FTerminalEdgeRecord& Entry) { return Entry.Id == ElementId; }))
			{
				const bool bDoor = Edge->Kind == ETerminalEdgeKind::EntranceDoor ||
					Edge->Kind == ETerminalEdgeKind::StandardDoor ||
					Edge->Kind == ETerminalEdgeKind::StaffDoor ||
					Edge->Kind == ETerminalEdgeKind::AirsideGateDoor;
				return (bDoor ? 120 : 35) / 4;
			}
			if (const FTerminalPlacedObjectRecord* Object = Layout.Objects.FindByPredicate(
				[ElementId](const FTerminalPlacedObjectRecord& Entry) { return Entry.Id == ElementId; }))
			{
				return ObjectCost(Object->Kind) / 4;
			}
			return 0;
		}

		bool HasBuiltObject(
			const FTerminalLayoutState& Layout,
			const ETerminalObjectKind Kind)
		{
			return Layout.Objects.ContainsByPredicate(
				[Kind](const FTerminalPlacedObjectRecord& Object)
				{
					return Object.Kind == Kind && Object.bBuilt &&
						Object.bOperational && !Object.bLocallyClosed;
				});
		}

		bool HasBuiltDoor(
			const FTerminalLayoutState& Layout,
			const ETerminalEdgeKind Kind)
		{
			return Layout.Edges.ContainsByPredicate(
				[Kind](const FTerminalEdgeRecord& Edge)
				{
					return Edge.Kind == Kind && Edge.bBuilt && !Edge.bLocallyClosed;
				});
		}

		bool HasConnectedFloorRoute(const FTerminalLayoutState& Layout)
		{
			const FTerminalEdgeRecord* Entrance = Layout.Edges.FindByPredicate(
				[](const FTerminalEdgeRecord& Edge)
				{
					return Edge.Kind == ETerminalEdgeKind::EntranceDoor &&
						Edge.bBuilt && !Edge.bLocallyClosed;
				});
			if (!Entrance)
			{
				return false;
			}
			FTerminalCellCoord Start = Entrance->From;
			if (!ContainsFloor(Layout, Start, true))
			{
				Start = Entrance->To;
			}
			if (!ContainsFloor(Layout, Start, true))
			{
				return false;
			}

			TSet<FTerminalCellCoord> Visited;
			TArray<FTerminalCellCoord> Frontier = {Start};
			Visited.Add(Start);
			for (int32 Index = 0; Index < Frontier.Num(); ++Index)
			{
				const FTerminalCellCoord Current = Frontier[Index];
				for (const FTerminalEdgeRecord& Gate : Layout.Edges)
				{
					if (Gate.Kind == ETerminalEdgeKind::AirsideGateDoor &&
						Gate.bBuilt && !Gate.bLocallyClosed &&
						(Gate.From == Current || Gate.To == Current))
					{
						return true;
					}
				}
				const FTerminalCellCoord Neighbors[] = {
					{Current.X + 1, Current.Y}, {Current.X - 1, Current.Y},
					{Current.X, Current.Y + 1}, {Current.X, Current.Y - 1}};
				for (const FTerminalCellCoord Neighbor : Neighbors)
				{
					if (!Visited.Contains(Neighbor) && ContainsFloor(Layout, Neighbor, true))
					{
						Visited.Add(Neighbor);
						Frontier.Add(Neighbor);
					}
				}
			}
			return false;
		}
	}

	void FPassengerTerminalSimulation::SeedStarterTerminalLayout(const bool bCompleted)
	{
		FTerminalLayoutState& Layout = State.TerminalLayout;
		if (Layout.bSeeded)
		{
			return;
		}
		Layout = {};
		Layout.bSeeded = true;
		Layout.MinimumX = 0;
		Layout.MinimumY = 0;
		Layout.MaximumX = 17;
		Layout.MaximumY = 11;

		for (int32 Y = 0; Y <= 11; ++Y)
		{
			for (int32 X = 0; X <= 17; ++X)
			{
				FTerminalFloorCellRecord Cell;
				Cell.Id = {AllocateDomainId()};
				Cell.Cell = {X, Y};
				Cell.Kind = Y == 0
					? ETerminalFloorKind::EntranceThreshold
					: (X < 4 && Y >= 7
						? ETerminalFloorKind::Restroom
						: (X >= 14 ? ETerminalFloorKind::StaffService : ETerminalFloorKind::Public));
				Cell.bBuilt = bCompleted;
				Layout.FloorCells.Add(Cell);
			}
		}

		auto AddEdge = [this, &Layout, bCompleted](
			const FTerminalCellCoord From,
			const FTerminalCellCoord To,
			const ETerminalEdgeKind Kind)
		{
			FTerminalEdgeRecord Edge;
			Edge.Id = {AllocateDomainId()};
			Edge.From = From;
			Edge.To = To;
			Edge.Kind = Kind;
			Edge.bBuilt = bCompleted;
			Layout.Edges.Add(Edge);
		};
		for (int32 X = 0; X < 17; ++X)
		{
			if (X != 8)
			{
				AddEdge({X, 0}, {X + 1, 0}, ETerminalEdgeKind::ExteriorWall);
			}
			if (X != 5 && X != 12)
			{
				AddEdge({X, 11}, {X + 1, 11}, ETerminalEdgeKind::ExteriorWall);
			}
		}
		for (int32 Y = 0; Y < 11; ++Y)
		{
			AddEdge({0, Y}, {0, Y + 1}, ETerminalEdgeKind::ExteriorWall);
			AddEdge({17, Y}, {17, Y + 1}, ETerminalEdgeKind::ExteriorWall);
		}
		AddEdge({8, 0}, {9, 0}, ETerminalEdgeKind::EntranceDoor);
		AddEdge({5, 11}, {6, 11}, ETerminalEdgeKind::AirsideGateDoor);
		AddEdge({12, 11}, {13, 11}, ETerminalEdgeKind::AirsideGateDoor);
		for (int32 Y = 1; Y < 11; ++Y)
		{
			AddEdge({14, Y}, {14, Y + 1},
				Y == 5 ? ETerminalEdgeKind::StaffDoor : ETerminalEdgeKind::InteriorWall);
		}
		for (int32 X = 0; X < 4; ++X)
		{
			AddEdge({X, 7}, {X + 1, 7},
				X == 2 ? ETerminalEdgeKind::StandardDoor : ETerminalEdgeKind::InteriorWall);
		}
		for (int32 Y = 7; Y < 11; ++Y)
		{
			AddEdge({4, Y}, {4, Y + 1}, ETerminalEdgeKind::InteriorWall);
		}
		for (int32 X = 14; X < 17; ++X)
		{
			AddEdge({X, 7}, {X + 1, 7},
				X == 15 ? ETerminalEdgeKind::StaffDoor : ETerminalEdgeKind::InteriorWall);
		}

		auto AddObject = [this, &Layout, bCompleted](
			const ETerminalObjectKind Kind,
			const FTerminalCellCoord Anchor,
			const int32 QuarterTurns = 0)
		{
			FTerminalPlacedObjectRecord Object;
			Object.Id = {AllocateDomainId()};
			Object.DefinitionId = ObjectDefinition(Kind);
			Object.Kind = Kind;
			Object.Anchor = Anchor;
			Object.QuarterTurns = QuarterTurns;
			GetObjectFootprint(Kind, Object.FootprintWidth, Object.FootprintHeight, Object.ClearanceCells);
			Object.InteractionPorts.Add({Anchor.X, Anchor.Y + Object.FootprintHeight});
			Object.bBuilt = bCompleted;
			Object.bOperational = bCompleted;
			Layout.Objects.Add(Object);
		};
		AddObject(ETerminalObjectKind::InformationDesk, {7, 2});
		AddObject(ETerminalObjectKind::SeatGroup6, {3, 5});
		AddObject(ETerminalObjectKind::SeatGroup4, {9, 5});
		AddObject(ETerminalObjectKind::Restroom, {1, 8});
		AddObject(ETerminalObjectKind::StaffDesk, {15, 3});
		AddObject(ETerminalObjectKind::Storage, {15, 7});
		AddObject(ETerminalObjectKind::Vending, {11, 2});
		AddObject(ETerminalObjectKind::WasteRecycling, {12, 2});
		AddObject(ETerminalObjectKind::SeatGroup4, {7, 7});
		AddObject(ETerminalObjectKind::SeatGroup2, {3, 2});
		AddObject(ETerminalObjectKind::SeatGroup2, {10, 9});
		AddObject(ETerminalObjectKind::Noticeboard, {10, 1});
		AddObject(ETerminalObjectKind::Plant, {6, 3});
		AddObject(ETerminalObjectKind::Plant, {12, 8});
		AddObject(ETerminalObjectKind::Plant, {1, 2});
		AddObject(ETerminalObjectKind::Plant, {16, 1});
		AddObject(ETerminalObjectKind::Signage, {5, 10});
		AddObject(ETerminalObjectKind::Signage, {12, 10});
		AddObject(ETerminalObjectKind::Water, {13, 3});
		Layout.Revision = 1;
		RefreshTerminalReadiness();
	}

	void FPassengerTerminalSimulation::PrepareLayoutMigration(
		const bool bPassengerInitialized)
	{
		State.TerminalLayout = {};
		SeedStarterTerminalLayout(true);
		if (bPassengerInitialized)
		{
			ExpandPassengerTerminalLayout(true);
		}
	}

	void FPassengerTerminalSimulation::ExpandPassengerTerminalLayout(const bool bCompleted)
	{
		SeedStarterTerminalLayout(bCompleted);
		FTerminalLayoutState& Layout = State.TerminalLayout;
		if (Layout.MaximumX >= 29)
		{
			return;
		}
		for (int32 Y = 0; Y <= 17; ++Y)
		{
			for (int32 X = 18; X <= 29; ++X)
			{
				FTerminalFloorCellRecord Cell;
				Cell.Id = {AllocateDomainId()};
				Cell.Cell = {X, Y};
				Cell.Kind = Y >= 11
					? ETerminalFloorKind::Sterile
					: (X >= 27 ? ETerminalFloorKind::Baggage : ETerminalFloorKind::Public);
				Cell.bBuilt = bCompleted;
				Layout.FloorCells.Add(Cell);
			}
		}
		for (int32 Y = 12; Y <= 17; ++Y)
		{
			for (int32 X = 0; X <= 17; ++X)
			{
				FTerminalFloorCellRecord Cell;
				Cell.Id = {AllocateDomainId()};
				Cell.Cell = {X, Y};
				Cell.Kind = Y >= 15
					? ETerminalFloorKind::Sterile
					: ETerminalFloorKind::Public;
				Cell.bBuilt = bCompleted;
				Layout.FloorCells.Add(Cell);
			}
		}
		for (FTerminalEdgeRecord& Edge : Layout.Edges)
		{
			if (Edge.From.X == 17 && Edge.To.X == 17)
			{
				Edge.Kind = Edge.From.Y == 4 || Edge.From.Y == 9
					? ETerminalEdgeKind::StandardDoor
					: ETerminalEdgeKind::InteriorWall;
			}
		}
		auto AddExpansionEdge = [this, &Layout, bCompleted](
			const FTerminalCellCoord From,
			const FTerminalCellCoord To,
			const ETerminalEdgeKind Kind)
		{
			FTerminalEdgeRecord Edge;
			Edge.Id = {AllocateDomainId()};
			Edge.From = From;
			Edge.To = To;
			Edge.Kind = Kind;
			Edge.bBuilt = bCompleted;
			Layout.Edges.Add(Edge);
		};
		for (int32 X = 18; X < 29; ++X)
		{
			AddExpansionEdge({X, 0}, {X + 1, 0}, ETerminalEdgeKind::ExteriorWall);
		}
		for (int32 X = 0; X < 29; ++X)
		{
			AddExpansionEdge({X, 17}, {X + 1, 17},
				X == 20 || X == 25
					? ETerminalEdgeKind::AirsideGateDoor
					: ETerminalEdgeKind::ExteriorWall);
		}
		for (int32 Y = 0; Y < 17; ++Y)
		{
			AddExpansionEdge({29, Y}, {29, Y + 1}, ETerminalEdgeKind::ExteriorWall);
		}
		Layout.MaximumX = 29;
		Layout.MaximumY = 17;
		FTerminalPlacedObjectRecord Desk;
		Desk.Id = {AllocateDomainId()};
		Desk.DefinitionId = TEXT("Terminal.Object.CheckInDesk");
		Desk.Kind = ETerminalObjectKind::StaffDesk;
		Desk.Anchor = {23, 1};
		Desk.FootprintWidth = 2;
		Desk.bBuilt = bCompleted;
		Desk.bOperational = bCompleted;
		Desk.InteractionPorts.Add({21, 5});
		Layout.Objects.Add(Desk);
		auto AddExpansionObject = [this, &Layout, bCompleted](
			const ETerminalObjectKind Kind,
			const FTerminalCellCoord Anchor,
			const int32 QuarterTurns = 0)
		{
			FTerminalPlacedObjectRecord Object;
			Object.Id = {AllocateDomainId()};
			Object.DefinitionId = ObjectDefinition(Kind);
			Object.Kind = Kind;
			Object.Anchor = Anchor;
			Object.QuarterTurns = QuarterTurns;
			GetObjectFootprint(Kind, Object.FootprintWidth, Object.FootprintHeight, Object.ClearanceCells);
			if ((QuarterTurns & 1) != 0) Swap(Object.FootprintWidth, Object.FootprintHeight);
			Object.InteractionPorts.Add({Anchor.X, Anchor.Y + Object.FootprintHeight});
			Object.bBuilt = bCompleted;
			Object.bOperational = bCompleted;
			Layout.Objects.Add(Object);
		};
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {19, 4});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {23, 4});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {20, 12}, 1);
		AddExpansionObject(ETerminalObjectKind::SeatGroup4, {25, 12});
		AddExpansionObject(ETerminalObjectKind::InformationDesk, {19, 1});
		AddExpansionObject(ETerminalObjectKind::Restroom, {26, 1});
		AddExpansionObject(ETerminalObjectKind::StaffDesk, {26, 8});
		AddExpansionObject(ETerminalObjectKind::Storage, {27, 13});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {1, 13});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {5, 13});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {9, 13});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {13, 13});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {19, 8});
		AddExpansionObject(ETerminalObjectKind::SeatGroup6, {23, 8});
		AddExpansionObject(ETerminalObjectKind::SeatGroup4, {19, 15});
		AddExpansionObject(ETerminalObjectKind::SeatGroup4, {24, 15});
		AddExpansionObject(ETerminalObjectKind::StaffDesk, {18, 6});
		AddExpansionObject(ETerminalObjectKind::StaffDesk, {21, 6});
		AddExpansionObject(ETerminalObjectKind::StaffDesk, {24, 6});
		AddExpansionObject(ETerminalObjectKind::Vending, {17, 14});
		AddExpansionObject(ETerminalObjectKind::Water, {22, 14});
		AddExpansionObject(ETerminalObjectKind::WasteRecycling, {28, 10});
		AddExpansionObject(ETerminalObjectKind::Noticeboard, {18, 1});
		AddExpansionObject(ETerminalObjectKind::Signage, {21, 16});
		AddExpansionObject(ETerminalObjectKind::Signage, {27, 16});
		AddExpansionObject(ETerminalObjectKind::Plant, {18, 10});
		AddExpansionObject(ETerminalObjectKind::Plant, {22, 10});
		AddExpansionObject(ETerminalObjectKind::Plant, {26, 10});
		for (int32 X = 18; X < 29; ++X)
		{
			AddExpansionEdge({X, 11}, {X + 1, 11},
				X == 20 || X == 25
					? ETerminalEdgeKind::StandardDoor
					: ETerminalEdgeKind::InteriorWall);
		}
		for (int32 X = 18; X < 27; ++X)
		{
			AddExpansionEdge({X, 5}, {X + 1, 5},
				X == 20 || X == 23
					? ETerminalEdgeKind::StandardDoor
					: ETerminalEdgeKind::GlassWall);
		}
		for (int32 Y = 5; Y < 11; ++Y)
		{
			AddExpansionEdge({27, Y}, {27, Y + 1},
				Y == 7 ? ETerminalEdgeKind::StaffDoor : ETerminalEdgeKind::InteriorWall);
		}
		++Layout.Revision;
		RefreshTerminalReadiness();
	}

	int64 FPassengerTerminalSimulation::QuoteTerminalEdit(const FPhase3Command& Command) const
	{
		switch (Command.Type)
		{
		case EPhase3CommandType::PlaceTerminalFloor:
			return static_cast<int64>(FMath::Abs(Command.EndCell.X - Command.StartCell.X) + 1) *
				(FMath::Abs(Command.EndCell.Y - Command.StartCell.Y) + 1) * 25;
		case EPhase3CommandType::PlaceTerminalWall:
			return static_cast<int64>(ManhattanLength(Command.StartCell, Command.EndCell)) * 35;
		case EPhase3CommandType::PlaceTerminalDoor: return 120;
		case EPhase3CommandType::PlaceTerminalObject: return ObjectCost(Command.ObjectKind);
		case EPhase3CommandType::RotateTerminalObject: return 15;
		default: return 0;
		}
	}

	bool FPassengerTerminalSimulation::ValidateTerminalEdit(
		const FPhase3Command& Command,
		FPhase3Validation& Result) const
	{
		const FTerminalLayoutState& Layout = State.TerminalLayout;
		auto Reject = [&Result](
			const EPhase3CommandResult Code,
			const TCHAR* Cause,
			const TCHAR* Remedy)
		{
			Result.bValid = false;
			Result.Result = Code;
			Result.Cause = Cause;
			Result.Remedy = Remedy;
			return false;
		};
		if (!Layout.bSeeded)
		{
			return Reject(EPhase3CommandResult::RejectedInvalidState,
				TEXT("The terminal layout is unavailable."), TEXT("Reload the airport."));
		}

		switch (Command.Type)
		{
		case EPhase3CommandType::PlaceTerminalFloor:
		{
			if (!IsInsideCoordinateLimit(Command.StartCell) ||
				!IsInsideCoordinateLimit(Command.EndCell) ||
				FMath::Abs(Command.EndCell.X - Command.StartCell.X) >= 32 ||
				FMath::Abs(Command.EndCell.Y - Command.StartCell.Y) >= 32)
			{
				return Reject(EPhase3CommandResult::RejectedInvalidTopology,
					TEXT("The floor stroke is outside the supported ground-floor grid."),
					TEXT("Draw a smaller floor area inside the terminal boundary."));
			}
			const int32 MinX = FMath::Min(Command.StartCell.X, Command.EndCell.X);
			const int32 MaxX = FMath::Max(Command.StartCell.X, Command.EndCell.X);
			const int32 MinY = FMath::Min(Command.StartCell.Y, Command.EndCell.Y);
			const int32 MaxY = FMath::Max(Command.StartCell.Y, Command.EndCell.Y);
			for (int32 Y = MinY; Y <= MaxY; ++Y)
			{
				for (int32 X = MinX; X <= MaxX; ++X)
				{
					if (ContainsFloor(Layout, {X, Y}))
					{
						return Reject(EPhase3CommandResult::RejectedOccupied,
							TEXT("The floor stroke overlaps an existing terminal cell."),
							TEXT("Draw only across empty ground."));
					}
				}
			}
			break;
		}
		case EPhase3CommandType::PlaceTerminalWall:
			if (!IsAxisAligned(Command.StartCell, Command.EndCell) ||
				ManhattanLength(Command.StartCell, Command.EndCell) > 32)
			{
				return Reject(EPhase3CommandResult::RejectedInvalidTopology,
					TEXT("Walls must be a straight grid-aligned stroke."),
					TEXT("Drag horizontally or vertically for up to 32 meters."));
			}
			break;
		case EPhase3CommandType::PlaceTerminalDoor:
			if (ManhattanLength(Command.StartCell, Command.EndCell) != 1)
			{
				return Reject(EPhase3CommandResult::RejectedInvalidTopology,
					TEXT("A door must occupy one wall edge."),
					TEXT("Snap the door to one highlighted edge."));
			}
			break;
		case EPhase3CommandType::PlaceTerminalObject:
		{
			int32 Width = 1;
			int32 Height = 1;
			int32 Clearance = 0;
			GetObjectFootprint(Command.ObjectKind, Width, Height, Clearance);
			if ((Command.QuarterTurns & 1) != 0)
			{
				Swap(Width, Height);
			}
			for (int32 Y = 0; Y < Height; ++Y)
			{
				for (int32 X = 0; X < Width; ++X)
				{
					if (!ContainsFloor(Layout, {Command.StartCell.X + X, Command.StartCell.Y + Y}))
					{
						return Reject(EPhase3CommandResult::RejectedInvalidTopology,
							TEXT("The object footprint is not fully supported by terminal floor."),
							TEXT("Move it onto completed or planned floor cells."));
					}
				}
			}
			for (const FTerminalPlacedObjectRecord& Existing : Layout.Objects)
			{
				const bool bOverlap =
					Command.StartCell.X < Existing.Anchor.X + Existing.FootprintWidth &&
					Command.StartCell.X + Width > Existing.Anchor.X &&
					Command.StartCell.Y < Existing.Anchor.Y + Existing.FootprintHeight &&
					Command.StartCell.Y + Height > Existing.Anchor.Y;
				if (bOverlap)
				{
					return Reject(EPhase3CommandResult::RejectedOccupied,
						TEXT("The object footprint overlaps existing furnishings."),
						TEXT("Use an open highlighted area."));
				}
			}
			break;
		}
		case EPhase3CommandType::RotateTerminalObject:
			if (!Layout.Objects.ContainsByPredicate(
				[&Command](const FTerminalPlacedObjectRecord& Object)
				{
					return Object.Id == Command.TerminalElementId;
				}))
			{
				return Reject(EPhase3CommandResult::RejectedMissingReference,
					TEXT("The selected terminal object no longer exists."),
					TEXT("Select another object."));
			}
			break;
		case EPhase3CommandType::DemolishTerminalElement:
			if (!ContainsElement(Layout, Command.TerminalElementId))
			{
				return Reject(EPhase3CommandResult::RejectedMissingReference,
					TEXT("The selected terminal element no longer exists."),
					TEXT("Select another element."));
			}
			if (const FTerminalFloorCellRecord* Floor = Layout.FloorCells.FindByPredicate(
				[&Command](const FTerminalFloorCellRecord& Entry)
				{
					return Entry.Id == Command.TerminalElementId;
				}))
			{
				if (Layout.Visitors.ContainsByPredicate(
					[Floor](const FTerminalVisitorRecord& Visitor)
					{
						return Visitor.bActive && Visitor.Cell == Floor->Cell;
					}))
				{
					return Reject(EPhase3CommandResult::RejectedRouteLoss,
						TEXT("An active terminal visitor occupies this cell."),
						TEXT("Wait for the visitor to leave or keep the route open."));
				}
			}
			if (State.bTerminalOpen || Layout.Visitors.ContainsByPredicate(
				[](const FTerminalVisitorRecord& Visitor) { return Visitor.bActive; }))
			{
				FTerminalLayoutState Candidate = Layout;
				Candidate.FloorCells.RemoveAll([&Command](const FTerminalFloorCellRecord& Entry) { return Entry.Id == Command.TerminalElementId; });
				Candidate.Edges.RemoveAll([&Command](const FTerminalEdgeRecord& Entry) { return Entry.Id == Command.TerminalElementId; });
				Candidate.Objects.RemoveAll([&Command](const FTerminalPlacedObjectRecord& Entry) { return Entry.Id == Command.TerminalElementId; });
				const bool bReadyAfterEdit =
					HasBuiltDoor(Candidate, ETerminalEdgeKind::EntranceDoor) &&
					HasConnectedFloorRoute(Candidate) &&
					HasBuiltObject(Candidate, ETerminalObjectKind::InformationDesk) &&
					(HasBuiltObject(Candidate, ETerminalObjectKind::SeatGroup2) ||
						HasBuiltObject(Candidate, ETerminalObjectKind::SeatGroup4) ||
						HasBuiltObject(Candidate, ETerminalObjectKind::SeatGroup6)) &&
					HasBuiltObject(Candidate, ETerminalObjectKind::Restroom) &&
					HasBuiltObject(Candidate, ETerminalObjectKind::StaffDesk) &&
					HasBuiltObject(Candidate, ETerminalObjectKind::Storage) &&
					HasBuiltDoor(Candidate, ETerminalEdgeKind::AirsideGateDoor);
				if (!bReadyAfterEdit)
				{
					return Reject(EPhase3CommandResult::RejectedRouteLoss,
						TEXT("This edit would remove required egress, service, or gate continuity."),
						TEXT("Close operations first or preserve an alternate completed route."));
				}
			}
			break;
		case EPhase3CommandType::UndoTerminalEdit:
			if (!Layout.EditTransactions.ContainsByPredicate(
				[](const FTerminalEditTransactionRecord& Transaction)
				{
					return Transaction.bRefundable && !Transaction.bUndone;
				}))
			{
				return Reject(EPhase3CommandResult::RejectedNothingToUndo,
					TEXT("No queued or active terminal edit can be undone."),
					TEXT("Completed work must be removed with Demolish."));
			}
			break;
		default:
			return Reject(EPhase3CommandResult::RejectedInvalidCommand,
				TEXT("This is not a terminal edit command."), TEXT("Choose a terminal build tool."));
		}

		Result.bValid = true;
		Result.Result = EPhase3CommandResult::Accepted;
		Result.QuotedCredits = QuoteTerminalEdit(Command);
		return true;
	}

	void FPassengerTerminalSimulation::ApplyTerminalEdit(
		const FPhase3Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		FTerminalLayoutState& Layout = State.TerminalLayout;
		if (Command.Type == EPhase3CommandType::UndoTerminalEdit)
		{
			FTerminalEditTransactionRecord* Transaction = nullptr;
			for (int32 Index = Layout.EditTransactions.Num() - 1; Index >= 0; --Index)
			{
				if (Layout.EditTransactions[Index].bRefundable &&
					!Layout.EditTransactions[Index].bUndone)
				{
					Transaction = &Layout.EditTransactions[Index];
					break;
				}
			}
			if (!Transaction)
			{
				return;
			}
			for (FTerminalConstructionJobRecord& Job : Layout.ConstructionJobs)
			{
				if (Job.TransactionId != Transaction->Id ||
					Job.Stage == ETerminalConstructionWorkStage::Completed)
				{
					continue;
				}
				Job.Stage = ETerminalConstructionWorkStage::Cancelled;
				if (Job.WorkKind == ETerminalConstructionWorkKind::DemolishElement)
				{
					for (FTerminalFloorCellRecord& Cell : Layout.FloorCells)
					{
						if (Cell.Id == Job.ElementId) Cell.bLocallyClosed = false;
					}
					for (FTerminalEdgeRecord& Edge : Layout.Edges)
					{
						if (Edge.Id == Job.ElementId) Edge.bLocallyClosed = false;
					}
					for (FTerminalPlacedObjectRecord& Object : Layout.Objects)
					{
						if (Object.Id == Job.ElementId) Object.bLocallyClosed = false;
					}
				}
				else if (Job.WorkKind == ETerminalConstructionWorkKind::RotateObject)
				{
					if (FTerminalPlacedObjectRecord* Object = Layout.Objects.FindByPredicate(
						[&Job](const FTerminalPlacedObjectRecord& Entry) { return Entry.Id == Job.ElementId; }))
					{
						Object->QuarterTurns = Transaction->PreviousQuarterTurns;
						Swap(Object->FootprintWidth, Object->FootprintHeight);
						Object->bLocallyClosed = false;
					}
				}
				else
				{
					Layout.FloorCells.RemoveAll([&Job](const FTerminalFloorCellRecord& Cell) { return Cell.Id == Job.ElementId; });
					Layout.Edges.RemoveAll([&Job](const FTerminalEdgeRecord& Edge) { return Edge.Id == Job.ElementId; });
					Layout.Objects.RemoveAll([&Job](const FTerminalPlacedObjectRecord& Object) { return Object.Id == Job.ElementId; });
				}
			}
			if (Transaction->CostCredits > 0)
			{
				Phase1.ApplyExternalEconomyChange(
					TEXT("TerminalEditRefund"), Transaction->CostCredits, 0,
					TEXT("Full refund for queued or active terminal work."),
					CurrentGameMilliseconds);
			}
			Transaction->bUndone = true;
			Transaction->bRefundable = false;
			++Layout.Revision;
			RefreshTerminalReadiness();
			EmitEvent(EPhase3EventType::TerminalEditUndone, Command.Id,
				Transaction->Id.Value, TEXT("Terminal edit undone and refunded."),
				CurrentGameMilliseconds);
			return;
		}

		const int64 Cost = QuoteTerminalEdit(Command);
		if (Cost > 0 && !Phase1.ApplyExternalEconomyChange(
			TEXT("TerminalEdit"), -Cost, 0,
			TEXT("Ground-floor terminal edit committed."),
			CurrentGameMilliseconds))
		{
			return;
		}

		FTerminalEditTransactionRecord Transaction;
		Transaction.Id = {AllocateDomainId()};
		Transaction.Cause = Command.Id;
		Transaction.CostCredits = Cost;
		Transaction.CreatedAtGameMilliseconds = CurrentGameMilliseconds;
		auto AddJob = [this, &Layout, &Transaction, CurrentGameMilliseconds](
			const FTerminalElementId ElementId,
			const ETerminalConstructionWorkKind WorkKind,
			const FTerminalCellCoord WorkCell)
		{
			Transaction.ElementIds.Add(ElementId);
			FTerminalConstructionJobRecord Job;
			Job.Id = {AllocateDomainId()};
			Job.TransactionId = Transaction.Id;
			Job.ElementId = ElementId;
			Job.WorkKind = WorkKind;
			Job.WorkCell = WorkCell;
			Job.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
			Layout.ConstructionJobs.Add(Job);
		};

		switch (Command.Type)
		{
		case EPhase3CommandType::PlaceTerminalFloor:
		{
			const int32 MinX = FMath::Min(Command.StartCell.X, Command.EndCell.X);
			const int32 MaxX = FMath::Max(Command.StartCell.X, Command.EndCell.X);
			const int32 MinY = FMath::Min(Command.StartCell.Y, Command.EndCell.Y);
			const int32 MaxY = FMath::Max(Command.StartCell.Y, Command.EndCell.Y);
			for (int32 Y = MinY; Y <= MaxY; ++Y)
			{
				for (int32 X = MinX; X <= MaxX; ++X)
				{
					FTerminalFloorCellRecord Cell;
					Cell.Id = {AllocateDomainId()};
					Cell.Cell = {X, Y};
					Cell.Kind = Command.FloorKind;
					Layout.FloorCells.Add(Cell);
					AddJob(Cell.Id, ETerminalConstructionWorkKind::BuildFloor, Cell.Cell);
				}
			}
			Layout.MinimumX = FMath::Min(Layout.MinimumX, MinX);
			Layout.MinimumY = FMath::Min(Layout.MinimumY, MinY);
			Layout.MaximumX = FMath::Max(Layout.MaximumX, MaxX);
			Layout.MaximumY = FMath::Max(Layout.MaximumY, MaxY);
			break;
		}
		case EPhase3CommandType::PlaceTerminalWall:
		{
			const int32 StepX = FMath::Sign(Command.EndCell.X - Command.StartCell.X);
			const int32 StepY = FMath::Sign(Command.EndCell.Y - Command.StartCell.Y);
			FTerminalCellCoord From = Command.StartCell;
			while (From != Command.EndCell)
			{
				const FTerminalCellCoord To = {From.X + StepX, From.Y + StepY};
				FTerminalEdgeRecord Edge;
				Edge.Id = {AllocateDomainId()};
				Edge.From = From;
				Edge.To = To;
				Edge.Kind = Command.EdgeKind;
				Layout.Edges.Add(Edge);
				AddJob(Edge.Id, ETerminalConstructionWorkKind::BuildEdge, From);
				From = To;
			}
			break;
		}
		case EPhase3CommandType::PlaceTerminalDoor:
		{
			FTerminalEdgeRecord Edge;
			Edge.Id = {AllocateDomainId()};
			Edge.From = Command.StartCell;
			Edge.To = Command.EndCell;
			Edge.Kind = Command.EdgeKind;
			Layout.Edges.Add(Edge);
			AddJob(Edge.Id, ETerminalConstructionWorkKind::BuildEdge, Edge.From);
			break;
		}
		case EPhase3CommandType::PlaceTerminalObject:
		{
			FTerminalPlacedObjectRecord Object;
			Object.Id = {AllocateDomainId()};
			Object.DefinitionId = ObjectDefinition(Command.ObjectKind);
			Object.Kind = Command.ObjectKind;
			Object.Anchor = Command.StartCell;
			Object.QuarterTurns = (Command.QuarterTurns % 4 + 4) % 4;
			GetObjectFootprint(Object.Kind, Object.FootprintWidth, Object.FootprintHeight, Object.ClearanceCells);
			if ((Object.QuarterTurns & 1) != 0)
			{
				Swap(Object.FootprintWidth, Object.FootprintHeight);
			}
			Object.InteractionPorts.Add({Object.Anchor.X, Object.Anchor.Y + Object.FootprintHeight});
			Layout.Objects.Add(Object);
			AddJob(Object.Id, ETerminalConstructionWorkKind::PlaceObject, Object.Anchor);
			break;
		}
		case EPhase3CommandType::RotateTerminalObject:
			if (FTerminalPlacedObjectRecord* Object = Layout.Objects.FindByPredicate(
				[&Command](const FTerminalPlacedObjectRecord& Entry) { return Entry.Id == Command.TerminalElementId; }))
			{
				Transaction.PreviousQuarterTurns = Object->QuarterTurns;
				Object->QuarterTurns = (Object->QuarterTurns + 1) % 4;
				Swap(Object->FootprintWidth, Object->FootprintHeight);
				Object->bLocallyClosed = true;
				AddJob(Object->Id, ETerminalConstructionWorkKind::RotateObject, Object->Anchor);
			}
			break;
		case EPhase3CommandType::DemolishTerminalElement:
		{
			Transaction.SalvageCredits = ElementSalvageCredits(
				Layout, Command.TerminalElementId);
			FTerminalCellCoord WorkCell;
			for (FTerminalFloorCellRecord& Cell : Layout.FloorCells)
			{
				if (Cell.Id == Command.TerminalElementId) { Cell.bLocallyClosed = true; WorkCell = Cell.Cell; }
			}
			for (FTerminalEdgeRecord& Edge : Layout.Edges)
			{
				if (Edge.Id == Command.TerminalElementId) { Edge.bLocallyClosed = true; WorkCell = Edge.From; }
			}
			for (FTerminalPlacedObjectRecord& Object : Layout.Objects)
			{
				if (Object.Id == Command.TerminalElementId) { Object.bLocallyClosed = true; WorkCell = Object.Anchor; }
			}
			AddJob(Command.TerminalElementId,
				ETerminalConstructionWorkKind::DemolishElement, WorkCell);
			break;
		}
		default: break;
		}

		Layout.EditTransactions.Add(Transaction);
		++Layout.Revision;
		RefreshTerminalReadiness();
		EmitEvent(EPhase3EventType::TerminalEditCommitted, Command.Id,
			Transaction.Id.Value, TEXT("Terminal edit committed; construction workers dispatched."),
			CurrentGameMilliseconds);
	}

	void FPassengerTerminalSimulation::AdvanceTerminalConstruction(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		const FPhase1State& Phase1State = Phase1.GetState();
		const FStaffTeamRecord* ConstructionTeam = Phase1State.Teams.FindByPredicate(
			[](const FStaffTeamRecord& Team)
			{
				return Team.RoleId == TEXT("Staff.Role.Construction") && Team.bAvailable;
			});
		if (!ConstructionTeam)
		{
			return;
		}
		FTerminalLayoutState& Layout = State.TerminalLayout;
		int32 ActiveCount = 0;
		for (FTerminalConstructionJobRecord& Job : Layout.ConstructionJobs)
		{
			if (Job.Stage == ETerminalConstructionWorkStage::Completed ||
				Job.Stage == ETerminalConstructionWorkStage::Cancelled)
			{
				continue;
			}
			if (ActiveCount >= ConstructionTeam->TeamSize)
			{
				break;
			}
			++ActiveCount;
			Job.AssignedTeamId = ConstructionTeam->Id;
			const int64 Elapsed = CurrentGameMilliseconds - Job.StageChangedAtGameMilliseconds;
			Job.ProgressPercent = FMath::Clamp(
				static_cast<int32>(Elapsed * 100 / TerminalWorkStageMilliseconds), 0, 99);
			if (Elapsed < TerminalWorkStageMilliseconds)
			{
				continue;
			}
			Job.StageChangedAtGameMilliseconds = CurrentGameMilliseconds;
			Job.ProgressPercent = 0;
			Job.Stage = static_cast<ETerminalConstructionWorkStage>(
				static_cast<uint8>(Job.Stage) + 1);
			if (Job.Stage != ETerminalConstructionWorkStage::Completed)
			{
				continue;
			}
			if (Job.WorkKind == ETerminalConstructionWorkKind::DemolishElement)
			{
				Layout.FloorCells.RemoveAll([&Job](const FTerminalFloorCellRecord& Cell) { return Cell.Id == Job.ElementId; });
				Layout.Edges.RemoveAll([&Job](const FTerminalEdgeRecord& Edge) { return Edge.Id == Job.ElementId; });
				Layout.Objects.RemoveAll([&Job](const FTerminalPlacedObjectRecord& Object) { return Object.Id == Job.ElementId; });
			}
			else
			{
				for (FTerminalFloorCellRecord& Cell : Layout.FloorCells)
				{
					if (Cell.Id == Job.ElementId) Cell.bBuilt = true;
				}
				for (FTerminalEdgeRecord& Edge : Layout.Edges)
				{
					if (Edge.Id == Job.ElementId) Edge.bBuilt = true;
				}
				for (FTerminalPlacedObjectRecord& Object : Layout.Objects)
				{
					if (Object.Id == Job.ElementId)
					{
						Object.bBuilt = true;
						Object.bOperational = true;
						Object.bLocallyClosed = false;
					}
				}
			}
			if (FTerminalEditTransactionRecord* Transaction = Layout.EditTransactions.FindByPredicate(
				[&Job](const FTerminalEditTransactionRecord& Entry) { return Entry.Id == Job.TransactionId; }))
			{
				const bool bAllComplete = Layout.ConstructionJobs.FilterByPredicate(
					[Transaction](const FTerminalConstructionJobRecord& Entry)
					{
						return Entry.TransactionId == Transaction->Id;
					}).ContainsByPredicate([](const FTerminalConstructionJobRecord& Entry)
					{
						return Entry.Stage != ETerminalConstructionWorkStage::Completed &&
							Entry.Stage != ETerminalConstructionWorkStage::Cancelled;
					}) == false;
				if (bAllComplete && !Transaction->bCompleted)
				{
					if (Transaction->SalvageCredits > 0)
					{
						Phase1.ApplyExternalEconomyChange(
							TEXT("TerminalDemolitionSalvage"),
							Transaction->SalvageCredits,
							0,
							TEXT("Twenty-five percent salvage from completed terminal demolition."),
							CurrentGameMilliseconds);
					}
					Transaction->bCompleted = true;
					Transaction->bRefundable = false;
				}
			}
			++Layout.Revision;
			RefreshTerminalReadiness();
			EmitEvent(EPhase3EventType::TerminalConstructionStageChanged, {},
				Job.Id.Value, TEXT("Terminal work passed inspection and became operational."),
				CurrentGameMilliseconds);
		}
	}

	void FPassengerTerminalSimulation::AdvanceGATerminalVisitors(
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State)
	{
		FTerminalLayoutState& Layout = State.TerminalLayout;
		if (!Layout.bReady)
		{
			return;
		}
		const FFlightRecord& Flight = Phase1State.Flight;
		const bool bVisitAtTerminal = Flight.Id.IsValid() &&
			Flight.State >= EFlightState::Parked &&
			Flight.State <= EFlightState::Ready;
		if (!bVisitAtTerminal)
		{
			bool bChanged = false;
			for (FTerminalVisitorRecord& Visitor : Layout.Visitors)
			{
				if (Visitor.bActive)
				{
					Visitor.bActive = false;
					Visitor.Activity = ETerminalVisitorActivity::Completed;
					bChanged = true;
				}
			}
			if (bChanged) ++Layout.Revision;
			return;
		}

		const bool bHasThisVisit = Layout.Visitors.ContainsByPredicate(
			[&Flight](const FTerminalVisitorRecord& Visitor)
			{
				return Visitor.FlightId == Flight.Id;
			});
		if (!bHasThisVisit)
		{
			for (int32 Index = 0; Index < 4; ++Index)
			{
				FTerminalVisitorRecord Visitor;
				Visitor.Id = {AllocateDomainId()};
				Visitor.FlightId = Flight.Id;
				Visitor.Activity = ETerminalVisitorActivity::Entering;
				Visitor.Cell = {7 + Index, 1};
				Visitor.ActivityChangedAtGameMilliseconds = CurrentGameMilliseconds;
				Visitor.bActive = true;
				Visitor.bRequiresAccessibleRoute = Index == 0;
				Layout.Visitors.Add(Visitor);
			}
			++Layout.Revision;
			return;
		}

		bool bChanged = false;
		for (int32 Index = 0; Index < Layout.Visitors.Num(); ++Index)
		{
			FTerminalVisitorRecord& Visitor = Layout.Visitors[Index];
			if (!Visitor.bActive || Visitor.FlightId != Flight.Id ||
				CurrentGameMilliseconds - Visitor.ActivityChangedAtGameMilliseconds < 15000)
			{
				continue;
			}
			Visitor.ActivityChangedAtGameMilliseconds = CurrentGameMilliseconds;
			switch (Visitor.Activity)
			{
			case ETerminalVisitorActivity::Entering:
				Visitor.Activity = ETerminalVisitorActivity::Information;
				Visitor.Cell = {7 + Index % 2, 3};
				break;
			case ETerminalVisitorActivity::Information:
				Visitor.Activity = Index == 1
					? ETerminalVisitorActivity::Restroom
					: ETerminalVisitorActivity::Waiting;
				Visitor.Cell = Index == 1
					? FTerminalCellCoord{2, 9}
					: FTerminalCellCoord{4 + Index, 6};
				break;
			case ETerminalVisitorActivity::Restroom:
			case ETerminalVisitorActivity::Waiting:
				Visitor.Activity = ETerminalVisitorActivity::AirsideGate;
				Visitor.Cell = {5 + (Index % 2) * 7, 10};
				break;
			case ETerminalVisitorActivity::AirsideGate:
				if (Flight.State >= EFlightState::Ready)
				{
					Visitor.Activity = ETerminalVisitorActivity::Completed;
					Visitor.bActive = false;
				}
				break;
			default: break;
			}
			bChanged = true;
		}
		if (bChanged) ++Layout.Revision;
	}

	void FPassengerTerminalSimulation::RefreshTerminalReadiness()
	{
		FTerminalLayoutState& Layout = State.TerminalLayout;
		Layout.bEntranceConnected = HasBuiltDoor(Layout, ETerminalEdgeKind::EntranceDoor) &&
			HasConnectedFloorRoute(Layout);
		Layout.bInformationPointAvailable = HasBuiltObject(Layout, ETerminalObjectKind::InformationDesk);
		Layout.bSeatingAvailable = HasBuiltObject(Layout, ETerminalObjectKind::SeatGroup2) ||
			HasBuiltObject(Layout, ETerminalObjectKind::SeatGroup4) ||
			HasBuiltObject(Layout, ETerminalObjectKind::SeatGroup6);
		Layout.bAccessibleRestroomRoute = HasBuiltObject(Layout, ETerminalObjectKind::Restroom) &&
			Layout.bEntranceConnected;
		Layout.bStaffSpaceAvailable = HasBuiltObject(Layout, ETerminalObjectKind::StaffDesk) &&
			HasBuiltObject(Layout, ETerminalObjectKind::Storage);
		Layout.ValidAirsideGateCount = 0;
		for (const FTerminalEdgeRecord& Edge : Layout.Edges)
		{
			if (Edge.Kind == ETerminalEdgeKind::AirsideGateDoor &&
				Edge.bBuilt && !Edge.bLocallyClosed)
			{
				++Layout.ValidAirsideGateCount;
			}
		}
		Layout.bReady = Layout.bEntranceConnected &&
			Layout.bInformationPointAvailable && Layout.bSeatingAvailable &&
			Layout.bAccessibleRestroomRoute && Layout.bStaffSpaceAvailable &&
			Layout.ValidAirsideGateCount > 0;
	}

	bool FPassengerTerminalSimulation::ValidateTerminalLayout(
		const FTerminalLayoutState& Candidate,
		FString& Failure) const
	{
		if (!Candidate.bSeeded || Candidate.FloorCells.Num() > 65536 ||
			Candidate.Edges.Num() > 65536 || Candidate.Objects.Num() > 16384 ||
			Candidate.ConstructionJobs.Num() > 65536 ||
			Candidate.EditTransactions.Num() > 65536 ||
			Candidate.Visitors.Num() > 1024)
		{
			Failure = TEXT("Terminal layout collection bounds are invalid.");
			return false;
		}
		TSet<uint64> ElementIds;
		for (const FTerminalFloorCellRecord& Cell : Candidate.FloorCells)
		{
			if (!Cell.Id.IsValid() || ElementIds.Contains(Cell.Id.Value) ||
				!IsInsideCoordinateLimit(Cell.Cell))
			{
				Failure = TEXT("Terminal floor identity or coordinate is invalid.");
				return false;
			}
			ElementIds.Add(Cell.Id.Value);
		}
		for (const FTerminalEdgeRecord& Edge : Candidate.Edges)
		{
			if (!Edge.Id.IsValid() || ElementIds.Contains(Edge.Id.Value) ||
				ManhattanLength(Edge.From, Edge.To) != 1)
			{
				Failure = TEXT("Terminal wall or door edge is invalid.");
				return false;
			}
			ElementIds.Add(Edge.Id.Value);
		}
		for (const FTerminalPlacedObjectRecord& Object : Candidate.Objects)
		{
			if (!Object.Id.IsValid() || ElementIds.Contains(Object.Id.Value) ||
				Object.DefinitionId.IsNone() || Object.FootprintWidth <= 0 ||
				Object.FootprintHeight <= 0 || Object.QuarterTurns < 0 ||
				Object.QuarterTurns > 3)
			{
				Failure = TEXT("Terminal placed-object record is invalid.");
				return false;
			}
			ElementIds.Add(Object.Id.Value);
		}
		TSet<uint64> JobIds;
		for (const FTerminalConstructionJobRecord& Job : Candidate.ConstructionJobs)
		{
			const bool bElementMayBeGone =
				Job.Stage == ETerminalConstructionWorkStage::Cancelled ||
				(Job.Stage == ETerminalConstructionWorkStage::Completed &&
					(Job.WorkKind == ETerminalConstructionWorkKind::DemolishElement ||
						Candidate.ConstructionJobs.ContainsByPredicate(
							[&Job](const FTerminalConstructionJobRecord& Other)
							{
								return Other.ElementId == Job.ElementId &&
									Other.WorkKind == ETerminalConstructionWorkKind::DemolishElement &&
									Other.Stage == ETerminalConstructionWorkStage::Completed;
							})));
			if (!Job.Id.IsValid() || JobIds.Contains(Job.Id.Value) ||
				(!ElementIds.Contains(Job.ElementId.Value) && !bElementMayBeGone) ||
				!Job.TransactionId.IsValid() || Job.ProgressPercent < 0 ||
				Job.ProgressPercent > 100)
			{
				Failure = TEXT("Terminal construction job reference is invalid.");
				return false;
			}
			JobIds.Add(Job.Id.Value);
		}
		for (const FTerminalEditTransactionRecord& Transaction : Candidate.EditTransactions)
		{
			if (!Transaction.Id.IsValid() || Transaction.CostCredits < 0 ||
				Transaction.SalvageCredits < 0)
			{
				Failure = TEXT("Terminal edit transaction is invalid.");
				return false;
			}
		}
		return true;
	}
}
