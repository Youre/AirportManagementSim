#pragma once

#include "Blueprint/UserWidget.h"
#include "AMSimTurnaroundView.generated.h"

class UBorder;
class UProgressBar;
class UTextBlock;
class UTexture2D;

UCLASS()
class AMSIMUI_API UAMSimTurnaroundView final : public UUserWidget
{
	GENERATED_BODY()

public:
	UAMSimTurnaroundView(const FObjectInitializer& ObjectInitializer);
	void RefreshFromSimulation();
	bool HasRequiredIdentityArt() const { return AircraftIdentityTexture != nullptr; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBorder> InspectionCard;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> FuelCard;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> ReadyCard;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InspectionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FuelText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ReadyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PredictionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> IdentityText;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> InspectionProgress;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> FuelProgress;
	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ReadyProgress;
	UPROPERTY()
	TObjectPtr<UTexture2D> AircraftIdentityTexture;

	uint64 LastRevision = MAX_uint64;
};
