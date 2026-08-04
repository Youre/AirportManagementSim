#include "AMSimUITheme.h"

#include "AMSimUISoundSubsystem.h"

#include "Brushes/SlateRoundedBoxBrush.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

namespace AMSim::UITheme
{
	namespace
	{
		const FLinearColor Navy900Value(0.002f, 0.012f, 0.025f, 0.99f);
		const FLinearColor Navy800Value(0.004f, 0.026f, 0.048f, 0.985f);
		const FLinearColor Navy700Value(0.008f, 0.052f, 0.092f, 0.985f);
		const FLinearColor Navy600Value(0.018f, 0.105f, 0.175f, 0.99f);
		const FLinearColor CyanValue(0.28f, 0.84f, 0.95f, 1.0f);
		const FLinearColor CyanSoftValue(0.12f, 0.48f, 0.65f, 1.0f);
		const FLinearColor AmberValue(1.0f, 0.70f, 0.20f, 1.0f);
		const FLinearColor GreenValue(0.30f, 0.70f, 0.35f, 1.0f);
		const FLinearColor CoralValue(0.96f, 0.34f, 0.28f, 1.0f);
		const FLinearColor WhiteValue(0.95f, 0.975f, 0.98f, 1.0f);
		const FLinearColor MutedValue(0.64f, 0.74f, 0.78f, 1.0f);
		const FLinearColor OutlineValue(0.09f, 0.31f, 0.45f, 1.0f);

		void ResolveSurface(
			const ESurface Surface,
			FLinearColor& OutFill,
			FLinearColor& OutStroke)
		{
			switch (Surface)
			{
			case ESurface::Chrome:
				OutFill = Navy900Value;
				OutStroke = FLinearColor(0.035f, 0.20f, 0.30f, 1.0f);
				break;
			case ESurface::Panel:
				OutFill = Navy800Value;
				OutStroke = OutlineValue;
				break;
			case ESurface::Card:
				OutFill = Navy700Value;
				OutStroke = FLinearColor(0.07f, 0.27f, 0.40f, 1.0f);
				break;
			case ESurface::RaisedCard:
				OutFill = FLinearColor(0.010f, 0.075f, 0.130f, 0.99f);
				OutStroke = CyanSoftValue;
				break;
			case ESurface::Field:
				OutFill = FLinearColor(0.002f, 0.020f, 0.038f, 1.0f);
				OutStroke = FLinearColor(0.10f, 0.35f, 0.50f, 1.0f);
				break;
			case ESurface::Chip:
				OutFill = FLinearColor(0.010f, 0.080f, 0.130f, 1.0f);
				OutStroke = CyanSoftValue;
				break;
			case ESurface::Positive:
				OutFill = FLinearColor(0.08f, 0.31f, 0.14f, 1.0f);
				OutStroke = GreenValue;
				break;
			case ESurface::Warning:
				OutFill = FLinearColor(0.25f, 0.16f, 0.035f, 1.0f);
				OutStroke = AmberValue;
				break;
			case ESurface::Danger:
				OutFill = FLinearColor(0.27f, 0.055f, 0.045f, 1.0f);
				OutStroke = CoralValue;
				break;
			default:
				OutFill = Navy800Value;
				OutStroke = OutlineValue;
				break;
			}
		}

