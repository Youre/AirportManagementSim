#include "AMSimPhase5Content.h"

#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase5Type(TEXT("AMSimPhase5"));
}

FPrimaryAssetId UAMSimPhase5Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(Phase5Type, StableContentId);
}

const TArray<FName>& FAMSimPhase5ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Aircraft.FeederFreighter.Phase5"),
		TEXT("Aircraft.RegionalFreighter.Phase5"),
		TEXT("Operator.MeadowFreight"),
		TEXT("Contract.Cargo.GeneralOutbound"),
		TEXT("Contract.Cargo.ExpressInbound"),
		TEXT("Contract.Cargo.ColdTransfer"),
		TEXT("Contract.Cargo.OversizedOutbound"),
		TEXT("Contract.Cargo.BellyGeneral"),
		TEXT("Facility.Cargo.Warehouse"),
		TEXT("Warehouse.General"),
		TEXT("Warehouse.Express"),
		TEXT("Warehouse.Cold"),
		TEXT("Warehouse.Oversized"),
		TEXT("Facility.Cargo.RoadDock"),
		TEXT("Facility.Cargo.BuildUp"),
		TEXT("Facility.Cargo.AirsidePort"),
		TEXT("Vehicle.Cargo.Forklift"),
		TEXT("Vehicle.Cargo.DollyTug"),
		TEXT("Vehicle.Cargo.DeliveryTruck"),
		TEXT("Vehicle.Cargo.HighLoader"),
		TEXT("Vehicle.Cargo.SupportUnit"),
		TEXT("StaffRole.CargoHandling"),
		TEXT("Tenant.FBO.Riverbend"),
		TEXT("Tenant.Maintenance.Hearthside"),
		TEXT("Tenant.Fuel.Meadow"),
		TEXT("Tenant.Cargo.MeadowFreight"),
		TEXT("Tenant.Concession.SkyCafe"),
		TEXT("Tenant.Concession.RunwayGoods"),
		TEXT("Event.Phase5.Airshow"),
		TEXT("Event.Phase5.GAFlyIn"),
		TEXT("Event.Phase5.FlightSchoolOpenDay"),
		TEXT("Event.Phase5.CharterVip"),
		TEXT("Event.Phase5.CargoSurge"),
		TEXT("Event.Phase5.HolidayPassengerSurge"),
		TEXT("Event.Phase5.EmergencyExercise"),
		TEXT("Event.Phase5.NotableAircraft"),
		TEXT("Balance.Phase5.AdvancedBreadth"),
		TEXT("Scenario.Phase5.CargoContinuity"),
		TEXT("Scenario.Phase5.BellyFreight"),
		TEXT("Scenario.Phase5.SixAdvancedPaths"),
		TEXT("Presentation.Phase5.Cargo"),
		TEXT("Presentation.Phase5.Providers"),
		TEXT("Presentation.Phase5.Events"),
		TEXT("Presentation.Phase5.Progression")
	};
	return Required;
}

FAMSimPhase5CatalogValidation
FAMSimPhase5ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase5CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase5Type,
		{TEXT("/Game/Phase5/Definitions")},
		UAMSimPhase5Definition::StaticClass(),
		false);
	TArray<FPrimaryAssetId> Assets;
	AssetManager.GetPrimaryAssetIdList(Phase5Type, Assets);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetId& AssetId : Assets)
	{
		Result.Assets.Add(AssetId);
		FoundIds.Add(AssetId.PrimaryAssetName);
		const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
		const UAMSimPhase5Definition* Definition =
			Cast<UAMSimPhase5Definition>(Path.TryLoad());
		if (!Definition)
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s did not resolve to a Phase 5 definition."),
				*AssetId.ToString()));
			continue;
		}
		if (Definition->StableContentId != AssetId.PrimaryAssetName ||
			Definition->DisplayName.IsEmpty() ||
			Definition->LocalizationKey.IsEmpty() ||
			Definition->OwningPhase != TEXT("Phase5") ||
			Definition->Provenance != TEXT("InternalApproved") ||
			Definition->Source != TEXT("ProjectAuthored") ||
			Definition->DefinitionKind.IsNone())
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s has invalid identity, text, phase, provenance, or kind."),
				*AssetId.ToString()));
		}
		if (Definition->StableContentId.ToString().StartsWith(TEXT("Aircraft.")))
		{
			const FString* Fictional =
				Definition->Attributes.Find(TEXT("Fictional"));
			const FString* External =
				Definition->Attributes.Find(TEXT("ExternalSourceFiles"));
			const FString* Dimension =
				Definition->Attributes.Find(TEXT("RuntimeDimension"));
			if (!Fictional || *Fictional != TEXT("true") ||
				!External || *External != TEXT("0") ||
				!Dimension || *Dimension != TEXT("2D"))
			{
				Result.Errors.Add(FString::Printf(
					TEXT("%s violates the project-authored 2D cargo-aircraft lock."),
					*Definition->StableContentId.ToString()));
			}
		}
		if (Definition->DefinitionKind == TEXT("SpecialEvent"))
		{
			const FString* Optional =
				Definition->Attributes.Find(TEXT("DeclinePenalty"));
			const FString* Ordinary =
				Definition->Attributes.Find(TEXT("OrdinarySystems"));
			if (!Optional || *Optional != TEXT("0") ||
				!Ordinary || *Ordinary != TEXT("true"))
			{
				Result.Errors.Add(
					TEXT("Every Phase 5 event must be optional and use ordinary systems."));
			}
		}
		if (Definition->DefinitionKind == TEXT("PresentationDescriptor"))
		{
			const FString* Dimension =
				Definition->Attributes.Find(TEXT("Dimension"));
			const FString* Required3D =
				Definition->Attributes.Find(TEXT("Required3DAssets"));
			const FString* RuntimeLoad =
				Definition->Attributes.Find(TEXT("RuntimeStringLoading"));
			if (!Dimension || *Dimension != TEXT("2D") ||
				!Required3D || *Required3D != TEXT("0") ||
				!RuntimeLoad || *RuntimeLoad != TEXT("false"))
			{
				Result.Errors.Add(
					TEXT("Phase 5 presentation must remain 2D and cooker-visible."));
			}
		}
	}
	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 5 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
