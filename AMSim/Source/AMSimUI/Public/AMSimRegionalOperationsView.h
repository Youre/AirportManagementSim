#pragma once

#include "AMSimPhase4ViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimRegionalOperationsView.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimRegionalOperationsView final : public UUserWidget
{
	GENERATED_BODY()

public:
	void RefreshFromSimulation();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime) override;

private:
	UFUNCTION()
	void InitializeRegionalAirport();
	UFUNCTION()
	void AcceptNextContract();
	UFUNCTION()
	void PublishTimetable();
	UFUNCTION()
	void ReviewGateChange();
	UFUNCTION()
	void ConfirmGateChange();
	UFUNCTION()
	void HoldDepartures();
	UFUNCTION()
	void DivertArrivals();
	UFUNCTION()
	void DispatchTow();
	UFUNCTION()
	void ProtectArea();
	UFUNCTION()
	void ReviewIncident();
	UFUNCTION()
	void ApplyRecovery();
	UFUNCTION()
	void AcceptRenewal();
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

	bool Submit(
		AMSim::FPhase4Command Command,
		const FString& SuccessMessage);
	void SubmitSpeed(int32 Multiplier, bool bPaused = false);
	const AMSim::FPhase4FlightRecord* GetDisruptedFlight() const;
	void RefreshFlightCards(const AMSim::FPhase4State& State);
	void RefreshContractCards(const AMSim::FPhase4State& State);
	void UpdateWorldPresentation(
		const AMSim::FPhase4QuerySnapshot& Query,
		const AMSim::FPhase4State& State);

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TimetableSurface;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> IncidentSurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> WeekGrid;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FundsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ClockText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TimetableText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SelectedFlightText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SelectedFlightDetailText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FeasibilityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConnectionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BorderText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TransportText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> WeatherText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ForecastText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IncidentHeadlineText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IncidentCauseText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IncidentConsequenceText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IncidentRemedyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IncidentLifecycleText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CaptionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RenewalText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InteractionText;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> FlightCards;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> FlightCardTexts;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> ContractCards;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ContractCardTexts;

	UPROPERTY(Transient)
	TObjectPtr<UButton> InitializeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptContractButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PublishButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ReviewGateButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ConfirmGateButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> HoldButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> DivertButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TowButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ProtectButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ReviewIncidentButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RecoveryButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RenewalButton;

	AMSim::FPhase4ViewState ViewState;
	bool bGateWarningReviewed = false;
	bool bCompactLayout = false;
};
