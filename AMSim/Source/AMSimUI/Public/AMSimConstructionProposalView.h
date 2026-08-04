#pragma once

#include "AMSimPhase1Types.h"
#include "Blueprint/UserWidget.h"
#include "AMSimConstructionProposalView.generated.h"

class UButton;
class UBorder;
class UCanvasPanelSlot;
class UTextBlock;
class UWidget;

UCLASS()
class AMSIMUI_API UAMSimConstructionProposalView final : public UUserWidget
{
	GENERATED_BODY()

public:
	enum class EPlacementTool : uint8
	{
		Runway,
		Taxiway,
		RoadAccess
	};
	enum EPlacementPart : uint8
	{
		RunwayPart = 1 << 0,
		TaxiwayPart = 1 << 1,
		RoadAccessPart = 1 << 2,
		AllPlacementParts = RunwayPart | TaxiwayPart
	};
	static constexpr int32 MaximumTaxiwaySegments = 8;

	struct FPlacementDiagnostic
	{
		AMSim::FPhase1Point Point;
		FString Label;
	};
	struct FTaxiwayEditHit
	{
		int32 SegmentIndex = INDEX_NONE;
		int32 EndpointIndex = INDEX_NONE;
	};

	DECLARE_DELEGATE_OneParam(FOnBuildModeVisibilityChanged, bool);
	FOnBuildModeVisibilityChanged OnBuildModeVisibilityChanged;

