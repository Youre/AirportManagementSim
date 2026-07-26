#include "AMSimProjectAuditCommandlet.h"
#include "AMSimImportManifestValidator.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"

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
		UE_LOG(LogTemp, Error, TEXT("Phase 0 audit failed: 3D renderer features are enabled."));
		bPassed = false;
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

	UE_LOG(LogTemp, Display, TEXT("AMSim Phase 0 project audit: %s"), bPassed ? TEXT("PASSED") : TEXT("FAILED"));
	return bPassed ? 0 : 1;
}