		void ResolveButton(
			const EButton Kind,
			FLinearColor& NormalFill,
			FLinearColor& NormalStroke,
			FLinearColor& HoverFill,
			FLinearColor& HoverStroke,
			FLinearColor& PressedFill)
		{
			switch (Kind)
			{
			case EButton::Primary:
				NormalFill = FLinearColor(0.02f, 0.22f, 0.31f, 1.0f);
				NormalStroke = CyanValue;
				HoverFill = FLinearColor(0.03f, 0.34f, 0.44f, 1.0f);
				HoverStroke = WhiteValue;
				PressedFill = FLinearColor(0.02f, 0.15f, 0.22f, 1.0f);
				break;
			case EButton::Positive:
				NormalFill = FLinearColor(0.19f, 0.52f, 0.23f, 1.0f);
				NormalStroke = FLinearColor(0.55f, 0.91f, 0.52f, 1.0f);
				HoverFill = FLinearColor(0.27f, 0.66f, 0.31f, 1.0f);
				HoverStroke = WhiteValue;
				PressedFill = FLinearColor(0.12f, 0.39f, 0.17f, 1.0f);
				break;
			case EButton::Destructive:
				NormalFill = FLinearColor(0.24f, 0.065f, 0.055f, 1.0f);
				NormalStroke = CoralValue;
				HoverFill = FLinearColor(0.42f, 0.085f, 0.070f, 1.0f);
				HoverStroke = WhiteValue;
				PressedFill = FLinearColor(0.17f, 0.035f, 0.030f, 1.0f);
				break;
			case EButton::Tool:
				NormalFill = Navy700Value;
				NormalStroke = OutlineValue;
				HoverFill = FLinearColor(0.035f, 0.22f, 0.32f, 1.0f);
				HoverStroke = CyanValue;
				PressedFill = Navy800Value;
				break;
			case EButton::Quiet:
				NormalFill = FLinearColor(0.01f, 0.055f, 0.085f, 0.82f);
				NormalStroke = FLinearColor(0.055f, 0.20f, 0.29f, 1.0f);
				HoverFill = Navy700Value;
				HoverStroke = CyanSoftValue;
				PressedFill = Navy900Value;
				break;
			case EButton::Secondary:
			default:
				NormalFill = FLinearColor(0.025f, 0.135f, 0.205f, 1.0f);
				NormalStroke = FLinearColor(0.08f, 0.30f, 0.43f, 1.0f);
				HoverFill = FLinearColor(0.04f, 0.23f, 0.33f, 1.0f);
				HoverStroke = CyanSoftValue;
				PressedFill = Navy800Value;
				break;
			}
		}
	}

	const FLinearColor& Navy900() { return Navy900Value; }
	const FLinearColor& Navy800() { return Navy800Value; }
	const FLinearColor& Navy700() { return Navy700Value; }
	const FLinearColor& Navy600() { return Navy600Value; }
	const FLinearColor& Cyan() { return CyanValue; }
	const FLinearColor& CyanSoft() { return CyanSoftValue; }
	const FLinearColor& Amber() { return AmberValue; }
	const FLinearColor& Green() { return GreenValue; }
	const FLinearColor& Coral() { return CoralValue; }
	const FLinearColor& White() { return WhiteValue; }
	const FLinearColor& Muted() { return MutedValue; }
	const FLinearColor& Outline() { return OutlineValue; }

	FSlateBrush RoundedBrush(
		const FLinearColor& Fill,
		const float Radius,
		const FLinearColor& Stroke,
		const float StrokeWidth)
	{
		return FSlateRoundedBoxBrush(Fill, Radius, Stroke, StrokeWidth);
	}

	FSlateBrush SurfaceBrush(
		const ESurface Surface,
		const float Radius,
		const float StrokeWidth)
	{
		FLinearColor Fill;
		FLinearColor Stroke;
		ResolveSurface(Surface, Fill, Stroke);
		return RoundedBrush(Fill, Radius, Stroke, StrokeWidth);
	}

