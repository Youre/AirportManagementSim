#pragma once

#include "AMSimGameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "AMSimSaveLoadView.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimSaveLoadView final : public UUserWidget
{
	GENERATED_BODY()

public:
	void OpenPicker(const TArray<AMSim::FSaveSlotSummary>& InSlots);
	void ClosePicker();
	bool IsPickerOpen() const;
	TFunction<bool(const FString&)> OnLoadRequested;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void ChooseSlot0();
	UFUNCTION()
	void ChooseSlot1();
	UFUNCTION()
	void ChooseSlot2();
	UFUNCTION()
	void ChooseSlot3();
	UFUNCTION()
	void Cancel();
	void ChooseSlot(int32 Index);
	void RefreshSlots();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> SlotButtons;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> SlotLabels;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> EmptyStateText;
	TArray<AMSim::FSaveSlotSummary> Slots;
};
