#include "AMSimPhase7Content.h"

#include "AMSimSimulationTypes.h"
#include "Engine/AssetManager.h"

namespace
{
	const FPrimaryAssetType Phase7Type(TEXT("AMSimPhase7"));
}

FPrimaryAssetId UAMSimPhase7Definition::GetPrimaryAssetId() const
{
	return StableContentId.IsNone()
		? FPrimaryAssetId()
		: FPrimaryAssetId(
			Phase7Type,
			StableContentId);
}

const TArray<FName>&
FAMSimPhase7ContentCatalog::RequiredContentIds()
{
	static const TArray<FName> Required = {
		TEXT("Release.Map.TemperateStarter"),
		TEXT("Release.Catalog.AircraftOperators"),
		TEXT("Guidance.Release.StarterJourney"),
		TEXT("Guidance.Release.Controls"),
		TEXT("Guidance.Release.AirportTerms"),
		TEXT("Guidance.Release.Operations"),
		TEXT("Accessibility.Release.Profile"),
		TEXT("Audio.Release.RadioCoverage"),
		TEXT("Audio.Release.CaptionPolicy"),
		TEXT("Rights.Release.ApprovedInventory"),
		TEXT("Scenario.Release.MigrationMatrix"),
		TEXT("Scenario.Release.NetworkDenied"),
		TEXT("Protocol.Release.ReferenceSoak"),
		TEXT("Protocol.Release.UnassistedTester"),
		TEXT("Manifest.Release.Candidate")
	};
	return Required;
}

const TArray<FAMSimReleasePhraseFamily>&
FAMSimPhase7ContentCatalog::RequiredPhraseFamilies()
{
	static const TArray<FAMSimReleasePhraseFamily> Families = {
		{TEXT("Radio.ATIS"), TEXT("Riverbend information Alpha: wind 090 at 6 knots, Runway 09 in use."), TEXT("Current airport weather and runway."), 1},
		{TEXT("Radio.ArrivalContact"), TEXT("Riverbend Tower, Riverbend 21, inbound for landing."), TEXT("An arriving aircraft calls the airport."), 2},
		{TEXT("Radio.PatternEntry"), TEXT("Riverbend 21, join left traffic Runway 09."), TEXT("How the aircraft enters the landing pattern."), 2},
		{TEXT("Radio.LandingClearance"), TEXT("Riverbend 21, Runway 09 cleared to land."), TEXT("The runway is reserved for landing."), 3},
		{TEXT("Radio.TakeoffClearance"), TEXT("Riverbend 21, Runway 09 cleared for takeoff."), TEXT("The runway is reserved for departure."), 3},
		{TEXT("Radio.HoldShort"), TEXT("Riverbend 21, hold short of Runway 09."), TEXT("Stop before entering the runway."), 3},
		{TEXT("Radio.RunwayCrossing"), TEXT("Operations 1, cross Runway 09, report clear."), TEXT("A vehicle may cross the protected runway."), 3},
		{TEXT("Radio.TaxiRoute"), TEXT("Riverbend 21, taxi via Taxiway A to Stand A1."), TEXT("The safe ground route to parking."), 2},
		{TEXT("Radio.StandAssignment"), TEXT("Riverbend 21, park Stand A1."), TEXT("The aircraft's parking position."), 2},
		{TEXT("Radio.GoAround"), TEXT("Riverbend 21, go around; runway occupied."), TEXT("Climb away and try the approach again."), 4},
		{TEXT("Radio.ServiceDispatch"), TEXT("Ramp team, inspection and fuel assigned at Stand A1."), TEXT("Automatic ground work has been assigned."), 1},
		{TEXT("Radio.Boarding"), TEXT("Riverbend 304 now boarding at Gate A1."), TEXT("Passengers may begin boarding."), 1},
		{TEXT("Radio.PassengerPage"), TEXT("Maya's party, please proceed to Gate A1."), TEXT("A named party receives a terminal message."), 1},
		{TEXT("Radio.WeatherWarning"), TEXT("Low visibility warning: review Runway 27 operations."), TEXT("Weather may affect safe operations."), 4},
		{TEXT("Radio.EmergencyResponse"), TEXT("Airport 1, enter Runway 27; protected route confirmed."), TEXT("Emergency vehicles are responding."), 5}
	};
	return Families;
}

