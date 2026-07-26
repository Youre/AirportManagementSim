#include "AMSimPhase2Content.h"
#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase2Type(TEXT("AMSimPhase2"));
	const FPrimaryAssetType Phase2AircraftType(TEXT("AMSimP2Aircraft"));
}

FPrimaryAssetId UAMSimPhase2Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(GetDefinitionType(), StableContentId);
}

FPrimaryAssetType UAMSimPhase2Definition::GetDefinitionType() const
{
	return Phase2Type;
}

FPrimaryAssetType UAMSimPhase2AircraftDefinition::GetDefinitionType() const
{
	return Phase2AircraftType;
}

const TArray<FName>& FAMSimPhase2ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Aircraft.BasicTrainer.Riverbend"),
		TEXT("Aircraft.TouringPiston.Wayfarer"),
		TEXT("Aircraft.BusinessTurboprop.Northstar"),
		TEXT("Aircraft.LightBusinessJet.Aster"),
		TEXT("Operator.SkywardSchool"),
		TEXT("Operator.MeadowAeroClub"),
		TEXT("Operator.NorthstarCharter"),
		TEXT("Operator.AsterExecutive"),
		TEXT("Tenant.SkywardSchool"),
		TEXT("Tenant.MeadowAeroClub"),
		TEXT("Tenant.NorthstarCharter"),
		TEXT("Contract.GA.CircuitAndTouring"),
		TEXT("Contract.FlightSchool.RecurringTraining"),
		TEXT("Contract.Charter.ExecutiveShuttle"),
		TEXT("Vehicle.OpsVan.Starter"),
		TEXT("Vehicle.FuelTruck.Light"),
		TEXT("Vehicle.Deicer.Light"),
		TEXT("Vehicle.Tug.Light"),
		TEXT("StaffRole.FlightOperations"),
		TEXT("StaffRole.GroundService"),
		TEXT("StaffRole.Maintenance"),
		TEXT("StaffRole.Emergency"),
		TEXT("Parcel.EastMeadow"),
		TEXT("Facility.GAStand.A2"),
		TEXT("Facility.GAStand.A3"),
		TEXT("Facility.PAPI.Starter"),
		TEXT("Service.Deicing"),
		TEXT("Service.Tow"),
		TEXT("Weather.Phase2.TemperateCycle"),
		TEXT("Incident.BasicOperations"),
		TEXT("Balance.Phase2.LivingGA"),
		TEXT("Achievement.FortnightOperator"),
		TEXT("Presentation.LivingGA")
	};
	return Required;
}

FAMSimPhase2CatalogValidation FAMSimPhase2ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase2CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase2Type,
		{TEXT("/Game/Phase2/Definitions")},
		UAMSimPhase2Definition::StaticClass(),
		false);
	AssetManager.ScanPathsForPrimaryAssets(
		Phase2AircraftType,
		{TEXT("/Game/Phase2/Aircraft")},
		UAMSimPhase2AircraftDefinition::StaticClass(),
		false);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetType Type : {Phase2Type, Phase2AircraftType})
	{
		TArray<FPrimaryAssetId> TypeAssets;
		AssetManager.GetPrimaryAssetIdList(Type, TypeAssets);
		for (const FPrimaryAssetId& AssetId : TypeAssets)
		{
			Result.Assets.Add(AssetId);
			FoundIds.Add(AssetId.PrimaryAssetName);
			const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
			const UAMSimPhase2Definition* Definition =
				Cast<UAMSimPhase2Definition>(Path.TryLoad());
			if (!Definition)
			{
				Result.Errors.Add(FString::Printf(
					TEXT("Content %s did not resolve to a Phase 2 definition."),
					*AssetId.ToString()));
				continue;
			}
			if (Definition->StableContentId != AssetId.PrimaryAssetName ||
				Definition->DisplayName.IsEmpty() ||
				Definition->LocalizationKey.IsEmpty() ||
				Definition->OwningPhase != TEXT("Phase2") ||
				Definition->Provenance != TEXT("InternalApproved") ||
				Definition->Source != TEXT("ProjectAuthored") ||
				Definition->DefinitionKind.IsNone())
			{
				Result.Errors.Add(FString::Printf(
					TEXT("Content %s has invalid identity, text, phase, provenance, or kind."),
					*AssetId.ToString()));
			}
			if (const UAMSimPhase2AircraftDefinition* Aircraft =
				Cast<UAMSimPhase2AircraftDefinition>(Definition))
			{
				if (Aircraft->RoleId.IsNone() ||
					Aircraft->LengthCentimeters <= 0 ||
					Aircraft->WingspanCentimeters <= 0 ||
					Aircraft->MinimumRunwayMeters < 500 ||
					Aircraft->HeadingDirectionCount < 16 ||
					!Aircraft->bFictionalLiveryApproved ||
					Aircraft->AccuracyReviewId !=
						TEXT("CT02.Phase2.InternalRoster.2026-07-26"))
				{
					Result.Errors.Add(FString::Printf(
						TEXT("Aircraft %s violates the fictional 2D Phase 2 roster lock."),
						*AssetId.ToString()));
				}
			}
			if (Definition->StableContentId == TEXT("Presentation.LivingGA"))
			{
				const FString* Dimension = Definition->Attributes.Find(TEXT("Dimension"));
				const FString* Required3D = Definition->Attributes.Find(TEXT("Required3DAssets"));
				if (!Dimension || *Dimension != TEXT("2D") ||
					!Required3D || *Required3D != TEXT("0"))
				{
					Result.Errors.Add(
						TEXT("Phase 2 presentation descriptor must remain 2D-only."));
				}
			}
		}
	}

	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 2 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
