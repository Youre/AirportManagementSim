#pragma once

#include "CommonActivatableWidget.h"
#include "AMSimRootScreen.generated.h"

UCLASS()
class AMSIMUI_API UAMSimRootScreen final : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
