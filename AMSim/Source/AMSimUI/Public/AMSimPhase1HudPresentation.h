#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FPhase1ConstructionActivityCard
	{
		FString Header;
		FString Detail;
	};

	class AMSIMUI_API FPhase1HudPresentation
	{
	public:
		static FPhase1ConstructionActivityCard MakeConstructionActivityCard(
			EConstructionStage Stage,
			const FStarterPlanProposal& Proposal);
	};
}
