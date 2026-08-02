#include "AMSimConstructionProposalView.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimUITheme.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

namespace AMSimConstructionProposalPresentationPrivate
{
	constexpr float ParcelLeft = 0.205f;
	constexpr float ParcelTop = 0.135f;
	constexpr float ParcelRight = 0.785f;
	constexpr float ParcelBottom = 0.825f;
	constexpr double ParcelCentimeters = 100000.0;
	constexpr int64 ConnectionSnapCentimeters = 5000;

	float MapX(const int64 X)
	{
		return ParcelLeft + static_cast<float>(X / ParcelCentimeters) *
			(ParcelRight - ParcelLeft);
	}

	float MapY(const int64 Y)
	{
		return ParcelTop + static_cast<float>(Y / ParcelCentimeters) *
			(ParcelBottom - ParcelTop);
	}

	int64 DistanceSquared(
		const AMSim::FPhase1Point Left,
		const AMSim::FPhase1Point Right)
	{
		const int64 DeltaX = Left.X - Right.X;
		const int64 DeltaY = Left.Y - Right.Y;
		return DeltaX * DeltaX + DeltaY * DeltaY;
	}

	void SetSegmentGeometry(
		UCanvasPanelSlot* Slot,
		UWidget* Widget,
		const AMSim::FPhase1Point Start,
		const AMSim::FPhase1Point End,
		const float HalfThickness)
	{
		if (!Slot || !Widget)
		{
			return;
		}
		const float StartX = MapX(Start.X);
		const float StartY = MapY(Start.Y);
		const float EndX = MapX(End.X);
		const float EndY = MapY(End.Y);
		const float DeltaX = EndX - StartX;
		const float ScreenDeltaY = (EndY - StartY) * (1080.0f / 1920.0f);
		const float Length = FMath::Max(
			FMath::Sqrt(DeltaX * DeltaX + ScreenDeltaY * ScreenDeltaY),
			0.006f);
		const float CenterX = (StartX + EndX) * 0.5f;
		const float CenterY = (StartY + EndY) * 0.5f;
		Slot->SetAnchors(FAnchors(
			CenterX - Length * 0.5f,
			CenterY - HalfThickness,
			CenterX + Length * 0.5f,
			CenterY + HalfThickness));
		Slot->SetOffsets(FMargin());
		Widget->SetRenderTransformPivot(FVector2D(0.5f));
		Widget->SetRenderTransformAngle(
			FMath::RadiansToDegrees(FMath::Atan2(ScreenDeltaY, DeltaX)));
	}

	void SetBoxGeometry(
		UCanvasPanelSlot* Slot,
		const AMSim::FPhase1Point Center,
		const float HalfWidth,
		const float HalfHeight)
	{
		if (Slot)
		{
			Slot->SetAnchors(FAnchors(
				MapX(Center.X) - HalfWidth,
				MapY(Center.Y) - HalfHeight,
				MapX(Center.X) + HalfWidth,
				MapY(Center.Y) + HalfHeight));
			Slot->SetOffsets(FMargin());
		}
	}

	void SetPointGeometry(
		UCanvasPanelSlot* Slot,
		const AMSim::FPhase1Point Point,
		const FVector2D Size,
		const FVector2D Offset = FVector2D::ZeroVector)
	{
		if (!Slot)
		{
			return;
		}
		Slot->SetAnchors(FAnchors(
			FMath::Clamp(MapX(Point.X), ParcelLeft, ParcelRight),
			FMath::Clamp(MapY(Point.Y), ParcelTop, ParcelBottom)));
		Slot->SetAlignment(FVector2D(0.5f));
		Slot->SetOffsets(FMargin(Offset.X, Offset.Y, Size.X, Size.Y));
	}

	void StyleProposalSurface(
		UBorder* Surface,
		const bool bSelected,
		const bool bSnapTarget,
		const FLinearColor Identity)
	{
		if (!Surface)
		{
			return;
		}
		const FLinearColor Stroke = bSnapTarget
			? AMSim::UITheme::Amber()
			: bSelected ? AMSim::UITheme::White() : AMSim::UITheme::Cyan();
		Surface->SetBrush(AMSim::UITheme::RoundedBrush(
			FLinearColor(Identity.R, Identity.G, Identity.B,
				bSelected || bSnapTarget ? 0.58f : 0.34f),
			8.0f,
			Stroke,
			bSelected || bSnapTarget ? 3.0f : 1.6f));
	}

