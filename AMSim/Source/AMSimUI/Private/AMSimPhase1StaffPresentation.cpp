#include "AMSimPhase1StaffPresentation.h"

#include "AMSimPhase1Fixture.h"

AMSim::FPhase1StaffPanelState AMSim::FPhase1StaffPresentation::Derive(
	const FPhase1QuerySnapshot& Query,
	const FPhase1State& State)
{
	FPhase1StaffPanelState Result;
	if (!Query.bInitialized)
	{
		Result.Status = TEXT("Create an airport to establish the starter crew.");
		return Result;
	}

	Result.TotalConstructionWorkers = GetPhase1Fixture().ConstructionWorkerCount;
	const FStaffTeamRecord* ConstructionTeam = State.Teams.FindByPredicate(
		[](const FStaffTeamRecord& Team)
		{
			return Team.RoleId == TEXT("Staff.Role.Construction");
		});
	if (ConstructionTeam && ConstructionTeam->TeamSize > 0)
	{
		Result.TotalConstructionWorkers = ConstructionTeam->TeamSize;
	}

	const bool bAssigned =
		Query.ConstructionStage >= EConstructionStage::Funded &&
		Query.ConstructionStage <= EConstructionStage::Inspection;
	Result.AssignedConstructionWorkers =
		bAssigned ? Result.TotalConstructionWorkers : 0;
	Result.AvailableConstructionWorkers =
		bAssigned ? 0 : Result.TotalConstructionWorkers;

	switch (Query.ConstructionStage)
	{
	case EConstructionStage::Funded:
	case EConstructionStage::AwaitingDelivery:
		Result.Status = TEXT("TRAVELLING TO SITE");
		Result.Detail = TEXT("The crew and truck dispatch automatically after purchase.");
		break;
	case EConstructionStage::Building:
		Result.Status = TEXT("BUILDING AIRFIELD");
		Result.Detail = TEXT("All construction workers are assigned to the active project.");
		break;
	case EConstructionStage::Inspection:
		Result.Status = TEXT("SUPPORTING INSPECTION");
		Result.Detail = TEXT("The crew remains assigned until the safety inspection clears.");
		break;
	default:
		Result.Status = TEXT("CREW AVAILABLE");
		Result.Detail = TEXT("Construction dispatch is automatic when a project is funded.");
		break;
	}
	return Result;
}
