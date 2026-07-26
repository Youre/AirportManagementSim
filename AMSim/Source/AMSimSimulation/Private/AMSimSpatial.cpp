#include "AMSimSpatial.h"

namespace AMSim
{
	namespace
	{
		int64 FloorDivide(const int64 Value, const int64 Divisor)
		{
			const int64 Quotient = Value / Divisor;
			const int64 Remainder = Value % Divisor;
			return Remainder < 0 ? Quotient - 1 : Quotient;
		}
	}

	FChunkAddress GetChunkAddress(const FIntPoint64 PositionCentimeters)
	{
		return {
			static_cast<int32>(FloorDivide(PositionCentimeters.X, ChunkSizeCentimeters)),
			static_cast<int32>(FloorDivide(PositionCentimeters.Y, ChunkSizeCentimeters))
		};
	}

	int32 GetPlanningCellIndex(const FIntPoint64 PositionCentimeters)
	{
		const FChunkAddress Chunk = GetChunkAddress(PositionCentimeters);
		const int64 LocalX = PositionCentimeters.X - static_cast<int64>(Chunk.X) * ChunkSizeCentimeters;
		const int64 LocalY = PositionCentimeters.Y - static_cast<int64>(Chunk.Y) * ChunkSizeCentimeters;
		const int32 CellX = static_cast<int32>(LocalX / PlanningCellCentimeters);
		const int32 CellY = static_cast<int32>(LocalY / PlanningCellCentimeters);
		return CellY * 64 + CellX;
	}

	bool FSpatialGrid::SetOccupied(
		const FIntPoint64 PositionCentimeters,
		const ESpatialLayer Layer,
		const bool bOccupied)
	{
		const FChunkAddress Address = GetChunkAddress(PositionCentimeters);
		FChunk* Chunk = Chunks.Find(Address);
		if (!Chunk && !bOccupied)
		{
			return false;
		}
		if (!Chunk)
		{
			Chunk = &Chunks.Add(Address);
		}

		TBitArray<>& Occupancy = Chunk->Layers.FindOrAdd(Layer);
		if (Occupancy.Num() == 0)
		{
			Occupancy.Init(false, 64 * 64);
		}
		const int32 CellIndex = GetPlanningCellIndex(PositionCentimeters);
		if (Occupancy[CellIndex] == bOccupied)
		{
			return false;
		}
		Occupancy[CellIndex] = bOccupied;
		++Chunk->Revision;
		return true;
	}

	bool FSpatialGrid::IsOccupied(
		const FIntPoint64 PositionCentimeters,
		const ESpatialLayer Layer) const
	{
		const FChunk* Chunk = Chunks.Find(GetChunkAddress(PositionCentimeters));
		if (!Chunk)
		{
			return false;
		}
		const TBitArray<>* Occupancy = Chunk->Layers.Find(Layer);
		return Occupancy && (*Occupancy)[GetPlanningCellIndex(PositionCentimeters)];
	}

	bool FSpatialGrid::IsChunkAllocated(const FChunkAddress Address) const
	{
		return Chunks.Contains(Address);
	}

	uint64 FSpatialGrid::GetChunkRevision(const FChunkAddress Address) const
	{
		const FChunk* Chunk = Chunks.Find(Address);
		return Chunk ? Chunk->Revision : 0;
	}

	TArray<FReservationRequest> ResolveReservationOrder(TArray<FReservationRequest> Requests)
	{
		Requests.Sort([](const FReservationRequest& Left, const FReservationRequest& Right)
		{
			if (Left.Priority != Right.Priority)
			{
				return Left.Priority > Right.Priority;
			}
			if (Left.RequestSequence != Right.RequestSequence)
			{
				return Left.RequestSequence < Right.RequestSequence;
			}
			return Left.Owner.Value < Right.Owner.Value;
		});
		return Requests;
	}
}
