#include "AMSimWorldPresenter.h"

#include "AMSimProceduralSurfaceComponent.h"
#include "PaperSpriteComponent.h"

void AAMSimWorldPresenter::SetPhase1OverlayMode(const int32 Mode)
{
	Phase1OverlayMode = FMath::Clamp(Mode, 0, 2);
	RefreshPhase1OverlayPresentation();
}

FLinearColor AAMSimWorldPresenter::GetPhase1RunwayTintForTest() const
{
	return Runway ? Runway->GetSurfaceTintForTest() : FLinearColor::Transparent;
}

void AAMSimWorldPresenter::RefreshPhase1OverlayPresentation()
{
	if (!bRequestedPhase1Operational)
	{
		return;
	}

	const bool bConnections = Phase1OverlayMode == 1;
	const bool bActivity = Phase1OverlayMode == 2;
	const FLinearColor SurfaceTint = bConnections
		? FLinearColor(0.33f, 0.84f, 0.91f, 1.0f)
		: bActivity
			? FLinearColor(0.48f, 0.55f, 0.56f, 0.76f)
			: FLinearColor::White;
	const FLinearColor GateTint = bConnections
		? FLinearColor(0.96f, 0.70f, 0.22f, 1.0f)
		: SurfaceTint;
	const FLinearColor RoadTint = bConnections
		? FLinearColor(0.96f, 0.70f, 0.22f, 1.0f)
		: bActivity
			? FLinearColor(0.40f, 0.46f, 0.46f, 0.68f)
			: FLinearColor::White;

	if (Terrain)
	{
		Terrain->SetSpriteColor(bActivity
			? FLinearColor(0.15f, 0.22f, 0.17f, 1.0f)
			: bConnections
				? FLinearColor(0.19f, 0.28f, 0.20f, 1.0f)
				: FLinearColor(0.25f, 0.34f, 0.22f, 1.0f));
	}
	if (Runway) Runway->SetSurfaceTint(SurfaceTint);
	if (Taxiway) Taxiway->SetSurfaceTint(SurfaceTint);
	for (UAMSimProceduralSurfaceComponent* Segment : Phase1TaxiwaySegments)
	{
		if (Segment) Segment->SetSurfaceTint(SurfaceTint);
	}
	if (Stand) Stand->SetSurfaceTint(GateTint);
	if (GateB) GateB->SetSurfaceTint(GateTint);
	if (Access) Access->SetSurfaceTint(RoadTint);
	if (OperationsHut)
	{
		OperationsHut->SetSpriteColor(
			bActivity ? FLinearColor(0.60f, 0.63f, 0.62f, 0.80f) : FLinearColor::White);
	}
	if (Windsock)
	{
		Windsock->SetSpriteColor(
			bActivity ? FLinearColor(0.60f, 0.63f, 0.62f, 0.80f) : FLinearColor::White);
	}
}
