#include "AMSimConstructionProposalView.h"

#include "AMSimUISoundSubsystem.h"

#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

bool UAMSimConstructionProposalView::IsProposalOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimConstructionProposalView::OpenProposal()
{
	const bool bConflictProof = FParse::Param(
		FCommandLine::Get(),
		TEXT("AMSimPhase45ConstructionProof"));
	CurrentProposal = bConflictProof
		? MakePresentationProposal(true)
		: MakeEmptyProposal();
	PlacementMask = bConflictProof ? AllPlacementParts : 0;
	SelectedPlacementTool = EPlacementTool::Runway;
	PathPlacementStep = 0;
	DraggedEndpointIndex = INDEX_NONE;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bPlacementDragging = false;
	bPlacementMoved = false;
	bCanUndo = false;
	bHoveringWorld = false;
	bPathStartedByPress = false;
	bSnapSoundActive = false;
	RefreshProposalPresentation();
	SetVisibility(ESlateVisibility::Visible);
	AMSim::UIAudio::Play(this, EAMSimUISound::BuildStart);
	OnBuildModeVisibilityChanged.ExecuteIfBound(true);
	SetKeyboardFocus();
}

void UAMSimConstructionProposalView::CloseProposal()
{
	const bool bWasOpen = IsProposalOpen();
	ClearWorldPreview();
	DraggedEndpointIndex = INDEX_NONE;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	PathPlacementStep = 0;
	bPlacementDragging = false;
	bPlacementMoved = false;
	bHoveringWorld = false;
	bPathStartedByPress = false;
	bSnapSoundActive = false;
	SetVisibility(ESlateVisibility::Collapsed);
	if (bWasOpen)
	{
		OnBuildModeVisibilityChanged.ExecuteIfBound(false);
	}
}
