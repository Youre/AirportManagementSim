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
