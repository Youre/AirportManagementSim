#pragma once

#include "Engine/DataAsset.h"
#include "AMSimPhase6Content.generated.h"

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimPhase6Definition final : public UPrimaryDataAsset
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
	FName OwningPhase = TEXT("Phase6");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FName Provenance = TEXT("InternalApproved");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FString Source = TEXT("ProjectAuthored");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	FName DefinitionKind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	TMap<FName, FString> Attributes;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

struct FAMSimPhase6CatalogValidation
{
	bool bValid = false;
	TArray<FString> Errors;
	TArray<FPrimaryAssetId> Assets;
};

class AMSIMGAMEPLAY_API FAMSimPhase6ContentCatalog
{
public:
	static FAMSimPhase6CatalogValidation ValidateLoadedCatalog();
	static const TArray<FName>& RequiredContentIds();
};
