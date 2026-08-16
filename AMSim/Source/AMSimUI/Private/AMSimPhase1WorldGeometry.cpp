#include "AMSimPhase1WorldGeometry.h"

namespace AMSim
{
	namespace
	{
		constexpr double ParcelCenterCentimeters = 50000.0;
		constexpr double WorldUnitsPerCentimeter = 1.2;
		constexpr double SpriteWorldSize = 1600.0;
	}

	FVector MapPhase1PointToWorld(
		const FPhase1Point& Point,
		const double Height)
	{
		return FVector(
			(ParcelCenterCentimeters - static_cast<double>(Point.Y)) *
				WorldUnitsPerCentimeter,
			(static_cast<double>(Point.X) - ParcelCenterCentimeters) *
				WorldUnitsPerCentimeter,
			Height);
	}

	FPhase1Point MapPhase1WorldToPoint(const FVector& WorldPosition)
	{
		return {
			FMath::RoundToInt64(
				ParcelCenterCentimeters +
				static_cast<double>(WorldPosition.Y) / WorldUnitsPerCentimeter),
			FMath::RoundToInt64(
				ParcelCenterCentimeters -
				static_cast<double>(WorldPosition.X) / WorldUnitsPerCentimeter)};
	}

	FPhase1WorldSegmentGeometry MakePhase1WorldSegmentGeometry(
		const FPhase1Point& Start,
		const FPhase1Point& End,
		const int64 WidthCentimeters,
		const double Height)
	{
		const FVector WorldStart = MapPhase1PointToWorld(Start, Height);
		const FVector WorldEnd = MapPhase1PointToWorld(End, Height);
		const FVector Delta = WorldEnd - WorldStart;
		FPhase1WorldSegmentGeometry Result;
		Result.Center = (WorldStart + WorldEnd) * 0.5;
		Result.Scale = FVector(
			FMath::Max(FVector2D(Delta.X, Delta.Y).Size() / SpriteWorldSize, 0.05),
			1.0,
			FMath::Max(
				static_cast<double>(WidthCentimeters) *
					WorldUnitsPerCentimeter / SpriteWorldSize,
				0.05));
		Result.YawDegrees = FMath::RadiansToDegrees(
			FMath::Atan2(Delta.Y, Delta.X));
		return Result;
	}

	float MakePhase1RunwayNumberYawDegrees(
		const float RunwayYawDegrees,
		const bool bReciprocalEnd)
	{
		return FMath::Fmod(
			RunwayYawDegrees + (bReciprocalEnd ? 0.0f : 180.0f) + 360.0f,
			360.0f);
	}

	float MakePhase1TopDownMovementYawDegrees(const FVector& Direction)
	{
		const FVector2D PlanarDirection(Direction.X, Direction.Y);
		if (PlanarDirection.IsNearlyZero())
		{
			return 0.0f;
		}
		const float DirectionYaw = FMath::RadiansToDegrees(
			FMath::Atan2(PlanarDirection.Y, PlanarDirection.X));
		return FMath::Fmod(DirectionYaw + 450.0f, 360.0f);
	}
}
