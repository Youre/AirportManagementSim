#include "AMSimWorldPresenter.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "PaperSpriteComponent.h"

namespace
{
	constexpr float SpritePlaneRoll = -90.0f;
	constexpr int32 MaximumTaxiwaySegments = 8;
	constexpr int32 MaximumMarkers = 32;

	void ApplySegment(
		UPaperSpriteComponent* Component,
		const AMSim::FPhase1WorldSegmentGeometry& Geometry,
		const FLinearColor& Tint)
	{
		if (!Component)
		{
			return;
		}
		Component->SetRelativeLocation(Geometry.Center);
		Component->SetRelativeScale3D(Geometry.Scale);
		Component->SetRelativeRotation(
			FRotator(0.0f, Geometry.YawDegrees, SpritePlaneRoll));
		Component->SetSpriteColor(Tint);
		Component->SetVisibility(true);
	}

	FLinearColor MarkerColor(
		const AMSim::EPhase1ConstructionMarkerStyle Style)
	{
		switch (Style)
		{
		case AMSim::EPhase1ConstructionMarkerStyle::Connection:
			return FLinearColor(0.98f, 0.72f, 0.18f, 0.92f);
		case AMSim::EPhase1ConstructionMarkerStyle::Pointer:
			return FLinearColor(0.35f, 0.91f, 0.96f, 1.0f);
		case AMSim::EPhase1ConstructionMarkerStyle::Invalid:
			return FLinearColor(0.96f, 0.30f, 0.23f, 1.0f);
		default:
			return FLinearColor(0.82f, 0.95f, 0.96f, 0.84f);
		}
	}
}

void AAMSimWorldPresenter::InitializePhase1ConstructionPreviewPresentation()
{
	Phase1ConstructionPreviewRunway = CreateSpriteComponent(
		TEXT("Phase1ConstructionPreviewRunway"),
		86);
	for (int32 Index = 0; Index < MaximumTaxiwaySegments; ++Index)
	{
		Phase1ConstructionPreviewTaxiways.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewTaxiway%d"), Index),
			87 + Index));
	}
	Phase1ConstructionPreviewRoad = CreateSpriteComponent(
		TEXT("Phase1ConstructionPreviewRoad"),
		86);
	for (int32 Index = 0; Index < MaximumMarkers; ++Index)
	{
		Phase1ConstructionPreviewMarkers.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewMarker%d"), Index),
			96 + Index));
	}
}

void AAMSimWorldPresenter::FinalizePhase1ConstructionPreviewPresentation()
{
	ConfigureSprite(
		Phase1ConstructionPreviewRunway,
		RunwaySprite,
		FVector::ZeroVector,
		FVector::OneVector);
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewTaxiways)
	{
		ConfigureSprite(Component, TaxiSprite, FVector::ZeroVector, FVector::OneVector);
	}
	ConfigureSprite(
		Phase1ConstructionPreviewRoad,
		AccessSprite,
		FVector::ZeroVector,
		FVector::OneVector);
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewMarkers)
	{
		ConfigureSprite(
			Component,
			SelectionSprite,
			FVector::ZeroVector,
			FVector(0.55f, 1.0f, 0.55f));
	}
	ClearPhase1ConstructionPreview();
}

