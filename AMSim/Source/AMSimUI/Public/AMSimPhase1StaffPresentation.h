#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FPhase1StaffPanelState
	{
		int32 TotalConstructionWorkers = 0;
		int32 AvailableConstructionWorkers = 0;
		int32 AssignedConstructionWorkers = 0;
		FString Status;
		FString Detail;
	};

	class AMSIMUI_API FPhase1StaffPresentation
	{
	public:
		static FPhase1StaffPanelState Derive(
			const FPhase1QuerySnapshot& Query,
			const FPhase1State& State);
	};
}
