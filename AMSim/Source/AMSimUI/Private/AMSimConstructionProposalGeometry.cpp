#include "AMSimConstructionProposalView.h"

#include "AMSimPhase1Fixture.h"

namespace AMSimConstructionProposalGeometryPrivate
{
	constexpr float ParcelLeft = 0.205f;
	constexpr float ParcelTop = 0.135f;
	constexpr float ParcelRight = 0.785f;
	constexpr float ParcelBottom = 0.825f;
	constexpr double ParcelCentimeters = 100000.0;
	constexpr int64 GridCentimeters = 1000;
	constexpr int64 ConnectionSnapCentimeters = 5000;
	constexpr int64 ParcelMinimum = 0;
	constexpr int64 ParcelMaximum = 100000;

	int64 Snap(const double Value)
	{
		return FMath::RoundToInt64(Value / GridCentimeters) * GridCentimeters;
	}

	int64 DistanceSquared(
		const AMSim::FPhase1Point& Left,
		const AMSim::FPhase1Point& Right)
	{
		const int64 DeltaX = Left.X - Right.X;
		const int64 DeltaY = Left.Y - Right.Y;
		return DeltaX * DeltaX + DeltaY * DeltaY;
	}

	bool IsInsideParcel(const AMSim::FPhase1Point& Point)
	{
		return Point.X >= ParcelMinimum && Point.X <= ParcelMaximum &&
			Point.Y >= ParcelMinimum && Point.Y <= ParcelMaximum;
	}
}

AMSim::FStarterPlanProposal
UAMSimConstructionProposalView::MakePresentationProposal(
	const bool bConflictVariant)
{
	AMSim::FStarterPlanProposal Proposal = AMSim::CreateDefaultStarterPlan();
	if (bConflictVariant)
	{
		Proposal.RunwayStart.X = -18000;
	}
	return Proposal;
}

AMSim::FStarterPlanProposal UAMSimConstructionProposalView::MakeEmptyProposal()
{
	AMSim::FStarterPlanProposal Proposal;
	Proposal.RunwayWidthCentimeters = 2000;
	Proposal.StandCenter = AMSim::GetStarterGatePoints()[0];
	Proposal.OperationsHutCenter = AMSim::GetStarterTerminalCenter();
	return Proposal;
}

bool UAMSimConstructionProposalView::AreAllRequiredPlacementsComplete(
	const uint8 InPlacementMask)
{
	return (InPlacementMask & AllPlacementParts) == AllPlacementParts;
}

AMSim::FPhase1Point
UAMSimConstructionProposalView::MapLocalPositionToParcel(
	const FVector2D& LocalPosition,
	const FVector2D& LocalSize)
{
	using namespace AMSimConstructionProposalGeometryPrivate;
	if (LocalSize.X <= 0.0 || LocalSize.Y <= 0.0)
	{
		return {};
	}
	const double NormalizedX = LocalPosition.X / LocalSize.X;
	const double NormalizedY = LocalPosition.Y / LocalSize.Y;
	return {
		Snap((NormalizedX - ParcelLeft) /
			(ParcelRight - ParcelLeft) * ParcelCentimeters),
		Snap((NormalizedY - ParcelTop) /
			(ParcelBottom - ParcelTop) * ParcelCentimeters)};
}

void UAMSimConstructionProposalView::TranslateProposal(
	AMSim::FStarterPlanProposal& Proposal,
	const int64 DeltaX,
	const int64 DeltaY)
{
	for (AMSim::FPhase1Point* Point : {
		&Proposal.RunwayStart,
		&Proposal.RunwayEnd,
		&Proposal.TaxiStart,
		&Proposal.TaxiEnd,
		&Proposal.AccessStart,
		&Proposal.AccessEnd})
	{
		Point->X += DeltaX;
		Point->Y += DeltaY;
	}
	for (AMSim::FTaxiwaySegment& Segment : Proposal.TaxiwaySegments)
	{
		Segment.Start.X += DeltaX;
		Segment.Start.Y += DeltaY;
		Segment.End.X += DeltaX;
		Segment.End.Y += DeltaY;
	}
}

