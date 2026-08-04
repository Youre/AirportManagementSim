#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	enum class EPhase1OperationsPage : uint8
	{
		Airfield,
		Schedule,
		Overlays
	};

	struct FPhase1OperationsHubState
	{
		FString Eyebrow;
		FString Title;
		FString Status;
		FString Detail;
		FString Timeline;
		FString Footer;
		FString AdvanceLabel = TEXT("ADVANCE TO ARRIVAL");
		bool bShowChooseArrival = false;
		bool bShowWatch = false;
		bool bShowAdvance = false;
		bool bShowOverlayModes = false;
	};

	class AMSIMUI_API FPhase1OperationsHubPresentation
	{
	public:
		static FPhase1OperationsHubState Derive(
			EPhase1OperationsPage Page,
			const FPhase1QuerySnapshot& Query,
			const FPhase1State& State,
			int32 OverlayMode);
		static FString FlightStateDisplayName(EFlightState State);
		static FString FormatGameTime(int64 GameTimeMilliseconds);
	};
}
