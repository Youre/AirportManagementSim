#pragma once

#include "AMSimPhase1Types.h"
#include "CoreMinimal.h"

namespace AMSim
{
	struct FPhase1ConstructionVisualState
	{
		float DeliveryTravelProgress = 0.0f;
		float SurfaceProgress = 0.0f;
		bool bCrewVisible = false;
		bool bTruckVisible = false;
		bool bEarthworkVisible = false;
		bool bProtectionVisible = false;
		bool bFinishedMarkingsVisible = false;
	};

	struct FPhase1PatrolMotion
	{
		float Progress = 0.0f;
		bool bForward = true;
	};

	class AMSIMUI_API FPhase1ConstructionPresentation
	{
	public:
		static FPhase1ConstructionVisualState Derive(
			const FPhase1QuerySnapshot& Query,
			const FConstructionProjectRecord& Project);

		static TArray<float> AllocateLengthProgress(
			float OverallProgress,
			TConstArrayView<double> SegmentLengths);

		static FPhase1PatrolMotion CalculatePatrolMotion(
			int64 ElapsedGameMilliseconds,
			int32 WorkerIndex);
	};
}
