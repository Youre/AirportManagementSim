#include "AMSimPhase1HudPresentation.h"

#include "AMSimPhase1Fixture.h"

namespace
{
	FString ConstructionStageLabel(const AMSim::EConstructionStage Stage)
	{
		switch (Stage)
		{
		case AMSim::EConstructionStage::Building:
			return TEXT("SURFACE WORK");
		case AMSim::EConstructionStage::Inspection:
			return TEXT("SAFETY INSPECTION");
		case AMSim::EConstructionStage::ReadyToOpen:
			return TEXT("READY TO OPEN");
		case AMSim::EConstructionStage::Funded:
		case AMSim::EConstructionStage::AwaitingDelivery:
			return TEXT("CREW EN ROUTE");
		default:
			return TEXT("INACTIVE");
		}
	}
}

namespace AMSim
{
	bool FPhase1HudPresentation::ShouldUseCompactLayout(
		const float InterfaceScale,
		const FIntPoint ViewportSize)
	{
		return InterfaceScale >= 1.75f ||
			ViewportSize.X < 1760 ||
			ViewportSize.Y < 990;
	}

	FPhase1AirportCreationLayout FPhase1HudPresentation::MakeAirportCreationLayout(
		const bool bCompact)
	{
		if (bCompact)
		{
			return { 0.20f, 0.58f, 0.80f, 0.89f, 12.0f, 10.0f, 1.0f, 1.0f };
		}
		return { 0.18f, 0.77f, 0.82f, 0.92f, 16.0f, 12.0f, 0.85f, 1.35f };
	}

	FPhase1ConstructionActivityCard FPhase1HudPresentation::
		MakeConstructionActivityCard(
			const EConstructionStage Stage,
			const FStarterPlanProposal& Proposal)
	{
		const FRunwayDesignation Designation = CalculateRunwayDesignation(
			Proposal.RunwayStart,
			Proposal.RunwayEnd);
		FPhase1ConstructionActivityCard Card;
		Card.Header = FString::Printf(
			TEXT("CONSTRUCTION  •  %s"),
			*ConstructionStageLabel(Stage));
		Card.Detail = FString::Printf(
			TEXT("RWY %02d/%02d  •  %s CR"),
			Designation.PrimaryNumber,
			Designation.ReciprocalNumber,
			*FText::AsNumber(GetPhase1Fixture().StarterPlanCost).ToString());
		return Card;
	}
}
