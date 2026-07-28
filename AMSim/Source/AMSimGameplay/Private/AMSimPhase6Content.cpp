#include "AMSimPhase6Content.h"

#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase6Type(TEXT("AMSimPhase6"));
}

FPrimaryAssetId UAMSimPhase6Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(Phase6Type, StableContentId);
}

const TArray<FName>& FAMSimPhase6ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Aircraft.Boeing787-9.Phase6"),
		TEXT("Operator.RiverbendLongreach"),
		TEXT("Facility.Major.Runway.Parallel"),
		TEXT("Facility.Major.Stand.Large"),
		TEXT("Facility.Major.GA.HangarCampus"),
		TEXT("Facility.Major.FlightSchool.InstructionCenter"),
		TEXT("Facility.Major.Charter.Executive"),
		TEXT("Facility.Major.Cargo.Hub"),
		TEXT("Facility.Major.Passenger.Concourse"),
		TEXT("Facility.Major.Baggage.Hall"),
		TEXT("Facility.Major.Service.Depot"),
		TEXT("Facility.Major.Emergency.Station"),
		TEXT("Facility.Major.Access.Hub"),
		TEXT("Facility.Major.Operations.Center"),
		TEXT("Runway.Major.ParallelConfiguration"),
		TEXT("Operation.Major.Boeing787-9"),
		TEXT("Incident.Phase6.AircraftLoss"),
		TEXT("Project.Phase6.IncidentRepair"),
		TEXT("Balance.Phase6.MajorCapability"),
		TEXT("Scenario.Phase6.SixMajorPaths"),
		TEXT("Scenario.Phase6.LargeAircraftTurnaround"),
		TEXT("Scenario.Phase6.SeriousIncidentRecovery"),
		TEXT("Scenario.Phase6.MaximumScale"),
		TEXT("Presentation.Phase6.Runways"),
		TEXT("Presentation.Phase6.MajorCapacity"),
		TEXT("Presentation.Phase6.LargeAircraft"),
		TEXT("Presentation.Phase6.Incident"),
		TEXT("Presentation.Phase6.Progression")
	};
	return Required;
}

FAMSimPhase6CatalogValidation
FAMSimPhase6ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase6CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase6Type,
		{TEXT("/Game/Phase6/Definitions")},
		UAMSimPhase6Definition::StaticClass(),
		false);
	TArray<FPrimaryAssetId> Assets;
	AssetManager.GetPrimaryAssetIdList(Phase6Type, Assets);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetId& AssetId : Assets)
	{
		Result.Assets.Add(AssetId);
		FoundIds.Add(AssetId.PrimaryAssetName);
		const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
		const UAMSimPhase6Definition* Definition =
			Cast<UAMSimPhase6Definition>(Path.TryLoad());
		if (!Definition)
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s did not resolve to a Phase 6 definition."),
				*AssetId.ToString()));
			continue;
		}
		if (Definition->StableContentId != AssetId.PrimaryAssetName ||
			Definition->DisplayName.IsEmpty() ||
			Definition->LocalizationKey.IsEmpty() ||
			Definition->OwningPhase != TEXT("Phase6") ||
			Definition->DefinitionKind.IsNone())
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s has invalid identity, text, phase, or kind."),
				*AssetId.ToString()));
		}
		const FString* RuntimeDimension =
			Definition->Attributes.Find(TEXT("RuntimeDimension"));
		const FString* Required3D =
			Definition->Attributes.Find(TEXT("Required3DAssets"));
		const FString* RuntimeLoad =
			Definition->Attributes.Find(TEXT("RuntimeStringLoading"));
		if (!RuntimeDimension ||
			*RuntimeDimension != TEXT("2D") ||
			!Required3D ||
			*Required3D != TEXT("0") ||
			!RuntimeLoad ||
			*RuntimeLoad != TEXT("false"))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("%s violates the Phase 6 2D/cooker-visible lock."),
				*Definition->StableContentId.ToString()));
		}
		if (Definition->StableContentId ==
			TEXT("Aircraft.Boeing787-9.Phase6"))
		{
			const FString* FictionalLivery =
				Definition->Attributes.Find(TEXT("FictionalLivery"));
			const FString* CandidateChecksum =
				Definition->Attributes.Find(TEXT("CandidateChecksum"));
			const FString* RealOperator =
				Definition->Attributes.Find(TEXT("RealOperator"));
			const FString* Approval =
				Definition->Attributes.Find(TEXT("CandidateApproval"));
			const FString* RuntimeImported =
				Definition->Attributes.Find(TEXT("RuntimeTextureImported"));
			const FString* HeadingVariants =
				Definition->Attributes.Find(TEXT("HeadingVariants"));
			if (!FictionalLivery ||
				*FictionalLivery != TEXT("true") ||
				!CandidateChecksum ||
				*CandidateChecksum !=
					TEXT("6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449") ||
				!RealOperator ||
				*RealOperator != TEXT("false") ||
				!Approval ||
				*Approval != TEXT("OwnerApprovedRiverAndSun") ||
				!RuntimeImported ||
				*RuntimeImported != TEXT("true") ||
				!HeadingVariants ||
				*HeadingVariants != TEXT("16"))
			{
				Result.Errors.Add(
					TEXT("The 787 definition violates its approved intake contract."));
			}
		}
	}
	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 6 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
