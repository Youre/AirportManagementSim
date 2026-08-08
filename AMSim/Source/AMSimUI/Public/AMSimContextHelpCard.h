#pragma once

#include "Blueprint/UserWidget.h"
#include "AMSimContextHelpCard.generated.h"

class UBorder;
class UButton;
class UTextBlock;

UCLASS()
class AMSIMUI_API UAMSimContextHelpCard final : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSuppressed(bool bInSuppressed);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime) override;

private:
	UFUNCTION()
	void AcknowledgeCurrentHelp();

	void RefreshFromSimulation();

	UPROPERTY(Transient)
	TObjectPtr<UBorder> CardSurface;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> EyebrowText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> BodyText;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ActionText;
	UPROPERTY(Transient)
	TObjectPtr<UButton> AcknowledgeButton;

	FName CurrentHelpId;
	uint64 LastRevision = MAX_uint64;
	bool bSuppressed = false;
};
