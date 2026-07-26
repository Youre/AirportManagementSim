#pragma once

#include "AMSimSimulationTypes.h"

namespace AMSim
{
	inline constexpr int32 PlanningCellCentimeters = 100;
	inline constexpr int32 ChunkSizeCentimeters = 6400;

	struct FIntPoint64
	{
		int64 X = 0;
		int64 Y = 0;
		auto operator<=>(const FIntPoint64&) const = default;
	};

	struct FChunkAddress
	{
		int32 X = 0;
		int32 Y = 0;
		auto operator<=>(const FChunkAddress&) const = default;
	};

	FORCEINLINE uint32 GetTypeHash(const FChunkAddress Address)
	{
		return HashCombine(::GetTypeHash(Address.X), ::GetTypeHash(Address.Y));
	}

	struct FReservationRequest
	{
		FEntityId Owner;
		int32 Priority = 0;
		uint64 RequestSequence = 0;
		int64 StartStep = 0;
		int64 EndStep = 0;
	};

	enum class ESpatialLayer : uint8
	{
		LandOwnership,
		TerrainBuildable,
		StructureOccupancy,
		Walkability,
		SecurityZone,
		MovementPavement,
		ServiceRoad,
		PublicRoad,
		Baggage,
		WorkZone,
		SafetyClearance,
		TemporaryClosure
	};

	class AMSIMSIMULATION_API FSpatialGrid
	{
	public:
		bool SetOccupied(FIntPoint64 PositionCentimeters, ESpatialLayer Layer, bool bOccupied);
		bool IsOccupied(FIntPoint64 PositionCentimeters, ESpatialLayer Layer) const;
		bool IsChunkAllocated(FChunkAddress Address) const;
		uint64 GetChunkRevision(FChunkAddress Address) const;
		int32 GetAllocatedChunkCount() const { return Chunks.Num(); }

	private:
		struct FChunk
		{
			uint64 Revision = 0;
			TMap<ESpatialLayer, TBitArray<>> Layers;
		};

		TMap<FChunkAddress, FChunk> Chunks;
	};

	AMSIMSIMULATION_API FChunkAddress GetChunkAddress(FIntPoint64 PositionCentimeters);
	AMSIMSIMULATION_API int32 GetPlanningCellIndex(FIntPoint64 PositionCentimeters);
	AMSIMSIMULATION_API TArray<FReservationRequest> ResolveReservationOrder(TArray<FReservationRequest> Requests);
}
