#pragma once

#include "CoreMinimal.h"

struct FAMSimManifestValidation
{
	bool bValid = false;
	int32 DataRowCount = 0;
	TArray<FString> Errors;
};

class AMSIMEDITOR_API FAMSimImportManifestValidator
{
public:
	static FAMSimManifestValidation Validate(const FString& CsvText);
	static const TArray<FString>& RequiredColumns();
};
