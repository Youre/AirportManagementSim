#pragma once

#include "AMSimPhase6ViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimPhase6View.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UTextBlock;
class UVerticalBox;

UCLASS()
class AMSIMUI_API UAMSimPhase6View final : public UUserWidget
{
	GENERATED_BODY()

public:
	void RefreshFromSimulation();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime) override;

private:
	UFUNCTION()
	void InitializeMajor();
	UFUNCTION()
	void ConstructNextFacility();
	UFUNCTION()
	void ActivateParallelRunways();
	UFUNCTION()
	void AcceptLargeAircraft();
	UFUNCTION()
	void StartLargeAircraftTurnaround();
	UFUNCTION()
	void AdvanceLargeAircraft();
	UFUNCTION()
	void RecordNextEvidence();
	UFUNCTION()
	void AdvanceOperatingDay();
	UFUNCTION()
	void ResolveSharedConflict();
	UFUNCTION()
	void AdvanceIncident();
	UFUNCTION()
	void NextPath();
	UFUNCTION()
	void ShowRunways();
	UFUNCTION()
	void ShowCapacity();
	UFUNCTION()
	void ShowAircraft();
	UFUNCTION()
	void ShowIncident();
	UFUNCTION()
	void ShowProgression();

	AMSim::EPhase6CommandResult Submit(
		AMSim::FPhase6Command Command,
		const FString& SuccessMessage);
	void ShowPanel(int32 PanelIndex);
	void RefreshCards(
		UVerticalBox* Box,
		const TArray<FString>& Cards,
		const FLinearColor& Accent);

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvas;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SummaryText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PathHeadlineText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PathEvidenceText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InteractionText;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> Panels;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> RunwayCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> FacilityCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> AircraftCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> IncidentCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> CapabilityCards;
	UPROPERTY(Transient)
	TObjectPtr<UButton> InitializeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ConstructButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ActivateParallelButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptAircraftButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> StartTurnaroundButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvanceAircraftButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RecordEvidenceButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvanceDayButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ResolveConflictButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> IncidentButton;

	AMSim::FPhase6ViewState ViewState;
	AMSim::ESpecializationPath SelectedPath =
		AMSim::ESpecializationPath::GeneralAviation;
	int32 ActivePanel = 0;
	bool bIncidentWarningReviewed = false;
};
