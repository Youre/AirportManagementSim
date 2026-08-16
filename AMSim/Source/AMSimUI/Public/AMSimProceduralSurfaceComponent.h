#pragma once

#include "AMSimProceduralSurfaceGeometry.h"
#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "AMSimProceduralSurfaceComponent.generated.h"

class UMaterialInterface;

UCLASS(ClassGroup=(AMSim), meta=(BlueprintSpawnableComponent))
class AMSIMUI_API UAMSimProceduralSurfaceComponent final
	: public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UAMSimProceduralSurfaceComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetPresentationLayer(double Height, int32 SortPriority);
	void BuildSurfaceMesh(const AMSim::FProceduralSurfaceMesh& Mesh);
	void BuildRunway(
		const FVector& Start,
		const FVector& End,
		double Width,
		const AMSim::FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	void BuildTaxiway(
		const FVector& Start,
		const FVector& End,
		double Width,
		const AMSim::FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	void BuildRoad(
		const FVector& Start,
		const FVector& End,
		double Width,
		const AMSim::FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	void BuildApron(
		const FVector& Center,
		const FVector& Forward,
		double Length,
		double Width,
		const AMSim::FProceduralSurfacePalette& Palette,
		bool bShowDetails = true);
	void BuildGateApron(
		const FVector& Center,
		const FVector& AirsideForward,
		double Length,
		double Width,
		const AMSim::FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	void BuildPlainSegment(
		const FVector& Start,
		const FVector& End,
		double Width,
		const FLinearColor& Color,
		bool bRoundCaps = false);
	void ClearSurface();
	void SetSurfaceTint(const FLinearColor& Tint);

	FLinearColor GetSurfaceTintForTest() const { return SurfaceTint; }
	int32 GetSurfaceVertexCountForTest() const { return BaseMesh.Vertices.Num(); }
	int32 GetSurfaceTriangleCountForTest() const
	{
		return BaseMesh.Triangles.Num() / 3;
	}
	FVector GetSurfaceSizeForTest() const { return LocalBounds.GetSize(); }
	FVector GetSurfaceCenterForTest() const
	{
		return LocalBounds.IsValid
			? GetRelativeTransform().TransformPosition(LocalBounds.GetCenter())
			: FVector::ZeroVector;
	}
	bool HasCookerVisibleMaterialForTest() const { return SurfaceMaterial != nullptr; }

private:
	void RebuildSection();

	UPROPERTY()
	TObjectPtr<UMaterialInterface> SurfaceMaterial;
	AMSim::FProceduralSurfaceMesh BaseMesh;
	FBox LocalBounds = FBox(ForceInit);
	FLinearColor SurfaceTint = FLinearColor::White;
};
