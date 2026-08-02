#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FPhase1AircraftVisualState
	{
		bool bVisible = false;
		FVector Location = FVector::ZeroVector;
		FVector Direction = FVector(0.0, -1.0, 0.0);
		float StateProgress = 0.0f;
		int32 TaxiPathPointCount = 0;
	};

	class AMSIMUI_API FPhase1AircraftPresentation
	{
	public:
		static FPhase1AircraftVisualState Derive(
			const FPhase1QuerySnapshot& Query,
			const FPhase1State& State);
		static TArray<FVector> BuildTaxiPath(
			const FStarterPlanProposal& Proposal,
			double Height = 60.0);
	};
}
