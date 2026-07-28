#pragma once

#include "AMSimPhase1Types.h"
#include "AMSimPhase2Types.h"
#include "AMSimPhase3Types.h"
#include "AMSimPhase4Types.h"

namespace AMSim
{
	struct AMSIMUI_API FProgressionPathViewState
	{
		FString Name;
		FString Summary;
		FString Bands;
		FString Status;
		bool bSelected = false;
		bool bFutureLocked = false;
	};

	struct AMSIMUI_API FProgressionViewState
	{
		uint64 Revision = MAX_uint64;
		FString Funds;
		TArray<FString> Objectives;
		TArray<float> ObjectiveProgress;
		TArray<FProgressionPathViewState> Paths;
		FString SelectedPath;
		FString Inspector;
		FString CombinedPathExplanation;
	};

	AMSIMUI_API FProgressionViewState MakeProgressionViewState(
		const FPhase1QuerySnapshot& Phase1,
		const FPhase2QuerySnapshot& Phase2,
		const FPhase2State& Phase2State,
		const FPhase3QuerySnapshot& Phase3,
		const FPhase4QuerySnapshot& Phase4);
}
