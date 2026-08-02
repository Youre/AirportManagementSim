#pragma once

#include "Blueprint/UserWidget.h"
#include "AMSimSchedulePickerView.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimSchedulePickerView final : public UUserWidget
{
	GENERATED_BODY()

public:
	void OpenPicker(
		const TArray<int64>& InOptions,
		int64 CurrentGameMilliseconds);
	void ClosePicker();
	bool IsPickerOpen() const;
	TFunction<bool(int64)> OnScheduleRequested;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void ChooseOption0();
	UFUNCTION()
	void ChooseOption1();
	UFUNCTION()
	void ChooseOption2();
	UFUNCTION()
	void ChooseOption3();
	UFUNCTION()
	void Cancel();
	void ChooseOption(int32 Index);
	void RefreshOptions();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> OptionButtons;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> OptionLabels;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CurrentTimeText;
	TArray<int64> Options;
	int64 CurrentTimeMilliseconds = 0;
};
