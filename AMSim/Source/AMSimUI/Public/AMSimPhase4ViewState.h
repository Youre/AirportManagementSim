#pragma once

#include "AMSimPhase4Types.h"

namespace AMSim
{
	struct AMSIMUI_API FPhase4ViewState
	{
		uint64 Revision = MAX_uint64;
		FString Status;
		FString Timetable;
		FString SelectedFlight;
		FString SelectedFlightDetail;
		FString Feasibility;
		FString Connections;
		FString Border;
		FString Transport;
		FString Weather;
		FString Forecast;
		FString IncidentHeadline;
		FString IncidentCause;
		FString IncidentConsequence;
		FString IncidentRemedy;
		FString IncidentLifecycle;
		FString Caption;
		FString Renewal;
		bool bVisible = false;
		bool bIncidentMode = false;
		bool bCanInitialize = false;
		bool bCanAcceptContract = false;
		bool bCanPublish = false;
		bool bCanReviewGateChange = false;
		bool bCanHold = false;
		bool bCanDivert = false;
		bool bCanDispatchTow = false;
		bool bCanProtect = false;
		bool bCanReviewIncident = false;
		bool bCanRecover = false;
		bool bCanRenew = false;
	};

	AMSIMUI_API FPhase4ViewState MakePhase4ViewState(
		const FPhase4QuerySnapshot& Query,
		const FPhase4State& State);
}
