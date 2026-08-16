#include "AMSimExpandingToolButton.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UAMSimExpandingToolButton::Configure(
	UTexture2D* InIcon,
	const FString& InLabel,
	const AMSim::UITheme::EButton InKind,
	const bool bInFlyoutLeft)
{
	IconTexture = InIcon;
	ActionLabel = InLabel;
	ButtonKind = InKind;
	bFlyoutLeft = bInFlyoutLeft;
	ApplyConfiguredContent();
}

TSharedRef<SWidget> UAMSimExpandingToolButton::RebuildWidget()
{
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("ExpandingToolButtonTree"));
	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(
		UOverlay::StaticClass(),
		TEXT("ExpandingToolButtonRoot"));
	Root->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = Root;

	FlyoutSurface = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("FlyoutSurface"));
	AMSim::UITheme::StyleSurface(
		FlyoutSurface,
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(16.0f, 10.0f, 14.0f, 10.0f),
		12.0f,
		1.5f);
	LabelText = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass(),
		TEXT("FlyoutLabel"));
	AMSim::UITheme::StyleText(
		LabelText,
		13,
		AMSim::UITheme::White(),
		true,
		true);
	LabelText->SetAutoWrapText(false);
	FlyoutSurface->SetContent(LabelText);
	FlyoutSurface->SetVisibility(
		bExpanded
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);
	USizeBox* FlyoutSize = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("FlyoutMinimumWidth"));
	FlyoutSize->SetMinDesiredWidth(GetMinimumFlyoutWidth());
	FlyoutSize->SetContent(FlyoutSurface);
	UOverlaySlot* FlyoutSlot = Root->AddChildToOverlay(FlyoutSize);
	FlyoutSlot->SetHorizontalAlignment(bFlyoutLeft ? HAlign_Right : HAlign_Left);
	FlyoutSlot->SetVerticalAlignment(VAlign_Center);
	FlyoutSize->SetRenderTranslation(FVector2D(bFlyoutLeft ? -98.0f : 98.0f, 0.0f));

	USizeBox* ButtonSize = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("ActionButtonSize"));
	ButtonSize->SetWidthOverride(88.0f);
	ButtonSize->SetHeightOverride(56.0f);
	ActionButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("ActionButton"));
	ActionButton->SetStyle(AMSim::UITheme::ButtonStyle(ButtonKind, 12.0f));
	ActionButton->OnHovered.AddDynamic(
		this,
		&UAMSimExpandingToolButton::HandleHovered);
	ActionButton->OnUnhovered.AddDynamic(
		this,
		&UAMSimExpandingToolButton::HandleUnhovered);
	ActionButton->OnClicked.AddDynamic(
		this,
		&UAMSimExpandingToolButton::HandleClicked);

	USizeBox* IconSize = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("IconSize"));
	IconSize->SetWidthOverride(42.0f);
	IconSize->SetHeightOverride(42.0f);
	IconImage = WidgetTree->ConstructWidget<UImage>(
		UImage::StaticClass(),
		TEXT("Icon"));
	IconScaleBox = WidgetTree->ConstructWidget<UScaleBox>(
		UScaleBox::StaticClass(),
		TEXT("IconAspectFit"));
	IconScaleBox->SetStretch(EStretch::ScaleToFit);
	IconScaleBox->SetStretchDirection(EStretchDirection::DownOnly);
	IconScaleBox->SetContent(IconImage);
	IconSize->SetContent(IconScaleBox);
	ActionButton->SetContent(IconSize);
	ButtonSize->SetContent(ActionButton);
	UOverlaySlot* ButtonSlot = Root->AddChildToOverlay(ButtonSize);
	ButtonSlot->SetHorizontalAlignment(bFlyoutLeft ? HAlign_Right : HAlign_Left);
	ButtonSlot->SetVerticalAlignment(VAlign_Center);

	ApplyConfiguredContent();
	return Super::RebuildWidget();
}

void UAMSimExpandingToolButton::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	SetExpanded(true);
}

void UAMSimExpandingToolButton::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (!ActionButton || !ActionButton->HasKeyboardFocus())
	{
		SetExpanded(false);
	}
}

void UAMSimExpandingToolButton::NativeOnAddedToFocusPath(
	const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	SetExpanded(true);
}

void UAMSimExpandingToolButton::NativeOnRemovedFromFocusPath(
	const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	SetExpanded(false);
}

void UAMSimExpandingToolButton::HandleHovered()
{
	SetExpanded(true);
}

void UAMSimExpandingToolButton::HandleUnhovered()
{
	if (!ActionButton || !ActionButton->HasKeyboardFocus())
	{
		SetExpanded(false);
	}
}

void UAMSimExpandingToolButton::HandleClicked()
{
	OnActivated.ExecuteIfBound();
}

void UAMSimExpandingToolButton::SetExpandedForTest(const bool bInExpanded)
{
	SetExpanded(bInExpanded);
}

ESlateVisibility UAMSimExpandingToolButton::GetFlyoutVisibilityForTest() const
{
	return FlyoutSurface
		? FlyoutSurface->GetVisibility()
		: ESlateVisibility::Collapsed;
}

bool UAMSimExpandingToolButton::PreservesIconAspectRatioForTest() const
{
	return IconScaleBox &&
		IconScaleBox->GetStretch() == EStretch::ScaleToFit;
}

void UAMSimExpandingToolButton::SetActionEnabled(const bool bInEnabled)
{
	bActionEnabled = bInEnabled;
	if (ActionButton)
	{
		ActionButton->SetIsEnabled(bActionEnabled);
	}
}

void UAMSimExpandingToolButton::SetExpanded(const bool bInExpanded)
{
	bExpanded = bInExpanded;
	if (FlyoutSurface)
	{
		FlyoutSurface->SetVisibility(
			bExpanded
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed);
	}
}

void UAMSimExpandingToolButton::ApplyConfiguredContent()
{
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(IconTexture, true);
	}
	if (LabelText)
	{
		LabelText->SetText(FText::FromString(ActionLabel));
	}
	if (ActionButton)
	{
		ActionButton->SetStyle(AMSim::UITheme::ButtonStyle(ButtonKind, 12.0f));
		ActionButton->SetToolTipText(FText::FromString(ActionLabel));
		ActionButton->SetIsEnabled(bActionEnabled);
	}
}
