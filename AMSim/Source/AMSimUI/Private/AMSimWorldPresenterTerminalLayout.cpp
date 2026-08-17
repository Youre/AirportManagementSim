#include "AMSimWorldPresenter.h"

#include "Algo/Count.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "AMSimTerminalPresentationGeometry.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float TerminalFurnitureFootprintFill = 0.84f;
	constexpr float TerminalSeatSpacingCells = 0.82f;
	// Retained roof assets share the spatial footprint but remain hidden. The
	// roofless interior is the terminal's only runtime world representation.
	constexpr float SpritePlaneRoll = -90.0f;
	// Terminal furniture source art is authored upright in screen space, with
	// its front facing the bottom of the texture. The terminal grid swaps its
	// logical axes into world X/Y, so a zero-turn object otherwise appears one
	// quarter-turn counter-clockwise in the top-down camera. Keep this as a
	// presentation-basis correction; saved/player-authored quarter turns remain
	// authoritative and unchanged.
	constexpr float TerminalObjectArtYawCorrectionDegrees = 90.0f;

	UPaperSprite* FindTerminalSprite(const TCHAR* ObjectPath)
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Finder(ObjectPath);
		return Finder.Get();
	}

	FVector CellWorldPosition(
		const AMSim::FTerminalCellCoord Cell,
		const FVector Center,
		const int32 MinimumX,
		const int32 MinimumY,
		const int32 MaximumX,
		const int32 MaximumY,
		const float Height)
	{
		const float GridUnits =
			AMSim::TerminalPresentationGeometry::CellWorldUnits;
		const float Width = (MaximumX - MinimumX + 1) * GridUnits;
		const float Depth = (MaximumY - MinimumY + 1) * GridUnits;
		return FVector(
			Center.X + (Cell.Y - MinimumY + 0.5f) * GridUnits - Depth * 0.5f,
			Center.Y + (Cell.X - MinimumX + 0.5f) * GridUnits - Width * 0.5f,
			Height);
	}

	void CalculateLayoutBounds(
		const AMSim::FTerminalLayoutQuerySnapshot& Query,
		int32& MinimumX,
		int32& MinimumY,
		int32& MaximumX,
		int32& MaximumY)
	{
		MinimumX = 0;
		MinimumY = 0;
		MaximumX = 0;
		MaximumY = 0;
		if (Query.FloorCells.IsEmpty())
		{
			return;
		}
		MinimumX = MaximumX = Query.FloorCells[0].Cell.X;
		MinimumY = MaximumY = Query.FloorCells[0].Cell.Y;
		for (const AMSim::FTerminalFloorCellRecord& Cell : Query.FloorCells)
		{
			MinimumX = FMath::Min(MinimumX, Cell.Cell.X);
			MinimumY = FMath::Min(MinimumY, Cell.Cell.Y);
			MaximumX = FMath::Max(MaximumX, Cell.Cell.X);
			MaximumY = FMath::Max(MaximumY, Cell.Cell.Y);
		}
	}

	bool IsTerminalSeatGroup(const AMSim::ETerminalObjectKind Kind)
	{
		return Kind == AMSim::ETerminalObjectKind::SeatGroup2 ||
			Kind == AMSim::ETerminalObjectKind::SeatGroup4 ||
			Kind == AMSim::ETerminalObjectKind::SeatGroup6;
	}

	FIntPoint GetTerminalSeatPattern(const AMSim::ETerminalObjectKind Kind)
	{
		switch (Kind)
		{
		case AMSim::ETerminalObjectKind::SeatGroup2:
			return {2, 1};
		case AMSim::ETerminalObjectKind::SeatGroup4:
			return {2, 2};
		case AMSim::ETerminalObjectKind::SeatGroup6:
			return {3, 2};
		default:
			return {0, 0};
		}
	}

	FVector CalculateTerminalObjectScale(
		const UPaperSprite* Sprite,
		const int32 FootprintWidth,
		const int32 FootprintHeight,
		const int32 QuarterTurns)
	{
		if (!Sprite)
		{
			return FVector(
				AMSim::TerminalPresentationGeometry::CellSpriteScale,
				1.0f,
				AMSim::TerminalPresentationGeometry::CellSpriteScale);
		}
		const FBoxSphereBounds RenderBounds = Sprite->GetRenderBounds();
		float SourceWidthUnits = RenderBounds.BoxExtent.X * 2.0f;
		float SourceHeightUnits = RenderBounds.BoxExtent.Z * 2.0f;
		if ((FMath::Abs(QuarterTurns) % 2) != 0)
		{
			Swap(SourceWidthUnits, SourceHeightUnits);
		}
		const float AvailableWidth =
			FMath::Max(FootprintWidth, 1) *
			AMSim::TerminalPresentationGeometry::FurnitureWorldUnitsPerLogicalCell *
			TerminalFurnitureFootprintFill;
		const float AvailableHeight =
			FMath::Max(FootprintHeight, 1) *
			AMSim::TerminalPresentationGeometry::FurnitureWorldUnitsPerLogicalCell *
			TerminalFurnitureFootprintFill;
		const float UniformScale = FMath::Min(
			AvailableWidth / FMath::Max(SourceWidthUnits, KINDA_SMALL_NUMBER),
			AvailableHeight / FMath::Max(SourceHeightUnits, KINDA_SMALL_NUMBER));
		return FVector(UniformScale, 1.0f, UniformScale);
	}
}

