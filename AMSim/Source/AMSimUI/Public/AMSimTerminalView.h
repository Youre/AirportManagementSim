#pragma once

#include "AMSimPhase3ViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimTerminalView.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UProgressBar;
class UTextBlock;
class UTexture2D;
class UAMSimRegionalOperationsView;

UCLASS()
class AMSIMUI_API UAMSimTerminalView final : public UUserWidget
{
	GENERATED_BODY()

public:
	UAMSimTerminalView(const FObjectInitializer& ObjectInitializer);
	void RefreshFromSimulation();
	void ShowPresentation();
	void ClosePresentation();
	void ShowRegionalOperations();
	void ShowAdvancedOperations();
	void ShowMajorOperations();
	bool IsPresentationOpen() const { return bPresentationOpen; }
	bool HasBeenOpened() const { return bHasBeenOpened; }
	FSimpleDelegate OnReturnRequested;
	bool HasRequiredPassengerIdentityArt() const
	{
		return PassengerFamilyTexture != nullptr;
	}

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void InitializePassengerAirport();
	UFUNCTION()
	void FundTerminal();
	UFUNCTION()
	void ConnectNextNetwork();
	UFUNCTION()
	void OpenTerminal();
	UFUNCTION()
	void SchedulePassengerService();
	UFUNCTION()
	void ToggleSecurityLane();
	UFUNCTION()
	void RequestPassengerAssistance();
	UFUNCTION()
	void ResolveBaggageException();
	UFUNCTION()
	void CycleOverlay();
	UFUNCTION()
	void PauseSimulation();
	UFUNCTION()
	void SetSpeedOne();
	UFUNCTION()
	void SetSpeedTwo();
	UFUNCTION()
	void SetSpeedFour();
	UFUNCTION()
	void SetSpeedEight();
	UFUNCTION()
	void SaveGame();
	UFUNCTION()
	void LoadGame();
	UFUNCTION()
	void OpenAdvancedOperations();
	UFUNCTION()
	void OpenMajorOperations();
	UFUNCTION()
	void ReturnToAirport();

	bool Submit(
		AMSim::FPhase3Command Command,
		const FString& SuccessMessage);
	void SubmitSpeed(int32 Multiplier, bool bPaused = false);
	void UpdateWorldPresentation(
		const AMSim::FPhase3QuerySnapshot& Query,
		const AMSim::FPhase3State& State);

	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalChrome;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TerminalCanvas;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimRegionalOperationsView> RegionalOperationsView;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> PlanningCard;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TerminalLabels;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FundsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ClockText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConstructionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FlightText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PassengerCountsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SecurityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PartyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CurrentStepText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> NeedsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RouteText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConfidenceText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BaggageText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LandsideText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InteractionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> OverlayText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BaggageExceptionLabelText;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> StepProgress;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ConfidenceProgress;

	UPROPERTY(Transient)
	TObjectPtr<UButton> InitializeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> FundButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ConnectButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> OpenButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ScheduleButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> SecurityButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AssistanceButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> BaggageButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvancedOperationsButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> MajorOperationsButton;
	UPROPERTY()
	TObjectPtr<UTexture2D> PassengerFamilyTexture;

	AMSim::FPhase3ViewState ViewState;
	int32 OverlayMode = 0;
	bool bPresentationOpen = false;
	bool bHasBeenOpened = false;
};