	const TCHAR* ToolName(const UAMSimConstructionProposalView::EPlacementTool Tool)
	{
		switch (Tool)
		{
		case UAMSimConstructionProposalView::EPlacementTool::Runway:
			return TEXT("RUNWAY");
		case UAMSimConstructionProposalView::EPlacementTool::Taxiway:
			return TEXT("TAXIWAY");
		case UAMSimConstructionProposalView::EPlacementTool::RoadAccess:
			return TEXT("SERVICE ROAD");
		}
		return TEXT("BUILD TOOL");
	}

	bool FindRunwayCrossing(
		const AMSim::FStarterPlanProposal& Proposal,
		AMSim::FPhase1Point& OutPoint)
	{
		for (const AMSim::FTaxiwaySegment& Segment :
			AMSim::GetTaxiwaySegments(Proposal))
		{
			if (AMSim::TryFindSegmentIntersection(
				Proposal.RunwayStart,
				Proposal.RunwayEnd,
				Segment.Start,
				Segment.End,
				OutPoint))
			{
				return true;
			}
		}
		return false;
	}
}

void UAMSimConstructionProposalView::SyncLegacyTaxiwayFields()
{
	RemoveDegenerateTaxiwaySegments();
	if (!CurrentProposal.TaxiwaySegments.IsEmpty())
	{
		CurrentProposal.TaxiStart = CurrentProposal.TaxiwaySegments[0].Start;
		CurrentProposal.TaxiEnd = CurrentProposal.TaxiwaySegments[0].End;
	}
	else
	{
		CurrentProposal.TaxiStart = {};
		CurrentProposal.TaxiEnd = {};
		SetToolPlaced(EPlacementTool::Taxiway, false);
	}
}

void UAMSimConstructionProposalView::RemoveDegenerateTaxiwaySegments()
{
	CurrentProposal.TaxiwaySegments.RemoveAll(
		[](const AMSim::FTaxiwaySegment& Segment)
		{
			return Segment.Start == Segment.End;
		});
}

AMSim::FPhase1Point UAMSimConstructionProposalView::SnapTaxiwayPoint(
	AMSim::FPhase1Point Point,
	const int32 ExcludedSegmentIndex) const
{
	using namespace AMSimConstructionProposalPresentationPrivate;
	AMSim::FPhase1Point BestPoint = Point;
	int64 BestDistance = TNumericLimits<int64>::Max();
	const auto Consider = [&Point, &BestPoint, &BestDistance](
		const AMSim::FPhase1Point Candidate)
	{
		const int64 CandidateDistance = DistanceSquared(Point, Candidate);
		if (CandidateDistance < BestDistance)
		{
			BestDistance = CandidateDistance;
			BestPoint = Candidate;
		}
	};
	if (IsToolPlaced(EPlacementTool::Runway))
	{
		Consider(AMSim::ClosestPointOnSegment(
			Point,
			CurrentProposal.RunwayStart,
			CurrentProposal.RunwayEnd));
	}
	for (const AMSim::FPhase1Point Gate : AMSim::GetStarterGatePoints())
	{
		Consider(Gate);
	}
	for (int32 Index = 0; Index < CurrentProposal.TaxiwaySegments.Num(); ++Index)
	{
		if (Index != ExcludedSegmentIndex)
		{
			const AMSim::FTaxiwaySegment& Segment =
				CurrentProposal.TaxiwaySegments[Index];
			Consider(AMSim::ClosestPointOnSegment(Point, Segment.Start, Segment.End));
		}
	}
	return BestDistance <= ConnectionSnapCentimeters * ConnectionSnapCentimeters
		? BestPoint
		: Point;
}

