#pragma once

#include "Commandlets/Commandlet.h"
#include "AMSimProjectAuditCommandlet.generated.h"

UCLASS()
class AMSIMEDITOR_API UAMSimProjectAuditCommandlet final : public UCommandlet
{
	GENERATED_BODY()

public:
	UAMSimProjectAuditCommandlet();
	virtual int32 Main(const FString& Params) override;
};
