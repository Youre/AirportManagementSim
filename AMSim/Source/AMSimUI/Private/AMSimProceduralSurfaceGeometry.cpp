#include "AMSimProceduralSurfaceGeometry.h"

namespace AMSim
{
	namespace
	{
		constexpr double MinimumSurfaceSize = 1.0;
		constexpr double WorldUvScale = 1.0 / 4096.0;

		void AppendVertex(
			FProceduralSurfaceMesh& Mesh,
			const FVector2D& Point,
			const FLinearColor& Color)
		{
			Mesh.Vertices.Add(FVector(Point.X, Point.Y, 0.0));
			Mesh.Normals.Add(FVector::UpVector);
			Mesh.UV0.Add(Point * WorldUvScale);
			Mesh.Colors.Add(Color);
		}

		void AppendTriangle(
			FProceduralSurfaceMesh& Mesh,
			const int32 A,
			const int32 B,
			const int32 C)
		{
			Mesh.Triangles.Append({A, B, C});
		}

		FVector2D SafeDirection(
			const FVector2D& Start,
			const FVector2D& End)
		{
			const FVector2D Delta = End - Start;
			return Delta.IsNearlyZero()
				? FVector2D(1.0, 0.0)
				: Delta.GetSafeNormal();
		}

		FVector2D RotateLocal(
			const FVector2D& Center,
			const FVector2D& Forward,
			const FVector2D& Local)
		{
			const FVector2D Right(-Forward.Y, Forward.X);
			return Center + Forward * Local.X + Right * Local.Y;
		}

		void AppendDashedSegment(
			FProceduralSurfaceMesh& Mesh,
			const FVector2D& Start,
			const FVector2D& End,
			const double Width,
			const double DashLength,
			const double GapLength,
			const FLinearColor& Color)
		{
			const double Length = (End - Start).Size();
			if (Length <= MinimumSurfaceSize)
			{
				return;
			}
			const FVector2D Direction = (End - Start) / Length;
			for (double Offset = GapLength; Offset < Length - GapLength;
				 Offset += DashLength + GapLength)
			{
				const double DashEnd = FMath::Min(Offset + DashLength, Length - GapLength);
				if (DashEnd > Offset)
				{
					AppendSurfaceSegment(
						Mesh,
						Start + Direction * Offset,
						Start + Direction * DashEnd,
						Width,
						Color,
						true);
				}
			}
		}
	}

	FProceduralSurfacePalette MakeOperationalRunwayPalette()
	{
		return {
			FLinearColor(0.055f, 0.065f, 0.065f, 1.0f),
			FLinearColor(0.92f, 0.91f, 0.82f, 1.0f),
			FLinearColor(0.92f, 0.91f, 0.82f, 1.0f),
			FLinearColor(0.95f, 0.78f, 0.20f, 1.0f)};
	}

	FProceduralSurfacePalette MakeOperationalTaxiwayPalette()
	{
		return {
			FLinearColor(0.09f, 0.10f, 0.095f, 1.0f),
			FLinearColor(0.22f, 0.23f, 0.20f, 1.0f),
			FLinearColor(0.94f, 0.75f, 0.16f, 1.0f),
			FLinearColor(0.95f, 0.78f, 0.20f, 1.0f)};
	}

	FProceduralSurfacePalette MakeOperationalRoadPalette()
	{
		return {
			FLinearColor(0.12f, 0.13f, 0.12f, 1.0f),
			FLinearColor(0.45f, 0.48f, 0.43f, 0.85f),
			FLinearColor(0.92f, 0.78f, 0.24f, 0.95f),
			FLinearColor(0.95f, 0.78f, 0.20f, 1.0f)};
	}

	FProceduralSurfacePalette MakeOperationalApronPalette()
	{
		return {
			FLinearColor(0.15f, 0.16f, 0.14f, 1.0f),
			FLinearColor(0.38f, 0.39f, 0.34f, 1.0f),
			FLinearColor(0.82f, 0.68f, 0.21f, 1.0f),
			FLinearColor(0.93f, 0.38f, 0.20f, 1.0f)};
	}

