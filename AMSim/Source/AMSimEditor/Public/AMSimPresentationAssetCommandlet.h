#pragma once

#include "Commandlets/Commandlet.h"
#include "AMSimPresentationAssetCommandlet.generated.h"

UCLASS()
class AMSIMEDITOR_API UAMSimPresentationAssetCommandlet final : public UCommandlet
{
	GENERATED_BODY()

public:
	UAMSimPresentationAssetCommandlet();
	virtual int32 Main(const FString& Params) override;
};
