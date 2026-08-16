#include "AMSimWorldPresenter.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "AMSimWorldPresentationLayers.h"
#include "Materials/MaterialInterface.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float SpritePlaneRoll = -90.0f;
	constexpr int32 MaximumTaxiwaySegments = 8;
	constexpr int32 MaximumMarkers = 32;
	constexpr int32 MaximumOutlines = 40;
	constexpr int32 MaximumPatternDashes = 128;
	constexpr int32 MajorGridLineCount = 22;
	constexpr int32 ParcelBoundaryLineCount = 4;
	constexpr int64 MajorGridSpacingCentimeters = 10000;
	constexpr int64 ParcelExtentCentimeters = 100000;
	constexpr int64 OutlineWidthCentimeters = 180;
	constexpr int64 PatternDashSpacingCentimeters = 4000;
	constexpr float GeometryReferenceSpriteWorldSize = 1600.0f;

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
		FVector SpriteAdjustedScale = Geometry.Scale;
		if (const UPaperSprite* Sprite = Component->GetSprite())
		{
			const FVector SpriteWorldSize =
				Sprite->GetRenderBounds().BoxExtent * 2.0;
			if (SpriteWorldSize.X > KINDA_SMALL_NUMBER &&
				SpriteWorldSize.Z > KINDA_SMALL_NUMBER)
			{
				SpriteAdjustedScale.X *=
					GeometryReferenceSpriteWorldSize / SpriteWorldSize.X;
				SpriteAdjustedScale.Z *=
					GeometryReferenceSpriteWorldSize / SpriteWorldSize.Z;
			}
		}
		Component->SetRelativeScale3D(SpriteAdjustedScale);
		Component->SetRelativeRotation(
			FRotator(0.0f, Geometry.YawDegrees, SpritePlaneRoll));
		Component->SetSpriteColor(Tint);
		Component->SetVisibility(true);
	}

	void HideComponents(
		const TArray<TObjectPtr<UPaperSpriteComponent>>& Components,
		const int32 FirstUnused = 0)
	{
		for (int32 Index = FirstUnused; Index < Components.Num(); ++Index)
		{
			if (Components[Index])
			{
				Components[Index]->SetVisibility(false);
			}
		}
	}

	int32 CountVisible(
		const TArray<TObjectPtr<UPaperSpriteComponent>>& Components)
	{
		int32 Result = 0;
		for (const UPaperSpriteComponent* Component : Components)
		{
			Result += Component && Component->IsVisible() ? 1 : 0;
		}
		return Result;
	}

	FLinearColor SurfaceTint(
		const AMSim::EPhase1ConstructionSurfaceStyle Style)
	{
		switch (Style)
		{
		case AMSim::EPhase1ConstructionSurfaceStyle::Selected:
			return FLinearColor(0.08f, 0.56f, 0.64f, 0.32f);
		case AMSim::EPhase1ConstructionSurfaceStyle::Invalid:
			return FLinearColor(0.86f, 0.12f, 0.08f, 0.34f);
		default:
			return FLinearColor(0.08f, 0.38f, 0.45f, 0.24f);
		}
	}

	FLinearColor PatternTint(
		const AMSim::EPhase1ConstructionSurfaceStyle Style)
	{
		return Style == AMSim::EPhase1ConstructionSurfaceStyle::Invalid
			? FLinearColor(0.95f, 0.15f, 0.12f, 0.80f)
			: Style == AMSim::EPhase1ConstructionSurfaceStyle::Selected
				? FLinearColor(0.12f, 0.82f, 0.90f, 0.72f)
				: FLinearColor(0.14f, 0.62f, 0.70f, 0.55f);
	}

	AMSim::FPhase1Point OffsetPoint(
		const AMSim::FPhase1Point& Point,
		const FVector2D& Offset)
	{
		return {
			Point.X + FMath::RoundToInt64(Offset.X),
			Point.Y + FMath::RoundToInt64(Offset.Y)};
	}

	AMSim::FPhase1Point PointAt(
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const double Alpha)
	{
		return {
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Start.X),
				static_cast<double>(End.X),
				Alpha)),
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Start.Y),
				static_cast<double>(End.Y),
				Alpha))};
	}

	void AddOutline(
		const TArray<TObjectPtr<UPaperSpriteComponent>>& Components,
		int32& NextIndex,
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const int64 WidthCentimeters,
		const FLinearColor& Tint)
	{
		const FVector2D Delta(
			static_cast<double>(End.X - Start.X),
			static_cast<double>(End.Y - Start.Y));
		const double Length = Delta.Size();
		if (Length <= KINDA_SMALL_NUMBER || NextIndex + 4 > Components.Num())
		{
			return;
		}
		const FVector2D Normal(-Delta.Y / Length, Delta.X / Length);
		const FVector2D HalfWidth = Normal *
			(static_cast<double>(WidthCentimeters) * 0.5);
		const AMSim::FPhase1Point StartLeft = OffsetPoint(Start, HalfWidth);
		const AMSim::FPhase1Point StartRight = OffsetPoint(Start, -HalfWidth);
		const AMSim::FPhase1Point EndLeft = OffsetPoint(End, HalfWidth);
		const AMSim::FPhase1Point EndRight = OffsetPoint(End, -HalfWidth);
		for (const TPair<AMSim::FPhase1Point, AMSim::FPhase1Point>& Edge : {
			TPair<AMSim::FPhase1Point, AMSim::FPhase1Point>(StartLeft, EndLeft),
			TPair<AMSim::FPhase1Point, AMSim::FPhase1Point>(StartRight, EndRight),
			TPair<AMSim::FPhase1Point, AMSim::FPhase1Point>(StartLeft, StartRight),
			TPair<AMSim::FPhase1Point, AMSim::FPhase1Point>(EndLeft, EndRight)})
		{
			ApplySegment(
				Components[NextIndex++],
				AMSim::MakePhase1WorldSegmentGeometry(
					Edge.Key,
					Edge.Value,
					OutlineWidthCentimeters,
					AMSim::WorldPresentationLayers::ProposalPattern.Height),
				Tint);
		}
	}

	void AddPatternDashes(
		const TArray<TObjectPtr<UPaperSpriteComponent>>& Components,
		int32& NextIndex,
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const int64 SurfaceWidthCentimeters,
		const FLinearColor& Tint)
	{
		const double Length = FVector2D(
			static_cast<double>(End.X - Start.X),
			static_cast<double>(End.Y - Start.Y)).Size();
		if (Length <= KINDA_SMALL_NUMBER || NextIndex >= Components.Num())
		{
			return;
		}
		const FVector2D Direction(
			static_cast<double>(End.X - Start.X) / Length,
			static_cast<double>(End.Y - Start.Y) / Length);
		const FVector2D Normal(-Direction.Y, Direction.X);
		const int32 DashCount = FMath::Max(
			1,
			FMath::FloorToInt(Length / PatternDashSpacingCentimeters));
		const FVector2D HalfAcross =
			Normal * static_cast<double>(SurfaceWidthCentimeters) * 0.34;
		const FVector2D HalfAlong =
			Direction * static_cast<double>(SurfaceWidthCentimeters) * 0.20;
		const int64 DashWidth = 120;
		for (int32 DashIndex = 0;
			DashIndex < DashCount && NextIndex < Components.Num();
			++DashIndex)
		{
			const double CenterAlpha =
				(static_cast<double>(DashIndex) + 0.5) / DashCount;
			const AMSim::FPhase1Point Center =
				PointAt(Start, End, CenterAlpha);
			ApplySegment(
				Components[NextIndex++],
				AMSim::MakePhase1WorldSegmentGeometry(
					OffsetPoint(Center, -HalfAcross - HalfAlong),
					OffsetPoint(Center, HalfAcross + HalfAlong),
					DashWidth,
					AMSim::WorldPresentationLayers::ProposalPattern.Height + 0.1),
				Tint);
		}
	}

	bool IsBelowStarterFacilities(const UPaperSpriteComponent* Component)
	{
		return !Component || !Component->IsVisible() ||
			(Component->TranslucencySortPriority <
				AMSim::WorldPresentationLayers::GateA.SortPriority &&
			 Component->GetRelativeLocation().Z <
				AMSim::WorldPresentationLayers::GateA.Height);
	}

	bool IsAboveStarterFacilities(const UPaperSpriteComponent* Component)
	{
		return !Component || !Component->IsVisible() ||
			(Component->TranslucencySortPriority >
				AMSim::WorldPresentationLayers::TerminalRoof.SortPriority &&
			 Component->GetRelativeLocation().Z >
				AMSim::WorldPresentationLayers::TerminalRoof.Height);
	}
}

