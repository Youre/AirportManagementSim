#include "AMSimWorldPresenter.h"

#include "Algo/Count.h"
#include "AMSimMatureAirportLayout.h"
#include "AMSimProceduralSurfaceComponent.h"
#include "PaperSpriteComponent.h"

namespace
{
	// Large scale-sensitive site art is replaced by procedural geometry. This
	// small apron-fixtures sprite remains a bounded top-down detail layer.
	constexpr int32 MatureApronFixturesSpriteIndex = 15;
}

void AAMSimWorldPresenter::SetMatureInfrastructureVisible(const bool bVisible)
{
	for (UAMSimProceduralSurfaceComponent* Component : {
		MatureRunway.Get(),
		MatureTaxiway.Get(),
		MatureApron.Get(),
		MatureAccessRoad.Get()})
	{
		if (Component)
		{
			Component->SetVisibility(bVisible);
		}
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureGatePads)
	{
		Component->SetVisibility(bVisible);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureTaxiConnectors)
	{
		Component->SetVisibility(bVisible);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureLandsideLinks)
	{
		Component->SetVisibility(bVisible);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilityBases)
	{
		Component->SetVisibility(bVisible);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilitySurfaces)
	{
		Component->SetVisibility(bVisible);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilityDetails)
	{
		Component->SetVisibility(bVisible);
	}
}

void AAMSimWorldPresenter::SetMatureInfrastructureTint(
	const FLinearColor& Tint)
{
	for (UAMSimProceduralSurfaceComponent* Component : {
		MatureRunway.Get(),
		MatureTaxiway.Get(),
		MatureApron.Get(),
		MatureAccessRoad.Get()})
	{
		if (Component)
		{
			Component->SetSurfaceTint(Tint);
		}
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureGatePads)
	{
		Component->SetSurfaceTint(Tint);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureTaxiConnectors)
	{
		Component->SetSurfaceTint(Tint);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureLandsideLinks)
	{
		Component->SetSurfaceTint(Tint);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilityBases)
	{
		Component->SetSurfaceTint(Tint);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilitySurfaces)
	{
		Component->SetSurfaceTint(Tint);
	}
	for (UAMSimProceduralSurfaceComponent* Component : MatureFacilityDetails)
	{
		Component->SetSurfaceTint(Tint);
	}
}

void AAMSimWorldPresenter::SetMatureSiteSpritesVisible(const bool bVisible)
{
	for (int32 Index = 0; Index < MatureSite.Num(); ++Index)
	{
		MatureSite[Index]->SetVisibility(
			bVisible && Index == MatureApronFixturesSpriteIndex);
	}
}

void AAMSimWorldPresenter::SetMatureSelectionFacility(
	const bool bFacilitySelected)
{
	bMatureFacilitySelected = bFacilitySelected;
	MatureSelection->SetRelativeLocation(
		bFacilitySelected
			? FVector(
				AMSim::MatureAirportLayout::TerminalCenterWorldX,
				AMSim::MatureAirportLayout::TerminalCenterWorldY,
				75.0)
			: FVector(-7000.0, -15000.0, 75.0));
	MatureSelection->SetRelativeScale3D(
		bFacilitySelected
			? FVector(28.0f, 1.0f, 28.0f)
			: FVector(16.0f, 1.0f, 16.0f));
}

void AAMSimWorldPresenter::SetMatureOverviewMode(const bool bEnabled)
{
	bMatureOverviewMode =
		bEnabled ||
		bIncidentPresentationMode ||
		bPhase4IncidentWorldVisible;
	// The fixed-coordinate Phase 3 proof renderer is retired. The schema-10
	// spatial terminal owns the only terminal interior in every world mode.
	const bool bShowInterior = false;
	for (UPaperSpriteComponent* Component : Phase3Floor)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Rooms)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3SecurityBoundary)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Props)
	{
		Component->SetVisibility(bShowInterior);
	}
	Phase3Aircraft->SetVisibility(
		bShowInterior && bPhase3AircraftAvailable);
	SetMatureSiteSpritesVisible(
		bPhase3WorldVisible && bMatureOverviewMode);
	SetMatureInfrastructureVisible(
		bPhase3WorldVisible && bMatureOverviewMode);
	for (UPaperSpriteComponent* Component : MatureLandscapeClusters)
	{
		Component->SetVisibility(
			bPhase3WorldVisible && bMatureOverviewMode);
	}
	for (int32 Index = 0; Index < MatureAircraft.Num(); ++Index)
	{
		MatureAircraft[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			(Index < MatureAircraftAvailableCount || Index == 0));
	}
	MatureSelection->SetVisibility(
		bPhase3WorldVisible &&
		bMatureOverviewMode &&
		!bPhase4IncidentWorldVisible);
	for (int32 Index = 0; Index < MatureGroundVehicles.Num(); ++Index)
	{
		MatureGroundVehicles[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MatureGroundVehicleAvailableCount);
	}
	for (int32 Index = 0; Index < MaturePeople.Num(); ++Index)
	{
		MaturePeople[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MaturePeopleAvailableCount);
	}
	for (int32 Index = 0; Index < MatureBags.Num(); ++Index)
	{
		MatureBags[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MatureBagAvailableCount);
	}
	Phase4IncidentRunway->SetVisibility(
		bPhase4IncidentWorldVisible && !bMatureOverviewMode);
	RefreshPhase3OverlayVisibility();
	RefreshIncidentPresentationVisibility();
}

