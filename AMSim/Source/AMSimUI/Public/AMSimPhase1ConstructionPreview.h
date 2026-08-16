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
		Snapped,
		Crossing,
		Invalid
	};

	enum class EPhase1ConstructionSurfaceStyle : uint8
	{
		Context,
		Selected,
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
		EPhase1ConstructionSurfaceStyle RunwayStyle =
			EPhase1ConstructionSurfaceStyle::Context;
		TArray<EPhase1ConstructionSurfaceStyle> TaxiwayStyles;
		EPhase1ConstructionSurfaceStyle RoadStyle =
			EPhase1ConstructionSurfaceStyle::Context;
		int32 ActiveTaxiwaySegmentIndex = INDEX_NONE;
		float MarkerScale = 4.0f;
		bool bRunwayVisible = false;
		bool bRoadVisible = false;
		bool bValid = false;
	};
}
