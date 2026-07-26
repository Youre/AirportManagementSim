#include "AMSimProjectAuditCommandlet.h"
#include "AMSimImportManifestValidator.h"
#include "AMSimPhase1Content.h"
#include "AMSimPhase2Content.h"
#include "Dom/JsonObject.h"
#include "Engine/AssetManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"

namespace
{
	bool LoadJsonObject(const FString& Path, TSharedPtr<FJsonObject>& Output)
	{
		FString Json;
		if (!FFileHelper::LoadFileToString(Json, *Path))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not read JSON artifact: %s"), *Path);
			return false;
		}
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
		if (!FJsonSerializer::Deserialize(Reader, Output) || !Output.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Could not parse JSON artifact: %s"), *Path);
			return false;
		}
		return true;
	}
}

UAMSimProjectAuditCommandlet::UAMSimProjectAuditCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UAMSimProjectAuditCommandlet::Main(const FString& Params)
{
	bool bPassed = true;
	bool bRayTracing = true;
	bool bSubstrate = true;
	GConfig->GetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.RayTracing"), bRayTracing, GEngineIni);
	GConfig->GetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.Substrate"), bSubstrate, GEngineIni);
	if (bRayTracing || bSubstrate)
	{
		UE_LOG(LogTemp, Error, TEXT("Project audit failed: disallowed 3D renderer features are enabled."));
		bPassed = false;
	}

	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.ScanPathsForPrimaryAssets(
		TEXT("AMSimPhase1"),
		{TEXT("/Game/Phase1/CoreDefinitions")},
		UAMSimPhase1Definition::StaticClass(),
		false);
	for (const TPair<FPrimaryAssetType, UClass*>& Type : {
		TPair<FPrimaryAssetType, UClass*>(TEXT("AMSimMap"), UAMSimMapDefinition::StaticClass()),
		TPair<FPrimaryAssetType, UClass*>(TEXT("AMSimFacility"), UAMSimFacilityDefinition::StaticClass()),
		TPair<FPrimaryAssetType, UClass*>(TEXT("AMSimAircraft"), UAMSimAircraftDefinition::StaticClass()),
		TPair<FPrimaryAssetType, UClass*>(TEXT("AMSimOperator"), UAMSimOperatorDefinition::StaticClass())})
	{
		AssetManager.ScanPathsForPrimaryAssets(
			Type.Key,
			{TEXT("/Game/Phase1/Definitions")},
			Type.Value,
			false);
	}
	const FAMSimPhase1CatalogValidation Catalog =
		FAMSimPhase1ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Catalog.Errors)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
	}
	bPassed &= Catalog.bValid;
	UE_LOG(
		LogTemp,
		Display,
		TEXT("Phase 1 catalog: %d Primary Assets; required definitions %s."),
		Catalog.Assets.Num(),
		Catalog.bValid ? TEXT("resolved") : TEXT("missing or invalid"));

	if (!FPackageName::DoesPackageExist(TEXT("/Game/Maps/L_TemperateStarter")))
	{
		UE_LOG(LogTemp, Error, TEXT("Project-owned Phase 1 map is missing."));
		bPassed = false;
	}

	const FString Phase1ConfigDirectory =
		FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("Phase1"));
	TSharedPtr<FJsonObject> Fixture;
	if (LoadJsonObject(
		FPaths::Combine(Phase1ConfigDirectory, TEXT("S01-StarterGrassAirfield.json")),
		Fixture))
	{
		const TSharedPtr<FJsonObject>* StarterPlan = nullptr;
		const TSharedPtr<FJsonObject>* FirstFlight = nullptr;
		const bool bFixtureValid =
			Fixture->GetStringField(TEXT("scenarioId")) == TEXT("S01.StarterGrassAirfield") &&
			Fixture->GetStringField(TEXT("mapId")) == TEXT("Map.TemperateStarter") &&
			Fixture->TryGetObjectField(TEXT("starterPlan"), StarterPlan) &&
			(*StarterPlan)->GetIntegerField(TEXT("totalCost")) <= 3500 &&
			(*StarterPlan)->GetIntegerField(TEXT("minimumContingencyPercent")) >= 30 &&
			Fixture->TryGetObjectField(TEXT("firstFlight"), FirstFlight) &&
			(*FirstFlight)->GetBoolField(TEXT("offerAppearsImmediatelyAtReadiness")) &&
			(*FirstFlight)->GetIntegerField(TEXT("maximumArrivalDelayAfterScheduleGameMinutes")) <= 3;
		if (!bFixtureValid)
		{
			UE_LOG(LogTemp, Error, TEXT("S01 fixture violates Phase 1 cost, timing, or identity gates."));
			bPassed = false;
		}
	}
	else
	{
		bPassed = false;
	}

	TSharedPtr<FJsonObject> ContentManifest;
	if (LoadJsonObject(
		FPaths::Combine(Phase1ConfigDirectory, TEXT("Phase1ContentManifest.json")),
		ContentManifest))
	{
		const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
		const bool bManifestValid =
			ContentManifest->GetIntegerField(TEXT("externalAircraftFilesCopied")) == 1 &&
			ContentManifest->GetStringField(TEXT("contentReviewId")) ==
				TEXT("CT02.Phase1_5.Cessna152Presentation.2026-07-26") &&
			ContentManifest->GetStringField(TEXT("contentReviewStatus")) ==
				TEXT("ApprovedPhase1_5") &&
			ContentManifest->TryGetArrayField(TEXT("entries"), Entries) &&
			Entries->Num() >= FAMSimPhase1ContentCatalog::RequiredContentIds().Num();
		if (!bManifestValid)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("Phase 1 content manifest is incomplete or records unapproved external copies."));
			bPassed = false;
		}
	}
	else
	{
		bPassed = false;
	}

	TSharedPtr<FJsonObject> ContentReview;
	if (LoadJsonObject(
		FPaths::Combine(Phase1ConfigDirectory, TEXT("Phase1ContentReview.json")),
		ContentReview))
	{
		const TSharedPtr<FJsonObject>* Aircraft = nullptr;
		const TSharedPtr<FJsonObject>* Visual = nullptr;
		const TSharedPtr<FJsonObject>* Checks = nullptr;
		const bool bReviewHeaderValid =
			ContentReview->GetIntegerField(TEXT("schema")) == 1 &&
			ContentReview->GetStringField(TEXT("reviewId")) ==
				TEXT("CT02.Phase1_5.Cessna152Presentation.2026-07-26") &&
			ContentReview->GetBoolField(TEXT("passed")) &&
			ContentReview->TryGetObjectField(TEXT("aircraft"), Aircraft) &&
			(*Aircraft)->GetStringField(TEXT("stableContentId")) ==
				TEXT("Aircraft.LightPiston.Starter") &&
			(*Aircraft)->GetStringField(TEXT("identityTreatment")) ==
				TEXT("FictionalLightPistonTrainer") &&
			(*Aircraft)->GetNumberField(TEXT("lengthMeters")) == 8.3 &&
			(*Aircraft)->GetNumberField(TEXT("wingspanMeters")) == 11.0 &&
			(*Aircraft)->GetIntegerField(TEXT("minimumGrassRunwayMeters")) == 600 &&
			(*Aircraft)->GetStringField(TEXT("registration")) == TEXT("RB-021") &&
			ContentReview->TryGetObjectField(TEXT("visual"), Visual) &&
			(*Visual)->GetIntegerField(TEXT("externalSourceFilesCopied")) == 1 &&
			(*Visual)->GetStringField(TEXT("runtimeDimension")) == TEXT("2D") &&
			(*Visual)->GetIntegerField(TEXT("headingDirectionCount")) >= 16 &&
			(*Visual)->GetBoolField(TEXT("smoothRotationApproved")) &&
			(*Visual)->GetBoolField(TEXT("colorIndependentDirection")) &&
			ContentReview->TryGetObjectField(TEXT("checks"), Checks);

		bool bReviewChecksValid = bReviewHeaderValid;
		if (bReviewChecksValid)
		{
			for (const TCHAR* CheckName : {
				TEXT("stableIdsPreserved"),
				TEXT("ownerApprovedSourceReuseConfirmed"),
				TEXT("externalSourceManifestMatchesOne"),
				TEXT("fictionalAircraftIdentityConfirmed"),
				TEXT("fictionalOperatorAndLiveryConfirmed"),
				TEXT("dimensionsAndCapabilityReviewed"),
				TEXT("directionalCoverageReviewed"),
				TEXT("colorIndependentSilhouetteReviewed"),
				TEXT("registrationTreatmentReviewed"),
				TEXT("aviationTerminologyReviewed"),
				TEXT("childReadableTerminologyReviewed"),
				TEXT("noRequired3DGameplayAsset")})
			{
				bool bCheckPassed = false;
				if (!(*Checks)->TryGetBoolField(CheckName, bCheckPassed) || !bCheckPassed)
				{
					bReviewChecksValid = false;
					break;
				}
			}
		}
		if (!bReviewChecksValid)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("Phase 1 player-facing content review is missing, incomplete, or failed."));
			bPassed = false;
		}
	}
	else
	{
		bPassed = false;
	}

	AssetManager.ScanPathsForPrimaryAssets(
		TEXT("AMSimPhase2"),
		{TEXT("/Game/Phase2/Definitions")},
		UAMSimPhase2Definition::StaticClass(),
		false);
	AssetManager.ScanPathsForPrimaryAssets(
		TEXT("AMSimP2Aircraft"),
		{TEXT("/Game/Phase2/Aircraft")},
		UAMSimPhase2AircraftDefinition::StaticClass(),
		false);
	const FAMSimPhase2CatalogValidation Phase2Catalog =
		FAMSimPhase2ContentCatalog::ValidateLoadedCatalog();
	for (const FString& Error : Phase2Catalog.Errors)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
	}
	bPassed &= Phase2Catalog.bValid;
	UE_LOG(
		LogTemp,
		Display,
		TEXT("Phase 2 catalog: %d Primary Assets; required definitions %s."),
		Phase2Catalog.Assets.Num(),
		Phase2Catalog.bValid ? TEXT("resolved") : TEXT("missing or invalid"));

	const FString Phase2ConfigDirectory =
		FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("Phase2"));
	TSharedPtr<FJsonObject> Phase2Manifest;
	if (LoadJsonObject(
		FPaths::Combine(Phase2ConfigDirectory, TEXT("Phase2ContentManifest.json")),
		Phase2Manifest))
	{
		const bool bPhase2ManifestValid =
			Phase2Manifest->GetIntegerField(TEXT("externalAircraftFilesCopied")) == 0 &&
			Phase2Manifest->GetIntegerField(TEXT("required3DGameplayAssets")) == 0 &&
			!Phase2Manifest->GetBoolField(TEXT("runtimeStringAssetLoadingAllowed")) &&
			Phase2Manifest->GetIntegerField(TEXT("requiredPrimaryAssetCount")) ==
				FAMSimPhase2ContentCatalog::RequiredContentIds().Num() &&
			Phase2Manifest->GetStringField(TEXT("contentReviewId")) ==
				TEXT("CT02.Phase2.InternalRoster.2026-07-26");
		if (!bPhase2ManifestValid)
		{
			UE_LOG(LogTemp, Error, TEXT("Phase 2 content manifest violates the 2D internal-content lock."));
			bPassed = false;
		}
	}
	else
	{
		bPassed = false;
	}

	TSharedPtr<FJsonObject> Phase2Review;
	if (LoadJsonObject(
		FPaths::Combine(Phase2ConfigDirectory, TEXT("Phase2ContentReview.json")),
		Phase2Review))
	{
		const TSharedPtr<FJsonObject>* Checks = nullptr;
		const TArray<TSharedPtr<FJsonValue>>* AircraftRoles = nullptr;
		bool bPhase2ReviewValid =
			Phase2Review->GetStringField(TEXT("reviewId")) ==
				TEXT("CT02.Phase2.InternalRoster.2026-07-26") &&
			Phase2Review->GetBoolField(TEXT("passed")) &&
			Phase2Review->GetStringField(TEXT("runtimeDimension")) == TEXT("2D") &&
			Phase2Review->GetIntegerField(TEXT("externalSourceFilesCopied")) == 0 &&
			Phase2Review->TryGetArrayField(TEXT("aircraftRoles"), AircraftRoles) &&
			AircraftRoles->Num() == 4 &&
			Phase2Review->TryGetObjectField(TEXT("checks"), Checks);
		if (bPhase2ReviewValid)
		{
			for (const TCHAR* CheckName : {
				TEXT("stableIdsPreserved"),
				TEXT("fictionalOperatorsAndLiveriesConfirmed"),
				TEXT("dimensionsAndCapabilitiesReviewed"),
				TEXT("directionalCoverageAtLeast16"),
				TEXT("colorIndependentSilhouettesRequired"),
				TEXT("aviationTerminologyReviewed"),
				TEXT("childReadableTerminologyReviewed"),
				TEXT("noRequired3DGameplayAsset"),
				TEXT("noExternalAircraftSourceCopied")})
			{
				bool bCheckPassed = false;
				if (!(*Checks)->TryGetBoolField(CheckName, bCheckPassed) || !bCheckPassed)
				{
					bPhase2ReviewValid = false;
					break;
				}
			}
		}
		if (!bPhase2ReviewValid)
		{
			UE_LOG(LogTemp, Error, TEXT("Phase 2 content review is incomplete or failed."));
			bPassed = false;
		}
	}
	else
	{
		bPassed = false;
	}

	for (const TCHAR* FixtureName : {
		TEXT("S02-SustainedGA.json"),
		TEXT("S03-FlightSchool.json"),
		TEXT("S04-Charter.json"),
		TEXT("S09-WeatherDeicing.json"),
		TEXT("S10-BasicIncident.json"),
		TEXT("S11-ConstructionClosure.json"),
		TEXT("S12-Recovery.json"),
		TEXT("S15-OfflinePackage.json")})
	{
		TSharedPtr<FJsonObject> Phase2Fixture;
		if (!LoadJsonObject(
			FPaths::Combine(Phase2ConfigDirectory, FixtureName),
			Phase2Fixture) ||
			Phase2Fixture->GetIntegerField(TEXT("schema")) != 1 ||
			Phase2Fixture->GetStringField(TEXT("scenarioId")).IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Phase 2 fixture is invalid: %s"), FixtureName);
			bPassed = false;
		}
	}

	FString ManifestPath;
	if (FParse::Value(*Params, TEXT("Manifest="), ManifestPath))
	{
		FString CsvText;
		if (!FFileHelper::LoadFileToString(CsvText, *ManifestPath))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not read manifest: %s"), *ManifestPath);
			bPassed = false;
		}
		else
		{
			const FAMSimManifestValidation Validation = FAMSimImportManifestValidator::Validate(CsvText);
			for (const FString& Error : Validation.Errors)
			{
				UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
			}
			bPassed &= Validation.bValid;
			UE_LOG(LogTemp, Display, TEXT("Manifest dry run: %d data rows; no files copied."), Validation.DataRowCount);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("AMSim Phase 0-2 project audit: %s"), bPassed ? TEXT("PASSED") : TEXT("FAILED"));
	return bPassed ? 0 : 1;
}