AMSim::FStarterPlanProposal
UAMSimConstructionProposalView::TranslateToolPlacement(
	const AMSim::FStarterPlanProposal& Proposal,
	const EPlacementTool Tool,
	const int64 DeltaX,
	const int64 DeltaY,
	const int32 TaxiwaySegmentIndex)
{
	AMSim::FStarterPlanProposal Result = Proposal;
	switch (Tool)
	{
	case EPlacementTool::Runway:
		Result.RunwayStart.X += DeltaX;
		Result.RunwayStart.Y += DeltaY;
		Result.RunwayEnd.X += DeltaX;
		Result.RunwayEnd.Y += DeltaY;
		break;
	case EPlacementTool::Taxiway:
		if (Result.TaxiwaySegments.IsValidIndex(TaxiwaySegmentIndex))
		{
			AMSim::FTaxiwaySegment& Segment =
				Result.TaxiwaySegments[TaxiwaySegmentIndex];
			Segment.Start.X += DeltaX;
			Segment.Start.Y += DeltaY;
			Segment.End.X += DeltaX;
			Segment.End.Y += DeltaY;
			if (TaxiwaySegmentIndex == 0)
			{
				Result.TaxiStart = Segment.Start;
				Result.TaxiEnd = Segment.End;
			}
		}
		break;
	case EPlacementTool::RoadAccess:
		Result.AccessStart.X += DeltaX;
		Result.AccessStart.Y += DeltaY;
		Result.AccessEnd.X += DeltaX;
		Result.AccessEnd.Y += DeltaY;
		break;
	}
	return Result;
}

AMSim::FStarterPlanProposal
UAMSimConstructionProposalView::SetToolEndpoint(
	const AMSim::FStarterPlanProposal& Proposal,
	const EPlacementTool Tool,
	const int32 EndpointIndex,
	AMSim::FPhase1Point Point,
	const bool bAllowRunwaySnap,
	const bool bAllowGateSnap,
	const int32 TaxiwaySegmentIndex)
{
	using namespace AMSimConstructionProposalGeometryPrivate;
	AMSim::FStarterPlanProposal Result = Proposal;
	switch (Tool)
	{
	case EPlacementTool::Runway:
		(EndpointIndex == 0 ? Result.RunwayStart : Result.RunwayEnd) = Point;
		break;
	case EPlacementTool::Taxiway:
	{
		if (!Result.TaxiwaySegments.IsValidIndex(TaxiwaySegmentIndex))
		{
			if (Result.TaxiwaySegments.Num() >= MaximumTaxiwaySegments)
			{
				return Result;
			}
			Result.TaxiwaySegments.SetNum(TaxiwaySegmentIndex + 1);
		}
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
		if (bAllowRunwaySnap && Result.RunwayStart != Result.RunwayEnd)
		{
			Consider(AMSim::ClosestPointOnSegment(
				Point,
				Result.RunwayStart,
				Result.RunwayEnd));
		}
		if (bAllowGateSnap)
		{
			for (const AMSim::FPhase1Point Gate : AMSim::GetStarterGatePoints())
			{
				Consider(Gate);
			}
		}
		for (int32 Index = 0; Index < Result.TaxiwaySegments.Num(); ++Index)
		{
			if (Index != TaxiwaySegmentIndex)
			{
				Consider(AMSim::ClosestPointOnSegment(
					Point,
					Result.TaxiwaySegments[Index].Start,
					Result.TaxiwaySegments[Index].End));
			}
		}
		const int64 SnapDistanceSquared =
			ConnectionSnapCentimeters * ConnectionSnapCentimeters;
		if (BestDistance <= SnapDistanceSquared)
		{
			Point = BestPoint;
		}
		AMSim::FTaxiwaySegment& Segment =
			Result.TaxiwaySegments[TaxiwaySegmentIndex];
		(EndpointIndex == 0 ? Segment.Start : Segment.End) = Point;
		if (TaxiwaySegmentIndex == 0)
		{
			Result.TaxiStart = Segment.Start;
			Result.TaxiEnd = Segment.End;
		}
		break;
	}
	case EPlacementTool::RoadAccess:
		(EndpointIndex == 0 ? Result.AccessStart : Result.AccessEnd) = Point;
		break;
	}
	return Result;
}

