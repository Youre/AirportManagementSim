#include "AMSimDefinitions.h"

namespace AMSim
{
	namespace
	{
		const TArray<FString>& GetKnownTagRoots()
		{
			static const TArray<FString> Roots{
				TEXT("Aircraft.Role"),
				TEXT("Aircraft.Size"),
				TEXT("Facility.Type"),
				TEXT("Network.Type"),
				TEXT("Service.Type"),
				TEXT("Staff.Role"),
				TEXT("Zone.Type"),
				TEXT("Cargo.Class"),
				TEXT("Weather.Hazard"),
				TEXT("Capability.Path"),
				TEXT("Event.Type")
			};
			return Roots;
		}

		bool VisitDefinition(
			const FName Id,
			const TMap<FName, const FDefinition*>& ById,
			TSet<FName>& Visiting,
			TSet<FName>& Visited)
		{
			if (Visited.Contains(Id))
			{
				return true;
			}
			if (Visiting.Contains(Id))
			{
				return false;
			}
			Visiting.Add(Id);
			const FDefinition* const* Definition = ById.Find(Id);
			if (Definition)
			{
				for (const FName Dependency : (*Definition)->Dependencies)
				{
					if (!VisitDefinition(Dependency, ById, Visiting, Visited))
					{
						return false;
					}
				}
			}
			Visiting.Remove(Id);
			Visited.Add(Id);
			return true;
		}
	}

	bool IsKnownGameplayTagRoot(const FName Tag)
	{
		const FString Value = Tag.ToString();
		for (const FString& Root : GetKnownTagRoots())
		{
			if (Value == Root || Value.StartsWith(Root + TEXT(".")))
			{
				return true;
			}
		}
		return false;
	}

	FDefinitionValidation ValidateDefinition(const FDefinition& Definition, const bool bRequiresProvenance)
	{
		FDefinitionValidation Result;
		if (Definition.StableId.IsNone())
		{
			Result.Errors.Add(TEXT("StableId is required."));
		}
		if (Definition.SchemaVersion == 0)
		{
			Result.Errors.Add(TEXT("SchemaVersion must be positive."));
		}
		if (Definition.OwningPhase.IsNone())
		{
			Result.Errors.Add(TEXT("OwningPhase is required."));
		}
		for (const FName Tag : Definition.Tags)
		{
			if (!IsKnownGameplayTagRoot(Tag))
			{
				Result.Errors.Add(FString::Printf(TEXT("Unknown gameplay tag root: %s."), *Tag.ToString()));
			}
		}
		if (bRequiresProvenance)
		{
			if (Definition.Provenance == EProvenanceStatus::Unknown)
			{
				Result.Errors.Add(TEXT("Provenance decision is required."));
			}
			if (Definition.SourcePath.IsEmpty() || Definition.SourceChecksum.IsEmpty())
			{
				Result.Errors.Add(TEXT("Source path and checksum are required."));
			}
		}
		Result.bValid = Result.Errors.IsEmpty();
		return Result;
	}

	FDefinitionValidation ValidateDefinitionRegistry(
		const TArray<FDefinition>& Definitions,
		const bool bRequiresProvenance)
	{
		FDefinitionValidation Result;
		TMap<FName, const FDefinition*> ById;
		for (const FDefinition& Definition : Definitions)
		{
			const FDefinitionValidation DefinitionResult = ValidateDefinition(Definition, bRequiresProvenance);
			Result.Errors.Append(DefinitionResult.Errors);
			if (ById.Contains(Definition.StableId))
			{
				Result.Errors.Add(FString::Printf(TEXT("Duplicate stable ID: %s."), *Definition.StableId.ToString()));
			}
			else if (!Definition.StableId.IsNone())
			{
				ById.Add(Definition.StableId, &Definition);
			}
		}

		for (const FDefinition& Definition : Definitions)
		{
			for (const FName Dependency : Definition.Dependencies)
			{
				if (!ById.Contains(Dependency))
				{
					Result.Errors.Add(FString::Printf(
						TEXT("%s references missing definition %s."),
						*Definition.StableId.ToString(),
						*Dependency.ToString()));
				}
			}
		}

		TSet<FName> Visiting;
		TSet<FName> Visited;
		for (const TPair<FName, const FDefinition*>& Pair : ById)
		{
			if (!VisitDefinition(Pair.Key, ById, Visiting, Visited))
			{
				Result.Errors.Add(TEXT("Definition dependency cycle detected."));
				break;
			}
		}
		Result.bValid = Result.Errors.IsEmpty();
		return Result;
	}
}
