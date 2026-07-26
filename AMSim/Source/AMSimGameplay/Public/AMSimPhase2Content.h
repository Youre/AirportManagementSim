#pragma once

#include "Engine/DataAsset.h"
#include "AMSimPhase2Content.generated.h"

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimPhase2Definition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FName StableContentId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FString LocalizationKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FName OwningPhase = TEXT("Phase2");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FName Provenance = TEXT("InternalApproved");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FString Source = TEXT("ProjectAuthored");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	FName DefinitionKind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	TMap<FName, FString> Attributes;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	virtual FPrimaryAssetType GetDefinitionType() const;
};

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimPhase2AircraftDefinition final
	: public UAMSimPhase2Definition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	FName RoleId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 LengthCentimeters = 800;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 WingspanCentimeters = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 MinimumRunwayMeters = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bGrassRunwayCompatible = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bFictionalLiveryApproved = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 HeadingDirectionCount = 16;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	FName AccuracyReviewId = TEXT("CT02.Phase2.InternalRoster.2026-07-26");

	virtual FPrimaryAssetType GetDefinitionType() const override;
};

struct FAMSimPhase2CatalogValidation
{
	bool bValid = false;
	TArray<FString> Errors;
	TArray<FPrimaryAssetId> Assets;
};

class AMSIMGAMEPLAY_API FAMSimPhase2ContentCatalog
{
public:
	static FAMSimPhase2CatalogValidation ValidateLoadedCatalog();
	static const TArray<FName>& RequiredContentIds();
};
