#include "AMSimWorldPresenter.h"

#include "AMSimPhase1AircraftPresentation.h"
#include "AMSimPhase1WorldGeometry.h"
#include "PaperSpriteComponent.h"

void AAMSimWorldPresenter::SetAircraftState(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	const AMSim::FPhase1AircraftVisualState Visual =
		AMSim::FPhase1AircraftPresentation::Derive(Query, State);
	Aircraft->SetVisibility(Visual.bVisible);
	Selection->SetVisibility(Visual.bVisible);
	if (!Visual.bVisible)
	{
		return;
	}

	const float HeadingDegrees =
		AMSim::MakePhase1TopDownMovementYawDegrees(Visual.Direction);
	const int32 HeadingIndex = FMath::RoundToInt(
		FMath::Fmod(HeadingDegrees + 360.0f, 360.0f) / 22.5f) % 16;
	if (AircraftHeadingSprites.IsValidIndex(HeadingIndex))
	{
		Aircraft->SetSprite(AircraftHeadingSprites[HeadingIndex]);
	}

	Aircraft->SetRelativeLocation(Visual.Location);
	Selection->SetRelativeLocation(FVector(
		Visual.Location.X,
		Visual.Location.Y,
		70.0));
	const bool bTurnaround = Query.FlightState == AMSim::EFlightState::Turnaround;
	Aircraft->SetRelativeScale3D(FVector(
		bTurnaround ? 13.0 : 10.0,
		1.0,
		bTurnaround ? 13.0 : 10.0));
}
