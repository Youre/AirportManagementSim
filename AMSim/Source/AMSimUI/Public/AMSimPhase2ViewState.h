#pragma once

#include "AMSimPhase2Types.h"

namespace AMSim
{
	struct AMSIMUI_API FPhase2ViewState
	{
		uint64 Revision = MAX_uint64;
		FString Status;
		FString Operations;
		FString Weather;
		FString Staff;
		FString Tenant;
		FString Economy;
		FString Cause;
		FString Remedy;
		bool bUnlocked = false;
		bool bInitialized = false;
		bool bCanSelectIdentity = false;
		bool bCanAcceptContract = false;
		bool bCanCancelContract = false;
		bool bCanRescheduleFlight = false;
		bool bCanDispatchService = false;
		bool bCanTow = false;
		bool bCanPurchaseParcel = false;
		bool bCanStartExpansion = false;
		bool bCanRespondToIncident = false;
		bool bCanRequestRecovery = false;
	};

	AMSIMUI_API FPhase2ViewState MakePhase2ViewState(
		const FPhase2QuerySnapshot& Query,
		const FPhase2State& State,
		const FPhase1State& Phase1State);
}
