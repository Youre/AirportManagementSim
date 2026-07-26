#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	inline float GetPhase1AircraftPresentationHeadingDegrees(const EFlightState State)
	{
		switch (State)
		{
		case EFlightState::Scheduled:
		case EFlightState::Inbound:
		case EFlightState::Approach:
		case EFlightState::TaxiIn:
			return 135.0f;
		case EFlightState::Landing:
		case EFlightState::RunwayRoll:
		case EFlightState::Takeoff:
		case EFlightState::Outbound:
			return 90.0f;
		case EFlightState::Parked:
		case EFlightState::Turnaround:
		case EFlightState::Ready:
			return 180.0f;
		case EFlightState::TaxiOut:
			return 45.0f;
		default:
			return 0.0f;
		}
	}
}
