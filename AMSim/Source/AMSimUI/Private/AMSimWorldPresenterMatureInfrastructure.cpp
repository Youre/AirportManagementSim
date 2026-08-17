#include "AMSimWorldPresenter.h"

#include "AMSimMatureAirportLayout.h"
#include "AMSimProceduralSurfaceComponent.h"
#include "AMSimWorldPresentationLayers.h"

namespace
{
	enum class EMatureFacilityDetail : uint8
	{
		Hangar,
		Operations,
		Fuel,
		Parking,
		Transit,
		Rail,
		Dropoff,
		PerimeterGate
	};

	struct FMatureFacilityDefinition
	{
		FVector2D Center;
		FVector2D Forward;
		double Length;
		double Width;
		FLinearColor Border;
		FLinearColor Surface;
		EMatureFacilityDetail Detail;
	};

	struct FMatureFacilityStyle
	{
		FLinearColor Border;
		FLinearColor Surface;
		EMatureFacilityDetail Detail;
	};

	FVector2D FacilityPoint(
		const FMatureFacilityDefinition& Definition,
		const double Along,
		const double Across)
	{
		const FVector2D Forward = Definition.Forward.GetSafeNormal();
		const FVector2D Right(-Forward.Y, Forward.X);
		return Definition.Center + Forward * Along + Right * Across;
	}

	void AppendFacilityDetails(
		AMSim::FProceduralSurfaceMesh& Mesh,
		const FMatureFacilityDefinition& Definition)
	{
		const FLinearColor Light(0.88f, 0.86f, 0.72f, 0.96f);
		const FLinearColor Cyan(0.18f, 0.74f, 0.80f, 0.96f);
		const FLinearColor Amber(0.95f, 0.72f, 0.16f, 0.98f);
		const FLinearColor Dark(0.05f, 0.10f, 0.12f, 0.92f);
		switch (Definition.Detail)
		{
		case EMatureFacilityDetail::Hangar:
			for (const double Along : {-5200.0, 0.0, 5200.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, Along, -Definition.Width * 0.42),
					FacilityPoint(Definition, Along, Definition.Width * 0.42),
					150.0,
					Dark);
			}
			for (const double Across : {-3900.0, -1300.0, 1300.0, 3900.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, -Definition.Length * 0.47, Across),
					FacilityPoint(Definition, -Definition.Length * 0.36, Across),
					360.0,
					Cyan,
					true);
			}
			break;
		case EMatureFacilityDetail::Operations:
			AMSim::AppendRoundedSurfaceRectangle(
				Mesh,
				FacilityPoint(Definition, -3200.0, 0.0),
				Definition.Forward,
				1100.0,
				4200.0,
				300.0,
				Cyan);
			for (const double Across : {-2400.0, 0.0, 2400.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, 900.0, Across),
					FacilityPoint(Definition, 4200.0, Across),
					170.0,
					Light);
			}
			break;
		case EMatureFacilityDetail::Fuel:
			for (const double Across : {-2200.0, 2200.0})
			{
				AMSim::AppendSurfaceDisc(
					Mesh,
					FacilityPoint(Definition, 0.0, Across),
					1500.0,
					Light,
					28);
				AMSim::AppendSurfaceDisc(
					Mesh,
					FacilityPoint(Definition, 0.0, Across),
					850.0,
					Dark,
					24);
			}
			AMSim::AppendSurfaceSegment(
				Mesh,
				FacilityPoint(Definition, -3800.0, 0.0),
				FacilityPoint(Definition, 3800.0, 0.0),
				220.0,
				Amber);
			break;
		case EMatureFacilityDetail::Parking:
			for (const double Along : {-7200.0, -4800.0, -2400.0, 0.0,
				2400.0, 4800.0, 7200.0})
			{
				for (const double Across : {-5000.0, 5000.0})
				{
					AMSim::AppendSurfaceSegment(
						Mesh,
						FacilityPoint(Definition, Along, Across - 1700.0),
						FacilityPoint(Definition, Along, Across + 1700.0),
						110.0,
						Light);
				}
			}
			AMSim::AppendSurfaceSegment(
				Mesh,
				FacilityPoint(Definition, -9000.0, 0.0),
				FacilityPoint(Definition, 9000.0, 0.0),
				160.0,
				Amber);
			break;
		case EMatureFacilityDetail::Transit:
			for (const double Across : {-2500.0, 0.0, 2500.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, -6500.0, Across),
					FacilityPoint(Definition, 6500.0, Across),
					150.0,
					Across == 0.0 ? Amber : Light,
					true);
			}
			break;
		case EMatureFacilityDetail::Rail:
			for (const double Across : {-1350.0, 1350.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, -8200.0, Across),
					FacilityPoint(Definition, 8200.0, Across),
					220.0,
					Dark);
			}
			for (const double Along : {-6500.0, -3900.0, -1300.0, 1300.0,
				3900.0, 6500.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, Along, -2500.0),
					FacilityPoint(Definition, Along, 2500.0),
					130.0,
					Light);
			}
			break;
		case EMatureFacilityDetail::Dropoff:
			for (const double Across : {-1900.0, 1900.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, -6200.0, Across),
					FacilityPoint(Definition, 6200.0, Across),
					140.0,
					Light,
					true);
			}
			AMSim::AppendRoundedSurfaceRectangle(
				Mesh,
				FacilityPoint(Definition, 0.0, 0.0),
				Definition.Forward,
				7000.0,
				900.0,
				300.0,
				Cyan);
			break;
		case EMatureFacilityDetail::PerimeterGate:
			for (const double Along : {-2300.0, -900.0, 900.0, 2300.0})
			{
				AMSim::AppendSurfaceSegment(
					Mesh,
					FacilityPoint(Definition, Along - 600.0, -1200.0),
					FacilityPoint(Definition, Along + 600.0, 1200.0),
					230.0,
					Amber);
			}
			break;
		}
	}
}

