#include "AMSimPhase4Content.h"

#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase4Type(TEXT("AMSimPhase4"));
}

FPrimaryAssetId UAMSimPhase4Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(Phase4Type, StableContentId);
}

const TArray<FName>& FAMSimPhase4ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Operator.RiverbendConnect.Phase4"),
		TEXT("Operator.NorthstarRegional"),
		TEXT("Operator.CoastalAirways"),
		TEXT("Aircraft.RegionalTurboprop.Phase4"),
		TEXT("Aircraft.RegionalJet.Phase4"),
		TEXT("Aircraft.NarrowBody.Phase4"),
		TEXT("Contract.Phase4.RiverbendConnect"),
		TEXT("Contract.Phase4.NorthstarRegional"),
		TEXT("Contract.Phase4.CoastalAirways"),
		TEXT("Facility.Gate.A1.Contact"),
		TEXT("Facility.Gate.A2.ContactInternational"),
		TEXT("Facility.Stand.R1.Remote"),
		TEXT("Vehicle.Airside.RemoteBus"),
		TEXT("Network.Passenger.RemoteBus"),
		TEXT("Network.Baggage.Transfer"),
		TEXT("Facility.Baggage.TransferSort"),
		TEXT("Facility.Border.Immigration"),
		TEXT("Facility.Border.Customs"),
		TEXT("Network.Controlled.International"),
		TEXT("Facility.Landside.RentalCar"),
		TEXT("Tenant.RiverbendRentals"),
		TEXT("Facility.Landside.RailStation"),
		TEXT("Tenant.ValleyRail"),
		TEXT("Weather.Phase4.SevenDayRegional"),
		TEXT("Approach.Phase4.Runway27"),
		TEXT("Incident.Phase4.DisabledAircraft"),
		TEXT("Vehicle.Emergency.TowTeam"),
		TEXT("Report.Phase4.Incident"),
		TEXT("Renewal.Phase4.RegionalOperators"),
		TEXT("Balance.Phase4.RegionalWeek"),
		TEXT("Scenario.S06.PassengerConnectionTransfer"),
		TEXT("Scenario.S10.WarnedIncidentRecovery"),
		TEXT("Presentation.Phase4.Timetable"),
		TEXT("Presentation.Phase4.Incident")
	};
	return Required;
}

FAMSimPhase4CatalogValidation FAMSimPhase4ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase4CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase4Type,
		{TEXT("/Game/Phase4/Definitions")},
		UAMSimPhase4Definition::StaticClass(),
		false);
	TArray<FPrimaryAssetId> Assets;
	AssetManager.GetPrimaryAssetIdList(Phase4Type, Assets);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetId& AssetId : Assets)
	{
		Result.Assets.Add(AssetId);
		FoundIds.Add(AssetId.PrimaryAssetName);
		const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
		const UAMSimPhase4Definition* Definition =
			Cast<UAMSimPhase4Definition>(Path.TryLoad());
		if (!Definition)
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s did not resolve to a Phase 4 definition."),
				*AssetId.ToString()));
			continue;
		}
		if (Definition->StableContentId != AssetId.PrimaryAssetName ||
			Definition->DisplayName.IsEmpty() ||
			Definition->LocalizationKey.IsEmpty() ||
			Definition->OwningPhase != TEXT("Phase4") ||
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
					TEXT("%s violates the fictional project-authored 2D aircraft lock."),
					*Definition->StableContentId.ToString()));
			}
		}
		if (Definition->StableContentId ==
			TEXT("Incident.Phase4.DisabledAircraft"))
		{
			const FString* ChildAppropriate =
				Definition->Attributes.Find(TEXT("ChildAppropriate"));
			const FString* Outcome =
				Definition->Attributes.Find(TEXT("HumanOutcome"));
			const FString* Provenance =
				Definition->Attributes.Find(TEXT("WarningProvenance"));
			if (!ChildAppropriate || *ChildAppropriate != TEXT("true") ||
				!Outcome || *Outcome != TEXT("NoInjuries") ||
				!Provenance || *Provenance != TEXT("required"))
			{
				Result.Errors.Add(
					TEXT("The Phase 4 incident must remain warned, abstract, and child-appropriate."));
			}
		}
		if (Definition->StableContentId ==
			TEXT("Network.Controlled.International"))
		{
			const FString* ProtectedTraitEffects =
				Definition->Attributes.Find(TEXT("ProtectedTraitEffects"));
			const FString* Controlled =
				Definition->Attributes.Find(TEXT("Controlled"));
			if (!ProtectedTraitEffects ||
				*ProtectedTraitEffects != TEXT("false") ||
				!Controlled || *Controlled != TEXT("true"))
			{
				Result.Errors.Add(
					TEXT("International processing must be controlled and trait-independent."));
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
			const FString* Reference =
				Definition->Attributes.Find(TEXT("VisualReference"));
			if (!Dimension || *Dimension != TEXT("2D") ||
				!Required3D || *Required3D != TEXT("0") ||
				!RuntimeLoad || *RuntimeLoad != TEXT("false") ||
				!Reference ||
				(*Reference != TEXT("VA-04") &&
				 *Reference != TEXT("VA-06")))
			{
				Result.Errors.Add(
					TEXT("Phase 4 presentation descriptor violates the VA-04/VA-06 2D lock."));
			}
		}
	}
	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 4 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
