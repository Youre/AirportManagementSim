#include "AMSimWorldPresenter.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "Components/TextRenderComponent.h"
#include "PaperSpriteComponent.h"

namespace
{
	constexpr float SpritePlaneRoll = -90.0f;
	constexpr double ConstructionBedHeight = 5.0;

	void ApplySegment(
		UPaperSpriteComponent* Component,
		const AMSim::FPhase1WorldSegmentGeometry& Geometry)
	{
		if (!Component)
		{
			return;
		}
		Component->SetRelativeLocation(Geometry.Center);
		Component->SetRelativeScale3D(Geometry.Scale);
		Component->SetRelativeRotation(
			FRotator(0.0f, Geometry.YawDegrees, SpritePlaneRoll));
	}

	FVector MapOffset(
		const AMSim::FPhase1Point& Point,
		const FVector2D& Offset,
		const double Height)
	{
		FVector Result = AMSim::MapPhase1PointToWorld(Point, Height);
		Result.X += Offset.X;
		Result.Y += Offset.Y;
		return Result;
	}
}

void AAMSimWorldPresenter::ApplyPhase1Geometry(
	const AMSim::FStarterPlanProposal& Proposal)
{
	const AMSim::FPhase1WorldSegmentGeometry RunwayGeometry =
		AMSim::MakePhase1WorldSegmentGeometry(
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			Proposal.RunwayWidthCentimeters,
			10.0);
	ApplySegment(Runway, RunwayGeometry);
	ApplySegment(
		Phase1RunwayEarthwork,
		AMSim::MakePhase1WorldSegmentGeometry(
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			Proposal.RunwayWidthCentimeters,
			ConstructionBedHeight));
	const TArray<AMSim::FTaxiwaySegment> TaxiSegments =
		AMSim::GetTaxiwaySegments(Proposal);
	ActivePhase1TaxiwaySegmentCount = FMath::Min(
		TaxiSegments.Num(),
		Phase1TaxiwaySegments.Num());
	for (int32 Index = 0; Index < Phase1TaxiwaySegments.Num(); ++Index)
	{
		const bool bHasSegment = TaxiSegments.IsValidIndex(Index);
		if (bHasSegment)
		{
			const AMSim::FPhase1WorldSegmentGeometry SegmentGeometry =
				AMSim::MakePhase1WorldSegmentGeometry(
					TaxiSegments[Index].Start,
					TaxiSegments[Index].End,
					1200,
					20.0 + Index);
			ApplySegment(Phase1TaxiwaySegments[Index], SegmentGeometry);
			if (Phase1TaxiwayEarthworks.IsValidIndex(Index))
			{
				ApplySegment(
					Phase1TaxiwayEarthworks[Index],
					AMSim::MakePhase1WorldSegmentGeometry(
						TaxiSegments[Index].Start,
						TaxiSegments[Index].End,
						1200,
						ConstructionBedHeight));
			}
		}
		Phase1TaxiwaySegments[Index]->SetVisibility(bHasSegment);
	}
	bPhase1RoadPresent = Proposal.AccessStart != Proposal.AccessEnd;
	if (bPhase1RoadPresent)
	{
		ApplySegment(
			Access,
			AMSim::MakePhase1WorldSegmentGeometry(
				Proposal.AccessStart,
				Proposal.AccessEnd,
				1000,
				20.0));
		ApplySegment(
			Phase1RoadEarthwork,
			AMSim::MakePhase1WorldSegmentGeometry(
				Proposal.AccessStart,
				Proposal.AccessEnd,
				1000,
				ConstructionBedHeight));
	}

	Phase1RunwayCenter = RunwayGeometry.Center;
	Phase1TaxiCenter = !TaxiSegments.IsEmpty()
		? AMSim::MakePhase1WorldSegmentGeometry(
			TaxiSegments[0].Start,
			TaxiSegments[0].End,
			1200,
			20.0).Center
		: RunwayGeometry.Center;
	Phase1StandCenter = AMSim::MapPhase1PointToWorld(
		Proposal.StandCenter,
		30.0);
	Phase1GeometryOffset = Phase1StandCenter -
		AMSim::MapPhase1PointToWorld(
			AMSim::CreateDefaultStarterPlan().StandCenter,
			30.0);

	if (Stand)
	{
		Stand->SetRelativeLocation(Phase1StandCenter);
		Stand->SetRelativeScale3D(FVector(7.5, 1.0, 7.5));
		Stand->SetRelativeRotation(FRotator(0.0f, 90.0f, SpritePlaneRoll));
	}
	if (GateB)
	{
		GateB->SetRelativeLocation(AMSim::MapPhase1PointToWorld(
			AMSim::GetStarterGatePoints()[1],
			31.0));
		GateB->SetRelativeScale3D(FVector(7.5, 1.0, 7.5));
		GateB->SetRelativeRotation(FRotator(0.0f, 90.0f, SpritePlaneRoll));
	}
	if (OperationsHut)
	{
		OperationsHut->SetRelativeLocation(AMSim::MapPhase1PointToWorld(
			AMSim::GetStarterTerminalCenter(),
			40.0));
		OperationsHut->SetRelativeScale3D(FVector(11.0, 1.0, 11.0));
		OperationsHut->SetRelativeRotation(
			FRotator(0.0f, 90.0f, SpritePlaneRoll));
	}

	const AMSim::FPhase1Point RunwayMidpoint = {
		(Proposal.RunwayStart.X + Proposal.RunwayEnd.X) / 2,
		(Proposal.RunwayStart.Y + Proposal.RunwayEnd.Y) / 2};
	if (Windsock)
	{
		Windsock->SetRelativeLocation(MapOffset(
			RunwayMidpoint,
			FVector2D(7000.0f, -7000.0f),
			45.0));
	}

	const AMSim::FRunwayDesignation Designation =
		AMSim::CalculateRunwayDesignation(
			Proposal.RunwayStart,
			Proposal.RunwayEnd);
	const auto PlaceNumber = [this, &RunwayGeometry](
		UTextRenderComponent* Component,
		const AMSim::FPhase1Point& Point,
		const int32 Number,
		const bool bReciprocal)
	{
		if (!Component)
		{
			return;
		}
		Component->SetText(FText::FromString(FString::Printf(TEXT("%02d"), Number)));
		Component->SetTextRenderColor(FColor(242, 239, 218));
		Component->SetHorizontalAlignment(EHTA_Center);
		Component->SetVerticalAlignment(EVRTA_TextCenter);
		Component->SetWorldSize(2500.0f);
		Component->SetRelativeLocation(AMSim::MapPhase1PointToWorld(Point, 52.0));
		Component->SetRelativeRotation(
			FRotator(
				90.0f,
				AMSim::MakePhase1RunwayNumberYawDegrees(
					RunwayGeometry.YawDegrees,
					bReciprocal),
				0.0f));
	};
	const auto InsetPoint = [&Proposal](const double Alpha)
	{
		return AMSim::FPhase1Point{
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Proposal.RunwayStart.X),
				static_cast<double>(Proposal.RunwayEnd.X),
				Alpha)),
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Proposal.RunwayStart.Y),
				static_cast<double>(Proposal.RunwayEnd.Y),
				Alpha))};
	};
	PlaceNumber(
		RunwayStartNumber,
		InsetPoint(0.10),
		Designation.PrimaryNumber,
		false);
	PlaceNumber(
		RunwayEndNumber,
		InsetPoint(0.90),
		Designation.ReciprocalNumber,
		true);

	const auto Place = [](UPaperSpriteComponent* Component, const FVector& Location)
	{
		if (Component)
		{
			Component->SetRelativeLocation(Location);
		}
	};
	Place(Selection, MapOffset(Proposal.StandCenter, FVector2D::ZeroVector, 70.0));
	Place(InspectionMarker, MapOffset(Proposal.StandCenter, FVector2D(-4500.0f, 0.0f), 72.0));
	Place(FuelMarker, MapOffset(Proposal.StandCenter, FVector2D(4500.0f, 0.0f), 72.0));

	const FVector ConstructionLocations[] = {
		RunwayGeometry.Center + FVector(-8000.0, 6000.0, 47.0),
		RunwayGeometry.Center + FVector(4000.0, -5000.0, 46.0),
		Phase1StandCenter + FVector(-4000.0, -3500.0, 45.0),
		Phase1StandCenter + FVector(4500.0, 3500.0, 44.0)};
	for (int32 Index = 0;
		Index < Phase1ConstructionProxies.Num() &&
			Index < UE_ARRAY_COUNT(ConstructionLocations);
		++Index)
	{
		Place(Phase1ConstructionProxies[Index], ConstructionLocations[Index]);
	}
	const FVector RunwayStartWorld = AMSim::MapPhase1PointToWorld(
		Proposal.RunwayStart,
		53.0);
	const FVector RunwayEndWorld = AMSim::MapPhase1PointToWorld(
		Proposal.RunwayEnd,
		53.0);
	const FVector RunwayDirection =
		(RunwayEndWorld - RunwayStartWorld).GetSafeNormal2D();
	const FVector RunwayNormal(-RunwayDirection.Y, RunwayDirection.X, 0.0);
	const FVector BoundaryLocations[] = {
		RunwayStartWorld + RunwayNormal * 2200.0,
		RunwayStartWorld - RunwayNormal * 2200.0,
		RunwayEndWorld + RunwayNormal * 2200.0,
		RunwayEndWorld - RunwayNormal * 2200.0};
	for (int32 Index = 0;
		Index < Phase1ConstructionBoundaryMarkers.Num() &&
			Index < UE_ARRAY_COUNT(BoundaryLocations);
		++Index)
	{
		Place(Phase1ConstructionBoundaryMarkers[Index], BoundaryLocations[Index]);
	}

	Place(TurnaroundFuelTruck, Phase1StandCenter + FVector(5000.0, 3500.0, 43.0));
	Place(TurnaroundRampWorker, Phase1StandCenter + FVector(-3500.0, -4200.0, 44.0));
	Place(TurnaroundSafetyCones, Phase1StandCenter + FVector(-4500.0, 4200.0, 41.0));
	Place(TurnaroundSafetyZone, Phase1StandCenter + FVector::UpVector * 38.0);
	const FVector ApproachLocations[] = {
		Phase1StandCenter + FVector(6000.0, 0.0, 39.0),
		Phase1StandCenter + FVector(0.0, -6000.0, 40.0)};
	for (int32 Index = 0;
		Index < TurnaroundApproachPaths.Num() &&
			Index < UE_ARRAY_COUNT(ApproachLocations);
		++Index)
	{
		Place(TurnaroundApproachPaths[Index], ApproachLocations[Index]);
	}
}