	bool IsProposalOpen() const;
	void OpenProposal();
	void CloseProposal();
	static AMSim::FStarterPlanProposal MakePresentationProposal(
		bool bConflictVariant);
	static AMSim::FStarterPlanProposal MakeEmptyProposal();
	static bool AreAllRequiredPlacementsComplete(uint8 PlacementMask);
	static void TranslateProposal(
		AMSim::FStarterPlanProposal& Proposal,
		int64 DeltaX,
		int64 DeltaY);
	static AMSim::FStarterPlanProposal TranslateToolPlacement(
		const AMSim::FStarterPlanProposal& Proposal,
		EPlacementTool Tool,
		int64 DeltaX,
		int64 DeltaY,
		int32 TaxiwaySegmentIndex = 0);
	static AMSim::FStarterPlanProposal SetToolEndpoint(
		const AMSim::FStarterPlanProposal& Proposal,
		EPlacementTool Tool,
		int32 EndpointIndex,
		AMSim::FPhase1Point Point,
		bool bAllowRunwaySnap = true,
		bool bAllowGateSnap = true,
		int32 TaxiwaySegmentIndex = 0);
	static TArray<FPlacementDiagnostic> MakePlacementDiagnostics(
		const AMSim::FStarterPlanProposal& Proposal,
		const AMSim::FPhase1Validation& Validation);
	static FString DescribeRunwayGeometry(
		const AMSim::FStarterPlanProposal& Proposal);
	static AMSim::FPhase1Point MapLocalPositionToParcel(
		const FVector2D& LocalPosition,
		const FVector2D& LocalSize);
	static FTaxiwayEditHit ResolveTaxiwayEditHit(
		const AMSim::FStarterPlanProposal& Proposal,
		const AMSim::FPhase1Point& Point);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(
		const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void ConfirmProposal();
	UFUNCTION()
	void CancelProposal();
	UFUNCTION()
	void ResetPlacement();
	UFUNCTION()
	void UndoPlacement();
	UFUNCTION()
	void SelectRunwayTool();
	UFUNCTION()
	void SelectTaxiwayTool();
	UFUNCTION()
	void SelectRoadAccessTool();
	void SelectTool(EPlacementTool Tool);
	void ApplyPointerDrag(
		const AMSim::FPhase1Point& PointerPoint,
		bool bCenterOnPointer);
	void ApplyPointerClick(const AMSim::FPhase1Point& PointerPoint);
	void CaptureUndo(const AMSim::FStarterPlanProposal& Proposal);
	AMSim::FStarterPlanProposal SetSelectedToolEndpoint(
		const AMSim::FStarterPlanProposal& Proposal,
		int32 EndpointIndex,
		AMSim::FPhase1Point Point);
	uint8 PlacementPartForTool(EPlacementTool Tool) const;
	bool IsToolPlaced(EPlacementTool Tool) const;
	void SetToolPlaced(EPlacementTool Tool, bool bPlaced);
	FString DescribePlacementProgress() const;
	AMSim::FPhase1Point GetSelectedToolCenter() const;
	int32 ResolveSelectedEndpoint(const AMSim::FPhase1Point& Point);
	bool IsWorldPlacementPosition(
		const FVector2D& LocalPosition,
		const FVector2D& LocalSize) const;
	void RefreshPointerGhost();
	void RefreshDiagnostics();
	void RefreshProposalPresentation();
	void SyncLegacyTaxiwayFields();
	void RemoveDegenerateTaxiwaySegments();
	AMSim::FPhase1Point SnapTaxiwayPoint(
		AMSim::FPhase1Point Point,
		int32 ExcludedSegmentIndex) const;

	UPROPERTY(Transient)
	TObjectPtr<UButton> ConfirmButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RunwayToolButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TaxiwayToolButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RoadAccessToolButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> UndoButton;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> HeaderStateText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ValidationLabelText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ValidationBodyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PlacementText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> GeometryText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RunwayGeometryLabel;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConnectionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConfirmLabelText;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> RunwayGeometryWidget;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UWidget>> TaxiGeometryWidgets;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> TerminalGeometryWidget;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> GateAGeometryWidget;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> GateBGeometryWidget;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> AccessGeometryWidget;
	UPROPERTY(Transient)
	TObjectPtr<UWidget> HoverGhostWidget;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> ValidationSurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> RunwayGeometrySurface;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> TaxiGeometrySurfaces;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TerminalGeometrySurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> GateAGeometrySurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> GateBGeometrySurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> AccessGeometrySurface;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> HoverGhostSurface;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> HoverGhostText;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> PointerEndpointSurface;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PointerEndpointText;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> PointerEndpointSlot;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> CrossingSurface;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CrossingText;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> CrossingSlot;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> RunwayGeometrySlot;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCanvasPanelSlot>> TaxiGeometrySlots;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> TerminalGeometrySlot;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> GateAGeometrySlot;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> GateBGeometrySlot;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> AccessGeometrySlot;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> HoverGhostSlot;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> HandleSurfaces;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> HandleTexts;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCanvasPanelSlot>> HandleSlots;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCanvasPanelSlot>> DiagnosticSlots;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> DiagnosticSurfaces;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> DiagnosticTexts;

	AMSim::FStarterPlanProposal CurrentProposal;
	AMSim::FPhase1Validation CurrentValidation;
	AMSim::FStarterPlanProposal DragStartProposal;
	AMSim::FStarterPlanProposal UndoProposal;
	uint8 PlacementMask = 0;
	uint8 DragStartPlacementMask = 0;
	uint8 UndoPlacementMask = 0;
	AMSim::FPhase1Point DragStartMapPoint;
	AMSim::FPhase1Point HoverMapPoint;
	EPlacementTool SelectedPlacementTool = EPlacementTool::Runway;
	int32 DraggedEndpointIndex = INDEX_NONE;
	int32 ActiveTaxiwaySegmentIndex = INDEX_NONE;
	int32 DraggedTaxiwaySegmentIndex = INDEX_NONE;
	int32 PathPlacementStep = 0;
	bool bPlacementDragging = false;
	bool bPlacementMoved = false;
	bool bCanUndo = false;
	bool bHoveringWorld = false;
	bool bUndoCapturedForGesture = false;
	bool bPathStartedByPress = false;
	bool bSnapSoundActive = false;
	AMSim::FPhase1Point LastSnapSoundPoint;
};
