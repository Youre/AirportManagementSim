#include "AMSimProceduralSurfaceGeometry.h"
#include "Misc/AutomationTest.h"

using namespace AMSim;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimProceduralInfrastructureGeometryTest,
	"AMSim.Presentation.ProceduralInfrastructure.Geometry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimProceduralInfrastructureGeometryTest::RunTest(
	const FString& Parameters)
{
	const FVector2D Start(-12000.0, 5000.0);
	const FVector2D End(42000.0, 5000.0);
	const FProceduralSurfaceMesh PlainRunway = MakeRunwaySurfaceMesh(
		Start,
		End,
		7200.0,
		MakeOperationalRunwayPalette(),
		false);
	const FProceduralSurfaceMesh MarkedRunway = MakeRunwaySurfaceMesh(
		Start,
		End,
		7200.0,
		MakeOperationalRunwayPalette(),
		true);
	TestTrue(TEXT("Unmarked runway produces valid flat geometry"),
		PlainRunway.IsValid());
	TestTrue(TEXT("Operational runway adds deterministic markings"),
		MarkedRunway.IsValid() &&
		MarkedRunway.Vertices.Num() > PlainRunway.Vertices.Num());
	TestEqual(TEXT("Runway geometry has no authored 3D height"),
		MarkedRunway.Vertices[0].Z, 0.0);

	const FProceduralSurfaceMesh Taxiway = MakeTaxiwaySurfaceMesh(
		Start,
		End,
		3600.0,
		MakeOperationalTaxiwayPalette(),
		true);
	const FProceduralSurfaceMesh Road = MakeRoadSurfaceMesh(
		Start,
		End,
		3000.0,
		MakeOperationalRoadPalette(),
		true);
	TestTrue(TEXT("Taxiway owns round junction-cap geometry"),
		Taxiway.IsValid() && Taxiway.Vertices.Num() > 40);
	TestTrue(TEXT("Road owns edge and center marking geometry"),
		Road.IsValid() && Road.Vertices.Num() > Taxiway.Vertices.Num());

	const FProceduralSurfaceMesh Apron = MakeApronSurfaceMesh(
		FVector2D::ZeroVector,
		FVector2D(0.0, 1.0),
		72000.0,
		30000.0,
		MakeOperationalApronPalette(),
		true);
	const FProceduralSurfaceMesh PlainApron = MakeApronSurfaceMesh(
		FVector2D::ZeroVector,
		FVector2D(0.0, 1.0),
		72000.0,
		30000.0,
		MakeOperationalApronPalette(),
		false);
	TestTrue(TEXT("Apron adds scalable panel seams and an inset border"),
		Apron.IsValid() && Apron.Vertices.Num() > PlainApron.Vertices.Num());

	const FProceduralSurfaceMesh Gate = MakeGateApronSurfaceMesh(
		FVector2D(0.0, 0.0),
		FVector2D(1.0, 0.0),
		15000.0,
		11000.0,
		MakeOperationalGatePalette(),
		true);
	TestTrue(TEXT("Gate apron owns a rounded footprint and guidance marks"),
		Gate.IsValid() && Gate.Vertices.Num() > 20);

	const FProceduralSurfaceMesh RepeatedRunway = MakeRunwaySurfaceMesh(
		Start,
		End,
		7200.0,
		MakeOperationalRunwayPalette(),
		true);
	TestEqual(TEXT("Repeated procedural builds preserve vertex count"),
		RepeatedRunway.Vertices.Num(), MarkedRunway.Vertices.Num());
	TestTrue(TEXT("Repeated procedural builds preserve triangle topology"),
		RepeatedRunway.Triangles == MarkedRunway.Triangles);
	return true;
}
