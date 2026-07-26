#include "AMSimPhase3Content.h"

#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase3Type(TEXT("AMSimPhase3"));
}

FPrimaryAssetId UAMSimPhase3Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(Phase3Type, StableContentId);
}

const TArray<FName>& FAMSimPhase3ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Facility.Terminal.Shell"),
		TEXT("Facility.Terminal.Entrance"),
		TEXT("Facility.Terminal.CheckIn"),
		TEXT("Facility.Terminal.Security"),
		TEXT("Facility.Terminal.DepartureLounge"),
		TEXT("Facility.Terminal.GateA1"),
		TEXT("Facility.Terminal.Arrivals"),
		TEXT("Facility.Terminal.BagMakeUp"),
		TEXT("Facility.Terminal.Reclaim"),
		TEXT("Facility.Landside.Curb"),
		TEXT("Facility.Landside.Parking"),
		TEXT("Facility.Landside.BusStop"),
		TEXT("Network.Landside.RoadCurbParkingBus"),
		TEXT("Network.Pedestrian.EntranceCheckIn"),
		TEXT("Network.Controlled.DomesticSecurity"),
		TEXT("Network.Pedestrian.SterileGateA1"),
		TEXT("Network.Controlled.ArrivalsReclaimCurb"),
		TEXT("Network.Baggage.Outbound"),
		TEXT("Network.Baggage.Arrival"),
		TEXT("StaffRole.PassengerService"),
		TEXT("StaffRole.Security"),
		TEXT("StaffRole.Baggage"),
		TEXT("StaffRole.TerminalOperations"),
		TEXT("Tenant.RiverbendConnect"),
		TEXT("Operator.RiverbendConnect"),
		TEXT("Contract.Passenger.DomesticStarter"),
		TEXT("PassengerNameSet.Riverbend"),
		TEXT("Aircraft.RegionalPassenger.Phase3"),
		TEXT("Vehicle.Landside.PrivateCar"),
		TEXT("Vehicle.Landside.Taxi"),
		TEXT("Vehicle.Landside.PublicBus"),
		TEXT("Balance.Phase3.Terminal"),
		TEXT("Scenario.S05.PassengerDepartureArrival"),
		TEXT("Presentation.TerminalFlow")
	};
	return Required;
}

FAMSimPhase3CatalogValidation FAMSimPhase3ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase3CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase3Type,
		{TEXT("/Game/Phase3/Definitions")},
		UAMSimPhase3Definition::StaticClass(),
		false);
	TArray<FPrimaryAssetId> Assets;
	AssetManager.GetPrimaryAssetIdList(Phase3Type, Assets);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetId& AssetId : Assets)
	{
		Result.Assets.Add(AssetId);
		FoundIds.Add(AssetId.PrimaryAssetName);
		const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(AssetId);
		const UAMSimPhase3Definition* Definition =
			Cast<UAMSimPhase3Definition>(Path.TryLoad());
		if (!Definition)
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s did not resolve to a Phase 3 definition."),
				*AssetId.ToString()));
			continue;
		}
		if (Definition->StableContentId != AssetId.PrimaryAssetName ||
			Definition->DisplayName.IsEmpty() ||
			Definition->LocalizationKey.IsEmpty() ||
			Definition->OwningPhase != TEXT("Phase3") ||
			Definition->Provenance != TEXT("InternalApproved") ||
			Definition->Source != TEXT("ProjectAuthored") ||
			Definition->DefinitionKind.IsNone())
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Content %s has invalid identity, text, phase, provenance, or kind."),
				*AssetId.ToString()));
		}
		if (Definition->StableContentId == TEXT("Presentation.TerminalFlow"))
		{
			const FString* Dimension = Definition->Attributes.Find(TEXT("Dimension"));
			const FString* Required3D =
				Definition->Attributes.Find(TEXT("Required3DAssets"));
			const FString* Visible =
				Definition->Attributes.Find(TEXT("VisiblePassengerProxyTarget"));
			const FString* Logical =
				Definition->Attributes.Find(TEXT("LogicalPassengerTarget"));
			const FString* Reference =
				Definition->Attributes.Find(TEXT("VisualReference"));
			if (!Dimension || *Dimension != TEXT("2D") ||
				!Required3D || *Required3D != TEXT("0") ||
				!Visible || *Visible != TEXT("2000") ||
				!Logical || *Logical != TEXT("10000") ||
				!Reference || *Reference != TEXT("VA-03"))
			{
				Result.Errors.Add(
					TEXT("Phase 3 presentation descriptor violates the VA-03 2D/scale lock."));
			}
		}
		if (Definition->StableContentId == TEXT("Aircraft.RegionalPassenger.Phase3"))
		{
			const FString* Fictional = Definition->Attributes.Find(TEXT("Fictional"));
			const FString* External =
				Definition->Attributes.Find(TEXT("ExternalSourceFiles"));
			if (!Fictional || *Fictional != TEXT("true") ||
				!External || *External != TEXT("0"))
			{
				Result.Errors.Add(
					TEXT("The Phase 3 passenger aircraft must remain fictional and project-authored."));
			}
		}
		if (Definition->StableContentId == TEXT("Facility.Terminal.Security"))
		{
			const FString* ChildAppropriate =
				Definition->Attributes.Find(TEXT("ChildAppropriate"));
			const FString* AccessibleLane =
				Definition->Attributes.Find(TEXT("AccessibleLane"));
			if (!ChildAppropriate || *ChildAppropriate != TEXT("true") ||
				!AccessibleLane || *AccessibleLane != TEXT("true"))
			{
				Result.Errors.Add(
					TEXT("Domestic security must retain child-appropriate, accessible processing."));
			}
		}
	}
	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 3 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
