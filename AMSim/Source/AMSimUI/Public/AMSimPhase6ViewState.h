#pragma once

#include "AMSimPhase6Types.h"

namespace AMSim
{
	struct FPhase6ViewState
	{
		uint64 Revision = 0;
		FString Status;
		FString RunwaySummary;
		FString CapacitySummary;
		FString LargeAircraftSummary;
		FString IncidentSummary;
		FString ProgressionSummary;
		FString Cause;
		FString Remedy;
		FString SelectedPathHeadline;
		FString SelectedPathEvidence;
		FString NextEvidenceLabel;
		TArray<FString> RunwayCards;
		TArray<FString> FacilityCards;
		TArray<FString> AircraftCards;
		TArray<FString> IncidentCards;
		TArray<FString> CapabilityCards;
		bool bCanInitialize = false;
		bool bCanConstruct = false;
		bool bCanActivateParallel = false;
		bool bCanAcceptLargeAircraft = false;
		bool bCanStartTurnaround = false;
		bool bCanAdvanceLargeAircraft = false;
		bool bCanAdvanceDay = false;
		bool bCanResolveConflict = false;
		bool bCanAdvanceIncident = false;
	};

	AMSIMUI_API FPhase6ViewState MakePhase6ViewState(
		const FPhase6QuerySnapshot& Query,
		const FPhase6State& State,
		ESpecializationPath SelectedPath);
}
