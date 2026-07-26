#pragma once

#include "Engine/DataAsset.h"
#include "AMSimPhase1Content.generated.h"

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimPhase1Definition : public UPrimaryDataAsset
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
	FName OwningPhase = TEXT("Phase1");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FName Provenance = TEXT("InternalPrototype");

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
class AMSIMGAMEPLAY_API UAMSimMapDefinition final : public UAMSimPhase1Definition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	FIntPoint ParcelSizeMeters = FIntPoint(1000, 1000);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	FName ClimateId = TEXT("Climate.Temperate.Clear");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	bool bSupportsStarterAirfield = true;

	virtual FPrimaryAssetType GetDefinitionType() const override;
};

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimFacilityDefinition final : public UAMSimPhase1Definition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Facility")
	FName FacilityType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Facility", meta=(ClampMin="0"))
	int64 BaseCostCredits = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Facility", meta=(ClampMin="0"))
	int32 MinimumLengthMeters = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Facility")
	bool bRequiredForStarterLoop = true;

	virtual FPrimaryAssetType GetDefinitionType() const override;
};

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimAircraftDefinition final : public UAMSimPhase1Definition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 LengthCentimeters = 830;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 WingspanCentimeters = 1100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 MinimumRunwayMeters = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 MaximumOccupants = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bGrassRunwayCompatible = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bRequiresInspection = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bRequiresFuel = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	int32 HeadingDirectionCount = 16;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bSmoothRotationApproved = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	bool bFictionalLiveryApproved = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	FName AccuracyReviewId = TEXT("CT02.Phase1.RiverbendTrainer.2026-07-26");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aircraft")
	FString RegistrationFormat = TEXT("RB-### (fictional)");

	virtual FPrimaryAssetType GetDefinitionType() const override;
};

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimOperatorDefinition final : public UAMSimPhase1Definition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Operator")
	FString CallSignPrefix = TEXT("Riverbend");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Operator")
	bool bFictionalBrandApproved = true;

	virtual FPrimaryAssetType GetDefinitionType() const override;
};

struct FAMSimPhase1CatalogValidation
{
	bool bValid = false;
	TArray<FString> Errors;
	TArray<FPrimaryAssetId> Assets;
};

class AMSIMGAMEPLAY_API FAMSimPhase1ContentCatalog
{
public:
	static FAMSimPhase1CatalogValidation ValidateLoadedCatalog();
	static const TArray<FName>& RequiredContentIds();
};
