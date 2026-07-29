#include "AMSimRegionalOperationsView.h"

#include "AMSimPhase6View.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"

void UAMSimRegionalOperationsView::InitializePhase6View()
{
	if (!WidgetTree || !RootSurface)
	{
		return;
	}

	Phase6View = WidgetTree->ConstructWidget<UAMSimPhase6View>(
		UAMSimPhase6View::StaticClass(),
		TEXT("Phase6MajorAirportView"));
	Phase6View->OnReturnRequested.BindUObject(
		this,
		&UAMSimRegionalOperationsView::CloseMajorOperations);
	UCanvasPanelSlot* Phase6Slot =
		RootSurface->AddChildToCanvas(Phase6View);
	Phase6Slot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	Phase6Slot->SetOffsets(FMargin(0.0f));
	Phase6Slot->SetZOrder(110);
}

bool UAMSimRegionalOperationsView::RefreshPhase6View(
	const AMSim::FPhase6QuerySnapshot& Query)
{
	if (Phase6View)
	{
		Phase6View->RefreshFromSimulation();
	}
	const bool bMajorAvailable =
		Query.bUnlocked || Query.bInitialized;
	if (!bMajorAvailable)
	{
		bMajorOperationsOpen = false;
	}
	if (MajorOperationsButton)
	{
		MajorOperationsButton->SetVisibility(
			bMajorAvailable
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (!bMajorAvailable || !bMajorOperationsOpen)
	{
		if (Phase6View)
		{
			Phase6View->SetVisibility(
				ESlateVisibility::Collapsed);
		}
		return false;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Phase6View)
	{
		Phase6View->SetVisibility(
			ESlateVisibility::SelfHitTestInvisible);
	}
	return true;
}
