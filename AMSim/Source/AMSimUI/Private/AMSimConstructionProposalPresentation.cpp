#include "AMSimConstructionProposalView.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimUITheme.h"
#include "AMSimWorldPresenter.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "EngineUtils.h"

namespace AMSimConstructionProposalPresentationPrivate
{
	constexpr int64 ConnectionSnapCentimeters = 5000;

	int64 DistanceSquared(
		const AMSim::FPhase1Point Left,
		const AMSim::FPhase1Point Right)
	{
		const int64 DeltaX = Left.X - Right.X;
		const int64 DeltaY = Left.Y - Right.Y;
		return DeltaX * DeltaX + DeltaY * DeltaY;
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
	SyncWorldPreview();
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
			TEXT("10 M SNAP  -  %s"),
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

	const TArray<AMSim::FTaxiwaySegment> Segments =
		CurrentProposal.TaxiwaySegments;
	const bool bRoadVisible = CurrentProposal.AccessStart != CurrentProposal.AccessEnd ||
		(SelectedPlacementTool == EPlacementTool::RoadAccess && PathPlacementStep == 1);
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

	SyncWorldPreview();
}

AAMSimWorldPresenter* UAMSimConstructionProposalView::ResolveWorldPresenter()
{
	if (IsValid(CachedWorldPresenter))
	{
		return CachedWorldPresenter;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	for (TActorIterator<AAMSimWorldPresenter> It(World); It; ++It)
	{
		CachedWorldPresenter = *It;
		return CachedWorldPresenter;
	}
	return nullptr;
}

void UAMSimConstructionProposalView::SyncWorldPreview()
{
	AAMSimWorldPresenter* Presenter = ResolveWorldPresenter();
	if (!Presenter)
	{
		return;
	}

	AMSim::FPhase1ConstructionPreviewState Preview;
	Preview.Proposal = CurrentProposal;
	Preview.SelectedTool = SelectedPlacementTool == EPlacementTool::Runway
		? AMSim::EPhase1ConstructionPreviewTool::Runway
		: SelectedPlacementTool == EPlacementTool::Taxiway
			? AMSim::EPhase1ConstructionPreviewTool::Taxiway
			: AMSim::EPhase1ConstructionPreviewTool::RoadAccess;
	Preview.ActiveTaxiwaySegmentIndex = ActiveTaxiwaySegmentIndex;
	Preview.bRunwayVisible = IsToolPlaced(EPlacementTool::Runway) ||
		(SelectedPlacementTool == EPlacementTool::Runway && PathPlacementStep == 1);
	Preview.bRoadVisible = CurrentProposal.AccessStart != CurrentProposal.AccessEnd ||
		(SelectedPlacementTool == EPlacementTool::RoadAccess && PathPlacementStep == 1);
	Preview.bValid = CurrentValidation.bValid;

	const bool bActivePath =
		PathPlacementStep == 1 || (bPlacementDragging && bPathStartedByPress);
	const bool bCanPreviewPointer = bHoveringWorld || bActivePath;
	AMSim::FPhase1Point PreviewPointer = HoverMapPoint;
	if (SelectedPlacementTool == EPlacementTool::Taxiway)
	{
		PreviewPointer = SnapTaxiwayPoint(
			PreviewPointer,
			ActiveTaxiwaySegmentIndex);
	}
	if (bActivePath && bCanPreviewPointer)
	{
		const int32 TaxiwayIndex = FMath::Max(ActiveTaxiwaySegmentIndex, 0);
		Preview.Proposal = SetToolEndpoint(
			CurrentProposal,
			SelectedPlacementTool,
			1,
			PreviewPointer,
			IsToolPlaced(EPlacementTool::Runway),
			true,
			TaxiwayIndex);
	}

	const auto AddMarker = [&Preview](
		const AMSim::FPhase1Point Point,
		const AMSim::EPhase1ConstructionMarkerStyle Style)
	{
		Preview.Markers.Add({Point, Style});
	};
	if (Preview.bRunwayVisible)
	{
		const auto Style = SelectedPlacementTool == EPlacementTool::Taxiway
			? AMSim::EPhase1ConstructionMarkerStyle::Connection
			: AMSim::EPhase1ConstructionMarkerStyle::Endpoint;
		AddMarker(Preview.Proposal.RunwayStart, Style);
		AddMarker(Preview.Proposal.RunwayEnd, Style);
	}
	const TArray<AMSim::FTaxiwaySegment> TaxiSegments =
		Preview.Proposal.TaxiwaySegments;
	for (int32 Index = 0; Index < TaxiSegments.Num(); ++Index)
	{
		const auto Style = SelectedPlacementTool == EPlacementTool::Taxiway &&
			Index != ActiveTaxiwaySegmentIndex
				? AMSim::EPhase1ConstructionMarkerStyle::Connection
				: AMSim::EPhase1ConstructionMarkerStyle::Endpoint;
		AddMarker(TaxiSegments[Index].Start, Style);
		AddMarker(TaxiSegments[Index].End, Style);
	}
	if (SelectedPlacementTool == EPlacementTool::Taxiway)
	{
		for (const AMSim::FPhase1Point Gate : AMSim::GetStarterGatePoints())
		{
			AddMarker(
				Gate,
				AMSim::EPhase1ConstructionMarkerStyle::Connection);
		}
	}
	if (Preview.bRoadVisible)
	{
		AddMarker(
			Preview.Proposal.AccessStart,
			AMSim::EPhase1ConstructionMarkerStyle::Endpoint);
		AddMarker(
			Preview.Proposal.AccessEnd,
			AMSim::EPhase1ConstructionMarkerStyle::Endpoint);
	}
	if (bCanPreviewPointer)
	{
		const bool bToolComplete =
			SelectedPlacementTool != EPlacementTool::Taxiway &&
			PathPlacementStep == 0 && IsToolPlaced(SelectedPlacementTool);
		if (!bToolComplete)
		{
			AddMarker(
				PreviewPointer,
				AMSim::EPhase1ConstructionMarkerStyle::Pointer);
		}
	}
	for (const FPlacementDiagnostic& Diagnostic :
		MakePlacementDiagnostics(CurrentProposal, CurrentValidation))
	{
		AddMarker(
			Diagnostic.Point,
			AMSim::EPhase1ConstructionMarkerStyle::Invalid);
	}
	AMSim::FPhase1Point CrossingPoint;
	if (Preview.bRunwayVisible &&
		AMSimConstructionProposalPresentationPrivate::FindRunwayCrossing(
			Preview.Proposal,
			CrossingPoint))
	{
		AddMarker(
			CrossingPoint,
			AMSim::EPhase1ConstructionMarkerStyle::Connection);
	}
	Presenter->SetPhase1ConstructionPreview(Preview);
}

void UAMSimConstructionProposalView::ClearWorldPreview()
{
	if (AAMSimWorldPresenter* Presenter = ResolveWorldPresenter())
	{
		Presenter->ClearPhase1ConstructionPreview();
	}
}
