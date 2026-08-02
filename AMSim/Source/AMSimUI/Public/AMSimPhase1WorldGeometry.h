#pragma once

#include "AMSimPhase1Types.h"
#include "CoreMinimal.h"

namespace AMSim
{
	struct FPhase1WorldSegmentGeometry
	{
		FVector Center = FVector::ZeroVector;
		FVector Scale = FVector::OneVector;
		float YawDegrees = 0.0f;
	};

	AMSIMUI_API FVector MapPhase1PointToWorld(
		const FPhase1Point& Point,
		double Height = 0.0);
	AMSIMUI_API FPhase1WorldSegmentGeometry MakePhase1WorldSegmentGeometry(
		const FPhase1Point& Start,
		const FPhase1Point& End,
		int64 WidthCentimeters,
		double Height);
	AMSIMUI_API float MakePhase1RunwayNumberYawDegrees(
		float RunwayYawDegrees,
		bool bReciprocalEnd);
}
