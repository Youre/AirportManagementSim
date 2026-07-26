#pragma once

#include "Blueprint/UserWidget.h"
#include "AMSimComponentGallery.generated.h"

UCLASS(Blueprintable)
class AMSIMUI_API UAMSimComponentGallery final : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
