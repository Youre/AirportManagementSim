#pragma once

#include "AMSimUITheme.h"
#include "Blueprint/UserWidget.h"
#include "AMSimExpandingToolButton.generated.h"

class UBorder;
class UButton;
class UImage;
class UScaleBox;
class UTextBlock;
class UTexture2D;

UCLASS()
class AMSIMUI_API UAMSimExpandingToolButton final : public UUserWidget
{
	GENERATED_BODY()

public:
	void Configure(
		UTexture2D* InIcon,
		const FString& InLabel,
		AMSim::UITheme::EButton InKind = AMSim::UITheme::EButton::Tool,
		bool bInFlyoutLeft = false);

	FSimpleDelegate OnActivated;

	bool IsExpanded() const { return bExpanded; }
	FString GetActionLabel() const { return ActionLabel; }
	UTexture2D* GetIconTexture() const { return IconTexture; }
	UButton* GetActionButton() const { return ActionButton; }
	void SetActionEnabled(bool bInEnabled);
	bool IsActionEnabled() const { return bActionEnabled; }
	ESlateVisibility GetFlyoutVisibilityForTest() const;
	void SetExpandedForTest(bool bInExpanded);
	bool PreservesIconAspectRatioForTest() const;
	bool IsFlyoutLeftForTest() const { return bFlyoutLeft; }
	static FVector2D GetHostSize() { return FVector2D(320.0f, 56.0f); }
	static float GetMinimumFlyoutWidth() { return 156.0f; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeOnMouseEnter(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(
		const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

private:
	UFUNCTION()
	void HandleHovered();
	UFUNCTION()
	void HandleUnhovered();
	UFUNCTION()
	void HandleClicked();
	void SetExpanded(bool bInExpanded);
	void ApplyConfiguredContent();

	UPROPERTY()
	TObjectPtr<UTexture2D> IconTexture;
	UPROPERTY(Transient)
	TObjectPtr<UButton> ActionButton;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> FlyoutSurface;
	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage;
	UPROPERTY(Transient)
	TObjectPtr<UScaleBox> IconScaleBox;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LabelText;

	FString ActionLabel = TEXT("TOOL");
	AMSim::UITheme::EButton ButtonKind = AMSim::UITheme::EButton::Tool;
	bool bExpanded = false;
	bool bActionEnabled = true;
	bool bFlyoutLeft = false;
};