void AAMSimWorldPresenter::InitializeTerminalLayoutPresentation()
{
	TerminalPublicFloorSprite = FindTerminalSprite(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Surface/S_PublicFloor.S_PublicFloor"));
	TerminalServiceFloorSprite = FindTerminalSprite(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Surface/S_ServiceFloor.S_ServiceFloor"));
	TerminalRoofSurfaceSprite = FindTerminalSprite(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Surface/S_RoofSurface.S_RoofSurface"));
	TerminalRoofDetailSprites = {
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Roof/S_RoofEdge.S_RoofEdge")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Roof/S_RoofCorner.S_RoofCorner")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Roof/S_EntranceCanopy.S_EntranceCanopy")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Roof/S_RoofHVAC.S_RoofHVAC"))};
	TerminalEdgeSprites = {
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Structure/S_ExteriorWall.S_ExteriorWall")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Structure/S_InteriorWall.S_InteriorWall")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Structure/S_GlassWall.S_GlassWall")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Door/S_EntranceDoor.S_EntranceDoor")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Door/S_StandardDoor.S_StandardDoor")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Door/S_ServiceDoor.S_ServiceDoor")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Door/S_AirsideGateDoor.S_AirsideGateDoor"))};
	TerminalSeatSprite = FindTerminalSprite(
		TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_SeatSingle.S_SeatSingle"));
	TerminalObjectSprites = {
		nullptr,
		nullptr,
		nullptr,
		FindTerminalSprite(TEXT("/Game/Phase45/Presentation/Sprites/Terminal/S_InformationDesk.S_InformationDesk")),
		FindTerminalSprite(TEXT("/Game/Phase45/Presentation/Sprites/Terminal/S_RestroomBlock.S_RestroomBlock")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_StaffDesk.S_StaffDesk")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Storage.S_Storage")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Vending.S_Vending")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Water.S_Water")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_WasteRecycling.S_WasteRecycling")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Noticeboard.S_Noticeboard")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Signage.S_Signage")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_Plant.S_Plant")),
		FindTerminalSprite(TEXT("/Game/Phase45/Presentation/Sprites/Terminal/S_ProtectionZone.S_ProtectionZone"))};
	TerminalConstructionSprites = {
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionClosure.S_ConstructionClosure")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionDelivery.S_ConstructionDelivery")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionFoundation.S_ConstructionFoundation")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionFraming.S_ConstructionFraming")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionActive.S_ConstructionActive")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Construction/S_ConstructionInspection.S_ConstructionInspection"))};
	TerminalVisitorSprite = FindTerminalSprite(
		TEXT("/Game/Phase45/Presentation/Sprites/Operations/S_Passenger.S_Passenger"));
	TerminalConstructionWorkerSprite = FindTerminalSprite(
		TEXT("/Game/Phase45/Presentation/Sprites/Operations/S_ConstructionWorker.S_ConstructionWorker"));
	TerminalPlacementOverlaySprites = {
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementValid.S_PlacementValid")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementBlocked.S_PlacementBlocked")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementUnaffordable.S_PlacementUnaffordable")),
		FindTerminalSprite(TEXT("/Game/TerminalGrowth/Presentation/Sprites/Overlay/S_PlacementRouteLoss.S_PlacementRouteLoss"))};

}

UPaperSpriteComponent* AAMSimWorldPresenter::AcquireTerminalLayoutProxy(
	TArray<TObjectPtr<UPaperSpriteComponent>>& Pool,
	const int32 Index,
	const TCHAR* Prefix,
	const int32 SortPriority)
{
	while (Pool.Num() <= Index)
	{
		const FName ComponentName = MakeUniqueObjectName(
			this,
			UPaperSpriteComponent::StaticClass(),
			FName(*FString::Printf(TEXT("%s%d"), Prefix, Pool.Num())));
		UPaperSpriteComponent* Component = NewObject<UPaperSpriteComponent>(
			this,
			ComponentName,
			RF_Transient);
		Component->SetupAttachment(Root);
		Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Component->SetTranslucentSortPriority(SortPriority);
		Component->SetRelativeRotation(FRotator(0.0f, 0.0f, SpritePlaneRoll));
		Component->RegisterComponent();
		AddInstanceComponent(Component);
		Pool.Add(Component);
	}
	return Pool[Index];
}

void AAMSimWorldPresenter::HideTerminalLayoutPool(
	TArray<TObjectPtr<UPaperSpriteComponent>>& Pool,
	const int32 FirstUnusedIndex)
{
	for (int32 Index = FirstUnusedIndex; Index < Pool.Num(); ++Index)
	{
		Pool[Index]->SetVisibility(false);
	}
}

FVector AAMSimWorldPresenter::GetTerminalWorldCenter() const
{
	// The terminal remains fixed map context. The presentation-only campus
	// offset places the full-size roofless footprint between the apron and the
	// landside access road; it never depends on the player's mutable runway
	// proposal.
	FVector Center = AMSim::MapPhase1PointToWorld(
		AMSim::GetStarterTerminalCenter(),
		40.0);
	Center.X += AMSim::TerminalPresentationGeometry::CampusOffsetWorldX;
	return Center;
}

void AAMSimWorldPresenter::SetTerminalInteractionMode(const bool bEnabled)
{
	if (bTerminalInteractionMode == bEnabled)
	{
		return;
	}
	bTerminalInteractionMode = bEnabled;
	if (!bEnabled)
	{
		ClearTerminalPlacementPreview();
	}
}

