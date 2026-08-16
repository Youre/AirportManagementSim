#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	enum class EPhase1ConstructionPreviewTool : uint8
	{
		Runway,
		Taxiway,
		RoadAccess
	};

	enum class EPhase1ConstructionMarkerStyle : uint8
	{
		Endpoint,
		Connection,
		Pointer,
		Invalid
	};

	struct FPhase1ConstructionPreviewMarker
	{
		FPhase1Point Point;
		EPhase1ConstructionMarkerStyle Style =
			EPhase1ConstructionMarkerStyle::Endpoint;
	};

	struct FPhase1ConstructionPreviewState
	{
		FStarterPlanProposal Proposal;
		TArray<FPhase1ConstructionPreviewMarker> Markers;
		EPhase1ConstructionPreviewTool SelectedTool =
			EPhase1ConstructionPreviewTool::Runway;
		int32 ActiveTaxiwaySegmentIndex = INDEX_NONE;
		bool bRunwayVisible = false;
		bool bRoadVisible = false;
		bool bValid = false;
	};
}
