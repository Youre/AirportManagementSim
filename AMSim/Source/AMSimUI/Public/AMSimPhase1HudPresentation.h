#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FPhase1ConstructionActivityCard
	{
		FString Header;
		FString Detail;
	};

	struct FPhase1AirportCreationLayout
	{
		float MinX = 0.0f;
		float MinY = 0.0f;
		float MaxX = 1.0f;
		float MaxY = 1.0f;
		float HorizontalPadding = 0.0f;
		float VerticalPadding = 0.0f;
		float ParcelWeight = 1.0f;
		float NameWeight = 1.0f;
	};

	class AMSIMUI_API FPhase1HudPresentation
	{
	public:
		static FPhase1ConstructionActivityCard MakeConstructionActivityCard(
			EConstructionStage Stage,
			const FStarterPlanProposal& Proposal);
		static bool ShouldUseCompactLayout(float InterfaceScale, FIntPoint ViewportSize);
		static FPhase1AirportCreationLayout MakeAirportCreationLayout(bool bCompact);
	};
}