int32 AAMSimWorldPresenter::GetActiveTerminalFloorProxyCount() const
{
	return Algo::CountIf(
		TerminalLayoutFloorProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

int32 AAMSimWorldPresenter::GetActiveTerminalRoofProxyCount() const
{
	return Algo::CountIf(
		TerminalLayoutRoofProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

FVector AAMSimWorldPresenter::GetTerminalFloorProxyScaleForTest() const
{
	return TerminalLayoutFloorProxies.IsEmpty()
		? FVector::ZeroVector
		: TerminalLayoutFloorProxies[0]->GetRelativeScale3D();
}

FVector2D AAMSimWorldPresenter::GetTerminalFloorWorldSizeForTest() const
{
	bool bHasFloor = false;
	FVector2D Minimum = FVector2D::ZeroVector;
	FVector2D Maximum = FVector2D::ZeroVector;
	for (const UPaperSpriteComponent* Floor : TerminalLayoutFloorProxies)
	{
		if (!Floor || !Floor->IsVisible())
		{
			continue;
		}
		const FVector Location = Floor->GetRelativeLocation();
		const FVector2D Point(Location.X, Location.Y);
		if (!bHasFloor)
		{
			Minimum = Maximum = Point;
			bHasFloor = true;
		}
		else
		{
			Minimum.X = FMath::Min(Minimum.X, Point.X);
			Minimum.Y = FMath::Min(Minimum.Y, Point.Y);
			Maximum.X = FMath::Max(Maximum.X, Point.X);
			Maximum.Y = FMath::Max(Maximum.Y, Point.Y);
		}
	}
	return bHasFloor
		? Maximum - Minimum + FVector2D(
			AMSim::TerminalPresentationGeometry::CellWorldUnits)
		: FVector2D::ZeroVector;
}

FVector AAMSimWorldPresenter::GetTerminalRoofProxyScaleForTest() const
{
	return TerminalLayoutRoofProxies.IsEmpty()
		? FVector::ZeroVector
		: TerminalLayoutRoofProxies[0]->GetRelativeScale3D();
}

FVector AAMSimWorldPresenter::GetTerminalFloorProxyLocationForTest() const
{
	return TerminalLayoutFloorProxies.IsEmpty()
		? FVector::ZeroVector
		: TerminalLayoutFloorProxies[0]->GetRelativeLocation();
}

FVector AAMSimWorldPresenter::GetTerminalRoofProxyLocationForTest() const
{
	return TerminalLayoutRoofProxies.IsEmpty()
		? FVector::ZeroVector
		: TerminalLayoutRoofProxies[0]->GetRelativeLocation();
}

bool AAMSimWorldPresenter::IsLegacyStarterTerminalVisibleForTest() const
{
	return OperationsHut && OperationsHut->IsVisible();
}

bool AAMSimWorldPresenter::IsLegacyMatureTerminalVisibleForTest() const
{
	return MatureSite.IsValidIndex(4) && MatureSite[4]->IsVisible();
}

float AAMSimWorldPresenter::GetTerminalObjectProxyYawForTest(
	const int32 ObjectIndex) const
{
	return TerminalLayoutObjectProxies.IsValidIndex(ObjectIndex) &&
		TerminalLayoutObjectProxies[ObjectIndex]
		? TerminalLayoutObjectProxies[ObjectIndex]->GetRelativeRotation().Yaw
		: 0.0f;
}

FVector AAMSimWorldPresenter::GetTerminalObjectProxyScaleForTest(
	const int32 ObjectIndex) const
{
	return TerminalLayoutObjectProxies.IsValidIndex(ObjectIndex) &&
		TerminalLayoutObjectProxies[ObjectIndex]
		? TerminalLayoutObjectProxies[ObjectIndex]->GetRelativeScale3D()
		: FVector::ZeroVector;
}

int32 AAMSimWorldPresenter::GetActiveTerminalSeatProxyCount() const
{
	return Algo::CountIf(
		TerminalLayoutSeatProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

FVector AAMSimWorldPresenter::GetTerminalSeatProxyScaleForTest(
	const int32 SeatIndex) const
{
	return TerminalLayoutSeatProxies.IsValidIndex(SeatIndex) &&
		TerminalLayoutSeatProxies[SeatIndex]
		? TerminalLayoutSeatProxies[SeatIndex]->GetRelativeScale3D()
		: FVector::ZeroVector;
}

float AAMSimWorldPresenter::GetTerminalSeatProxyYawForTest(
	const int32 SeatIndex) const
{
	return TerminalLayoutSeatProxies.IsValidIndex(SeatIndex) &&
		TerminalLayoutSeatProxies[SeatIndex]
		? TerminalLayoutSeatProxies[SeatIndex]->GetRelativeRotation().Yaw
		: 0.0f;
}

int32 AAMSimWorldPresenter::GetActiveTerminalConstructionProxyCount() const
{
	return Algo::CountIf(
		TerminalLayoutConstructionProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

int32 AAMSimWorldPresenter::GetActiveTerminalPlacementPreviewCount() const
{
	return Algo::CountIf(
		TerminalPlacementPreviewProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

void AAMSimWorldPresenter::SetTerminalPlacementPreview(
	const AMSim::FTerminalCellCoord& Start,
	const AMSim::FTerminalCellCoord& End,
	const int32 VisualState)
{
	if (!bTerminalInteractionMode || CachedTerminalLayoutSnapshot.FloorCells.IsEmpty())
	{
		ClearTerminalPlacementPreview();
		return;
	}
	int32 MinimumX = 0;
	int32 MinimumY = 0;
	int32 MaximumX = 0;
	int32 MaximumY = 0;
	CalculateLayoutBounds(CachedTerminalLayoutSnapshot,
		MinimumX, MinimumY, MaximumX, MaximumY);
	const int32 MinX = FMath::Min(Start.X, End.X);
	const int32 MaxX = FMath::Max(Start.X, End.X);
	const int32 MinY = FMath::Min(Start.Y, End.Y);
	const int32 MaxY = FMath::Max(Start.Y, End.Y);
	const int32 SpriteIndex = FMath::Clamp(VisualState - 1, 0, 3);
	int32 PreviewIndex = 0;
	for (int32 Y = MinY; Y <= MaxY && PreviewIndex < 1024; ++Y)
	{
		for (int32 X = MinX; X <= MaxX && PreviewIndex < 1024; ++X)
		{
			UPaperSpriteComponent* Preview = AcquireTerminalLayoutProxy(
				TerminalPlacementPreviewProxies, PreviewIndex++,
				TEXT("TerminalPlacementPreview"), 70);
			ConfigureSprite(Preview,
				TerminalPlacementOverlaySprites.IsValidIndex(SpriteIndex)
					? TerminalPlacementOverlaySprites[SpriteIndex].Get()
					: WhiteSprite.Get(),
				CellWorldPosition({X, Y}, GetTerminalWorldCenter(),
					MinimumX, MinimumY, MaximumX, MaximumY, 70.0f),
				FVector(
					AMSim::TerminalPresentationGeometry::CellSpriteScale,
					1.0f,
					AMSim::TerminalPresentationGeometry::CellSpriteScale));
			Preview->SetSpriteColor(FLinearColor::White);
			Preview->SetVisibility(true);
		}
	}
	HideTerminalLayoutPool(TerminalPlacementPreviewProxies, PreviewIndex);
}

void AAMSimWorldPresenter::ClearTerminalPlacementPreview()
{
	HideTerminalLayoutPool(TerminalPlacementPreviewProxies, 0);
}

void AAMSimWorldPresenter::RefreshTerminalLayoutPresentation(
	const AMSim::FTerminalLayoutQuerySnapshot& Query,
	const AMSim::FPhase3State& State,
	const bool bForce)
{
	if (!bForce && LastAppliedTerminalLayoutRevision == Query.Revision)
	{
		return;
	}
	LastAppliedTerminalLayoutRevision = Query.Revision;
	// A restored schema-10 terminal is authoritative on its own. Do not depend
	// on a preceding Phase 1 presentation pass: mature saves intentionally skip
	// some starter-world rendering, and loading one directly from the new-
	// airport screen; direct restore must leave the persistent interior complete.
	const bool bShowTerminal = !Query.FloorCells.IsEmpty();
	if (!bShowTerminal)
	{
		HideTerminalLayoutPool(TerminalLayoutFloorProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutRoofProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutRoofDetailProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutEdgeProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutObjectProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutSeatProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutConstructionProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutWorkerProxies, 0);
		HideTerminalLayoutPool(TerminalLayoutVisitorProxies, 0);
		for (UPaperSpriteComponent* Component : Phase3Passengers)
		{
			Component->SetVisibility(false);
		}
		for (UPaperSpriteComponent* Component : Phase3Bags)
		{
			Component->SetVisibility(false);
		}
		for (UPaperSpriteComponent* Component : Phase3Vehicles)
		{
			Component->SetVisibility(false);
		}
		for (UPaperSpriteComponent* Component : Phase3Staff)
		{
			Component->SetVisibility(false);
		}
		if (Phase3Aircraft)
		{
			Phase3Aircraft->SetVisibility(false);
		}
		ClearTerminalPlacementPreview();
		return;
	}
	if (OperationsHut)
	{
		OperationsHut->SetVisibility(false);
	}

	int32 MinimumX = 0;
	int32 MinimumY = 0;
	int32 MaximumX = 0;
	int32 MaximumY = 0;
	CalculateLayoutBounds(Query, MinimumX, MinimumY, MaximumX, MaximumY);
	const FVector Center = GetTerminalWorldCenter();

	int32 FloorIndex = 0;
	int32 RoofIndex = 0;
	for (const AMSim::FTerminalFloorCellRecord& Cell : Query.FloorCells)
	{
		UPaperSpriteComponent* Floor = AcquireTerminalLayoutProxy(
			TerminalLayoutFloorProxies, FloorIndex++, TEXT("TerminalLayoutFloor"), 41);
		const FVector Location = CellWorldPosition(
			Cell.Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY, 42.0f);
		UPaperSprite* FloorSprite =
			Cell.Kind == AMSim::ETerminalFloorKind::StaffService ||
			Cell.Kind == AMSim::ETerminalFloorKind::Baggage
				? TerminalServiceFloorSprite.Get()
				: TerminalPublicFloorSprite.Get();
		ConfigureSprite(Floor, FloorSprite, Location,
			FVector(
				AMSim::TerminalPresentationGeometry::CellSpriteScale,
				1.0f,
				AMSim::TerminalPresentationGeometry::CellSpriteScale));
		const FLinearColor BuiltTint =
			Cell.Kind == AMSim::ETerminalFloorKind::Restroom
				? FLinearColor(0.73f, 0.94f, 0.91f, 1.0f)
				: Cell.Kind == AMSim::ETerminalFloorKind::EntranceThreshold
					? FLinearColor(1.0f, 0.83f, 0.52f, 1.0f)
					: Cell.Kind == AMSim::ETerminalFloorKind::StaffService
						? FLinearColor(0.78f, 0.88f, 0.91f, 1.0f)
						: Cell.Kind == AMSim::ETerminalFloorKind::Sterile
							? FLinearColor(0.82f, 0.96f, 1.0f, 1.0f)
							: Cell.Kind == AMSim::ETerminalFloorKind::Baggage
								? FLinearColor(0.92f, 0.80f, 0.55f, 1.0f)
								: FLinearColor(1.0f, 0.93f, 0.78f, 1.0f);
		Floor->SetSpriteColor(Cell.bBuilt
			? BuiltTint
			: FLinearColor(0.48f, 0.62f, 0.60f, 0.52f));
		Floor->SetVisibility(true);

		if (Cell.bBuilt)
		{
			UPaperSpriteComponent* Roof = AcquireTerminalLayoutProxy(
				TerminalLayoutRoofProxies, RoofIndex++, TEXT("TerminalLayoutRoof"), 47);
			const FVector RoofLocation = CellWorldPosition(
				Cell.Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY,
				47.0f);
			ConfigureSprite(Roof, TerminalRoofSurfaceSprite, RoofLocation,
				FVector(
					AMSim::TerminalPresentationGeometry::CellSpriteScale,
					1.0f,
					AMSim::TerminalPresentationGeometry::CellSpriteScale));
			Roof->SetSpriteColor(FLinearColor::White);
			Roof->SetVisibility(false);
		}
	}
	HideTerminalLayoutPool(TerminalLayoutFloorProxies, FloorIndex);
	HideTerminalLayoutPool(TerminalLayoutRoofProxies, RoofIndex);
	int32 RoofDetailIndex = 0;
	const auto HasFloor = [&Query](const AMSim::FTerminalCellCoord Cell)
	{
		return Query.FloorCells.ContainsByPredicate(
			[Cell](const AMSim::FTerminalFloorCellRecord& Entry)
			{
				return Entry.Cell == Cell && Entry.bBuilt;
			});
	};
	for (const AMSim::FTerminalFloorCellRecord& Cell : Query.FloorCells)
	{
		if (!Cell.bBuilt)
		{
			continue;
		}
		const AMSim::FTerminalCellCoord Neighbors[] = {
			{Cell.Cell.X - 1, Cell.Cell.Y}, {Cell.Cell.X + 1, Cell.Cell.Y},
			{Cell.Cell.X, Cell.Cell.Y - 1}, {Cell.Cell.X, Cell.Cell.Y + 1}};
		int32 ExposedEdgeCount = 0;
		for (int32 Direction = 0; Direction < UE_ARRAY_COUNT(Neighbors); ++Direction)
		{
			if (HasFloor(Neighbors[Direction]))
			{
				continue;
			}
			++ExposedEdgeCount;
			UPaperSpriteComponent* Edge = AcquireTerminalLayoutProxy(
				TerminalLayoutRoofDetailProxies, RoofDetailIndex++,
				TEXT("TerminalRoofEdge"), 49);
			ConfigureSprite(Edge, TerminalRoofDetailSprites[0],
				CellWorldPosition(Cell.Cell, Center,
					MinimumX, MinimumY, MaximumX, MaximumY, 49.0f),
				FVector(
					AMSim::TerminalPresentationGeometry::CellSpriteScale,
					1.0f,
					AMSim::TerminalPresentationGeometry::CellSpriteScale));
			Edge->SetRelativeRotation(FRotator(
				0.0f, Direction < 2 ? 90.0f : 0.0f, SpritePlaneRoll));
			Edge->SetVisibility(false);
		}
		if (ExposedEdgeCount >= 2)
		{
			UPaperSpriteComponent* Corner = AcquireTerminalLayoutProxy(
				TerminalLayoutRoofDetailProxies, RoofDetailIndex++,
				TEXT("TerminalRoofCorner"), 50);
			ConfigureSprite(Corner, TerminalRoofDetailSprites[1],
				CellWorldPosition(Cell.Cell, Center,
					MinimumX, MinimumY, MaximumX, MaximumY, 50.0f),
				FVector(
					AMSim::TerminalPresentationGeometry::CellSpriteScale,
					1.0f,
					AMSim::TerminalPresentationGeometry::CellSpriteScale));
			Corner->SetVisibility(false);
		}
		if (((Cell.Cell.X * 31 + Cell.Cell.Y * 17) & 63) == 0)
		{
			UPaperSpriteComponent* HVAC = AcquireTerminalLayoutProxy(
				TerminalLayoutRoofDetailProxies, RoofDetailIndex++,
				TEXT("TerminalRoofHVAC"), 51);
			ConfigureSprite(HVAC, TerminalRoofDetailSprites[3],
				CellWorldPosition(Cell.Cell, Center,
					MinimumX, MinimumY, MaximumX, MaximumY, 51.0f),
				FVector(
					AMSim::TerminalPresentationGeometry::CellSpriteScale,
					1.0f,
					AMSim::TerminalPresentationGeometry::CellSpriteScale));
			HVAC->SetVisibility(false);
		}
	}
	if (const AMSim::FTerminalEdgeRecord* Entrance = Query.Edges.FindByPredicate(
		[](const AMSim::FTerminalEdgeRecord& Edge)
		{
			return Edge.Kind == AMSim::ETerminalEdgeKind::EntranceDoor && Edge.bBuilt;
		}))
	{
		UPaperSpriteComponent* Canopy = AcquireTerminalLayoutProxy(
			TerminalLayoutRoofDetailProxies, RoofDetailIndex++,
			TEXT("TerminalRoofCanopy"), 52);
		ConfigureSprite(Canopy, TerminalRoofDetailSprites[2],
			CellWorldPosition(Entrance->From, Center,
				MinimumX, MinimumY, MaximumX, MaximumY, 52.0f),
			FVector(
				AMSim::TerminalPresentationGeometry::CellSpriteScale * 1.5f,
				1.0f,
				AMSim::TerminalPresentationGeometry::CellSpriteScale * 1.5f));
		Canopy->SetVisibility(false);
	}
	HideTerminalLayoutPool(TerminalLayoutRoofDetailProxies, RoofDetailIndex);

	int32 EdgeIndex = 0;
	for (const AMSim::FTerminalEdgeRecord& Edge : Query.Edges)
	{
		UPaperSpriteComponent* Proxy = AcquireTerminalLayoutProxy(
			TerminalLayoutEdgeProxies, EdgeIndex++, TEXT("TerminalLayoutEdge"), 48);
		const FVector From = CellWorldPosition(
			Edge.From, Center, MinimumX, MinimumY, MaximumX, MaximumY, 48.0f);
		const FVector To = CellWorldPosition(
			Edge.To, Center, MinimumX, MinimumY, MaximumX, MaximumY, 48.0f);
		const int32 SpriteIndex = FMath::Clamp(static_cast<int32>(Edge.Kind), 0, 6);
		ConfigureSprite(Proxy, TerminalEdgeSprites.IsValidIndex(SpriteIndex)
			? TerminalEdgeSprites[SpriteIndex].Get() : WhiteSprite.Get(),
			(From + To) * 0.5f,
			FVector(
				AMSim::TerminalPresentationGeometry::CellSpriteScale,
				1.0f,
				AMSim::TerminalPresentationGeometry::CellSpriteScale));
		const FVector Delta = To - From;
		Proxy->SetRelativeRotation(FRotator(
			0.0f,
			FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X)),
			SpritePlaneRoll));
		Proxy->SetSpriteColor(Edge.bLocallyClosed
			? FLinearColor(1.0f, 0.45f, 0.26f, 0.78f)
			: Edge.bBuilt ? FLinearColor::White : FLinearColor(0.42f, 0.84f, 0.88f, 0.55f));
		Proxy->SetVisibility(true);
	}
	HideTerminalLayoutPool(TerminalLayoutEdgeProxies, EdgeIndex);

	int32 ObjectIndex = 0;
	int32 SeatIndex = 0;
	for (const AMSim::FTerminalPlacedObjectRecord& Object : Query.Objects)
	{
		const FVector ObjectCenter = [&Object, &Center,
			MinimumX, MinimumY, MaximumX, MaximumY]()
		{
			FVector Location = CellWorldPosition(
				Object.Anchor, Center, MinimumX, MinimumY, MaximumX, MaximumY, 52.0f);
			Location.X += (FMath::Max(Object.FootprintHeight, 1) - 1) *
				AMSim::TerminalPresentationGeometry::CellWorldUnits * 0.5f;
			Location.Y += (FMath::Max(Object.FootprintWidth, 1) - 1) *
				AMSim::TerminalPresentationGeometry::CellWorldUnits * 0.5f;
			return Location;
		}();
		if (IsTerminalSeatGroup(Object.Kind))
		{
			const FIntPoint Pattern = GetTerminalSeatPattern(Object.Kind);
			const int32 QuarterTurns = ((Object.QuarterTurns % 4) + 4) % 4;
			for (int32 Row = 0; Row < Pattern.Y; ++Row)
			{
				for (int32 Column = 0; Column < Pattern.X; ++Column)
				{
					const float LocalX =
						(Column - (Pattern.X - 1) * 0.5f) * TerminalSeatSpacingCells;
					const float LocalY =
						(Row - (Pattern.Y - 1) * 0.5f) * TerminalSeatSpacingCells;
					float RotatedX = LocalX;
					float RotatedY = LocalY;
					switch (QuarterTurns)
					{
					case 1: RotatedX = -LocalY; RotatedY = LocalX; break;
					case 2: RotatedX = -LocalX; RotatedY = -LocalY; break;
					case 3: RotatedX = LocalY; RotatedY = -LocalX; break;
					default: break;
					}
					UPaperSpriteComponent* SeatProxy = AcquireTerminalLayoutProxy(
						TerminalLayoutSeatProxies, SeatIndex++, TEXT("TerminalLayoutSeat"), 52);
					ConfigureSprite(
						SeatProxy,
						TerminalSeatSprite ? TerminalSeatSprite.Get() : WhiteSprite.Get(),
						ObjectCenter + FVector(
							RotatedY * AMSim::TerminalPresentationGeometry::
								FurnitureWorldUnitsPerLogicalCell,
							RotatedX * AMSim::TerminalPresentationGeometry::
								FurnitureWorldUnitsPerLogicalCell,
							0.0f),
						CalculateTerminalObjectScale(TerminalSeatSprite.Get(), 1, 1, 0));
					const float RowFacing = Pattern.Y > 1 && Row == 1 ? 180.0f : 0.0f;
					SeatProxy->SetRelativeRotation(FRotator(
						0.0f,
						TerminalObjectArtYawCorrectionDegrees +
							QuarterTurns * 90.0f + RowFacing,
						SpritePlaneRoll));
					SeatProxy->SetSpriteColor(Object.bLocallyClosed
						? FLinearColor(1.0f, 0.45f, 0.26f, 0.70f)
						: Object.bOperational
							? FLinearColor::White
							: FLinearColor(0.55f, 0.72f, 0.72f, 0.62f));
					SeatProxy->SetVisibility(true);
				}
			}
			continue;
		}
		UPaperSpriteComponent* Proxy = AcquireTerminalLayoutProxy(
			TerminalLayoutObjectProxies, ObjectIndex++, TEXT("TerminalLayoutObject"), 52);
		const int32 SpriteIndex = FMath::Clamp(static_cast<int32>(Object.Kind), 0, 13);
		UPaperSprite* ObjectSprite = TerminalObjectSprites.IsValidIndex(SpriteIndex)
			? TerminalObjectSprites[SpriteIndex].Get()
			: WhiteSprite.Get();
		ConfigureSprite(
			Proxy,
			ObjectSprite,
			ObjectCenter,
			CalculateTerminalObjectScale(
				ObjectSprite,
				Object.FootprintWidth,
				Object.FootprintHeight,
				Object.QuarterTurns));
		Proxy->SetRelativeRotation(FRotator(
			0.0f,
			TerminalObjectArtYawCorrectionDegrees +
				Object.QuarterTurns * 90.0f,
			SpritePlaneRoll));
		Proxy->SetSpriteColor(Object.bLocallyClosed
			? FLinearColor(1.0f, 0.45f, 0.26f, 0.70f)
			: Object.bOperational ? FLinearColor::White : FLinearColor(0.55f, 0.72f, 0.72f, 0.62f));
		Proxy->SetVisibility(true);
	}
	HideTerminalLayoutPool(TerminalLayoutObjectProxies, ObjectIndex);
	HideTerminalLayoutPool(TerminalLayoutSeatProxies, SeatIndex);

	int32 ConstructionIndex = 0;
	int32 WorkerIndex = 0;
	for (const AMSim::FTerminalConstructionJobRecord& Job : Query.ConstructionJobs)
	{
		if (Job.Stage == AMSim::ETerminalConstructionWorkStage::Completed ||
			Job.Stage == AMSim::ETerminalConstructionWorkStage::Cancelled)
		{
			continue;
		}
		UPaperSpriteComponent* Proxy = AcquireTerminalLayoutProxy(
			TerminalLayoutConstructionProxies, ConstructionIndex++,
			TEXT("TerminalLayoutConstruction"), 57);
		const int32 StageIndex = FMath::Clamp(static_cast<int32>(Job.Stage), 0, 5);
		ConfigureSprite(Proxy, TerminalConstructionSprites.IsValidIndex(StageIndex)
			? TerminalConstructionSprites[StageIndex].Get() : WhiteSprite.Get(),
			CellWorldPosition(Job.WorkCell, Center,
				MinimumX, MinimumY, MaximumX, MaximumY, 57.0f),
			FVector(
				AMSim::TerminalPresentationGeometry::CellSpriteScale,
				1.0f,
				AMSim::TerminalPresentationGeometry::CellSpriteScale));
		Proxy->SetSpriteColor(FLinearColor::White);
		Proxy->SetVisibility(true);

		UPaperSpriteComponent* Worker = AcquireTerminalLayoutProxy(
			TerminalLayoutWorkerProxies, WorkerIndex++,
			TEXT("TerminalLayoutWorker"), 61);
		const float MotionPhase = static_cast<float>(Job.ProgressPercent) / 100.0f;
		const FVector WorkLocation = CellWorldPosition(Job.WorkCell, Center,
			MinimumX, MinimumY, MaximumX, MaximumY, 61.0f);
		const FVector MotionOffset(
			FMath::Sin(MotionPhase * UE_TWO_PI) * 42.0f *
				AMSim::TerminalPresentationGeometry::IdentityScale,
			FMath::Cos(MotionPhase * UE_TWO_PI) * 32.0f *
				AMSim::TerminalPresentationGeometry::IdentityScale,
			0.0f);
		ConfigureSprite(Worker, TerminalConstructionWorkerSprite,
			WorkLocation + MotionOffset,
			FVector(
				0.32f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.32f * AMSim::TerminalPresentationGeometry::IdentityScale));
		Worker->SetRelativeRotation(FRotator(
			0.0f,
			FMath::RadiansToDegrees(FMath::Atan2(MotionOffset.Y, MotionOffset.X)) + 90.0f,
			SpritePlaneRoll));
		Worker->SetSpriteColor(FLinearColor::White);
		Worker->SetVisibility(true);
	}
	HideTerminalLayoutPool(TerminalLayoutConstructionProxies, ConstructionIndex);
	HideTerminalLayoutPool(TerminalLayoutWorkerProxies, WorkerIndex);

	int32 VisitorIndex = 0;
	for (const AMSim::FTerminalVisitorRecord& Visitor : Query.Visitors)
	{
		if (!Visitor.bActive)
		{
			continue;
		}
		UPaperSpriteComponent* Proxy = AcquireTerminalLayoutProxy(
			TerminalLayoutVisitorProxies, VisitorIndex++,
			TEXT("TerminalLayoutVisitor"), 62);
		ConfigureSprite(Proxy, TerminalVisitorSprite,
			CellWorldPosition(Visitor.Cell, Center,
				MinimumX, MinimumY, MaximumX, MaximumY, 62.0f),
			FVector(
				0.34f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.34f * AMSim::TerminalPresentationGeometry::IdentityScale));
		Proxy->SetSpriteColor(Visitor.bRequiresAccessibleRoute
			? FLinearColor(1.0f, 0.88f, 0.28f, 1.0f)
			: FLinearColor::White);
		Proxy->SetVisibility(true);
	}
	HideTerminalLayoutPool(TerminalLayoutVisitorProxies, VisitorIndex);

	{
		for (UPaperSpriteComponent* Component : Phase3Floor) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3Rooms) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3SecurityBoundary) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3Props) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3DepartureFlow) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3ArrivalFlow) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3LandsideFlow) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3BaggageFlow) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3AccessibleFlow) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3AccessibleDashes) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3Congestion) Component->SetVisibility(false);
		for (UPaperSpriteComponent* Component : Phase3BaggageExceptionRoute) Component->SetVisibility(false);
		if (Phase3BaggageExceptionZone) Phase3BaggageExceptionZone->SetVisibility(false);
		if (Phase3BaggageExceptionStation) Phase3BaggageExceptionStation->SetVisibility(false);
		if (Phase3Aircraft) Phase3Aircraft->SetVisibility(false);

		for (int32 Index = 0; Index < Phase3PassengerAvailableCount; ++Index)
		{
			if (Index >= 28)
			{
				Phase3Passengers[Index]->SetVisibility(false);
				continue;
			}
			const AMSim::FPassengerRecord* Passenger = State.Passengers.IsValidIndex(Index)
				? &State.Passengers[Index] : nullptr;
			AMSim::FTerminalCellCoord Cell = {4 + Index % 8, 3 + (Index / 8) % 5};
			if (Passenger)
			{
				switch (Passenger->JourneyState)
				{
				case AMSim::EPassengerJourneyState::GateArea:
				case AMSim::EPassengerJourneyState::Boarding:
					Cell = {18 + Index % 9, 12 + (Index / 9) % 4};
					break;
				case AMSim::EPassengerJourneyState::ArrivalsCorridor:
				case AMSim::EPassengerJourneyState::BaggageReclaim:
					Cell = {15 + Index % 9, 7 + (Index / 9) % 4};
					break;
				case AMSim::EPassengerJourneyState::SecurityQueue:
				case AMSim::EPassengerJourneyState::Screening:
					Cell = {9 + Index % 7, 8 + (Index / 7) % 4};
					break;
				default: break;
				}
			}
			Phase3Passengers[Index]->SetRelativeLocation(CellWorldPosition(
				Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY, 61.0f));
			Phase3Passengers[Index]->SetRelativeScale3D(FVector(
				0.11f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.11f * AMSim::TerminalPresentationGeometry::IdentityScale));
			Phase3Passengers[Index]->SetVisibility(true);
		}
		for (int32 Index = 0; Index < Phase3BagAvailableCount; ++Index)
		{
			if (Index >= 18)
			{
				Phase3Bags[Index]->SetVisibility(false);
				continue;
			}
			const AMSim::FTerminalCellCoord Cell = {15 + Index % 3, 6 + (Index / 3) % 4};
			Phase3Bags[Index]->SetRelativeLocation(CellWorldPosition(
				Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY, 59.0f));
			Phase3Bags[Index]->SetRelativeScale3D(FVector(
				0.08f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.08f * AMSim::TerminalPresentationGeometry::IdentityScale));
			Phase3Bags[Index]->SetVisibility(true);
		}
		for (int32 Index = 0; Index < Phase3Staff.Num(); ++Index)
		{
			const AMSim::FTerminalCellCoord Cell = {
				7 + Index % 4, 2 + (Index / 4) * 3};
			Phase3Staff[Index]->SetRelativeLocation(CellWorldPosition(
				Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY, 63.0f));
			Phase3Staff[Index]->SetRelativeScale3D(FVector(
				0.13f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.13f * AMSim::TerminalPresentationGeometry::IdentityScale));
			Phase3Staff[Index]->SetVisibility(Index < 6);
		}
		for (int32 Index = 0; Index < Phase3Vehicles.Num(); ++Index)
		{
			const AMSim::FTerminalCellCoord Cell = {2 + Index * 3, -2};
			Phase3Vehicles[Index]->SetRelativeLocation(CellWorldPosition(
				Cell, Center, MinimumX, MinimumY, MaximumX, MaximumY, 58.0f));
			Phase3Vehicles[Index]->SetRelativeScale3D(FVector(
				0.38f * AMSim::TerminalPresentationGeometry::IdentityScale,
				1.0f,
				0.38f * AMSim::TerminalPresentationGeometry::IdentityScale));
			Phase3Vehicles[Index]->SetVisibility(Index < 3);
		}
	}
}
