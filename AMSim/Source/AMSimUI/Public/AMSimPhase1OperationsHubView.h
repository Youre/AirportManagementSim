#pragma once

#include "AMSimPhase1OperationsHubPresentation.h"
#include "Blueprint/UserWidget.h"
#include "AMSimPhase1OperationsHubView.generated.h"

class UButton;
class UTextBlock;

DECLARE_DELEGATE(FAMSimPhase1HubAction);
DECLARE_DELEGATE_OneParam(FAMSimPhase1OverlayAction, int32);

UCLASS()
class AMSIMUI_API UAMSimPhase1OperationsHubView final : public UUserWidget
{
	GENERATED_BODY()

public:
	FAMSimPhase1HubAction OnChooseArrival;
	FAMSimPhase1HubAction OnWatchAtOne;
	FAMSimPhase1HubAction OnAdvanceToArrival;
	FAMSimPhase1OverlayAction OnOverlaySelected;

	void OpenPage(AMSim::EPhase1OperationsPage Page);
	void ClosePanel();
	bool IsPanelOpen() const;
	AMSim::EPhase1OperationsPage GetPage() const { return CurrentPage; }
	void RefreshFromSnapshot(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State,
		int32 OverlayMode);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void ChooseArrival();
	UFUNCTION()
	void WatchAtOne();
	UFUNCTION()
	void AdvanceToArrival();
	UFUNCTION()
	void SelectAirfieldOverlay();
	UFUNCTION()
	void SelectConnectionsOverlay();
	UFUNCTION()
	void SelectActivityOverlay();
	UFUNCTION()
	void Close();

	AMSim::EPhase1OperationsPage CurrentPage =
		AMSim::EPhase1OperationsPage::Schedule;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> EyebrowText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DetailText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TimelineText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FooterText;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ChooseArrivalButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> WatchButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvanceButton;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AdvanceButtonText;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AirfieldOverlayButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ConnectionsOverlayButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ActivityOverlayButton;
};
