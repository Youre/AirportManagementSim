#include "AMSimPhase4Fixture.h"

namespace AMSim
{
	const FPhase4Fixture& GetPhase4Fixture()
	{
		static const FPhase4Fixture Fixture;
		return Fixture;
	}

	FString Phase4FlightStateDisplayName(const EPhase4FlightState State)
	{
		switch (State)
		{
		case EPhase4FlightState::Planned: return TEXT("Planned");
		case EPhase4FlightState::Locked: return TEXT("Locked");
		case EPhase4FlightState::Early: return TEXT("Early");
		case EPhase4FlightState::OnTime: return TEXT("On time");
		case EPhase4FlightState::Late: return TEXT("Late");
		case EPhase4FlightState::Holding: return TEXT("Holding");
		case EPhase4FlightState::Diverted: return TEXT("Diverted");
		case EPhase4FlightState::AtGate: return TEXT("At gate");
		case EPhase4FlightState::Completed: return TEXT("Complete");
		case EPhase4FlightState::Cancelled: return TEXT("Cancelled");
		default: return TEXT("Unknown");
		}
	}

	FString Phase4IncidentDisplayName(const EPhase4IncidentLifecycle State)
	{
		switch (State)
		{
		case EPhase4IncidentLifecycle::Warned: return TEXT("Warned");
		case EPhase4IncidentLifecycle::Alerted: return TEXT("Alerted");
		case EPhase4IncidentLifecycle::ResourcesDispatched:
			return TEXT("Resources dispatched");
		case EPhase4IncidentLifecycle::AreaProtected:
			return TEXT("Area protected");
		case EPhase4IncidentLifecycle::Stabilized: return TEXT("Stabilized");
		case EPhase4IncidentLifecycle::Investigating:
			return TEXT("Investigating");
		case EPhase4IncidentLifecycle::Reported: return TEXT("Report ready");
		case EPhase4IncidentLifecycle::Recovered: return TEXT("Recovered");
		default: return TEXT("No active incident");
		}
	}
}