	FButtonStyle ButtonStyle(const EButton Kind, const float Radius)
	{
		FLinearColor NormalFill;
		FLinearColor NormalStroke;
		FLinearColor HoverFill;
		FLinearColor HoverStroke;
		FLinearColor PressedFill;
		ResolveButton(
			Kind,
			NormalFill,
			NormalStroke,
			HoverFill,
			HoverStroke,
			PressedFill);

		FButtonStyle Style;
		Style.SetNormal(RoundedBrush(NormalFill, Radius, NormalStroke, 1.5f));
		Style.SetHovered(RoundedBrush(HoverFill, Radius, HoverStroke, 2.0f));
		Style.SetPressed(RoundedBrush(PressedFill, Radius, AmberValue, 2.0f));
		const bool bKeepSelectedOutline = Kind == EButton::Primary;
		Style.SetDisabled(RoundedBrush(
			bKeepSelectedOutline
				? FLinearColor(0.010f, 0.085f, 0.130f, 0.90f)
				: FLinearColor(0.012f, 0.040f, 0.055f, 0.78f),
			Radius,
			bKeepSelectedOutline
				? CyanSoftValue
				: FLinearColor(0.09f, 0.16f, 0.18f, 0.8f),
			bKeepSelectedOutline ? 1.6f : 1.0f));
		Style.SetNormalPadding(FMargin(14.0f, 10.0f));
		Style.SetPressedPadding(FMargin(14.0f, 11.0f, 14.0f, 9.0f));
		// Hover is communicated visually. Repeated pointer-enter audio made the
		// compact rail noisy, so sound is reserved for committed actions and
		// semantic feedback.
		const EAMSimUISound PressCue =
			Kind == EButton::Primary || Kind == EButton::Positive
				? EAMSimUISound::ClickPrimary
				: Kind == EButton::Tool
					? EAMSimUISound::FocusTick
					: EAMSimUISound::ClickSecondary;
		FSlateSound PressSound;
		PressSound.SetResourceObject(
			UAMSimUISoundSubsystem::GetDefaultCue(PressCue));
		Style.SetPressedSound(PressSound);
		return Style;
	}

	FEditableTextBoxStyle EntryStyle()
	{
		FEditableTextBoxStyle Style =
			FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>(TEXT("NormalEditableTextBox"));
		Style.SetFont(Font(17, true));
		Style.SetBackgroundImageNormal(SurfaceBrush(ESurface::Field, 11.0f, 1.2f));
		Style.SetBackgroundImageHovered(RoundedBrush(
			Navy800Value,
			11.0f,
			CyanSoftValue,
			1.8f));
		Style.SetBackgroundImageFocused(RoundedBrush(
			Navy800Value,
			11.0f,
			CyanValue,
			2.0f));
		Style.SetBackgroundImageReadOnly(RoundedBrush(
			FLinearColor(0.012f, 0.035f, 0.045f, 0.9f),
			11.0f,
			FLinearColor(0.07f, 0.15f, 0.18f, 1.0f),
			1.0f));
		Style.SetForegroundColor(WhiteValue);
		Style.SetFocusedForegroundColor(WhiteValue);
		Style.SetReadOnlyForegroundColor(MutedValue);
		Style.SetPadding(FMargin(14.0f, 10.0f));
		return Style;
	}

	FSlateFontInfo Font(const int32 Size, const bool bBold)
	{
		return FCoreStyle::GetDefaultFontStyle(bBold ? TEXT("Bold") : TEXT("Regular"), Size);
	}

	void StyleSurface(
		UBorder* Border,
		const ESurface Surface,
		const FMargin& Padding,
		const float Radius,
		const float StrokeWidth)
	{
		if (!Border)
		{
			return;
		}
		Border->SetBrush(SurfaceBrush(Surface, Radius, StrokeWidth));
		Border->SetBrushColor(FLinearColor::White);
		Border->SetPadding(Padding);
		Border->SetClipping(EWidgetClipping::ClipToBounds);
	}

	void StyleText(
		UTextBlock* Text,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold,
		const bool bShadow)
	{
		if (!Text)
		{
			return;
		}
		Text->SetColorAndOpacity(FSlateColor(Color));
		Text->SetFont(Font(Size, bBold));
		if (bShadow)
		{
			Text->SetShadowOffset(FVector2D(1.0f, 2.0f));
			Text->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.72f));
		}
	}
}
