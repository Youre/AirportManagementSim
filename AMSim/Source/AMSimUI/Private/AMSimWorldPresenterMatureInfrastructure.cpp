#include "AMSimWorldPresenter.h"

#include "AMSimProceduralSurfaceComponent.h"
#include "AMSimWorldPresentationLayers.h"

void AAMSimWorldPresenter::InitializeMatureInfrastructurePresentation()
{
	MatureRunway->SetPresentationLayer(
		AMSim::WorldPresentationLayers::Runway.Height,
		AMSim::WorldPresentationLayers::Runway.SortPriority);
	MatureRunway->BuildRunway(
		FVector(32000.0, -60000.0, 0.0),
		FVector(32000.0, 60000.0, 0.0),
		11000.0,
		AMSim::MakeOperationalRunwayPalette());
	MatureTaxiway->SetPresentationLayer(
		AMSim::WorldPresentationLayers::Taxiway.Height,
		AMSim::WorldPresentationLayers::Taxiway.SortPriority);
	MatureTaxiway->BuildTaxiway(
		FVector(21000.0, -54000.0, 0.0),
		FVector(21000.0, 54000.0, 0.0),
		6200.0,
		AMSim::MakeOperationalTaxiwayPalette());
	MatureApron->SetPresentationLayer(
		AMSim::WorldPresentationLayers::GateA.Height,
		AMSim::WorldPresentationLayers::GateA.SortPriority);
	MatureApron->BuildApron(
		FVector(7000.0, 0.0, 0.0),
		FVector(0.0, 1.0, 0.0),
		72000.0,
		30000.0,
		AMSim::MakeOperationalApronPalette(),
		true);
	MatureAccessRoad->SetPresentationLayer(
		AMSim::WorldPresentationLayers::ServiceRoad.Height,
		AMSim::WorldPresentationLayers::ServiceRoad.SortPriority);
	MatureAccessRoad->BuildRoad(
		FVector(-34000.0, -56000.0, 0.0),
		FVector(-34000.0, 56000.0, 0.0),
		4600.0,
		AMSim::MakeOperationalRoadPalette());

	const double GateOffsets[] = {-23000.0, 0.0, 23000.0};
	for (int32 Index = 0; Index < MatureGatePads.Num(); ++Index)
	{
		MatureGatePads[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::GateA.Height + 0.2 + Index * 0.05,
			AMSim::WorldPresentationLayers::GateA.SortPriority + 1 + Index);
		MatureGatePads[Index]->BuildGateApron(
			FVector(7000.0, GateOffsets[Index], 0.0),
			FVector(1.0, 0.0, 0.0),
			22000.0,
			15000.0,
			AMSim::MakeOperationalGatePalette());
	}

	const FVector AirsideLinkLocations[] = {
		FVector(14500.0, -26000.0, 12.0),
		FVector(14500.0, 26000.0, 13.0),
		FVector(14500.0, 0.0, 14.0),
		FVector(7000.0, -21000.0, 15.0),
		FVector(7000.0, 21000.0, 16.0),
		FVector(0.0, 0.0, 17.0)};
	const double AirsideLinkLengths[] = {
		26000.0, 26000.0, 22000.0, 18000.0, 18000.0, 18000.0};
	for (int32 Index = 0; Index < MatureTaxiConnectors.Num(); ++Index)
	{
		MatureTaxiConnectors[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::Taxiway.Height + 0.3 + Index * 0.03,
			AMSim::WorldPresentationLayers::Taxiway.SortPriority + 1 + Index);
		MatureTaxiConnectors[Index]->BuildTaxiway(
			AirsideLinkLocations[Index] -
				FVector(AirsideLinkLengths[Index] * 0.5, 0.0, 0.0),
			AirsideLinkLocations[Index] +
				FVector(AirsideLinkLengths[Index] * 0.5, 0.0, 0.0),
			5200.0,
			AMSim::MakeOperationalTaxiwayPalette());
		MatureTaxiConnectors[Index]->SetVisibility(false);
	}

	const FVector LandsideLinkLocations[] = {
		FVector(-15500.0, -18000.0, 12.0),
		FVector(-15500.0, 18000.0, 13.0),
		FVector(-28500.0, -31000.0, 14.0),
		FVector(-28500.0, -12000.0, 15.0),
		FVector(-28500.0, 8000.0, 16.0),
		FVector(-28500.0, 27000.0, 17.0),
		FVector(-17500.0, 6000.0, 18.0)};
	for (int32 Index = 0; Index < MatureLandsideLinks.Num(); ++Index)
	{
		const double Length = Index < 2 ? 22000.0 : 18000.0;
		MatureLandsideLinks[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::ServiceRoad.Height + 0.2 + Index * 0.03,
			AMSim::WorldPresentationLayers::ServiceRoad.SortPriority + 1 + Index);
		MatureLandsideLinks[Index]->BuildRoad(
			LandsideLinkLocations[Index] - FVector(Length * 0.5, 0.0, 0.0),
			LandsideLinkLocations[Index] + FVector(Length * 0.5, 0.0, 0.0),
			4000.0,
			AMSim::MakeOperationalRoadPalette());
		MatureLandsideLinks[Index]->SetVisibility(false);
	}
}