void AAMSimWorldPresenter::InitializePhase1ConstructionPreviewPresentation()
{
	Phase1ConstructionPreviewRunway = CreateSpriteComponent(
		TEXT("Phase1ConstructionPreviewRunway"),
		AMSim::WorldPresentationLayers::Runway.SortPriority);
	for (int32 Index = 0; Index < MaximumTaxiwaySegments; ++Index)
	{
		Phase1ConstructionPreviewTaxiways.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewTaxiway%d"), Index),
			AMSim::WorldPresentationLayers::Taxiway.SortPriority));
	}
	Phase1ConstructionPreviewRoad = CreateSpriteComponent(
		TEXT("Phase1ConstructionPreviewRoad"),
		AMSim::WorldPresentationLayers::ServiceRoad.SortPriority);
	for (int32 Index = 0; Index < MaximumOutlines; ++Index)
	{
		Phase1ConstructionPreviewOutlines.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewOutline%d"), Index),
			AMSim::WorldPresentationLayers::ProposalPattern.SortPriority));
	}
	for (int32 Index = 0; Index < MaximumPatternDashes; ++Index)
	{
		Phase1ConstructionPreviewPattern.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewPattern%d"), Index),
			AMSim::WorldPresentationLayers::ProposalPattern.SortPriority));
	}
	for (int32 Index = 0;
		Index < MajorGridLineCount + ParcelBoundaryLineCount;
		++Index)
	{
		Phase1ConstructionPlanningGrid.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPlanningGrid%d"), Index),
			Index < MajorGridLineCount
				? AMSim::WorldPresentationLayers::PlanningGrid.SortPriority
				: AMSim::WorldPresentationLayers::ParcelBoundary.SortPriority));
	}
	for (int32 Index = 0; Index < MaximumMarkers; ++Index)
	{
		Phase1ConstructionPreviewMarkers.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionPreviewMarker%d"), Index),
			AMSim::WorldPresentationLayers::ProposalMarker.SortPriority));
	}
}