void AAMSimWorldPresenter::SetFacilitiesVisible(
	const bool bNetworkVisible,
	const bool bOperational,
	const bool bStarterContextVisible)
{
	bRequestedPhase1NetworkVisible = bNetworkVisible;
	bRequestedPhase1Operational = bOperational;
	bRequestedStarterContextVisible = bStarterContextVisible;
	const bool bShowNetwork = bNetworkVisible;
	const bool bShowContext = bStarterContextVisible || bNetworkVisible;
	Runway->SetVisibility(bShowNetwork);
	for (int32 Index = 0; Index < Phase1TaxiwaySegments.Num(); ++Index)
	{
		Phase1TaxiwaySegments[Index]->SetVisibility(
			bShowNetwork && Index < ActivePhase1TaxiwaySegmentCount);
	}
	Access->SetVisibility(bShowNetwork && bPhase1RoadPresent);
	Windsock->SetVisibility(bShowNetwork);
	RunwayStartNumber->SetVisibility(bShowNetwork);
	RunwayEndNumber->SetVisibility(bShowNetwork);

	Stand->SetVisibility(bShowContext);
	GateB->SetVisibility(bShowContext);
	const bool bHasSpatialTerminal =
		bPhase1AirportInitialized &&
		!CachedTerminalLayoutSnapshot.FloorCells.IsEmpty();
	OperationsHut->SetVisibility(bShowContext && !bHasSpatialTerminal);

	const FLinearColor ProposalTint(0.24f, 0.68f, 0.74f, 0.52f);
	const FLinearColor NetworkTint = bOperational
		? FLinearColor::White
		: ProposalTint;
	Runway->SetSpriteColor(NetworkTint);
	for (UPaperSpriteComponent* Component : Phase1TaxiwaySegments)
	{
		Component->SetSpriteColor(NetworkTint);
	}
	Access->SetSpriteColor(NetworkTint);
	Windsock->SetSpriteColor(NetworkTint);
	Stand->SetSpriteColor(FLinearColor::White);
	GateB->SetSpriteColor(FLinearColor::White);
	OperationsHut->SetSpriteColor(FLinearColor::White);
}

FVector AAMSimWorldPresenter::GetStarterTerminalScaleForTest() const
{
	return OperationsHut
		? OperationsHut->GetRelativeScale3D()
		: FVector::ZeroVector;
}

FVector AAMSimWorldPresenter::GetStarterGateScaleForTest() const
{
	return Stand ? Stand->GetRelativeScale3D() : FVector::ZeroVector;
}

float AAMSimWorldPresenter::GetStarterTerminalYawForTest() const
{
	return OperationsHut
		? OperationsHut->GetRelativeRotation().Yaw
		: 0.0f;
}

float AAMSimWorldPresenter::GetStarterGateYawForTest() const
{
	return Stand ? Stand->GetRelativeRotation().Yaw : 0.0f;
}

void AAMSimWorldPresenter::SetConstructionEditorOverlayVisible(
	const bool bVisible)
{
	if (!bVisible)
	{
		ClearPhase1ConstructionPreview();
	}
	SetFacilitiesVisible(
		bRequestedPhase1NetworkVisible,
		bRequestedPhase1Operational,
		bRequestedStarterContextVisible);
}
