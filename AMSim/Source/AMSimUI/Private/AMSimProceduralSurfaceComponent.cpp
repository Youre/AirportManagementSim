#include "AMSimProceduralSurfaceComponent.h"

#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	FVector2D Planar(const FVector& Point)
	{
		return FVector2D(Point.X, Point.Y);
	}

	FLinearColor MultiplyColor(
		const FLinearColor& Base,
		const FLinearColor& Tint)
	{
		return FLinearColor(
			Base.R * Tint.R,
			Base.G * Tint.G,
			Base.B * Tint.B,
			Base.A * Tint.A);
	}
}

UAMSimProceduralSurfaceComponent::UAMSimProceduralSurfaceComponent(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);
	SetCanEverAffectNavigation(false);
	SetCastShadow(false);
	bUseAsyncCooking = false;

	static ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> MaterialFinder(
		TEXT("/Game/Phase1/Presentation/Materials/"
			"M_ProceduralInfrastructure.M_ProceduralInfrastructure"));
	SurfaceMaterial = MaterialFinder.Get();
	if (SurfaceMaterial)
	{
		SetMaterial(0, SurfaceMaterial);
	}
}

void UAMSimProceduralSurfaceComponent::SetPresentationLayer(
	const double Height,
	const int32 SortPriority)
{
	SetRelativeLocation(FVector(0.0, 0.0, Height));
	TranslucencySortPriority = SortPriority;
}

void UAMSimProceduralSurfaceComponent::BuildSurfaceMesh(
	const AMSim::FProceduralSurfaceMesh& Mesh)
{
	BaseMesh = Mesh;
	LocalBounds = FBox(ForceInit);
	for (const FVector& Vertex : BaseMesh.Vertices)
	{
		LocalBounds += Vertex;
	}
	RebuildSection();
}

void UAMSimProceduralSurfaceComponent::BuildRunway(
	const FVector& Start,
	const FVector& End,
	const double Width,
	const AMSim::FProceduralSurfacePalette& Palette,
	const bool bShowMarkings)
{
	BuildSurfaceMesh(AMSim::MakeRunwaySurfaceMesh(
		Planar(Start), Planar(End), Width, Palette, bShowMarkings));
}

void UAMSimProceduralSurfaceComponent::BuildTaxiway(
	const FVector& Start,
	const FVector& End,
	const double Width,
	const AMSim::FProceduralSurfacePalette& Palette,
	const bool bShowMarkings)
{
	BuildSurfaceMesh(AMSim::MakeTaxiwaySurfaceMesh(
		Planar(Start), Planar(End), Width, Palette, bShowMarkings));
}

void UAMSimProceduralSurfaceComponent::BuildRoad(
	const FVector& Start,
	const FVector& End,
	const double Width,
	const AMSim::FProceduralSurfacePalette& Palette,
	const bool bShowMarkings)
{
	BuildSurfaceMesh(AMSim::MakeRoadSurfaceMesh(
		Planar(Start), Planar(End), Width, Palette, bShowMarkings));
}

void UAMSimProceduralSurfaceComponent::BuildApron(
	const FVector& Center,
	const FVector& Forward,
	const double Length,
	const double Width,
	const AMSim::FProceduralSurfacePalette& Palette,
	const bool bShowDetails)
{
	BuildSurfaceMesh(AMSim::MakeApronSurfaceMesh(
		Planar(Center),
		Planar(Forward),
		Length,
		Width,
		Palette,
		bShowDetails));
}

void UAMSimProceduralSurfaceComponent::BuildGateApron(
	const FVector& Center,
	const FVector& AirsideForward,
	const double Length,
	const double Width,
	const AMSim::FProceduralSurfacePalette& Palette,
	const bool bShowMarkings)
{
	BuildSurfaceMesh(AMSim::MakeGateApronSurfaceMesh(
		Planar(Center),
		Planar(AirsideForward),
		Length,
		Width,
		Palette,
		bShowMarkings));
}

void UAMSimProceduralSurfaceComponent::BuildPlainSegment(
	const FVector& Start,
	const FVector& End,
	const double Width,
	const FLinearColor& Color,
	const bool bRoundCaps)
{
	AMSim::FProceduralSurfaceMesh Mesh;
	AMSim::AppendSurfaceSegment(
		Mesh, Planar(Start), Planar(End), Width, Color, bRoundCaps);
	BuildSurfaceMesh(Mesh);
}

void UAMSimProceduralSurfaceComponent::ClearSurface()
{
	BaseMesh = {};
	LocalBounds = FBox(ForceInit);
	ClearAllMeshSections();
}

void UAMSimProceduralSurfaceComponent::SetSurfaceTint(
	const FLinearColor& Tint)
{
	if (SurfaceTint.Equals(Tint))
	{
		return;
	}
	SurfaceTint = Tint;
	RebuildSection();
}

void UAMSimProceduralSurfaceComponent::RebuildSection()
{
	if (!BaseMesh.IsValid())
	{
		ClearAllMeshSections();
		return;
	}
	TArray<FLinearColor> Colors;
	Colors.Reserve(BaseMesh.Colors.Num());
	for (const FLinearColor& Color : BaseMesh.Colors)
	{
		Colors.Add(MultiplyColor(Color, SurfaceTint));
	}
	TArray<FProcMeshTangent> Tangents;
	CreateMeshSection_LinearColor(
		0,
		BaseMesh.Vertices,
		BaseMesh.Triangles,
		BaseMesh.Normals,
		BaseMesh.UV0,
		Colors,
		Tangents,
		false,
		false);
	if (SurfaceMaterial)
	{
		SetMaterial(0, SurfaceMaterial);
	}
}
