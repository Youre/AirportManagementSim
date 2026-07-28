#pragma once

#include "AMSimProgressionViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimProgressionView.generated.h"

class UBorder;
class UProgressBar;
class UTextBlock;
class UTexture2D;

UCLASS()
class AMSIMUI_API UAMSimProgressionView final : public UUserWidget
{
	GENERATED_BODY()

public:
	UAMSimProgressionView(const FObjectInitializer& ObjectInitializer);

	void RefreshFromSimulation();
	int32 GetLoadedThumbnailCount() const;
	bool HasRequiredThumbnailKit() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void SelectPath0();
	UFUNCTION()
	void SelectPath1();
	UFUNCTION()
	void SelectPath2();
	UFUNCTION()
	void SelectPath3();
	UFUNCTION()
	void SelectPath4();
	UFUNCTION()
	void SelectPath5();
	void SelectPath(int32 Index);
	void ApplySelection();

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FundsText;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ObjectiveTexts;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UProgressBar>> ObjectiveProgress;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> PathCards;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> PathTexts;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InspectorTitle;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InspectorText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CombinedText;
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> PathThumbnails;

	AMSim::FProgressionViewState ViewState;
	bool bCompactLayout = false;
	int32 SelectedPathIndex = INDEX_NONE;
};