FAMSimPhase7CatalogValidation
FAMSimPhase7ContentCatalog::ValidateLoadedCatalog()
{
	FAMSimPhase7CatalogValidation Result;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		Phase7Type,
		{TEXT("/Game/Phase7/Definitions")},
		UAMSimPhase7Definition::StaticClass(),
		false);
	TArray<FPrimaryAssetId> Assets;
	AssetManager.GetPrimaryAssetIdList(
		Phase7Type,
		Assets);
	TSet<FName> FoundIds;
	for (const FPrimaryAssetId& AssetId : Assets)
	{
		Result.Assets.Add(AssetId);
		FoundIds.Add(AssetId.PrimaryAssetName);
		const FSoftObjectPath Path =
			AssetManager.GetPrimaryAssetPath(AssetId);
		const UAMSimPhase7Definition* Definition =
			Cast<UAMSimPhase7Definition>(Path.TryLoad());
		if (!Definition)
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Release content did not resolve: %s"),
				*AssetId.ToString()));
			continue;
		}
		const FString* RuntimeDimension =
			Definition->Attributes.Find(
				TEXT("RuntimeDimension"));
		const FString* Required3DAssets =
			Definition->Attributes.Find(
				TEXT("Required3DAssets"));
		const FString* RuntimeStringLoading =
			Definition->Attributes.Find(
				TEXT("RuntimeStringLoading"));
		const FString* RightsStatus =
			Definition->Attributes.Find(
				TEXT("RightsStatus"));
		if (Definition->StableContentId !=
				AssetId.PrimaryAssetName ||
			Definition->DisplayName.IsEmpty() ||
			Definition->LocalizationKey.IsEmpty() ||
			Definition->OwningPhase != TEXT("Phase7") ||
			Definition->DefinitionKind.IsNone() ||
			!RuntimeDimension ||
			*RuntimeDimension != TEXT("2D") ||
			!Required3DAssets ||
			*Required3DAssets != TEXT("0") ||
			!RuntimeStringLoading ||
			*RuntimeStringLoading != TEXT("false") ||
			!RightsStatus ||
			*RightsStatus != TEXT("Reviewed"))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("%s violates the release identity, 2D, "
					"cooking, or rights lock."),
				*AssetId.PrimaryAssetName.ToString()));
		}
		if (Definition->StableContentId ==
			TEXT("Manifest.Release.Candidate"))
		{
			const FString* EngineVersion =
				Definition->Attributes.Find(
					TEXT("EngineVersion"));
			const FString* SaveSchema =
				Definition->Attributes.Find(
					TEXT("SaveSchema"));
			const FString* Offline =
				Definition->Attributes.Find(
					TEXT("OfflineRequired"));
			const FString* HumanGate =
				Definition->Attributes.Find(
					TEXT("HumanGateStatus"));
			if (!EngineVersion ||
				*EngineVersion != TEXT("5.8.0") ||
				!SaveSchema ||
				FCString::Atoi(**SaveSchema) !=
					static_cast<int32>(
						AMSim::SnapshotSchemaVersion) ||
				!Offline ||
				*Offline != TEXT("true") ||
				!HumanGate ||
				*HumanGate != TEXT("PendingOwnerTest"))
			{
				Result.Errors.Add(
					TEXT("The release candidate manifest has "
						"an invalid engine, schema, offline, "
						"or human-gate identity."));
			}
		}
	}
	for (const FName RequiredId : RequiredContentIds())
	{
		if (!FoundIds.Contains(RequiredId))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("Required Phase 7 content is missing: %s"),
				*RequiredId.ToString()));
		}
	}
	if (RequiredPhraseFamilies().Num() != 15)
	{
		Result.Errors.Add(
			TEXT("The release radio catalog does not contain "
				"all 15 approved phrase families."));
	}
	TSet<FName> PhraseIds;
	for (const FAMSimReleasePhraseFamily& Family :
		RequiredPhraseFamilies())
	{
		if (Family.FamilyId.IsNone() ||
			Family.ExampleCaption.IsEmpty() ||
			Family.PlainMeaning.IsEmpty() ||
			PhraseIds.Contains(Family.FamilyId))
		{
			Result.Errors.Add(
				TEXT("The release radio catalog has an invalid "
					"or duplicate phrase family."));
			break;
		}
		PhraseIds.Add(Family.FamilyId);
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
