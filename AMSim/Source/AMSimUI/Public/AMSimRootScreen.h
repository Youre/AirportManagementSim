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
class UWidget;
class UAMSimConstructionProposalView;
class UAMSimContextHelpCard;
class UAMSimExpandingToolButton;
class UAMSimPhase1OperationsHubView;
class UAMSimPhase1StaffView;
class UAMSimReleaseGuideView;
class UAMSimSaveLoadView;
class UAMSimSchedulePickerView;
class UAMSimTerminalView;
class UAMSimTurnaroundView;
class UTexture2D;

namespace AMSim
{
	enum class EPhase1OperationsPage : uint8;
	struct FPhase3QuerySnapshot;
	struct FPhase4QuerySnapshot;
	struct FPhase5QuerySnapshot;
	struct FPhase6QuerySnapshot;
}

UCLASS(Blueprintable)
class AMSIMUI_API UAMSimRootScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UAMSimRootScreen(const FObjectInitializer& ObjectInitializer);
	static FUIInputConfig MakeGameplayInputConfig();
	static ESlateVisibility GetActivityNavigationLayerVisibility();
	void ShowReleaseGuide();
	int32 GetLoadedNavigationIconCount() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void CreateAirport();
	UFUNCTION()
	void CommitStarterPlan();
	UFUNCTION()
	void ToggleConstructionProposal();
	void ToggleSchedulePanel();
	void ToggleOverlayPanel();
	UFUNCTION()
	void ToggleStaffPanel();
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
	void ToggleWindowMode();
	UFUNCTION()
	void ToggleObjectiveDrawer();
	UFUNCTION()
	void ToggleOperationsDrawer();
	UFUNCTION()
	void ToggleReleaseGuide();
	UFUNCTION()
	void ToggleTerminalPresentation();
	UFUNCTION()
	void OpenRegionalPresentation();
	UFUNCTION()
	void OpenAdvancedPresentation();
	UFUNCTION()
	void OpenMajorPresentation();
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
	bool SubmitScheduleAt(int64 ScheduledArrivalGameMilliseconds);
	bool LoadSlotById(const FString& SlotId);
	void RefreshFromSimulation();
	void RefreshAudioFeedback(
		const AMSim::FPhase1QuerySnapshot& Phase1Query,
		const AMSim::FPhase1State& Phase1State,
		const AMSim::FPhase2QuerySnapshot& Phase2Query,
		const AMSim::FPhase4QuerySnapshot& Phase4Query,
		const AMSim::FPhase5QuerySnapshot& Phase5Query,
		const AMSim::FPhase6QuerySnapshot& Phase6Query);
	void SelectPhase2Specialization(AMSim::EAirportSpecialization Specialization);
	bool SubmitPhase2Command(
		AMSim::FPhase2Command Command,
		const FString& SuccessMessage);
	void RefreshPhase2Presentation(
		const AMSim::FPhase2QuerySnapshot& Query,
		const AMSim::FPhase2State& State,
		const AMSim::FPhase1State& Phase1State);
	void SetInteractionMessage(const FString& Message, bool bSucceeded);
	void RefreshSaveSlots();
	void SetConstructionModeChrome(bool bOpen);
	FString GetSelectedSaveSlotId() const;
	void RefreshDestinationButtons(
		const AMSim::FPhase1QuerySnapshot& Phase1Query,
		const AMSim::FPhase3QuerySnapshot& Phase3Query,
		const AMSim::FPhase4QuerySnapshot& Phase4Query,
		const AMSim::FPhase5QuerySnapshot& Phase5Query,
		const AMSim::FPhase6QuerySnapshot& Phase6Query);
	void RefreshPhase1ContextPanel(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void BindPhase1UtilityNavigation();
	void OpenPhase1OperationsPage(AMSim::EPhase1OperationsPage Page);
	void RefreshPhase1OperationsHub(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void WatchFirstVisitAtOne();
	void AdvanceToFirstVisit();
	void SelectPhase1Overlay(int32 Mode);
	static void SetButtonLabel(UButton* Button, const FString& Label);

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
	TObjectPtr<UAMSimPhase1OperationsHubView> Phase1OperationsHubView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimPhase1StaffView> StaffView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimTurnaroundView> TurnaroundView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimContextHelpCard> ContextHelpCard;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimReleaseGuideView> ReleaseGuideView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimSchedulePickerView> SchedulePickerView;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimSaveLoadView> SaveLoadView;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> Phase1Page;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> BuildModeLeftChrome;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> BuildModeRightChrome;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> BuildModeFooterChrome;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> ContextPanel;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> CreateAirportTray;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> ObjectiveDrawer;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> OperationsDrawer;
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
	TObjectPtr<UTextBlock> ContextCompactText;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> ContextDetailRowWidget;

	UPROPERTY(Transient)
	TObjectPtr<UButton> CreateButton;
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> CreateButtonWidget;
	UPROPERTY()
	TSubclassOf<UUserWidget> PrimaryButtonWidgetClass;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> BuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> ScheduleNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> StaffButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> OverlayButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> CancelBuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> OpenButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> CloseButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PinOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> DeclineOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ScheduleButton;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimExpandingToolButton> RecoveryButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TerminalNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RegionalNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvancedNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> MajorNavigationButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> WindowModeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PauseButton;
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
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> NavigationIcons;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UAMSimExpandingToolButton>> NavigationButtons;

	AMSim::FPhase1ViewState CurrentViewState;
	AMSim::FPhase2ViewState CurrentPhase2ViewState;
	uint64 LastAppliedRevision = MAX_uint64;
	int32 LastPhraseCount = 0;
	bool bPhraseCursorInitialized = false;
	bool bAudioFeedbackPrimed = false;
	uint8 LastAudioOfferState = 0;
	uint8 LastAudioPhase2Incident = 0;
	uint8 LastAudioPhase4Incident = 0;
	uint8 LastAudioPhase6Incident = 0;
	int32 LastAudioCompletedObjectives = 0;
	int32 LastAudioAdvancedPathCount = 0;
	int32 LastAudioMajorPathCount = 0;
	int32 LastAudioRecoveryGrantCount = 0;
	bool bOpenReleaseGuideWhenReady = false;
	bool bWorldPanning = false;
	bool bTerminalGrowthProofOpened = false;
	bool bRightMousePanning = false;
	bool bCompactLayoutActive = false;
	bool bReturnToOneAtInbound = false;
	int32 Phase1OverlayMode = 0;
	TArray<FString> SaveSlotIds;
	int32 SelectedSaveSlotIndex = 0;
};
