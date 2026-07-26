#pragma once

#include "AMSimPhase3Types.h"

namespace AMSim
{
	struct AMSIMUI_API FPhase3ViewState
	{
		uint64 Revision = MAX_uint64;
		FString Status;
		FString Flight;
		FString PassengerCounts;
		FString Security;
		FString FeaturedParty;
		FString FeaturedStep;
		FString FeaturedNeeds;
		FString FeaturedRoute;
		FString Baggage;
		FString Landside;
		FString Construction;
		int32 FeaturedTimeConfidencePercent = 0;
		float FeaturedStepProgress = 0.0f;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bShowTerminal = false;
		bool bCanInitialize = false;
		bool bCanFund = false;
		bool bCanConnect = false;
		bool bCanOpen = false;
		bool bCanSchedule = false;
		bool bCanToggleSecurity = false;
		bool bCanRequestAssistance = false;
		bool bCanResolveBaggage = false;
	};

	AMSIMUI_API FPhase3ViewState MakePhase3ViewState(
		const FPhase3QuerySnapshot& Query,
		const FPhase3State& State);
}
