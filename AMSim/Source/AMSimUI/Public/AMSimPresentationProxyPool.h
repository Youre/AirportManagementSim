#pragma once

#include "AMSimSimulationTypes.h"

namespace AMSim
{
	struct FPresentationProxyHandle
	{
		int32 Index = INDEX_NONE;
		uint32 Generation = 0;

		bool IsValid() const { return Index != INDEX_NONE; }
		auto operator<=>(const FPresentationProxyHandle&) const = default;
	};

	class AMSIMUI_API FPresentationProxyPool
	{
	public:
		FPresentationProxyHandle Acquire(FEntityId Entity);
		bool Release(FEntityId Entity);
		TOptional<FPresentationProxyHandle> Find(FEntityId Entity) const;
		int32 GetActiveCount() const { return Active.Num(); }

	private:
		struct FSlot
		{
			uint32 Generation = 0;
			bool bInUse = false;
		};

		TArray<FSlot> Slots;
		TArray<int32> FreeIndices;
		TMap<FEntityId, FPresentationProxyHandle> Active;
	};
}
