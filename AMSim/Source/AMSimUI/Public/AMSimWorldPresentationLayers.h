#pragma once

#include "CoreMinimal.h"

namespace AMSim::WorldPresentationLayers
{
	struct FLayer
	{
		double Height = 0.0;
		int32 SortPriority = 0;
	};

	inline constexpr FLayer Terrain{0.0, 0};
	inline constexpr FLayer PlanningGrid{1.0, 1};
	inline constexpr FLayer ParcelBoundary{2.0, 2};
	inline constexpr FLayer ConstructionBed{5.0, 5};
	inline constexpr FLayer ServiceRoad{7.0, 8};
	inline constexpr FLayer Runway{10.0, 10};
	inline constexpr FLayer Taxiway{18.0, 18};
	inline constexpr FLayer ProposalPattern{26.0, 26};
	inline constexpr FLayer GateA{30.0, 30};
	inline constexpr FLayer GateB{31.0, 31};
	inline constexpr FLayer Structure{40.0, 40};
	inline constexpr FLayer TerminalRoof{47.0, 47};
	inline constexpr FLayer ProposalMarker{90.0, 90};
}
