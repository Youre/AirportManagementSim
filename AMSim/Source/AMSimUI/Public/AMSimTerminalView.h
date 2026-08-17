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
class UVerticalBox;
class UAMSimRegionalOperationsView;
class UAMSimExpandingToolButton;

enum class EAMSimTerminalEditorTool : uint8
{
	Floor,
	Wall,
	Door,
	Seating,
	Information,
	Restroom,
	StaffDesk,
	Demolish,
	Rotate
};

enum class EAMSimTerminalPresentationDestination : uint8
{
	Terminal,
	Regional,
	Advanced,
	Major
};

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
	UFUNCTION()
	void ShowBuildMode();
	bool IsPresentationOpen() const { return bPresentationOpen; }
	bool IsAirportWorldOverlayOpen() const
	{
		return bPresentationOpen &&
			PresentationDestination == EAMSimTerminalPresentationDestination::Terminal;
	}
	bool HasBeenOpened() const { return bHasBeenOpened; }
	EAMSimTerminalPresentationDestination GetPresentationDestinationForTest() const
	{
		return PresentationDestination;
	}
	FSimpleDelegate OnReturnRequested;
	bool HasRequiredPassengerIdentityArt() const
	{
		return PassengerFamilyTexture != nullptr;
	}
	static AMSim::FTerminalCellCoord MapTerminalWorldPointToCell(
		const FVector& WorldPoint,
		const FVector& Center,
		int32 MinimumX,
		int32 MinimumY,
		int32 MaximumX,
		int32 MaximumY);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
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
	UFUNCTION()
	void ShowOperationsMode();
	void SelectFloorTool();
	void CycleFloorFunction();
	void SelectWallTool();
	void SelectDoorTool();
	void SelectSeatingTool();
	void SelectInformationTool();
	void SelectRestroomTool();
	void SelectStaffDeskTool();
	void SelectDemolishTool();
	void SelectRotateTool();
	void UndoTerminalEdit();

	bool Submit(
		AMSim::FPhase3Command Command,
		const FString& SuccessMessage);
	void SubmitSpeed(int32 Multiplier, bool bPaused = false);
	void UpdateWorldPresentation(
		const AMSim::FPhase3QuerySnapshot& Query,
		const AMSim::FPhase3State& State);
	void SetTerminalInteractionEnabled(bool bEnabled);
	void RestoreTerminalShellVisibility();
	AMSim::FTerminalCellCoord PointerToTerminalCell(
		const FGeometry& Geometry,
		const FVector2D& ScreenPosition) const;
	void CommitTerminalGesture(
		const AMSim::FTerminalCellCoord& Start,
		const AMSim::FTerminalCellCoord& End);
	void SelectEditorTool(EAMSimTerminalEditorTool Tool, const FString& Label);

	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalChrome;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalTopBar;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalFooterLegend;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalInteractionSurface;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TerminalCanvas;
	UPROPERTY(Transient)
	TObjectPtr<UAMSimRegionalOperationsView> RegionalOperationsView;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> PlanningCard;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> TerminalBuildToolsPanel;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> TerminalOperationsToolsPanel;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalOperationsInspector;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> TerminalLabels;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BrandText;
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
	UPROPERTY(Transient)
	TObjectPtr<UButton> BuildModeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> OperationsModeButton;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UAMSimExpandingToolButton>> TerminalEditorToolButtons;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> TerminalLegendTexts;
	UPROPERTY()
	TObjectPtr<UTexture2D> PassengerFamilyTexture;
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> TerminalEditorIcons;

	AMSim::FPhase3ViewState ViewState;
	int32 OverlayMode = 0;
	bool bPresentationOpen = false;
	bool bHasBeenOpened = false;
	bool bBuildMode = false;
	bool bStarterGAShellActive = false;
	bool bCompactLayout = false;
	bool bTerminalGestureActive = false;
	bool bRightMousePanning = false;
	EAMSimTerminalPresentationDestination PresentationDestination =
		EAMSimTerminalPresentationDestination::Terminal;
	AMSim::FTerminalCellCoord TerminalGestureStart;
	EAMSimTerminalEditorTool ActiveEditorTool = EAMSimTerminalEditorTool::Floor;
	AMSim::ETerminalFloorKind ActiveFloorKind = AMSim::ETerminalFloorKind::Public;
};