void AAMSimWorldPresenter::InitializeMatureInfrastructurePresentation()
{
	MatureRunway->SetPresentationLayer(
		AMSim::WorldPresentationLayers::Runway.Height,
		AMSim::WorldPresentationLayers::Runway.SortPriority);
	MatureRunway->BuildRunway(
		FVector(
			AMSim::MatureAirportLayout::RunwayCenterWorldX,
			-AMSim::MatureAirportLayout::RunwayHalfLength,
			0.0),
		FVector(
			AMSim::MatureAirportLayout::RunwayCenterWorldX,
			AMSim::MatureAirportLayout::RunwayHalfLength,
			0.0),
		AMSim::MatureAirportLayout::RunwayWidth,
		AMSim::MakeOperationalRunwayPalette());
	MatureTaxiway->SetPresentationLayer(
		AMSim::WorldPresentationLayers::Taxiway.Height,
		AMSim::WorldPresentationLayers::Taxiway.SortPriority);
	MatureTaxiway->BuildTaxiway(
		FVector(
			AMSim::MatureAirportLayout::TaxiwayCenterWorldX,
			-AMSim::MatureAirportLayout::TaxiwayHalfLength,
			0.0),
		FVector(
			AMSim::MatureAirportLayout::TaxiwayCenterWorldX,
			AMSim::MatureAirportLayout::TaxiwayHalfLength,
			0.0),
		AMSim::MatureAirportLayout::TaxiwayWidth,
		AMSim::MakeOperationalTaxiwayPalette());
	MatureApron->SetPresentationLayer(
		AMSim::WorldPresentationLayers::GateA.Height,
		AMSim::WorldPresentationLayers::GateA.SortPriority);
	MatureApron->BuildApron(
		FVector(
			AMSim::MatureAirportLayout::ApronCenterWorldX,
			0.0,
			0.0),
		FVector(0.0, 1.0, 0.0),
		AMSim::MatureAirportLayout::ApronLength,
		AMSim::MatureAirportLayout::ApronWidth,
		AMSim::MakeOperationalApronPalette(),
		true);
	MatureAccessRoad->SetPresentationLayer(
		AMSim::WorldPresentationLayers::ServiceRoad.Height,
		AMSim::WorldPresentationLayers::ServiceRoad.SortPriority);
	MatureAccessRoad->BuildRoad(
		FVector(
			AMSim::MatureAirportLayout::AccessRoadCenterWorldX,
			-AMSim::MatureAirportLayout::AccessRoadHalfLength,
			0.0),
		FVector(
			AMSim::MatureAirportLayout::AccessRoadCenterWorldX,
			AMSim::MatureAirportLayout::AccessRoadHalfLength,
			0.0),
		AMSim::MatureAirportLayout::AccessRoadWidth,
		AMSim::MakeOperationalRoadPalette());

	for (int32 Index = 0; Index < MatureGatePads.Num(); ++Index)
	{
		MatureGatePads[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::GateA.Height + 0.2 + Index * 0.05,
			AMSim::WorldPresentationLayers::GateA.SortPriority + 1 + Index);
		MatureGatePads[Index]->BuildGateApron(
			FVector(
				AMSim::MatureAirportLayout::GateCenterWorldX,
				AMSim::MatureAirportLayout::GateCenterWorldY[Index],
				0.0),
			FVector(1.0, 0.0, 0.0),
			AMSim::MatureAirportLayout::GateLength,
			AMSim::MatureAirportLayout::GateWidth,
			AMSim::MakeOperationalGatePalette());
	}

	const TConstArrayView<AMSim::MatureAirportLayout::FLinearConnection>
		AirsideConnections =
			AMSim::MatureAirportLayout::GetAirsideConnections();
	check(AirsideConnections.Num() == MatureTaxiConnectors.Num());
	for (int32 Index = 0; Index < MatureTaxiConnectors.Num(); ++Index)
	{
		const AMSim::MatureAirportLayout::FLinearConnection& Connection =
			AirsideConnections[Index];
		MatureTaxiConnectors[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::Taxiway.Height + 0.3 + Index * 0.03,
			AMSim::WorldPresentationLayers::Taxiway.SortPriority + 1 + Index);
		MatureTaxiConnectors[Index]->BuildTaxiway(
			FVector(
				Connection.Center.X - Connection.Length * 0.5,
				Connection.Center.Y,
				0.0),
			FVector(
				Connection.Center.X + Connection.Length * 0.5,
				Connection.Center.Y,
				0.0),
			5200.0,
			AMSim::MakeOperationalTaxiwayPalette());
		MatureTaxiConnectors[Index]->SetVisibility(false);
	}

	const TConstArrayView<AMSim::MatureAirportLayout::FLinearConnection>
		LandsideConnections =
			AMSim::MatureAirportLayout::GetLandsideConnections();
	check(LandsideConnections.Num() == MatureLandsideLinks.Num());
	for (int32 Index = 0; Index < MatureLandsideLinks.Num(); ++Index)
	{
		const AMSim::MatureAirportLayout::FLinearConnection& Connection =
			LandsideConnections[Index];
		MatureLandsideLinks[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::ServiceRoad.Height + 0.2 + Index * 0.03,
			AMSim::WorldPresentationLayers::ServiceRoad.SortPriority + 1 + Index);
		MatureLandsideLinks[Index]->BuildRoad(
			FVector(
				Connection.Center.X - Connection.Length * 0.5,
				Connection.Center.Y,
				0.0),
			FVector(
				Connection.Center.X + Connection.Length * 0.5,
				Connection.Center.Y,
				0.0),
			4000.0,
			AMSim::MakeOperationalRoadPalette());
		MatureLandsideLinks[Index]->SetVisibility(false);
	}

	const FMatureFacilityStyle FacilityStyles[] = {
		{{0.08f, 0.17f, 0.20f, 1.0f}, {0.34f, 0.51f, 0.55f, 1.0f},
			EMatureFacilityDetail::Hangar},
		{{0.08f, 0.17f, 0.20f, 1.0f}, {0.74f, 0.68f, 0.52f, 1.0f},
			EMatureFacilityDetail::Operations},
		{{0.40f, 0.28f, 0.08f, 1.0f}, {0.18f, 0.22f, 0.20f, 1.0f},
			EMatureFacilityDetail::Fuel},
		{{0.28f, 0.31f, 0.29f, 1.0f}, {0.14f, 0.16f, 0.15f, 1.0f},
			EMatureFacilityDetail::Parking},
		{{0.30f, 0.34f, 0.31f, 1.0f}, {0.16f, 0.19f, 0.17f, 1.0f},
			EMatureFacilityDetail::Transit},
		{{0.34f, 0.32f, 0.27f, 1.0f}, {0.68f, 0.63f, 0.51f, 1.0f},
			EMatureFacilityDetail::Rail},
		{{0.35f, 0.31f, 0.24f, 1.0f}, {0.62f, 0.55f, 0.42f, 1.0f},
			EMatureFacilityDetail::Dropoff},
		{{0.12f, 0.18f, 0.18f, 1.0f}, {0.25f, 0.33f, 0.31f, 1.0f},
			EMatureFacilityDetail::PerimeterGate}};
	const TConstArrayView<AMSim::MatureAirportLayout::FFacility> Facilities =
		AMSim::MatureAirportLayout::GetFacilities();
	check(Facilities.Num() == MatureFacilityBases.Num());
	check(UE_ARRAY_COUNT(FacilityStyles) == MatureFacilityBases.Num());
	for (int32 Index = 0; Index < MatureFacilityBases.Num(); ++Index)
	{
		const AMSim::MatureAirportLayout::FFacility& Layout = Facilities[Index];
		const FMatureFacilityStyle& Style = FacilityStyles[Index];
		const FMatureFacilityDefinition Definition = {
			Layout.Center,
			Layout.Forward,
			Layout.Length,
			Layout.Width,
			Style.Border,
			Style.Surface,
			Style.Detail};
		AMSim::FProceduralSurfaceMesh BaseMesh;
		AMSim::AppendRoundedSurfaceRectangle(
			BaseMesh,
			Definition.Center,
			Definition.Forward,
			Definition.Length,
			Definition.Width,
			FMath::Min(900.0, Definition.Width * 0.12),
			Definition.Border);
		MatureFacilityBases[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::Structure.Height - 0.6,
			AMSim::WorldPresentationLayers::Structure.SortPriority - 2);
		MatureFacilityBases[Index]->BuildSurfaceMesh(BaseMesh);

		AMSim::FProceduralSurfaceMesh SurfaceMesh;
		AMSim::AppendRoundedSurfaceRectangle(
			SurfaceMesh,
			Definition.Center,
			Definition.Forward,
			FMath::Max(Definition.Length - 700.0, 1.0),
			FMath::Max(Definition.Width - 700.0, 1.0),
			FMath::Min(700.0, Definition.Width * 0.10),
			Definition.Surface);
		MatureFacilitySurfaces[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::Structure.Height - 0.3,
			AMSim::WorldPresentationLayers::Structure.SortPriority - 1);
		MatureFacilitySurfaces[Index]->BuildSurfaceMesh(SurfaceMesh);

		AMSim::FProceduralSurfaceMesh DetailMesh;
		AppendFacilityDetails(DetailMesh, Definition);
		MatureFacilityDetails[Index]->SetPresentationLayer(
			AMSim::WorldPresentationLayers::Structure.Height + 0.1,
			AMSim::WorldPresentationLayers::Structure.SortPriority);
		MatureFacilityDetails[Index]->BuildSurfaceMesh(DetailMesh);

		MatureFacilityBases[Index]->SetVisibility(false);
		MatureFacilitySurfaces[Index]->SetVisibility(false);
		MatureFacilityDetails[Index]->SetVisibility(false);
	}
}