void AAMSimWorldPresenter::LoadPhase1ConstructionPreviewAssets()
{
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Snap(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementSnap.S_PlacementSnap"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Connected(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementValid.S_PlacementValid"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Crossing(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementConnection.S_PlacementConnection"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Invalid(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementBlocked.S_PlacementBlocked"));
	ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> PreviewMaterial(
		TEXT("/Paper2D/TranslucentUnlitSpriteMaterial.TranslucentUnlitSpriteMaterial"));
	ConstructionSnapSprite = Snap.Get();
	ConstructionConnectedSprite = Connected.Get();
	ConstructionCrossingSprite = Crossing.Get();
	ConstructionInvalidSprite = Invalid.Get();
	ConstructionPreviewMaterial = PreviewMaterial.Get();
}

void AAMSimWorldPresenter::FinalizePhase1ConstructionPreviewPresentation()
{
	ConfigureSprite(
		Phase1ConstructionPreviewRunway,
		WhiteSprite,
		FVector::ZeroVector,
		FVector::OneVector);
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewTaxiways)
	{
		ConfigureSprite(Component, WhiteSprite, FVector::ZeroVector, FVector::OneVector);
	}
	ConfigureSprite(
		Phase1ConstructionPreviewRoad,
		WhiteSprite,
		FVector::ZeroVector,
		FVector::OneVector);
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewOutlines)
	{
		ConfigureSprite(Component, WhiteSprite, FVector::ZeroVector, FVector::OneVector);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewPattern)
	{
		ConfigureSprite(Component, WhiteSprite, FVector::ZeroVector, FVector::OneVector);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPlanningGrid)
	{
		ConfigureSprite(Component, WhiteSprite, FVector::ZeroVector, FVector::OneVector);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewMarkers)
	{
		ConfigureSprite(
			Component,
			SelectionSprite,
			FVector::ZeroVector,
			FVector::OneVector);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewOutlines)
	{
		Component->SetMaterial(0, ConstructionPreviewMaterial);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewPattern)
	{
		Component->SetMaterial(0, ConstructionPreviewMaterial);
	}
	for (UPaperSpriteComponent* Component : Phase1ConstructionPlanningGrid)
	{
		Component->SetMaterial(0, ConstructionPreviewMaterial);
	}
	Phase1ConstructionPreviewRunway->SetMaterial(0, ConstructionPreviewMaterial);
	Phase1ConstructionPreviewRoad->SetMaterial(0, ConstructionPreviewMaterial);
	for (UPaperSpriteComponent* Component : Phase1ConstructionPreviewTaxiways)
	{
		Component->SetMaterial(0, ConstructionPreviewMaterial);
	}
	ClearPhase1ConstructionPreview();
}

