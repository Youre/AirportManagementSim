#pragma once

#include "CoreMinimal.h"

namespace AMSim
{
	enum class EProvenanceStatus : uint8
	{
		Unknown,
		InternalPrototype,
		Approved
	};

	struct FDefinition
	{
		FName StableId;
		uint32 SchemaVersion = 1;
		FName OwningPhase;
		TArray<FName> Tags;
		TArray<FName> Dependencies;
		EProvenanceStatus Provenance = EProvenanceStatus::Unknown;
		FString SourcePath;
		FString SourceChecksum;
	};

	struct FDefinitionValidation
	{
		bool bValid = false;
		TArray<FString> Errors;
	};

	AMSIMSIMULATION_API FDefinitionValidation ValidateDefinition(const FDefinition& Definition, bool bRequiresProvenance);
	AMSIMSIMULATION_API FDefinitionValidation ValidateDefinitionRegistry(
		const TArray<FDefinition>& Definitions,
		bool bRequiresProvenance);
	AMSIMSIMULATION_API bool IsKnownGameplayTagRoot(FName Tag);
}
