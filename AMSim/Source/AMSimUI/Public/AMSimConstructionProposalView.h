#pragma once

#include "AMSimPhase1Types.h"
#include "Blueprint/UserWidget.h"
#include "AMSimConstructionProposalView.generated.h"

class UButton;
class UBorder;
class UTextBlock;
class AAMSimWorldPresenter;

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
	static AMSim::FPhase1Point MapWorldPositionToParcel(
		const FVector& WorldPosition);
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
	bool TryMapScreenPositionToParcel(
		const FVector2D& ScreenPosition,
		AMSim::FPhase1Point& OutPoint) const;
	void RefreshPointerGhost();
	void RefreshProposalPresentation();
	void SyncWorldPreview();
	void ClearWorldPreview();
	AAMSimWorldPresenter* ResolveWorldPresenter();
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
	TObjectPtr<UTextBlock> ConnectionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConfirmLabelText;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> ValidationSurface;
	UPROPERTY(Transient)
	TObjectPtr<AAMSimWorldPresenter> CachedWorldPresenter;

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