TArray<UAMSimConstructionProposalView::FPlacementDiagnostic>
UAMSimConstructionProposalView::MakePlacementDiagnostics(
	const AMSim::FStarterPlanProposal& Proposal,
	const AMSim::FPhase1Validation& Validation)
{
	using namespace AMSimConstructionProposalGeometryPrivate;
	TArray<FPlacementDiagnostic> Result;
	if (Validation.bValid ||
		Validation.ReasonCode == FName(TEXT("Build.Placement.Incomplete")))
	{
		return Result;
	}
	const auto AddOutside = [&Result](
		const AMSim::FPhase1Point Point,
		const TCHAR* Label)
	{
		if (!IsInsideParcel(Point))
		{
			Result.Add({Point, Label});
		}
	};
	if (Validation.ReasonCode == FName(TEXT("Build.OutsideOwnedLand")))
	{
		AddOutside(Proposal.RunwayStart, TEXT("RUNWAY OUTSIDE PARCEL"));
		AddOutside(Proposal.RunwayEnd, TEXT("RUNWAY OUTSIDE PARCEL"));
		for (const AMSim::FTaxiwaySegment& Segment :
			AMSim::GetTaxiwaySegments(Proposal))
		{
			AddOutside(Segment.Start, TEXT("TAXIWAY OUTSIDE PARCEL"));
			AddOutside(Segment.End, TEXT("TAXIWAY OUTSIDE PARCEL"));
		}
		if (Proposal.AccessStart != Proposal.AccessEnd)
		{
			AddOutside(Proposal.AccessStart, TEXT("ROAD OUTSIDE PARCEL"));
			AddOutside(Proposal.AccessEnd, TEXT("ROAD OUTSIDE PARCEL"));
		}
		return Result;
	}
	if (Validation.ReasonCode == FName(TEXT("Build.Runway.TooSmall")))
	{
		Result.Add({Proposal.RunwayStart, TEXT("RUNWAY START")});
		Result.Add({Proposal.RunwayEnd, TEXT("600 M MINIMUM")});
		return Result;
	}
	if (Validation.ReasonCode == FName(TEXT("Build.Taxiway.TooShort")))
	{
		for (const AMSim::FTaxiwaySegment& Segment :
			AMSim::GetTaxiwaySegments(Proposal))
		{
			if (DistanceSquared(Segment.Start, Segment.End) < 1000ll * 1000ll)
			{
				Result.Add({Segment.End, TEXT("TAXIWAY 10 M MINIMUM")});
				break;
			}
		}
		return Result;
	}
	if (Validation.ReasonCode == FName(TEXT("Build.Network.NoRunwayConnection")))
	{
		int64 BestDistance = TNumericLimits<int64>::Max();
		AMSim::FPhase1Point BestPoint;
		for (const AMSim::FTaxiwaySegment& Segment :
			AMSim::GetTaxiwaySegments(Proposal))
		{
			for (const AMSim::FPhase1Point Endpoint : {Segment.Start, Segment.End})
			{
				const int64 CandidateDistance = DistanceSquared(
					Endpoint,
					AMSim::ClosestPointOnSegment(
						Endpoint,
						Proposal.RunwayStart,
						Proposal.RunwayEnd));
				if (CandidateDistance < BestDistance)
				{
					BestDistance = CandidateDistance;
					BestPoint = Endpoint;
				}
			}
		}
		Result.Add({BestPoint, TEXT("CONNECT TAXIWAY TO RUNWAY")});
	}
	else if (Validation.ReasonCode == FName(TEXT("Build.Network.NoGateConnection")))
	{
		int64 BestDistance = TNumericLimits<int64>::Max();
		AMSim::FPhase1Point BestPoint;
		for (const AMSim::FTaxiwaySegment& Segment :
			AMSim::GetTaxiwaySegments(Proposal))
		{
			for (const AMSim::FPhase1Point Endpoint : {Segment.Start, Segment.End})
			{
				for (const AMSim::FPhase1Point Gate : AMSim::GetStarterGatePoints())
				{
					const int64 CandidateDistance = DistanceSquared(Endpoint, Gate);
					if (CandidateDistance < BestDistance)
					{
						BestDistance = CandidateDistance;
						BestPoint = Endpoint;
					}
				}
			}
		}
		Result.Add({BestPoint, TEXT("CONTINUE TO GATE A OR B")});
	}
	else if (Validation.ReasonCode == FName(TEXT("Build.Network.OrphanSegment")))
	{
		const AMSim::FTaxiwayNetworkValidation Network =
			AMSim::ValidateTaxiwayNetwork(Proposal);
		const TArray<AMSim::FTaxiwaySegment> Segments =
			AMSim::GetTaxiwaySegments(Proposal);
		if (Segments.IsValidIndex(Network.FirstDisconnectedSegment))
		{
			Result.Add({
				Segments[Network.FirstDisconnectedSegment].Start,
				TEXT("JOIN THIS TAXIWAY TO THE NETWORK")});
		}
	}
	return Result;
}
