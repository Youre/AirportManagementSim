#include "AMSimPhase1Content.h"
#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType BaseType(TEXT("AMSimPhase1"));
	const FPrimaryAssetType MapType(TEXT("AMSimMap"));
	const FPrimaryAssetType FacilityPrimaryAssetType(TEXT("AMSimFacility"));
	const FPrimaryAssetType AircraftType(TEXT("AMSimAircraft"));
	const FPrimaryAssetType OperatorType(TEXT("AMSimOperator"));
}

FPrimaryAssetId UAMSimPhase1Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(GetDefinitionType(), StableContentId);
}

FPrimaryAssetType UAMSimPhase1Definition::GetDefinitionType() const
{
	return BaseType;
}

FPrimaryAssetType UAMSimMapDefinition::GetDefinitionType() const
{
	return MapType;
}

FPrimaryAssetType UAMSimFacilityDefinition::GetDefinitionType() const
{
	return FacilityPrimaryAssetType;
}

FPrimaryAssetType UAMSimAircraftDefinition::GetDefinitionType() const
{
	return AircraftType;
}

FPrimaryAssetType UAMSimOperatorDefinition::GetDefinitionType() const
{
	return OperatorType;
}

const TArray<FName>& FAMSimPhase1ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Map.TemperateStarter"),
		TEXT("Aircraft.LightPiston.Starter"),
		TEXT("Operator.RiverbendFlyingClub"),
		TEXT("Facility.GrassRunway.Starter"),
		TEXT("Facility.GrassTaxiway.Starter"),
		TEXT("Facility.GAStand.Starter"),
		TEXT("Facility.AccessConnection.Starter"),
		TEXT("Facility.OperationsHut.Starter"),
		TEXT("Facility.MarkingsWindsock.Starter"),
		TEXT("Balance.Phase1.Starter"),
		TEXT("Recipe.StarterAirfield"),
		TEXT("StaffRole.Construction"),
		TEXT("StaffRole.Ramp"),
		TEXT("StaffRole.Fueling"),
		TEXT("Offer.FirstFlight"),
		TEXT("Service.Inspection"),
		TEXT("Service.Fueling"),
		TEXT("Objective.FirstFlight"),
		TEXT("Weather.TemperateClear"),
		TEXT("Presentation.GrassAirfield")
	};
	return Required;
}

FAMSimPhase1CatalogValidation FAMSimPhase1ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase1CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	const TArray<FPrimaryAssetType> Types = {
		BaseType,
		MapType,
		FacilityPrimaryAssetType,
		AircraftType,
		OperatorType
	};
	TSet<FName> FoundIds;
	for (const FPrimaryAssetType Type : Types)
	{
		TArray<FPrimaryAssetId> TypeAssets;
		AssetManager.GetPrimaryAssetIdList(Type, TypeAssets);
		for (const FPrimaryAssetId& AssetId : TypeAssets)
		{
			Result.Assets.Add(AssetId);
			FoundIds.Add(AssetId.PrimaryAssetName);
			const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
			const UAMSimPhase1Definition* Definition =
				Cast<UAMSimPhase1Definition>(Path.TryLoad());
			if (!Definition)
			{
				Result.Errors.Add(FString::Printf(
					TEXT("Content %s did not resolve to a Phase 1 definition."),
					*AssetId.ToString()));
				continue;
			}
			if (Definition->StableContentId != AssetId.PrimaryAssetName ||
				Definition->LocalizationKey.IsEmpty() ||
				Definition->OwningPhase != TEXT("Phase1") ||
				Definition->Provenance.IsNone())
			{
				Result.Errors.Add(FString::Printf(
					TEXT("Content %s has invalid identity, text, phase, or provenance."),
					*AssetId.ToString()));
			}
			if (const UAMSimAircraftDefinition* Aircraft =
				Cast<UAMSimAircraftDefinition>(Definition))
			{
				if (Aircraft->Provenance != TEXT("InternalApproved") ||
					Aircraft->LengthCentimeters != 830 ||
					Aircraft->WingspanCentimeters != 1100 ||
					Aircraft->MinimumRunwayMeters != 600 ||
					Aircraft->MaximumOccupants != 4 ||
					!Aircraft->bGrassRunwayCompatible ||
					!Aircraft->bRequiresInspection ||
					!Aircraft->bRequiresFuel ||
					Aircraft->HeadingDirectionCount < 16 ||
					!Aircraft->bSmoothRotationApproved ||
					!Aircraft->bFictionalLiveryApproved ||
					Aircraft->AccuracyReviewId !=
						TEXT("CT02.Phase1.RiverbendTrainer.2026-07-26") ||
					Aircraft->RegistrationFormat != TEXT("RB-### (fictional)"))
				{
					Result.Errors.Add(FString::Printf(
						TEXT("Aircraft content %s does not match the approved Phase 1 content lock."),
						*AssetId.ToString()));
				}
			}
			if (const UAMSimOperatorDefinition* Operator =
				Cast<UAMSimOperatorDefinition>(Definition))
			{
				if (Operator->Provenance != TEXT("InternalApproved") ||
					Operator->CallSignPrefix != TEXT("Riverbend") ||
					!Operator->bFictionalBrandApproved)
				{
					Result.Errors.Add(FString::Printf(
						TEXT("Operator content %s does not match the approved fictional brand lock."),
						*AssetId.ToString()));
				}
			}
		}
	}

	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(TEXT("Required content is missing: %s"), *RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
