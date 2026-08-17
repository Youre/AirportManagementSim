#pragma once

#include "AMSimMatureAirportLayout.h"

namespace AMSim::TerminalPresentationGeometry
{
	// Terminal layouts retain one stable logical construction grid in the
	// simulation. In the airport world, each logical cell is presented as a
	// a sixteen-meter module so starter and regional terminals read as primary
	// facilities beside gameplay-scaled aprons, gates, roads, and aircraft.
	inline constexpr float PresentedMetersPerCell = 16.0f;
	inline constexpr float WorldUnitsPerMeter = 100.0f;
	inline constexpr float CellWorldUnits =
		PresentedMetersPerCell * WorldUnitsPerMeter;

	// The modular floor art was normalized for a one-meter, 100-world-unit cell.
	inline constexpr float BaseCellSpriteScale = 0.125f;
	inline constexpr float CellSpriteScale =
		BaseCellSpriteScale * PresentedMetersPerCell;

	// The legacy hut marker sits below the mature landside composition. Move the
	// doubled roofless footprint between the apron and access road so it remains
	// fully visible in the shared overview without a facility-owned camera.
	inline constexpr float CampusOffsetWorldX =
		static_cast<float>(
			MatureAirportLayout::TerminalCenterWorldX + 38400.0);

	// Static furniture uses the full presentation cell so its uniform,
	// footprint-safe fit remains proportional to the enlarged room geometry.
	inline constexpr float FurnitureWorldUnitsPerLogicalCell = CellWorldUnits;

	// People, bags, and vehicles remain smaller than a full cell, but need a
	// stronger readability scale than the previous dot-sized overview pass.
	inline constexpr float IdentityScale = 12.0f;
}
