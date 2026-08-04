#pragma once

#include "AMSimPhase1Types.h"
#include "Blueprint/UserWidget.h"
#include "AMSimPhase1StaffView.generated.h"

class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimPhase1StaffView final : public UUserWidget
{
	GENERATED_BODY()

public:
	void TogglePanel();
	void ClosePanel();
	bool IsPanelOpen() const;
	void RefreshFromSnapshot(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void Close();

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CrewCountText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AvailabilityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DetailText;
};