	FProceduralSurfacePalette MakeOperationalGatePalette()
	{
		return {
			FLinearColor(0.24f, 0.20f, 0.13f, 1.0f),
			FLinearColor(0.50f, 0.46f, 0.36f, 1.0f),
			FLinearColor(0.95f, 0.77f, 0.19f, 1.0f),
			FLinearColor(0.93f, 0.38f, 0.20f, 1.0f)};
	}

	void AppendSurfaceSegment(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Start,
		const FVector2D& End,
		const double Width,
		const FLinearColor& Color,
		const bool bRoundCaps)
	{
		const FVector2D Direction = SafeDirection(Start, End);
		const double Length = (End - Start).Size();
		if (Length <= MinimumSurfaceSize || Width <= MinimumSurfaceSize)
		{
			return;
		}
		const FVector2D Normal(-Direction.Y, Direction.X);
		const FVector2D HalfWidth = Normal * Width * 0.5;
		const int32 First = Mesh.Vertices.Num();
		AppendVertex(Mesh, Start - HalfWidth, Color);
		AppendVertex(Mesh, End - HalfWidth, Color);
		AppendVertex(Mesh, End + HalfWidth, Color);
		AppendVertex(Mesh, Start + HalfWidth, Color);
		AppendTriangle(Mesh, First, First + 1, First + 2);
		AppendTriangle(Mesh, First, First + 2, First + 3);
		if (bRoundCaps)
		{
			AppendSurfaceDisc(Mesh, Start, Width * 0.5, Color);
			AppendSurfaceDisc(Mesh, End, Width * 0.5, Color);
		}
	}

	void AppendSurfaceDisc(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Center,
		const double Radius,
		const FLinearColor& Color,
		const int32 Sides)
	{
		if (Radius <= MinimumSurfaceSize || Sides < 3)
		{
			return;
		}
		const int32 First = Mesh.Vertices.Num();
		AppendVertex(Mesh, Center, Color);
		for (int32 Index = 0; Index < Sides; ++Index)
		{
			const double Angle = TWO_PI * static_cast<double>(Index) / Sides;
			AppendVertex(
				Mesh,
				Center + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius,
				Color);
		}
		for (int32 Index = 0; Index < Sides; ++Index)
		{
			AppendTriangle(
				Mesh,
				First,
				First + 1 + Index,
				First + 1 + ((Index + 1) % Sides));
		}
	}

	void AppendRoundedSurfaceRectangle(
		FProceduralSurfaceMesh& Mesh,
		const FVector2D& Center,
		const FVector2D& ForwardInput,
		const double Length,
		const double Width,
		const double CornerRadius,
		const FLinearColor& Color,
		const int32 CornerSteps)
	{
		if (Length <= MinimumSurfaceSize || Width <= MinimumSurfaceSize)
		{
			return;
		}
		const FVector2D Forward = ForwardInput.IsNearlyZero()
			? FVector2D(1.0, 0.0)
			: ForwardInput.GetSafeNormal();
		const double HalfLength = Length * 0.5;
		const double HalfWidth = Width * 0.5;
		const double Radius = FMath::Clamp(
			CornerRadius,
			0.0,
			FMath::Min(HalfLength, HalfWidth));
		const int32 Steps = FMath::Max(CornerSteps, 1);
		const int32 First = Mesh.Vertices.Num();
		AppendVertex(Mesh, Center, Color);
		const FVector2D CornerCenters[] = {
			{HalfLength - Radius, HalfWidth - Radius},
			{-HalfLength + Radius, HalfWidth - Radius},
			{-HalfLength + Radius, -HalfWidth + Radius},
			{HalfLength - Radius, -HalfWidth + Radius}};
		for (int32 Corner = 0; Corner < 4; ++Corner)
		{
			for (int32 Step = 0; Step <= Steps; ++Step)
			{
				const double Angle =
					(HALF_PI * Corner) +
					(HALF_PI * static_cast<double>(Step) / Steps);
				const FVector2D Local = CornerCenters[Corner] +
					FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius;
				AppendVertex(Mesh, RotateLocal(Center, Forward, Local), Color);
			}
		}
		const int32 PerimeterCount = Mesh.Vertices.Num() - First - 1;
		for (int32 Index = 0; Index < PerimeterCount; ++Index)
		{
			AppendTriangle(
				Mesh,
				First,
				First + 1 + Index,
				First + 1 + ((Index + 1) % PerimeterCount));
		}
	}

