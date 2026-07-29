#pragma once

#include "Blueprint/UserWidget.h"
#include "AMSimReleaseGuideView.generated.h"

class UBorder;
class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS()
class AMSIMUI_API UAMSimReleaseGuideView final : public UUserWidget
{
	GENERATED_BODY()

public:
	void OpenGuide();
	void CloseGuide();
	bool IsGuideOpen() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
	void ClosePressed();
	UFUNCTION()
	void ShowStart();
	UFUNCTION()
	void ShowControls();
	UFUNCTION()
	void ShowAirport();
	UFUNCTION()
	void ShowOperations();
	UFUNCTION()
	void ShowAccessibility();
	UFUNCTION()
	void Scale100();
	UFUNCTION()
	void Scale125();
	UFUNCTION()
	void Scale150();
	UFUNCTION()
	void Scale175();
	UFUNCTION()
	void Scale200();
	UFUNCTION()
	void ToggleReducedMotion();
	UFUNCTION()
	void ResetAccessibility();

	void SetSection(int32 SectionIndex);
	void AddContentCard(
		const FString& Eyebrow,
		const FString& Headline,
		const FString& Body,
		const FLinearColor& Accent);
	void ApplyScale(float Scale);
	void RefreshAccessibilitySummary();

	UPROPERTY(Transient)
	TObjectPtr<UBorder> ModalSurface;
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ContentCards;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SectionEyebrowText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SectionHeadlineText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SectionIntroText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> AccessibilitySummaryText;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ReducedMotionButton;

	int32 ActiveSection = 0;
	bool bRequestedOpen = false;
};
