#pragma once

#include "AMSimSpeechProvider.h"
#include "CommonActivatableWidget.h"
#include "AMSimRootScreen.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimRootScreen final : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void CreateAirport();
	UFUNCTION()
	void CommitStarterPlan();
	UFUNCTION()
	void CancelStarterPlan();
	UFUNCTION()
	void OpenAirport();
	UFUNCTION()
	void CloseAirport();
	UFUNCTION()
	void PinOffer();
	UFUNCTION()
	void DeclineOffer();
	UFUNCTION()
	void AcceptOffer();
	UFUNCTION()
	void ScheduleFlight();
	UFUNCTION()
	void RequestRecovery();
	UFUNCTION()
	void PauseSimulation();
	UFUNCTION()
	void SetSpeedOne();
	UFUNCTION()
	void SetSpeedTwo();
	UFUNCTION()
	void SetSpeedFour();
	UFUNCTION()
	void SetSpeedEight();
	UFUNCTION()
	void SaveGame();
	UFUNCTION()
	void LoadGame();

	void SubmitSpeed(int32 Multiplier);
	void RefreshFromSimulation();
	void SetInteractionMessage(const FString& Message, bool bSucceeded);

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AirportNameText;
	UPROPERTY(Transient)
	TObjectPtr<UEditableTextBox> AirportNameEntry;
	UPROPERTY(Transient)
	TObjectPtr<UEditableTextBox> SaveSlotEntry;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ClockText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FundsText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ObjectiveText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CauseText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RemedyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ProjectText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> OfferText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CompatibilityText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> FlightText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TimetableText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ServicesText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LedgerText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RatingText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CaptionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InteractionText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AircraftLabel;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RunwayVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> TaxiVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> StandVisual;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> HutVisual;
	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> AircraftMarker;

	UPROPERTY(Transient)
	TObjectPtr<UButton> CreateButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> BuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CancelBuildButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> OpenButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> PinOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> DeclineOfferButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcceptButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ScheduleButton;
	UPROPERTY(Transient)
	TObjectPtr<UButton> RecoveryButton;

	TUniquePtr<IAMSimSpeechProvider> SpeechProvider;
	int32 LastPhraseCount = 0;
};