	FProceduralSurfaceMesh MakeRunwaySurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		const double Width,
		const FProceduralSurfacePalette& Palette,
		const bool bShowMarkings)
	{
		FProceduralSurfaceMesh Mesh;
		AppendSurfaceSegment(Mesh, Start, End, Width, Palette.Base, false);
		if (!bShowMarkings)
		{
			return Mesh;
		}
		const FVector2D Direction = SafeDirection(Start, End);
		const FVector2D Normal(-Direction.Y, Direction.X);
		const double EdgeInset = FMath::Min(260.0, Width * 0.10);
		const double EdgeWidth = FMath::Clamp(Width * 0.025, 90.0, 180.0);
		for (const double Side : {-1.0, 1.0})
		{
			const FVector2D Offset = Normal * Side * (Width * 0.5 - EdgeInset);
			AppendSurfaceSegment(
				Mesh, Start + Offset, End + Offset, EdgeWidth, Palette.Edge, false);
		}
		AppendDashedSegment(
			Mesh,
			Start,
			End,
			FMath::Clamp(Width * 0.025, 100.0, 190.0),
			3400.0,
			2400.0,
			Palette.Center);
		const double Length = (End - Start).Size();
		const double ThresholdInset = FMath::Min(Length * 0.09, 6500.0);
		const double BarLength = FMath::Max(Width * 0.62, 800.0);
		for (const double Alpha : {
			ThresholdInset / Length,
			1.0 - ThresholdInset / Length})
		{
			const FVector2D Center = FMath::Lerp(Start, End, Alpha);
			AppendSurfaceSegment(
				Mesh,
				Center - Normal * BarLength * 0.5,
				Center + Normal * BarLength * 0.5,
				EdgeWidth * 1.35,
				Palette.Edge,
				false);
		}
		return Mesh;
	}

	FProceduralSurfaceMesh MakeTaxiwaySurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		const double Width,
		const FProceduralSurfacePalette& Palette,
		const bool bShowMarkings)
	{
		FProceduralSurfaceMesh Mesh;
		AppendSurfaceSegment(Mesh, Start, End, Width, Palette.Edge, true);
		AppendSurfaceSegment(
			Mesh, Start, End, Width * 0.88, Palette.Base, true);
		if (bShowMarkings)
		{
			AppendSurfaceSegment(
				Mesh,
				Start,
				End,
				FMath::Clamp(Width * 0.055, 80.0, 150.0),
				Palette.Center,
				true);
		}
		return Mesh;
	}

	FProceduralSurfaceMesh MakeRoadSurfaceMesh(
		const FVector2D& Start,
		const FVector2D& End,
		const double Width,
		const FProceduralSurfacePalette& Palette,
		const bool bShowMarkings)
	{
		FProceduralSurfaceMesh Mesh;
		AppendSurfaceSegment(Mesh, Start, End, Width, Palette.Edge, true);
		AppendSurfaceSegment(
			Mesh, Start, End, Width * 0.86, Palette.Base, true);
		if (!bShowMarkings)
		{
			return Mesh;
		}
		const FVector2D Direction = SafeDirection(Start, End);
		const FVector2D Normal(-Direction.Y, Direction.X);
		for (const double Side : {-1.0, 1.0})
		{
			const FVector2D Offset = Normal * Side * Width * 0.40;
			AppendSurfaceSegment(
				Mesh, Start + Offset, End + Offset, 65.0, Palette.Edge, true);
		}
		AppendDashedSegment(
			Mesh, Start, End, 65.0, 1800.0, 1400.0, Palette.Center);
		return Mesh;
	}

	FProceduralSurfaceMesh MakeApronSurfaceMesh(
		const FVector2D& Center,
		const FVector2D& ForwardInput,
		const double Length,
		const double Width,
		const FProceduralSurfacePalette& Palette,
		const bool bShowDetails)
	{
		FProceduralSurfaceMesh Mesh;
		const FVector2D Forward = ForwardInput.IsNearlyZero()
			? FVector2D(1.0, 0.0)
			: ForwardInput.GetSafeNormal();
		const FVector2D Right(-Forward.Y, Forward.X);
		const double BorderInset = FMath::Clamp(
			FMath::Min(Length, Width) * 0.025,
			180.0,
			520.0);
		AppendRoundedSurfaceRectangle(
			Mesh,
			Center,
			Forward,
			Length,
			Width,
			FMath::Min(1800.0, Width * 0.10),
			Palette.Edge);
		AppendRoundedSurfaceRectangle(
			Mesh,
			Center,
			Forward,
			FMath::Max(Length - BorderInset * 2.0, MinimumSurfaceSize),
			FMath::Max(Width - BorderInset * 2.0, MinimumSurfaceSize),
			FMath::Min(1500.0, Width * 0.08),
			Palette.Base);
		if (!bShowDetails)
		{
			return Mesh;
		}

		const FLinearColor SeamColor = FMath::Lerp(
			Palette.Base,
			Palette.Edge,
			0.42f);
		const double UsableLength = Length - BorderInset * 3.0;
		const double UsableWidth = Width - BorderInset * 3.0;
		const double LengthStep = FMath::Clamp(Length / 8.0, 6000.0, 10000.0);
		for (double Offset = -Length * 0.5 + LengthStep;
			 Offset < Length * 0.5 - LengthStep * 0.45;
			 Offset += LengthStep)
		{
			const FVector2D SeamCenter = Center + Forward * Offset;
			AppendSurfaceSegment(
				Mesh,
				SeamCenter - Right * UsableWidth * 0.5,
				SeamCenter + Right * UsableWidth * 0.5,
				55.0,
				SeamColor,
				false);
		}
		const double WidthStep = FMath::Clamp(Width / 5.0, 5000.0, 8000.0);
		for (double Offset = -Width * 0.5 + WidthStep;
			 Offset < Width * 0.5 - WidthStep * 0.45;
			 Offset += WidthStep)
		{
			const FVector2D SeamCenter = Center + Right * Offset;
			AppendSurfaceSegment(
				Mesh,
				SeamCenter - Forward * UsableLength * 0.5,
				SeamCenter + Forward * UsableLength * 0.5,
				55.0,
				SeamColor,
				false);
		}
		return Mesh;
	}

	FProceduralSurfaceMesh MakeGateApronSurfaceMesh(
		const FVector2D& Center,
		const FVector2D& AirsideForwardInput,
		const double Length,
		const double Width,
		const FProceduralSurfacePalette& Palette,
		const bool bShowMarkings)
	{
		FProceduralSurfaceMesh Mesh;
		const FVector2D Forward = AirsideForwardInput.IsNearlyZero()
			? FVector2D(1.0, 0.0)
			: AirsideForwardInput.GetSafeNormal();
		const double BorderInset = FMath::Clamp(Width * 0.035, 150.0, 360.0);
		AppendRoundedSurfaceRectangle(
			Mesh,
			Center,
			Forward,
			Length,
			Width,
			FMath::Min(1400.0, Width * 0.16),
			Palette.Edge);
		AppendRoundedSurfaceRectangle(
			Mesh,
			Center,
			Forward,
			FMath::Max(Length - BorderInset * 2.0, MinimumSurfaceSize),
			FMath::Max(Width - BorderInset * 2.0, MinimumSurfaceSize),
			FMath::Min(1200.0, Width * 0.13),
			Palette.Base);
		if (!bShowMarkings)
		{
			return Mesh;
		}
		const FVector2D Right(-Forward.Y, Forward.X);
		const FVector2D AirsideEdge = Center + Forward * Length * 0.48;
		const FVector2D StopCenter = Center - Forward * Length * 0.10;
		AppendSurfaceSegment(
			Mesh,
			AirsideEdge,
			StopCenter,
			120.0,
			Palette.Center,
			true);
		AppendSurfaceSegment(
			Mesh,
			StopCenter - Right * Width * 0.24,
			StopCenter + Right * Width * 0.24,
			180.0,
			Palette.Safety,
			false);
		const double EnvelopeHalfLength = Length * 0.30;
		const double EnvelopeHalfWidth = Width * 0.33;
		for (const double Side : {-1.0, 1.0})
		{
			const FVector2D Offset = Right * Side * EnvelopeHalfWidth;
			AppendDashedSegment(
				Mesh,
				Center - Forward * EnvelopeHalfLength + Offset,
				Center + Forward * EnvelopeHalfLength + Offset,
				120.0,
				900.0,
				650.0,
				Palette.Safety);
		}
		return Mesh;
	}
}
