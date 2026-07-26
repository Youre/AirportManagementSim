#include "AMSimPresentationProxyPool.h"

namespace AMSim
{
	FPresentationProxyHandle FPresentationProxyPool::Acquire(const FEntityId Entity)
	{
		check(Entity.IsValid());
		if (const FPresentationProxyHandle* Existing = Active.Find(Entity))
		{
			return *Existing;
		}

		const int32 Index = FreeIndices.IsEmpty() ? Slots.AddDefaulted() : FreeIndices.Pop(EAllowShrinking::No);
		FSlot& Slot = Slots[Index];
		Slot.bInUse = true;
		++Slot.Generation;
		const FPresentationProxyHandle Handle{Index, Slot.Generation};
		Active.Add(Entity, Handle);
		return Handle;
	}

	bool FPresentationProxyPool::Release(const FEntityId Entity)
	{
		FPresentationProxyHandle Handle;
		if (!Active.RemoveAndCopyValue(Entity, Handle))
		{
			return false;
		}
		Slots[Handle.Index].bInUse = false;
		FreeIndices.Add(Handle.Index);
		return true;
	}

	TOptional<FPresentationProxyHandle> FPresentationProxyPool::Find(const FEntityId Entity) const
	{
		if (const FPresentationProxyHandle* Handle = Active.Find(Entity))
		{
			return *Handle;
		}
		return {};
	}
}