void AAMSimWorldPresenter::SetPhase1ConstructionPreview(
	const AMSim::FPhase1ConstructionPreviewState& Preview)
{
	ClearPhase1ConstructionPreview();
	const FLinearColor SelectedTint(0.31f, 0.86f, 0.93f, 0.78f);
	const FLinearColor ReadyTint(0.36f, 0.82f, 0.48f, 0.78f);
	const FLinearColor ContextTint(0.68f, 0.88f, 0.91f, 0.58f);
	const FLinearColor DefaultTint = Preview.bValid ? ReadyTint : ContextTint;

	if (Preview.bRunwayVisible)
	{
		ApplySegment(
			Phase1ConstructionPreviewRunway,
			AMSim::MakePhase1WorldSegmentGeometry(
				Preview.Proposal.RunwayStart,
				Preview.Proposal.RunwayEnd,
				Preview.Proposal.RunwayWidthCentimeters,
				84.0),
			Preview.SelectedTool ==
				AMSim::EPhase1ConstructionPreviewTool::Runway
					? SelectedTint : DefaultTint);
	}

	const TArray<AMSim::FTaxiwaySegment> TaxiSegments =
		AMSim::GetTaxiwaySegments(Preview.Proposal);
	for (int32 Index = 0;
		Index < Phase1ConstructionPreviewTaxiways.Num();
		++Index)
	{
		if (!TaxiSegments.IsValidIndex(Index))
		{
			continue;
		}
		ApplySegment(
			Phase1ConstructionPreviewTaxiways[Index],
			AMSim::MakePhase1WorldSegmentGeometry(
				TaxiSegments[Index].Start,
				TaxiSegments[Index].End,
				1200,
				85.0 + Index),
			Preview.SelectedTool ==
					AMSim::EPhase1ConstructionPreviewTool::Taxiway &&
				Index == Preview.ActiveTaxiwaySegmentIndex
					? SelectedTint : DefaultTint);
	}

	if (Preview.bRoadVisible)
	{
		ApplySegment(
			Phase1ConstructionPreviewRoad,
			AMSim::MakePhase1WorldSegmentGeometry(
				Preview.Proposal.AccessStart,
				Preview.Proposal.AccessEnd,
				1000,
				84.0),
			Preview.SelectedTool ==
				AMSim::EPhase1ConstructionPreviewTool::RoadAccess
					? SelectedTint : ContextTint);
	}

	const int32 MarkerCount = FMath::Min(
		Preview.Markers.Num(),
		Phase1ConstructionPreviewMarkers.Num());
	for (int32 Index = 0; Index < MarkerCount; ++Index)
	{
		UPaperSpriteComponent* Component = Phase1ConstructionPreviewMarkers[Index];
		const AMSim::FPhase1ConstructionPreviewMarker& Marker =
			Preview.Markers[Index];
		Component->SetRelativeLocation(AMSim::MapPhase1PointToWorld(
			Marker.Point,
			96.0 + Index));
		const bool bEmphasis =
			Marker.Style != AMSim::EPhase1ConstructionMarkerStyle::Endpoint;
		Component->SetRelativeScale3D(
			FVector(bEmphasis ? 0.72f : 0.55f, 1.0f, bEmphasis ? 0.72f : 0.55f));
		Component->SetSpriteColor(MarkerColor(Marker.Style));
		Component->SetVisibility(true);
	}
}

void AAMSimWorldPresenter::ClearPhase1ConstructionPreview()
{
	if (Phase1ConstructionPreviewRunway)
	{
		Phase1ConstructionPreviewRunway->SetVisibility(false);
	}
	if (Phase1ConstructionPreviewRoad)
	{
		Phase1ConstructionPreviewRoad->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewTaxiways)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewMarkers)
	{
		Component->SetVisibility(false);
	}
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPreviewSurfaceCount() const
{
	int32 Result = Phase1ConstructionPreviewRunway &&
		Phase1ConstructionPreviewRunway->IsVisible() ? 1 : 0;
	Result += Phase1ConstructionPreviewRoad &&
		Phase1ConstructionPreviewRoad->IsVisible() ? 1 : 0;
	for (const UPaperSpriteComponent* Component : Phase1ConstructionPreviewTaxiways)
	{
		Result += Component && Component->IsVisible() ? 1 : 0;
	}
	return Result;
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPreviewMarkerCount() const
{
	int32 Result = 0;
	for (const UPaperSpriteComponent* Component : Phase1ConstructionPreviewMarkers)
	{
		Result += Component && Component->IsVisible() ? 1 : 0;
	}
	return Result;
}

FVector AAMSimWorldPresenter::GetPhase1ConstructionPreviewRunwayCenterForTest() const
{
	return Phase1ConstructionPreviewRunway
		? Phase1ConstructionPreviewRunway->GetRelativeLocation()
		: FVector::ZeroVector;
}

bool AAMSimWorldPresenter::IsStarterContextVisibleForTest() const
{
	return Stand && Stand->IsVisible() && GateB && GateB->IsVisible();
}
