#pragma once

#include "AMSimPhase5ViewState.h"
#include "Blueprint/UserWidget.h"
#include "AMSimPhase5View.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UTextBlock;
class UVerticalBox;

UCLASS()
class AMSIMUI_API UAMSimPhase5View final : public UUserWidget
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
	void InitializeBreadth();
	UFUNCTION()
	void AcceptCargo();
	UFUNCTION()
	void AdvanceCargo();
	UFUNCTION()
	void AcceptProvider();
	UFUNCTION()
	void RecoverProvider();
	UFUNCTION()
	void AcceptEvent();
	UFUNCTION()
	void AdvanceEvent();
	UFUNCTION()
	void ShowCargo();
	UFUNCTION()
	void ShowProviders();
	UFUNCTION()
	void ShowEvents();
	UFUNCTION()
	void ShowCapabilities();

	bool Submit(
		AMSim::EPhase5CommandType Type,
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
	TObjectPtr<UTextBlock> ShipmentHeadlineText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ShipmentDetailText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FlowText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CauseText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RemedyText;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> Panels;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ContractCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ZoneCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ProviderCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> EventCards;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> CapabilityCards;
	UPROPERTY(Transient)
	TObjectPtr<UButton> InitializeButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptCargoButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvanceCargoButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptProviderButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RecoverProviderButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptEventButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AdvanceEventButton;

	AMSim::FPhase5ViewState ViewState;
	int32 ActivePanel = 0;
};
