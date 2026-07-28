#pragma once

#include "AMSimPhase4Types.h"
#include "Blueprint/UserWidget.h"
#include "AMSimOverviewView.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class UButton;
class UProgressBar;

UCLASS()
class AMSIMUI_API UAMSimOverviewView final : public UUserWidget
{
	GENERATED_BODY()

public:
	UAMSimOverviewView(const FObjectInitializer& ObjectInitializer);

	void RefreshFromSimulation();
	int32 GetLoadedIconCount() const;
	bool HasRequiredIconKit() const;
	bool HasRequiredIdentityArt() const;
	void SetFacilitySelected(bool bSelected);
	FSimpleDelegate OnOpenTimetable;
	FSimpleDelegate OnOpenCapabilities;
	FSimpleDelegate OnSelectAircraft;
	FSimpleDelegate OnSelectFacility;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void OpenTimetable();
	UFUNCTION()
	void OpenCapabilities();
	UFUNCTION()
	void SelectAircraft();
	UFUNCTION()
	void SelectFacility();
	void RefreshInspector(const AMSim::FPhase4QuerySnapshot& Query);

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AlertText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FlightsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ProjectsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> GroundText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InspectorTitleText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SelectedText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SelectedDetailText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CapacityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ReadinessText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ServiceText;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ReadinessMeter;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ConnectionMeter;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> BagMeter;
	UPROPERTY(Transient)
	TObjectPtr<UImage> SelectedIdentityImage;
	UPROPERTY(Transient)
	TObjectPtr<UButton> TimetableAction;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CapabilitiesAction;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AircraftSelectionAction;
	UPROPERTY(Transient)
	TObjectPtr<UButton> FacilitySelectionAction;
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> ToolIcons;
	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> ActivityIcons;
	UPROPERTY()
	TObjectPtr<UTexture2D> SelectedAircraftTexture;
	UPROPERTY()
	TObjectPtr<UTexture2D> SelectedFacilityTexture;

	AMSim::FPhase4QuerySnapshot LastQuery;
	uint64 LastRevision = MAX_uint64;
	bool bFacilitySelected = false;
};
