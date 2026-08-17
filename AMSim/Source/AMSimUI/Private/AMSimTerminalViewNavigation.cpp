#include "AMSimTerminalView.h"

#include "AMSimRegionalOperationsView.h"
#include "AMSimWorldPresenter.h"
#include "EngineUtils.h"

void UAMSimTerminalView::ShowRegionalOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Regional;
	SetTerminalInteractionEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowRegionalOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowAdvancedOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Advanced;
	SetTerminalInteractionEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowAdvancedOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowMajorOperations()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Major;
	SetTerminalInteractionEnabled(false);
	if (RegionalOperationsView)
	{
		RegionalOperationsView->ShowMajorOperations();
	}
	RefreshFromSimulation();
	SetKeyboardFocus();
}

void UAMSimTerminalView::ShowPresentation()
{
	bHasBeenOpened = true;
	bPresentationOpen = true;
	PresentationDestination = EAMSimTerminalPresentationDestination::Terminal;
	// Refresh restored spatial state before enabling contextual terminal tools.
	RefreshFromSimulation();
	SetTerminalInteractionEnabled(true);
	SetKeyboardFocus();
}

void UAMSimTerminalView::SetTerminalInteractionEnabled(const bool bEnabled)
{
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetTerminalInteractionMode(bEnabled);
	}
}

void UAMSimTerminalView::ClosePresentation()
{
	bPresentationOpen = false;
	PresentationDestination = EAMSimTerminalPresentationDestination::Terminal;
	SetTerminalInteractionEnabled(false);
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetMatureOverviewMode(true);
		break;
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAMSimTerminalView::ReturnToAirport()
{
	ClosePresentation();
	OnReturnRequested.ExecuteIfBound();
}
