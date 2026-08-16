#include "AMSimWorldPresenter.h"

#include "AMSimAircraftPresentation.h"

int32 AAMSimWorldPresenter::GetHeadingIndex(
	const AMSim::EFlightState FlightState)
{
	const float Degrees =
		AMSim::GetPhase1AircraftPresentationHeadingDegrees(FlightState);
	return FMath::RoundToInt(
		FMath::Fmod(Degrees + 360.0f, 360.0f) / 22.5f) % 16;
}

int32 AAMSimWorldPresenter::GetPhase2HeadingIndex(
	const AMSim::EPhase2FlightState FlightState)
{
	switch (FlightState)
	{
	case AMSim::EPhase2FlightState::Inbound:
	case AMSim::EPhase2FlightState::Approach:
	case AMSim::EPhase2FlightState::Landing:
	case AMSim::EPhase2FlightState::TaxiIn:
		return 10;
	case AMSim::EPhase2FlightState::ReadyToDepart:
	case AMSim::EPhase2FlightState::TaxiOut:
	case AMSim::EPhase2FlightState::Outbound:
		return 2;
	default:
		return 0;
	}
}
