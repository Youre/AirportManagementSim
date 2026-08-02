#include "AMSimRegionalOperationsView.h"

#include "AMSimWorldPresenter.h"
#include "Algo/Count.h"
#include "EngineUtils.h"

int32 UAMSimRegionalOperationsView::GetLoadedContractIdentityCount() const
{
	return Algo::CountIf(
		ContractIdentityTextures,
		[](const UTexture2D* Texture)
		{
			return Texture != nullptr;
		});
}

bool UAMSimRegionalOperationsView::HasRequiredContractIdentityArt() const
{
	return GetLoadedContractIdentityCount() == 3 &&
		ContractAircraftTexture;
}

void UAMSimRegionalOperationsView::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::PauseSimulation()
{
	SubmitSpeed(0, true);
}

void UAMSimRegionalOperationsView::SetSpeedOne()
{
	SubmitSpeed(1);
}

void UAMSimRegionalOperationsView::SetSpeedTwo()
{
	SubmitSpeed(2);
}

void UAMSimRegionalOperationsView::SetSpeedFour()
{
	SubmitSpeed(4);
}

void UAMSimRegionalOperationsView::SetSpeedEight()
{
	SubmitSpeed(8);
}

void UAMSimRegionalOperationsView::UpdateWorldPresentation(
	const AMSim::FPhase4QuerySnapshot& Query,
	const AMSim::FPhase4State& State)
{
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->ApplyPhase4Snapshot(Query, State);
		const bool bIncidentMode =
			Query.IncidentLifecycle >=
				AMSim::EPhase4IncidentLifecycle::Alerted &&
			Query.IncidentLifecycle <
				AMSim::EPhase4IncidentLifecycle::Recovered;
		It->SetMatureOverviewMode(
			bOverviewOpen || bIncidentMode);
		break;
	}
}

void UAMSimRegionalOperationsView::ShowAdvancedOperations()
{
	bAdvancedOperationsOpen = true;
	bMajorOperationsOpen = false;
	bOverviewOpen = false;
	bProgressionOpen = false;
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::ShowRegionalOperations()
{
	bAdvancedOperationsOpen = false;
	bMajorOperationsOpen = false;
	bOverviewOpen = false;
	bProgressionOpen = false;
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::ToggleAdvancedOperations()
{
	if (bAdvancedOperationsOpen)
	{
		CloseAdvancedOperations();
	}
	else
	{
		ShowAdvancedOperations();
	}
}

void UAMSimRegionalOperationsView::CloseAdvancedOperations()
{
	bAdvancedOperationsOpen = false;
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::ShowMajorOperations()
{
	bMajorOperationsOpen = true;
	bAdvancedOperationsOpen = false;
	bOverviewOpen = false;
	bProgressionOpen = false;
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::ToggleMajorOperations()
{
	if (bMajorOperationsOpen)
	{
		CloseMajorOperations();
	}
	else
	{
		ShowMajorOperations();
	}
}

void UAMSimRegionalOperationsView::CloseMajorOperations()
{
	bMajorOperationsOpen = false;
	RefreshFromSimulation();
}

void UAMSimRegionalOperationsView::ReturnToAirport()
{
	bAdvancedOperationsOpen = false;
	bMajorOperationsOpen = false;
	OnReturnRequested.ExecuteIfBound();
}
