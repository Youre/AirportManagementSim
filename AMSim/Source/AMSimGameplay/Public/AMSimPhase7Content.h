#pragma once

#include "Engine/DataAsset.h"
#include "AMSimPhase7Content.generated.h"

UCLASS(BlueprintType)
class AMSIMGAMEPLAY_API UAMSimPhase7Definition final
	: public UPrimaryDataAsset
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
	FName OwningPhase = TEXT("Phase7");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Provenance")
	FName Provenance = TEXT("InternalApproved");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	FName DefinitionKind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Definition")
	TMap<FName, FString> Attributes;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

struct FAMSimReleasePhraseFamily
{
	FName FamilyId;
	FString ExampleCaption;
	FString PlainMeaning;
	int32 Priority = 0;
};

struct FAMSimPhase7CatalogValidation
{
	bool bValid = false;
	TArray<FString> Errors;
	TArray<FPrimaryAssetId> Assets;
};

class AMSIMGAMEPLAY_API FAMSimPhase7ContentCatalog
{
public:
	static FAMSimPhase7CatalogValidation ValidateLoadedCatalog();
	static const TArray<FName>& RequiredContentIds();
	static const TArray<FAMSimReleasePhraseFamily>&
		RequiredPhraseFamilies();
};