void AAMSimWorldPresenter::SetPhase1ConstructionPreview(
	const AMSim::FPhase1ConstructionPreviewState& Preview)
{
	ClearPhase1ConstructionPreview();

	int32 GridIndex = 0;
	const FLinearColor GridTint(0.34f, 0.72f, 0.78f, 0.08f);
	for (int64 Coordinate = 0;
		Coordinate <= ParcelExtentCentimeters;
		Coordinate += MajorGridSpacingCentimeters)
	{
		ApplySegment(
			Phase1ConstructionPlanningGrid[GridIndex++],
			AMSim::MakePhase1WorldSegmentGeometry(
				{Coordinate, 0},
				{Coordinate, ParcelExtentCentimeters},
				180,
				AMSim::WorldPresentationLayers::PlanningGrid.Height),
			GridTint);
		ApplySegment(
			Phase1ConstructionPlanningGrid[GridIndex++],
			AMSim::MakePhase1WorldSegmentGeometry(
				{0, Coordinate},
				{ParcelExtentCentimeters, Coordinate},
				180,
				AMSim::WorldPresentationLayers::PlanningGrid.Height),
			GridTint);
	}
	const FLinearColor BoundaryTint(0.30f, 0.87f, 0.94f, 0.28f);
	const AMSim::FPhase1Point BoundaryStarts[] = {
		{0, 0},
		{ParcelExtentCentimeters, 0},
		{ParcelExtentCentimeters, ParcelExtentCentimeters},
		{0, ParcelExtentCentimeters}};
	const AMSim::FPhase1Point BoundaryEnds[] = {
		{ParcelExtentCentimeters, 0},
		{ParcelExtentCentimeters, ParcelExtentCentimeters},
		{0, ParcelExtentCentimeters},
		{0, 0}};
	for (int32 EdgeIndex = 0; EdgeIndex < ParcelBoundaryLineCount; ++EdgeIndex)
	{
		ApplySegment(
			Phase1ConstructionPlanningGrid[GridIndex++],
			AMSim::MakePhase1WorldSegmentGeometry(
				BoundaryStarts[EdgeIndex],
				BoundaryEnds[EdgeIndex],
				220,
				AMSim::WorldPresentationLayers::ParcelBoundary.Height),
			BoundaryTint);
	}

	int32 OutlineIndex = 0;
	int32 PatternIndex = 0;
	const auto ApplyPreviewSurface = [this, &OutlineIndex, &PatternIndex](
		UPaperSpriteComponent* Component,
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const int64 WidthCentimeters,
		const double Height,
		const AMSim::EPhase1ConstructionSurfaceStyle Style)
	{
		ApplySegment(
			Component,
			AMSim::MakePhase1WorldSegmentGeometry(
				Start, End, WidthCentimeters, Height),
			SurfaceTint(Style));
		AddOutline(
			Phase1ConstructionPreviewOutlines,
			OutlineIndex,
			Start,
			End,
			WidthCentimeters,
			PatternTint(Style));
		AddPatternDashes(
			Phase1ConstructionPreviewPattern,
			PatternIndex,
			Start,
			End,
			WidthCentimeters,
			PatternTint(Style));
	};

	if (Preview.bRunwayVisible)
	{
		ApplyPreviewSurface(
			Phase1ConstructionPreviewRunway,
			Preview.Proposal.RunwayStart,
			Preview.Proposal.RunwayEnd,
			Preview.Proposal.RunwayWidthCentimeters,
			AMSim::WorldPresentationLayers::Runway.Height,
			Preview.RunwayStyle);
	}

	const TArray<AMSim::FTaxiwaySegment> TaxiSegments =
		AMSim::GetTaxiwaySegments(Preview.Proposal);
	for (int32 Index = 0;
		Index < Phase1ConstructionPreviewTaxiways.Num() &&
			TaxiSegments.IsValidIndex(Index);
		++Index)
	{
		const AMSim::EPhase1ConstructionSurfaceStyle Style =
			Preview.TaxiwayStyles.IsValidIndex(Index)
				? Preview.TaxiwayStyles[Index]
				: Preview.SelectedTool ==
						AMSim::EPhase1ConstructionPreviewTool::Taxiway &&
					Index == Preview.ActiveTaxiwaySegmentIndex
					? AMSim::EPhase1ConstructionSurfaceStyle::Selected
					: AMSim::EPhase1ConstructionSurfaceStyle::Context;
		ApplyPreviewSurface(
			Phase1ConstructionPreviewTaxiways[Index],
			TaxiSegments[Index].Start,
			TaxiSegments[Index].End,
			1200,
			AMSim::WorldPresentationLayers::Taxiway.Height + Index * 0.05,
			Style);
	}

	if (Preview.bRoadVisible)
	{
		ApplyPreviewSurface(
			Phase1ConstructionPreviewRoad,
			Preview.Proposal.AccessStart,
			Preview.Proposal.AccessEnd,
			1000,
			AMSim::WorldPresentationLayers::ServiceRoad.Height,
			Preview.RoadStyle);
	}
	HideComponents(Phase1ConstructionPreviewOutlines, OutlineIndex);
	HideComponents(Phase1ConstructionPreviewPattern, PatternIndex);

	const int32 MarkerCount = FMath::Min(
		Preview.Markers.Num(),
		Phase1ConstructionPreviewMarkers.Num());
	for (int32 Index = 0; Index < MarkerCount; ++Index)
	{
		UPaperSpriteComponent* Component = Phase1ConstructionPreviewMarkers[Index];
		const AMSim::FPhase1ConstructionPreviewMarker& Marker = Preview.Markers[Index];
		UPaperSprite* MarkerSprite = SelectionSprite;
		float ScaleMultiplier = 1.0f;
		switch (Marker.Style)
		{
		case AMSim::EPhase1ConstructionMarkerStyle::Connection:
			MarkerSprite = ConstructionSnapSprite;
			ScaleMultiplier = 1.45f;
			break;
		case AMSim::EPhase1ConstructionMarkerStyle::Snapped:
			MarkerSprite = ConstructionConnectedSprite;
			ScaleMultiplier = 1.45f;
			break;
		case AMSim::EPhase1ConstructionMarkerStyle::Crossing:
			MarkerSprite = ConstructionCrossingSprite;
			ScaleMultiplier = 1.45f;
			break;
		case AMSim::EPhase1ConstructionMarkerStyle::Invalid:
			MarkerSprite = ConstructionInvalidSprite;
			ScaleMultiplier = 1.35f;
			break;
		case AMSim::EPhase1ConstructionMarkerStyle::Pointer:
			MarkerSprite = ConstructionSnapSprite;
			ScaleMultiplier = 1.30f;
			break;
		default:
			break;
		}
		Component->SetSprite(MarkerSprite);
		Component->SetRelativeLocation(AMSim::MapPhase1PointToWorld(
			Marker.Point,
			AMSim::WorldPresentationLayers::ProposalMarker.Height + Index * 0.01));
		const float Scale = FMath::Clamp(Preview.MarkerScale, 2.4f, 5.5f) *
			ScaleMultiplier;
		Component->SetRelativeScale3D(FVector(Scale, 1.0f, Scale));
		Component->SetSpriteColor(FLinearColor::White);
		Component->SetVisibility(true);
	}
	HideComponents(Phase1ConstructionPreviewMarkers, MarkerCount);
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
	HideComponents(Phase1ConstructionPreviewTaxiways);
	HideComponents(Phase1ConstructionPreviewOutlines);
	HideComponents(Phase1ConstructionPreviewPattern);
	HideComponents(Phase1ConstructionPlanningGrid);
	HideComponents(Phase1ConstructionPreviewMarkers);
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPreviewSurfaceCount() const
{
	int32 Result = Phase1ConstructionPreviewRunway &&
		Phase1ConstructionPreviewRunway->IsVisible() ? 1 : 0;
	Result += Phase1ConstructionPreviewRoad &&
		Phase1ConstructionPreviewRoad->IsVisible() ? 1 : 0;
	return Result + CountVisible(Phase1ConstructionPreviewTaxiways);
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPreviewMarkerCount() const
{
	return CountVisible(Phase1ConstructionPreviewMarkers);
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPreviewPatternCount() const
{
	return CountVisible(Phase1ConstructionPreviewOutlines) +
		CountVisible(Phase1ConstructionPreviewPattern);
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionPlanningGridCount() const
{
	return CountVisible(Phase1ConstructionPlanningGrid);
}

bool AAMSimWorldPresenter::HasPhase1ConstructionPreviewMarkerAssets() const
{
	return SelectionSprite && ConstructionSnapSprite &&
		ConstructionConnectedSprite && ConstructionCrossingSprite &&
		ConstructionInvalidSprite;
}

bool AAMSimWorldPresenter::ArePhase1ConstructionPreviewSurfacesBelowStarterFacilities() const
{
	if (!IsBelowStarterFacilities(Phase1ConstructionPreviewRunway) ||
		!IsBelowStarterFacilities(Phase1ConstructionPreviewRoad))
	{
		return false;
	}
	for (const TArray<TObjectPtr<UPaperSpriteComponent>>* Components : {
		&Phase1ConstructionPreviewTaxiways,
		&Phase1ConstructionPreviewOutlines,
		&Phase1ConstructionPreviewPattern})
	{
		for (const UPaperSpriteComponent* Component : *Components)
		{
			if (!IsBelowStarterFacilities(Component))
			{
				return false;
			}
		}
	}
	return true;
}

bool AAMSimWorldPresenter::ArePhase1ConstructionPreviewMarkersAboveStarterFacilities() const
{
	for (const UPaperSpriteComponent* Component : Phase1ConstructionPreviewMarkers)
	{
		if (!IsAboveStarterFacilities(Component))
		{
			return false;
		}
	}
	return true;
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
