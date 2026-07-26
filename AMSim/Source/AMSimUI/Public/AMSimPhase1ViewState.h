#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct AMSIMUI_API FPhase1ViewState
	{
		uint64 Revision = MAX_uint64;
		FString AirportName;
		FString Clock;
		FString Funds;
		FString Objective;
		FString Status;
		FString Cause;
		FString Remedy;
		FString Project;
		FString Offer;
		FString Compatibility;
		FString Flight;
		FString Timetable;
		FString Services;
		FString Ledger;
		FString Rating;
		FString Caption;
		bool bAirportInitialized = false;
		bool bCanBuild = false;
		bool bCanCancelBuild = false;
		bool bCanOpen = false;
		bool bCanClose = false;
		bool bCanPinOffer = false;
		bool bCanDeclineOffer = false;
		bool bCanAcceptOffer = false;
		bool bCanSchedule = false;
		bool bCanRequestRecovery = false;
	};

	AMSIMUI_API FPhase1ViewState MakePhase1ViewState(
		const FPhase1QuerySnapshot& Query,
		const FPhase1State& State);
}

