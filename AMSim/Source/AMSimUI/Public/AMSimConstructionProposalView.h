#pragma once

#include "AMSimPhase1Types.h"
#include "Blueprint/UserWidget.h"
#include "AMSimConstructionProposalView.generated.h"

class UButton;

UCLASS()
class AMSIMUI_API UAMSimConstructionProposalView final : public UUserWidget
{
	GENERATED_BODY()

public:
	bool IsProposalOpen() const;
	void OpenProposal();
	void CloseProposal();
	static AMSim::FStarterPlanProposal MakePresentationProposal(
		bool bConflictVariant);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void ConfirmProposal();
	UFUNCTION()
	void KeepEditing();

	UPROPERTY(Transient)
	TObjectPtr<UButton> ConfirmButton;

	AMSim::FStarterPlanProposal CurrentProposal;
	AMSim::FPhase1Validation CurrentValidation;
};