void UAMSimConstructionProposalView::RefreshPointerGhost()
{
	using namespace AMSimConstructionProposalPresentationPrivate;
	const bool bActivePathDrag = bPlacementDragging && bPathStartedByPress;
	if (!HoverGhostWidget || !HoverGhostSlot || !HoverGhostSurface ||
		!PointerEndpointSurface || !PointerEndpointSlot ||
		(!bHoveringWorld && !bActivePathDrag))
	{
		if (HoverGhostWidget)
		{
			HoverGhostWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (PointerEndpointSurface)
		{
			PointerEndpointSurface->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}
	const bool bToolAlreadyComplete =
		SelectedPlacementTool != EPlacementTool::Taxiway &&
		PathPlacementStep == 0 && IsToolPlaced(SelectedPlacementTool);
	if (!bActivePathDrag && bToolAlreadyComplete)
	{
		HoverGhostWidget->SetVisibility(ESlateVisibility::Collapsed);
		PointerEndpointSurface->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	AMSim::FPhase1Point PointerPoint = HoverMapPoint;
	if (SelectedPlacementTool == EPlacementTool::Taxiway)
	{
		PointerPoint = SnapTaxiwayPoint(
			HoverMapPoint,
			ActiveTaxiwaySegmentIndex);
	}
	SetPointGeometry(PointerEndpointSlot, PointerPoint, FVector2D(26.0f));
	PointerEndpointSurface->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (PointerEndpointText)
	{
		PointerEndpointText->SetText(FText::GetEmpty());
	}
	HoverGhostWidget->SetVisibility(ESlateVisibility::Collapsed);

	AMSim::FStarterPlanProposal Candidate = CurrentProposal;
	bool bHasPreviewSegment = PathPlacementStep == 1 || bActivePathDrag;
	if (bHasPreviewSegment)
	{
		Candidate = SetSelectedToolEndpoint(CurrentProposal, 1, PointerPoint);
		AMSim::FPhase1Point Start;
		AMSim::FPhase1Point End;
		if (SelectedPlacementTool == EPlacementTool::Runway)
		{
			Start = Candidate.RunwayStart;
			End = Candidate.RunwayEnd;
		}
		else if (SelectedPlacementTool == EPlacementTool::RoadAccess)
		{
			Start = Candidate.AccessStart;
			End = Candidate.AccessEnd;
		}
		else if (Candidate.TaxiwaySegments.IsValidIndex(
			ActiveTaxiwaySegmentIndex))
		{
			Start = Candidate.TaxiwaySegments[ActiveTaxiwaySegmentIndex].Start;
			End = Candidate.TaxiwaySegments[ActiveTaxiwaySegmentIndex].End;
		}
		SetSegmentGeometry(HoverGhostSlot, HoverGhostWidget, Start, End, 0.0035f);
		HoverGhostWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	FLinearColor GhostColor = AMSim::UITheme::Cyan();
	if (bHasPreviewSegment && SelectedPlacementTool == EPlacementTool::Taxiway)
	{
		const AMSim::FTaxiwayNetworkValidation Network =
			AMSim::ValidateTaxiwayNetwork(Candidate);
		if (Network.bHasRunwayConnection && Network.bHasGateConnection)
		{
			GhostColor = AMSim::UITheme::Green();
		}
		else if (PointerPoint != HoverMapPoint)
		{
			GhostColor = AMSim::UITheme::Amber();
		}
	}
	HoverGhostSurface->SetBrush(AMSim::UITheme::RoundedBrush(
		FLinearColor(GhostColor.R, GhostColor.G, GhostColor.B, 0.30f),
		8.0f,
		GhostColor,
		2.4f));
	PointerEndpointSurface->SetBrush(AMSim::UITheme::RoundedBrush(
		AMSim::UITheme::Navy900(),
		13.0f,
		GhostColor,
		2.6f));
}

void UAMSimConstructionProposalView::RefreshDiagnostics()
{
	using namespace AMSimConstructionProposalPresentationPrivate;
	const TArray<FPlacementDiagnostic> Diagnostics =
		MakePlacementDiagnostics(CurrentProposal, CurrentValidation);
	for (int32 Index = 0; Index < DiagnosticSlots.Num(); ++Index)
	{
		UWidget* Widget = DiagnosticSurfaces.IsValidIndex(Index)
			? DiagnosticSurfaces[Index]
			: nullptr;
		if (Index >= Diagnostics.Num())
		{
			if (Widget)
			{
				Widget->SetVisibility(ESlateVisibility::Collapsed);
			}
			continue;
		}
		if (Widget)
		{
			Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		SetPointGeometry(
			DiagnosticSlots[Index],
			Diagnostics[Index].Point,
			FVector2D(154.0f, 34.0f),
			Index % 2 == 0 ? FVector2D(-82.0f, -24.0f) : FVector2D(82.0f, 24.0f));
		if (DiagnosticTexts.IsValidIndex(Index) && DiagnosticTexts[Index])
		{
			DiagnosticTexts[Index]->SetText(
				FText::FromString(Diagnostics[Index].Label));
		}
	}
}

void UAMSimConstructionProposalView::RefreshProposalPresentation()
{
	using namespace AMSimConstructionProposalPresentationPrivate;
	const bool bAllPlaced = AreAllRequiredPlacementsComplete(PlacementMask);
	const AMSim::FPhase1Validation AuthoritativeValidation =
		AMSim::ValidateStarterPlan(CurrentProposal);
	const bool bCanShowAuthoritative = bAllPlaced && PathPlacementStep == 0;
	if (bCanShowAuthoritative ||
		(IsToolPlaced(EPlacementTool::Runway) &&
			(AuthoritativeValidation.ReasonCode == FName(TEXT("Build.Runway.TooSmall")) ||
			 AuthoritativeValidation.ReasonCode == FName(TEXT("Build.OutsideOwnedLand")))))
	{
		CurrentValidation = AuthoritativeValidation;
	}
	else
	{
		CurrentValidation = {};
		CurrentValidation.Result = AMSim::EPhase1CommandResult::RejectedInvalidGeometry;
		CurrentValidation.ReasonCode = TEXT("Build.Placement.Incomplete");
		CurrentValidation.Cause = TEXT("The starter movement network is incomplete.");
		CurrentValidation.Remedy = DescribePlacementProgress();
		CurrentValidation.QuotedCost = AMSim::GetPhase1Fixture().StarterPlanCost;
	}
	const bool bIncomplete = CurrentValidation.ReasonCode ==
		FName(TEXT("Build.Placement.Incomplete"));
	const FLinearColor StateColor = CurrentValidation.bValid
		? AMSim::UITheme::Green()
		: bIncomplete ? AMSim::UITheme::Amber() : AMSim::UITheme::Coral();
	const TArray<FPlacementDiagnostic> Diagnostics =
		MakePlacementDiagnostics(CurrentProposal, CurrentValidation);

	if (HeaderStateText)
	{
		HeaderStateText->SetText(FText::FromString(FString::Printf(
			TEXT("10 M GRID  -  %s"),
			ToolName(SelectedPlacementTool))));
	}
	if (ValidationLabelText)
	{
		ValidationLabelText->SetText(FText::FromString(
			CurrentValidation.bValid ? TEXT("NETWORK READY") :
				bIncomplete ? TEXT("PLACE REQUIRED NETWORK") :
					TEXT("FIX HIGHLIGHTED CONNECTION")));
		ValidationLabelText->SetColorAndOpacity(StateColor);
	}
	if (ValidationBodyText)
	{
		ValidationBodyText->SetText(FText::FromString(
			CurrentValidation.bValid
				? TEXT("A continuous taxi route joins the runway to a terminal gate.")
				: bIncomplete ? DescribePlacementProgress() : CurrentValidation.Remedy));
	}
	if (ValidationSurface)
	{
		ValidationSurface->SetBrush(AMSim::UITheme::RoundedBrush(
			AMSim::UITheme::Navy800(), 14.0f, StateColor, 2.0f));
		ValidationSurface->SetPadding(FMargin(13.0f, 10.0f));
	}
	if (PlacementText)
	{
		FString Instruction;
		if (SelectedPlacementTool == EPlacementTool::Runway)
		{
			Instruction = PathPlacementStep == 0
				? TEXT("RUNWAY  -  CLICK AND DRAG, OR CLICK TWO POINTS")
				: TEXT("RUNWAY  -  CHOOSE THE SECOND POINT");
		}
		else if (SelectedPlacementTool == EPlacementTool::Taxiway)
		{
			Instruction = PathPlacementStep == 0
				? TEXT("TAXIWAY  -  DRAW SEGMENTS; VALID RUNWAY, GATE, AND TAXI LINKS GLOW")
				: TEXT("TAXIWAY  -  CONNECT TO A HIGHLIGHTED TARGET");
		}
		else
		{
			Instruction = PathPlacementStep == 0
				? TEXT("SERVICE ROAD  -  OPTIONAL; CLICK AND DRAG ANY ROUTE")
				: TEXT("SERVICE ROAD  -  CHOOSE THE SECOND POINT");
		}
		PlacementText->SetText(FText::FromString(Instruction));
	}

	const auto StyleTool = [this](UButton* Button, const EPlacementTool Tool)
	{
		if (Button)
		{
			Button->SetStyle(AMSim::UITheme::ButtonStyle(
				SelectedPlacementTool == Tool
					? AMSim::UITheme::EButton::Primary
					: AMSim::UITheme::EButton::Tool));
		}
	};
	StyleTool(RunwayToolButton, EPlacementTool::Runway);
	StyleTool(TaxiwayToolButton, EPlacementTool::Taxiway);
	StyleTool(RoadAccessToolButton, EPlacementTool::RoadAccess);
	if (UndoButton)
	{
		UndoButton->SetIsEnabled(bCanUndo);
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(
			bAllPlaced && CurrentValidation.bValid && PathPlacementStep == 0);
	}
	if (ConfirmLabelText)
	{
		ConfirmLabelText->SetText(FText::FromString(
			bAllPlaced && CurrentValidation.bValid && PathPlacementStep == 0
				? TEXT("BUILD AIRFIELD  -  3.4K CR")
				: PathPlacementStep != 0 ? TEXT("PLACE SECOND POINT")
					: bIncomplete ? TEXT("COMPLETE RUNWAY AND TAXI NETWORK")
						: TEXT("FIX NETWORK CONNECTION")));
	}

	SetSegmentGeometry(
		RunwayGeometrySlot,
		RunwayGeometryWidget,
		CurrentProposal.RunwayStart,
		CurrentProposal.RunwayEnd,
		0.028f);
	if (RunwayGeometryWidget)
	{
		RunwayGeometryWidget->SetVisibility(
			IsToolPlaced(EPlacementTool::Runway) ||
				(SelectedPlacementTool == EPlacementTool::Runway && PathPlacementStep == 1)
				? ESlateVisibility::SelfHitTestInvisible
				: ESlateVisibility::Collapsed);
	}
	const TArray<AMSim::FTaxiwaySegment> Segments =
		CurrentProposal.TaxiwaySegments;
	for (int32 Index = 0; Index < TaxiGeometryWidgets.Num(); ++Index)
	{
		const bool bVisible = Segments.IsValidIndex(Index);
		if (bVisible && TaxiGeometrySlots.IsValidIndex(Index))
		{
			SetSegmentGeometry(
				TaxiGeometrySlots[Index],
				TaxiGeometryWidgets[Index],
				Segments[Index].Start,
				Segments[Index].End,
				0.013f);
		}
		TaxiGeometryWidgets[Index]->SetVisibility(
			bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		if (TaxiGeometrySurfaces.IsValidIndex(Index))
		{
			StyleProposalSurface(
				TaxiGeometrySurfaces[Index],
				SelectedPlacementTool == EPlacementTool::Taxiway &&
					Index == ActiveTaxiwaySegmentIndex,
				SelectedPlacementTool == EPlacementTool::Taxiway &&
					Index != ActiveTaxiwaySegmentIndex,
				AMSim::UITheme::Cyan());
		}
	}

	const TArray<AMSim::FPhase1Point>& Gates = AMSim::GetStarterGatePoints();
	SetBoxGeometry(TerminalGeometrySlot, AMSim::GetStarterTerminalCenter(), 0.060f, 0.034f);
	SetBoxGeometry(GateAGeometrySlot, Gates[0], 0.027f, 0.026f);
	SetBoxGeometry(GateBGeometrySlot, Gates[1], 0.027f, 0.026f);
	for (UWidget* FixedWidget : {
		TerminalGeometryWidget.Get(), GateAGeometryWidget.Get(), GateBGeometryWidget.Get()})
	{
		if (FixedWidget)
		{
			FixedWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	StyleProposalSurface(
		TerminalGeometrySurface,
		false,
		false,
		AMSim::UITheme::Amber());
	StyleProposalSurface(
		GateAGeometrySurface,
		false,
		SelectedPlacementTool == EPlacementTool::Taxiway,
		AMSim::UITheme::Cyan());
	StyleProposalSurface(
		GateBGeometrySurface,
		false,
		SelectedPlacementTool == EPlacementTool::Taxiway,
		AMSim::UITheme::Cyan());

	const bool bRoadVisible = CurrentProposal.AccessStart != CurrentProposal.AccessEnd ||
		(SelectedPlacementTool == EPlacementTool::RoadAccess && PathPlacementStep == 1);
	SetSegmentGeometry(
		AccessGeometrySlot,
		AccessGeometryWidget,
		CurrentProposal.AccessStart,
		CurrentProposal.AccessEnd,
		0.010f);
	if (AccessGeometryWidget)
	{
		AccessGeometryWidget->SetVisibility(
			bRoadVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
	StyleProposalSurface(
		RunwayGeometrySurface,
		SelectedPlacementTool == EPlacementTool::Runway,
		SelectedPlacementTool == EPlacementTool::Taxiway &&
			IsToolPlaced(EPlacementTool::Runway),
		AMSim::UITheme::Cyan());
	StyleProposalSurface(
		AccessGeometrySurface,
		SelectedPlacementTool == EPlacementTool::RoadAccess,
		false,
		AMSim::UITheme::Cyan());

	if (RunwayGeometryLabel && IsToolPlaced(EPlacementTool::Runway))
	{
		FString Label = DescribeRunwayGeometry(CurrentProposal);
		Label.RemoveFromStart(TEXT("RUNWAY "));
		const int32 WidthSeparator = Label.Find(TEXT(" x "));
		if (WidthSeparator != INDEX_NONE)
		{
			Label.LeftInline(WidthSeparator);
		}
		RunwayGeometryLabel->SetText(FText::FromString(Label));
	}
	if (GeometryText)
	{
		FString Description;
		if (SelectedPlacementTool == EPlacementTool::Runway)
		{
			Description = IsToolPlaced(EPlacementTool::Runway)
				? DescribeRunwayGeometry(CurrentProposal)
				: TEXT("RUNWAY - NOT PLACED");
		}
		else if (SelectedPlacementTool == EPlacementTool::Taxiway)
		{
			const AMSim::FTaxiwayNetworkValidation Network =
				AMSim::ValidateTaxiwayNetwork(CurrentProposal);
			Description = FString::Printf(
				TEXT("TAXI NETWORK - %d SEGMENT%s - %s"),
				Segments.Num(),
				Segments.Num() == 1 ? TEXT("") : TEXT("S"),
				Network.bHasRunwayConnection && Network.bHasGateConnection
					? TEXT("RUNWAY TO GATE") : TEXT("NEEDS CONNECTION"));
		}
		else if (!bRoadVisible)
		{
			Description = TEXT("SERVICE ROAD - OPTIONAL - WORKERS WALK WITHOUT IT");
		}
		else
		{
			const int64 LengthMeters = FMath::RoundToInt64(
				FMath::Sqrt(static_cast<double>(DistanceSquared(
					CurrentProposal.AccessStart,
					CurrentProposal.AccessEnd))) / 100.0);
			Description = FString::Printf(
				TEXT("SERVICE ROAD - %lld M - %s"),
				LengthMeters,
				AMSim::HasConstructionRoadBenefit(CurrentProposal)
					? TEXT("20% FASTER CREW TRAVEL") : TEXT("NO REQUIRED CONNECTION"));
		}
		GeometryText->SetText(FText::FromString(Description));
	}

	const AMSim::FTaxiwayNetworkValidation Network =
		AMSim::ValidateTaxiwayNetwork(CurrentProposal);
	if (ConnectionText)
	{
		FString Summary;
		if (CurrentValidation.bValid)
		{
			Summary = FString::Printf(
				TEXT("CONNECTED - RUNWAY > %d TAXI SEGMENT%s > GATE %c%s"),
				Segments.Num(),
				Segments.Num() == 1 ? TEXT("") : TEXT("S"),
				Network.ConnectedGateIndex == 1 ? TCHAR('B') : TCHAR('A'),
				bRoadVisible ? TEXT("  -  SERVICE ROAD OPTIONAL") : TEXT(""));
		}
		else if (bIncomplete)
		{
			Summary = DescribePlacementProgress();
		}
		else
		{
			Summary = Diagnostics.IsEmpty()
				? CurrentValidation.Remedy
				: Diagnostics[0].Label;
		}
		ConnectionText->SetText(FText::FromString(Summary));
		ConnectionText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ConnectionText->SetColorAndOpacity(StateColor);
	}

	AMSim::FPhase1Point CrossingPoint;
	const bool bHasCrossing = IsToolPlaced(EPlacementTool::Runway) &&
		FindRunwayCrossing(CurrentProposal, CrossingPoint);
	if (CrossingSurface && CrossingSlot)
	{
		CrossingSurface->SetVisibility(
			bHasCrossing ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		if (bHasCrossing)
		{
			SetPointGeometry(CrossingSlot, CrossingPoint, FVector2D(132.0f, 30.0f),
				FVector2D(0.0f, -34.0f));
		}
	}

	TArray<AMSim::FPhase1Point> HandlePoints;
	HandlePoints.SetNum(HandleSlots.Num());
	if (HandlePoints.Num() >= 22)
	{
		HandlePoints[0] = CurrentProposal.RunwayStart;
		HandlePoints[1] = CurrentProposal.RunwayEnd;
		for (int32 Index = 0; Index < MaximumTaxiwaySegments; ++Index)
		{
			if (Segments.IsValidIndex(Index))
			{
				HandlePoints[2 + Index * 2] = Segments[Index].Start;
				HandlePoints[3 + Index * 2] = Segments[Index].End;
			}
		}
		HandlePoints[18] = Gates[0];
		HandlePoints[19] = Gates[1];
		HandlePoints[20] = CurrentProposal.AccessStart;
		HandlePoints[21] = CurrentProposal.AccessEnd;
	}
	for (int32 Index = 0; Index < HandleSlots.Num(); ++Index)
	{
		const bool bRunwayHandle = Index < 2;
		const bool bTaxiHandle = Index >= 2 && Index < 18;
		const int32 TaxiIndex = bTaxiHandle ? (Index - 2) / 2 : INDEX_NONE;
		const bool bGateHandle = Index == 18 || Index == 19;
		const bool bRoadHandle = Index >= 20;
		const bool bVisible =
			(bRunwayHandle && (IsToolPlaced(EPlacementTool::Runway) ||
				(SelectedPlacementTool == EPlacementTool::Runway && PathPlacementStep == 1))) ||
			(bTaxiHandle && Segments.IsValidIndex(TaxiIndex)) ||
			bGateHandle ||
			(bRoadHandle && bRoadVisible);
		if (!HandleSurfaces.IsValidIndex(Index) || !HandleSurfaces[Index])
		{
			continue;
		}
		HandleSurfaces[Index]->SetVisibility(
			bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		if (!bVisible)
		{
			continue;
		}
		const FVector2D Size = bGateHandle ? FVector2D(34.0f) : FVector2D(26.0f);
		SetPointGeometry(HandleSlots[Index], HandlePoints[Index], Size);
		const bool bSelected =
			(SelectedPlacementTool == EPlacementTool::Runway && bRunwayHandle) ||
			(SelectedPlacementTool == EPlacementTool::Taxiway && bTaxiHandle &&
				TaxiIndex == ActiveTaxiwaySegmentIndex) ||
			(SelectedPlacementTool == EPlacementTool::RoadAccess && bRoadHandle);
		const bool bSnapTarget = SelectedPlacementTool == EPlacementTool::Taxiway &&
			(bGateHandle || bRunwayHandle ||
				(bTaxiHandle && TaxiIndex != ActiveTaxiwaySegmentIndex));
		bool bDiagnostic = false;
		for (const FPlacementDiagnostic& Diagnostic : Diagnostics)
		{
			bDiagnostic |= Diagnostic.Point == HandlePoints[Index];
		}
		const FLinearColor Stroke = bDiagnostic ? AMSim::UITheme::Coral() :
			bSelected ? AMSim::UITheme::Cyan() :
				bSnapTarget ? AMSim::UITheme::Amber() : AMSim::UITheme::Muted();
		HandleSurfaces[Index]->SetBrush(AMSim::UITheme::RoundedBrush(
			AMSim::UITheme::Navy900(),
			bGateHandle ? 17.0f : 13.0f,
			Stroke,
			bDiagnostic ? 3.2f : bSelected || bSnapTarget ? 2.6f : 1.2f));
	}

	RefreshDiagnostics();
	RefreshPointerGhost();
}
