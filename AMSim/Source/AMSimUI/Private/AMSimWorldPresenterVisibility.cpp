#include "AMSimWorldPresenter.h"

#include "PaperSpriteComponent.h"

void AAMSimWorldPresenter::SetMatureSelectionFacility(
	const bool bFacilitySelected)
{
	bMatureFacilitySelected = bFacilitySelected;
	MatureSelection->SetRelativeLocation(
		bFacilitySelected
			? FVector(-7500.0, 0.0, 75.0)
			: FVector(7000.0, -9000.0, 75.0));
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
	const bool bShowInterior =
		bPhase3WorldVisible && !bMatureOverviewMode;
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
	for (int32 Index = 0; Index < Phase3Passengers.Num(); ++Index)
	{
		Phase3Passengers[Index]->SetVisibility(
			bShowInterior && Index < Phase3PassengerAvailableCount);
	}
	for (int32 Index = 0; Index < Phase3Bags.Num(); ++Index)
	{
		Phase3Bags[Index]->SetVisibility(
			bShowInterior && Index < Phase3BagAvailableCount);
	}
	for (UPaperSpriteComponent* Component : Phase3Vehicles)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Staff)
	{
		Component->SetVisibility(bShowInterior);
	}
	Phase3Aircraft->SetVisibility(
		bShowInterior && bPhase3AircraftAvailable);
	for (UPaperSpriteComponent* Component : MatureSite)
	{
		Component->SetVisibility(
			bPhase3WorldVisible && bMatureOverviewMode);
	}
	for (UPaperSpriteComponent* Component : MatureTaxiConnectors)
	{
		Component->SetVisibility(
			bPhase3WorldVisible && bMatureOverviewMode);
	}
	for (UPaperSpriteComponent* Component : MatureLandsideLinks)
	{
		Component->SetVisibility(
			bPhase3WorldVisible && bMatureOverviewMode);
	}
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
	const bool bShowInterior = bVisible && !bMatureOverviewMode;
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
	for (int32 Index = 0; Index < Phase3Passengers.Num(); ++Index)
	{
		Phase3Passengers[Index]->SetVisibility(
			bShowInterior && Index < Phase3PassengerAvailableCount);
	}
	for (int32 Index = 0; Index < Phase3Bags.Num(); ++Index)
	{
		Phase3Bags[Index]->SetVisibility(
			bShowInterior && Index < Phase3BagAvailableCount);
	}
	for (UPaperSpriteComponent* Component : Phase3Vehicles)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Staff)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : Phase3Props)
	{
		Component->SetVisibility(bShowInterior);
	}
	for (UPaperSpriteComponent* Component : MatureSite)
	{
		Component->SetVisibility(bVisible && bMatureOverviewMode);
	}
	for (UPaperSpriteComponent* Component : MatureTaxiConnectors)
	{
		Component->SetVisibility(bVisible && bMatureOverviewMode);
	}
	for (UPaperSpriteComponent* Component : MatureLandsideLinks)
	{
		Component->SetVisibility(bVisible && bMatureOverviewMode);
	}
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
	for (UPaperSpriteComponent* Component : MatureSite)
	{
		Component->SetSpriteColor(SiteTint);
	}
	for (UPaperSpriteComponent* Component : MatureTaxiConnectors)
	{
		Component->SetSpriteColor(SiteTint);
	}
	for (UPaperSpriteComponent* Component : MatureLandsideLinks)
	{
		Component->SetSpriteColor(SiteTint);
	}
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
