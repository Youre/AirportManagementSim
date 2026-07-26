#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"

class UBorder;
class UTextBlock;

namespace AMSim::UITheme
{
	enum class ESurface : uint8
	{
		Chrome,
		Panel,
		Card,
		RaisedCard,
		Field,
		Chip,
		Positive,
		Warning,
		Danger
	};

	enum class EButton : uint8
	{
		Primary,
		Positive,
		Secondary,
		Destructive,
		Tool,
		Quiet
	};

	AMSIMUI_API const FLinearColor& Navy900();
	AMSIMUI_API const FLinearColor& Navy800();
	AMSIMUI_API const FLinearColor& Navy700();
	AMSIMUI_API const FLinearColor& Navy600();
	AMSIMUI_API const FLinearColor& Cyan();
	AMSIMUI_API const FLinearColor& CyanSoft();
	AMSIMUI_API const FLinearColor& Amber();
	AMSIMUI_API const FLinearColor& Green();
	AMSIMUI_API const FLinearColor& Coral();
	AMSIMUI_API const FLinearColor& White();
	AMSIMUI_API const FLinearColor& Muted();
	AMSIMUI_API const FLinearColor& Outline();

	AMSIMUI_API FSlateBrush RoundedBrush(
		const FLinearColor& Fill,
		float Radius,
		const FLinearColor& Stroke,
		float StrokeWidth = 1.0f);
	AMSIMUI_API FSlateBrush SurfaceBrush(
		ESurface Surface,
		float Radius = 16.0f,
		float StrokeWidth = 1.0f);
	AMSIMUI_API FButtonStyle ButtonStyle(EButton Kind, float Radius = 12.0f);
	AMSIMUI_API FEditableTextBoxStyle EntryStyle();
	AMSIMUI_API FSlateFontInfo Font(int32 Size, bool bBold = false);

	AMSIMUI_API void StyleSurface(
		UBorder* Border,
		ESurface Surface,
		const FMargin& Padding,
		float Radius = 16.0f,
		float StrokeWidth = 1.0f);
	AMSIMUI_API void StyleText(
		UTextBlock* Text,
		int32 Size,
		const FLinearColor& Color,
		bool bBold = false,
		bool bShadow = false);
}
