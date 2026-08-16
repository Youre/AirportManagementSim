#pragma once

#include "CoreMinimal.h"

namespace AMSim
{
	struct FProceduralSurfaceMesh
	{
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FLinearColor> Colors;

		bool IsValid() const
		{
			return Vertices.Num() >= 3 &&
				Triangles.Num() >= 3 &&
				Vertices.Num() == Normals.Num() &&
				Vertices.Num() == UV0.Num() &&
				Vertices.Num() == Colors.Num();
		}
	};

	struct FProceduralSurfacePalette
	{
		FLinearColor Base = FLinearColor::White;
		FLinearColor Edge = FLinearColor::White;
		FLinearColor Center = FLinearColor::White;
		FLinearColor Safety = FLinearColor::White;
	};

	AMSIMUI_API FProceduralSurfacePalette MakeOperationalRunwayPalette();
	AMSIMUI_API FProceduralSurfacePalette MakeOperationalTaxiwayPalette();
	AMSIMUI_API FProceduralSurfacePalette MakeOperationalRoadPalette();
	AMSIMUI_API FProceduralSurfacePalette MakeOperationalApronPalette();
	AMSIMUI_API FProceduralSurfacePalette MakeOperationalGatePalette();

	AMSIMUI_API void AppendSurfaceSegment(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Start,
		const FVector2D& End,
		double Width,
		const FLinearColor& Color,
		bool bRoundCaps = false);
	AMSIMUI_API void AppendSurfaceDisc(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Center,
		double Radius,
		const FLinearColor& Color,
		int32 Sides = 20);
	AMSIMUI_API void AppendRoundedSurfaceRectangle(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Center,
		const FVector2D& Forward,
		double Length,
		double Width,
		double CornerRadius,
		const FLinearColor& Color,
		int32 CornerSteps = 5);
	AMSIMUI_API FProceduralSurfaceMesh MakeRunwaySurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		double Width,
		const FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	AMSIMUI_API FProceduralSurfaceMesh MakeTaxiwaySurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		double Width,
		const FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	AMSIMUI_API FProceduralSurfaceMesh MakeRoadSurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		double Width,
		const FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
	AMSIMUI_API FProceduralSurfaceMesh MakeApronSurfaceMesh(
		const FVector2D& Center,
		const FVector2D& Forward,
		double Length,
		double Width,
		const FProceduralSurfacePalette& Palette,
		bool bShowDetails = true);
	AMSIMUI_API FProceduralSurfaceMesh MakeGateApronSurfaceMesh(
		const FVector2D& Center,
		const FVector2D& AirsideForward,
		double Length,
		double Width,
		const FProceduralSurfacePalette& Palette,
		bool bShowMarkings = true);
}
