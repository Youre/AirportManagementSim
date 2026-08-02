#pragma once

#include "AMSimPhase4ViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimRegionalOperationsView.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UCanvasPanelSlot;
class UProgressBar;
class UTextBlock;
class UTexture2D;
class UAMSimOverviewView;
class UAMSimProgressionView;
class UAMSimPhase5View;
class UAMSimPhase6View;

namespace AMSim
{
	struct FPhase6QuerySnapshot;
}

UCLASS()
class AMSIMUI_API UAMSimRegionalOperationsView final : public UUserWidget
{
	GENERATED_BODY()

public:
	UAMSimRegionalOperationsView(const FObjectInitializer& ObjectInitializer);
	void RefreshFromSimulation();
	int32 GetLoadedContractIdentityCount() const;
	bool HasRequiredContractIdentityArt() const;
	bool HasRequiredWeatherArt() const { return WeatherIconTexture != nullptr; }
	FSimpleDelegate OnReturnRequested;
	void ShowAdvancedOperations();
	void ShowMajorOperations();
	void ShowRegionalOperations();
	bool IsAdvancedOperationsOpen() const
	{
		return bAdvancedOperationsOpen;
	}
	bool IsMajorOperationsOpen() const
	{
		return bMajorOperationsOpen;
	}

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
	UFUNCTION()
	void ToggleProgression();
	UFUNCTION()
	void ToggleOverview();
	UFUNCTION()
	void SelectOverviewAircraft();
	UFUNCTION()
	void SelectOverviewFacility();
	UFUNCTION()
	void ToggleAdvancedOperations();
	void CloseAdvancedOperations();
	UFUNCTION()
	void ToggleMajorOperations();
	void CloseMajorOperations();
	UFUNCTION()
	void ReturnToAirport();

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
	void InitializePhase6View();
	bool RefreshPhase6View(
		const AMSim::FPhase6QuerySnapshot& Query);

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TimetableSurface;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootSurface;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> IncidentSurface;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimOverviewView> OverviewView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimProgressionView> ProgressionView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimPhase5View> Phase5View;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimPhase6View> Phase6View;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> WeekGrid;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> WeatherWindowOverlay;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> WeatherWindowSlot;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> WeatherWindowText;
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
	TObjectPtr<UProgressBar> ForecastConfidenceBar;
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
	TArray<TObjectPtr<UCanvasPanelSlot>> FlightCardSlots;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> DayChips;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> DayChipTexts;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> ContractCards;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ContractCardTexts;
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> ContractIdentityTextures;
	UPROPERTY()
	TObjectPtr<UTexture2D> ContractAircraftTexture;
	UPROPERTY()
	TObjectPtr<UTexture2D> WeatherIconTexture;

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
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvancedOperationsButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> MajorOperationsButton;

	AMSim::FPhase4ViewState ViewState;
	bool bGateWarningReviewed = false;
	bool bCompactLayout = false;
	bool bOverviewOpen = false;
	bool bProgressionOpen = false;
	bool bAdvancedOperationsOpen = false;
	bool bMajorOperationsOpen = false;
};
