#pragma once

#include "CoreMinimal.h"

namespace AMSim
{
	inline constexpr int64 FixedStepMilliseconds = 250;
	inline constexpr uint32 SnapshotSchemaVersion = 5;
	inline constexpr uint32 MinimumSupportedSnapshotSchemaVersion = 1;

	struct FEntityId
	{
		uint64 Value = 0;

		bool IsValid() const { return Value != 0; }
		auto operator<=>(const FEntityId&) const = default;
	};

	struct FCommandId
	{
		uint64 Value = 0;

		bool IsValid() const { return Value != 0; }
		auto operator<=>(const FCommandId&) const = default;
	};

	enum class ECommandType : uint8
	{
		NoOp,
		CreateEntity,
		DestroyEntity
	};

	enum class ECommandResult : uint8
	{
		Accepted,
		RejectedInvalidId,
		RejectedEntityMissing
	};

	struct FCommand
	{
		FCommandId Id;
		ECommandType Type = ECommandType::NoOp;
		int64 RequestedGameTimeMilliseconds = 0;
		FEntityId Target;
	};

	struct FEvent
	{
		uint64 Sequence = 0;
		int64 GameTimeMilliseconds = 0;
		ECommandType Type = ECommandType::NoOp;
		FCommandId Cause;
		FEntityId Entity;
	};

	struct FQuerySnapshot
	{
		uint64 Revision = 0;
		int64 GameTimeMilliseconds = 0;
		TArray<FEntityId> Entities;
		uint64 StateChecksum = 0;
	};

	struct FSimulationDiagnostics
	{
		uint64 StepIndex = 0;
		int64 GameTimeMilliseconds = 0;
		int32 EntityCount = 0;
		int32 PendingCommandCount = 0;
		int32 EventCount = 0;
		uint64 StateChecksum = 0;
	};

	FORCEINLINE uint32 GetTypeHash(const FEntityId Id)
	{
		return HashCombine(::GetTypeHash(Id.Value), 0xA17F11D5u);
	}
}