void AAMSimWorldPresenter::SetPhase3WorldVisible(const bool bVisible)
{
	bPhase3WorldVisible = bVisible;
	if (bVisible &&
		(bIncidentPresentationMode || bPhase4IncidentWorldVisible))
	{
		bMatureOverviewMode = true;
	}
	const bool bShowInterior = false;
	for (UPaperSpriteComponent* Component : Phase3Floor)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Rooms)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3SecurityBoundary)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Congestion)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase3Props)
	{
		Component->SetVisibility(bShowInterior);
	}
	SetMatureSiteSpritesVisible(
		bVisible && bMatureOverviewMode);
	SetMatureInfrastructureVisible(
		bVisible && bMatureOverviewMode);
	for (UPaperSpriteComponent* Component : MatureLandscapeClusters)
	{
		Component->SetVisibility(bVisible && bMatureOverviewMode);
	}
	for (int32 Index = 0; Index < MatureAircraft.Num(); ++Index)
	{
		MatureAircraft[Index]->SetVisibility(
			bVisible &&
			bMatureOverviewMode &&
			(Index < MatureAircraftAvailableCount || Index == 0));
	}
	MatureSelection->SetVisibility(
		bVisible &&
		bMatureOverviewMode &&
		!bPhase4IncidentWorldVisible);
	for (int32 Index = 0; Index < MatureGroundVehicles.Num(); ++Index)
	{
		MatureGroundVehicles[Index]->SetVisibility(
			bVisible &&
			bMatureOverviewMode &&
			Index < MatureGroundVehicleAvailableCount);
	}
	for (int32 Index = 0; Index < MaturePeople.Num(); ++Index)
	{
		MaturePeople[Index]->SetVisibility(
			bVisible &&
			bMatureOverviewMode &&
			Index < MaturePeopleAvailableCount);
	}
	for (int32 Index = 0; Index < MatureBags.Num(); ++Index)
	{
		MatureBags[Index]->SetVisibility(
			bVisible &&
			bMatureOverviewMode &&
			Index < MatureBagAvailableCount);
	}
	if (bVisible)
	{
		TurnaroundFuelTruck->SetVisibility(false);
		TurnaroundRampWorker->SetVisibility(false);
		TurnaroundSafetyCones->SetVisibility(false);
	}
	Phase3Aircraft->SetVisibility(
		bShowInterior && bPhase3AircraftAvailable);
	RefreshPhase3OverlayVisibility();
	RefreshIncidentPresentationVisibility();
}

