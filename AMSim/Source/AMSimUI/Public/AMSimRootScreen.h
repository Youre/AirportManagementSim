#pragma once

#include "AMSimPhase1ViewState.h"
#include "AMSimPhase2ViewState.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "AMSimRootScreen.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UEditableTextBox;
class UTextBlock;
class UUserWidget;
class UVerticalBox;
class UAMSimConstructionProposalView;
class UAMSimContextHelpCard;
class UAMSimReleaseGuideView;
class UAMSimTerminalView;
class UAMSimTurnaroundView;

UCLASS(Blueprintable)
class AMSIMUI_API UAMSimRootScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UAMSimRootScreen(const FObjectInitializer& ObjectInitializer);
	static FUIInputConfig MakeGameplayInputConfig();
	void ShowReleaseGuide();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void CreateAirport();
	UFUNCTION()
	void CommitStarterPlan();
	UFUNCTION()
	void ToggleConstructionProposal();
	UFUNCTION()
	void CancelStarterPlan();
	UFUNCTION()
	void OpenAirport();
	UFUNCTION()
	void CloseAirport();
	UFUNCTION()
	void PinOffer();
	UFUNCTION()
	void DeclineOffer();
	UFUNCTION()
	void AcceptOffer();
	UFUNCTION()
	void ScheduleFlight();
	UFUNCTION()
	void RequestRecovery();
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
	void ToggleObjectiveDrawer();
	UFUNCTION()
	void ToggleOperationsDrawer();
	UFUNCTION()
	void ToggleReleaseGuide();
	UFUNCTION()
	void ToggleTerminalPresentation();
	void CloseTerminalPresentation();
	UFUNCTION()
	void InitializePhase2();
	UFUNCTION()
	void SelectGeneralAviation();
	UFUNCTION()
	void SelectFlightSchool();
	UFUNCTION()
	void SelectCharter();
	UFUNCTION()
	void AcceptPhase2Contract();
	UFUNCTION()
	void CancelPhase2Contract();
	UFUNCTION()
	void ReschedulePhase2Flight();
	UFUNCTION()
	void DispatchPhase2Service();
	UFUNCTION()
	void TogglePhase2Runway();
	UFUNCTION()
	void TowPhase2Aircraft();
	UFUNCTION()
	void RotatePhase2TeamZone();
	UFUNCTION()
	void PurchasePhase2Parcel();
	UFUNCTION()
	void StartPhase2Expansion();
	UFUNCTION()
	void RespondPhase2Incident();

	void SubmitSpeed(int32 Multiplier);
	void RefreshFromSimulation();
	void SelectPhase2Specialization(AMSim::EAirportSpecialization Specialization);
	bool SubmitPhase2Command(
		AMSim::FPhase2Command Command,
		const FString& SuccessMessage);
	void RefreshPhase2Presentation(
		const AMSim::FPhase2QuerySnapshot& Query,
		const AMSim::FPhase2State& State,
		const AMSim::FPhase1State& Phase1State);
	void SetInteractionMessage(const FString& Message, bool bSucceeded);

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AirportNameText;
	UPROPERTY(Transient)
	TObjectPtr<UEditableTextBox> AirportNameEntry;
	UPROPERTY(Transient)
	TObjectPtr<UEditableTextBox> SaveSlotEntry;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ClockText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FundsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ObjectiveText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CauseText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RemedyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ProjectText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> OfferText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CompatibilityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FlightText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TimetableText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ServicesText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LedgerText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RatingText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CaptionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InteractionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AircraftLabel;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2OperationsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2WeatherText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2StaffText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2TenantText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Phase2EconomyText;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimTerminalView> TerminalView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimConstructionProposalView> ConstructionProposalView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimTurnaroundView> TurnaroundView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimContextHelpCard> ContextHelpCard;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimReleaseGuideView> ReleaseGuideView;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> Phase1Page;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RunwayVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TaxiVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> StandVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> HutVisual;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> AircraftMarker;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> ContextPanel;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> CreateAirportTray;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> ObjectiveDrawer;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> OperationsDrawer;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> Phase1ActivityCard;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> Phase1FlightCard;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> Phase2Panel;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ContextHeaderText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ContextBodyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ContextStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> CreateButton;
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> CreateButtonWidget;
	UPROPERTY()
	TSubclassOf<UUserWidget> PrimaryButtonWidgetClass;
	UPROPERTY(Transient)
	TObjectPtr<UButton> BuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CancelBuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> OpenButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PinOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> DeclineOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ScheduleButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RecoveryButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TerminalNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> InitializePhase2Button;
	UPROPERTY(Transient)
	TObjectPtr<UButton> SelectGAButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> SelectSchoolButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> SelectCharterButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptPhase2ContractButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CancelPhase2ContractButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ReschedulePhase2FlightButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> DispatchPhase2ServiceButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TogglePhase2RunwayButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TowPhase2AircraftButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RotatePhase2TeamZoneButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PurchasePhase2ParcelButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> StartPhase2ExpansionButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RespondPhase2IncidentButton;

	AMSim::FPhase1ViewState CurrentViewState;
	AMSim::FPhase2ViewState CurrentPhase2ViewState;
	uint64 LastAppliedRevision = MAX_uint64;
	int32 LastPhraseCount = 0;
	bool bOpenReleaseGuideWhenReady = false;
};
