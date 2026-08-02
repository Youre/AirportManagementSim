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

	class AMSIMUI_API FPhase1ConstructionPresentation
	{
	public:
		static FPhase1ConstructionVisualState Derive(
			const FPhase1QuerySnapshot& Query,
			const FConstructionProjectRecord& Project);

		static TArray<float> AllocateLengthProgress(
			float OverallProgress,
			TConstArrayView<double> SegmentLengths);
	};
}