void AAMSimWorldPresenter::RefreshIncidentPresentationVisibility()
{
	const bool bShowIncident =
		bPhase3WorldVisible &&
		bMatureOverviewMode &&
		bPhase4IncidentWorldVisible;
	if (bMatureOverviewMode)
	{
		Terrain->SetSpriteColor(
			bShowIncident
				? FLinearColor(0.025f, 0.075f, 0.095f, 1.0f)
				: FLinearColor(0.20f, 0.30f, 0.23f, 1.0f));
	}
	Phase4WeatherOverlay->SetVisibility(false);
	for (UPaperSpriteComponent* Component : Phase4WetSurfaces)
	{
		Component->SetVisibility(bShowIncident);
	}
	for (UPaperSpriteComponent* Component : Phase4RainStreaks)
	{
		Component->SetVisibility(bShowIncident);
	}
	const FLinearColor SiteTint =
		bShowIncident
			? FLinearColor(0.40f, 0.57f, 0.66f, 1.0f)
			: FLinearColor::White;
	for (int32 Index = 4; Index < MatureSite.Num(); ++Index)
	{
		MatureSite[Index]->SetSpriteColor(SiteTint);
	}
	SetMatureInfrastructureTint(SiteTint);
	for (UPaperSpriteComponent* Component : MatureLandscapeClusters)
	{
		Component->SetSpriteColor(
			bShowIncident
				? FLinearColor(0.25f, 0.42f, 0.44f, 1.0f)
				: FLinearColor::White);
	}
	for (int32 Index = 0; Index < MatureAircraft.Num(); ++Index)
	{
		MatureAircraft[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			(Index < MatureAircraftAvailableCount || Index == 0) &&
			!(bShowIncident && Index == 2));
	}
	Phase4AffectedAircraft->SetVisibility(bShowIncident);
	const bool bShowClosure = bShowIncident && bPhase4RunwayClosed;
	IncidentOverlay->SetVisibility(bShowClosure);
	for (UPaperSpriteComponent* Component : Phase4ClosureHatch)
	{
		Component->SetVisibility(bShowClosure);
	}
	for (UPaperSpriteComponent* Component : Phase4RunwayClosure)
	{
		Component->SetVisibility(bShowClosure);
	}
	const bool bShowRoute = bShowIncident && bPhase4RouteVisible;
	for (UPaperSpriteComponent* Component : Phase4EmergencyRoute)
	{
		Component->SetVisibility(bShowRoute);
	}
	for (UPaperSpriteComponent* Component : Phase4EmergencyRouteSegments)
	{
		Component->SetVisibility(bShowRoute);
	}
	MatureSelection->SetVisibility(
		bPhase3WorldVisible &&
		bMatureOverviewMode &&
		!bShowIncident);
	for (int32 Index = 0; Index < Phase4ResponseVehicles.Num(); ++Index)
	{
		Phase4ResponseVehicles[Index]->SetVisibility(
			bShowIncident &&
			(Index == 0
				? bPhase4RouteVisible
				: bPhase4ProtectionVisible));
	}
	Phase4ProtectionZone->SetVisibility(
		bShowIncident && bPhase4ProtectionVisible);
}

int32 AAMSimWorldPresenter::GetActiveMatureFacilityProceduralCount() const
{
	const auto CountVisible = [](const auto& Components)
	{
		return Algo::CountIf(
			Components,
			[](const UAMSimProceduralSurfaceComponent* Component)
			{
				return Component && Component->IsVisible();
			});
	};
	return CountVisible(MatureFacilityBases) +
		CountVisible(MatureFacilitySurfaces) +
		CountVisible(MatureFacilityDetails);
}

int32 AAMSimWorldPresenter::GetActiveMatureLegacySiteSpriteCount() const
{
	return Algo::CountIf(
		MatureSite,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

int32 AAMSimWorldPresenter::GetActiveLegacyPhase3InteriorProxyCount() const
{
	const auto CountVisible = [](const auto& Components)
	{
		return Algo::CountIf(
			Components,
			[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
	};
	int32 Count = CountVisible(Phase3Floor) +
		CountVisible(Phase3Rooms) +
		CountVisible(Phase3DepartureFlow) +
		CountVisible(Phase3ArrivalFlow) +
		CountVisible(Phase3LandsideFlow) +
		CountVisible(Phase3BaggageFlow) +
		CountVisible(Phase3AccessibleFlow) +
		CountVisible(Phase3AccessibleDashes) +
		CountVisible(Phase3SecurityBoundary) +
		CountVisible(Phase3BaggageExceptionRoute) +
		CountVisible(Phase3Congestion) +
		CountVisible(Phase3Props);
	for (const UPaperSpriteComponent* Component : {
		Phase3Aircraft.Get(),
		Phase3BaggageExceptionZone.Get(),
		Phase3BaggageExceptionStation.Get()})
	{
		Count += Component && Component->IsVisible() ? 1 : 0;
	}
	return Count;
}

bool AAMSimWorldPresenter::HasMatureFacilityProceduralMaterialForTest() const
{
	const auto IsValidFamily = [](const auto& Components)
	{
		return Components.Num() > 0 &&
			Algo::CountIf(
				Components,
				[](const UAMSimProceduralSurfaceComponent* Component)
				{
					return Component &&
						Component->HasCookerVisibleMaterialForTest() &&
						Component->GetSurfaceVertexCountForTest() >= 3;
				}) == Components.Num();
	};
	return IsValidFamily(MatureFacilityBases) &&
		IsValidFamily(MatureFacilitySurfaces) &&
		IsValidFamily(MatureFacilityDetails);
}
