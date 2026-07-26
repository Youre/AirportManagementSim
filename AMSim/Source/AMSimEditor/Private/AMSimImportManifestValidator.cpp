#include "AMSimImportManifestValidator.h"

const TArray<FString>& FAMSimImportManifestValidator::RequiredColumns()
{
	static const TArray<FString> Columns{
		TEXT("logical_source_name"), TEXT("physical_source_path"), TEXT("sha256"),
		TEXT("creator"), TEXT("license"), TEXT("reuse_decision"), TEXT("depicted_aircraft"),
		TEXT("accuracy_confidence"), TEXT("format"), TEXT("width_px"), TEXT("height_px"),
		TEXT("orientation"), TEXT("duplicate_status"), TEXT("cleanup_required"),
		TEXT("pixels_per_meter"), TEXT("directional_variants"), TEXT("livery_treatment"),
		TEXT("destination_asset_id"), TEXT("reviewer"), TEXT("review_date"), TEXT("notes")
	};
	return Columns;
}

FAMSimManifestValidation FAMSimImportManifestValidator::Validate(const FString& CsvText)
{
	FAMSimManifestValidation Result;
	TArray<FString> Lines;
	CsvText.ParseIntoArrayLines(Lines, true);
	if (Lines.IsEmpty())
	{
		Result.Errors.Add(TEXT("Manifest is empty."));
		return Result;
	}

	TArray<FString> Header;
	Lines[0].ParseIntoArray(Header, TEXT(","), false);
	if (Header != RequiredColumns())
	{
		Result.Errors.Add(TEXT("Manifest header does not match the CT-01 template."));
		return Result;
	}

	for (int32 LineIndex = 1; LineIndex < Lines.Num(); ++LineIndex)
	{
		TArray<FString> Fields;
		Lines[LineIndex].ParseIntoArray(Fields, TEXT(","), false);
		if (Fields.Num() != Header.Num())
		{
			Result.Errors.Add(FString::Printf(TEXT("Row %d has %d fields; expected %d."), LineIndex + 1, Fields.Num(), Header.Num()));
			continue;
		}
		++Result.DataRowCount;
		const FString& Decision = Fields[5];
		if (Decision.Equals(TEXT("approved"), ESearchCase::IgnoreCase))
		{
			for (const int32 RequiredIndex : {0, 1, 2, 3, 4, 6, 7, 11, 13, 14, 15, 16, 17, 18, 19})
			{
				if (Fields[RequiredIndex].TrimStartAndEnd().IsEmpty())
				{
					Result.Errors.Add(FString::Printf(TEXT("Approved row %d is missing %s."), LineIndex + 1, *Header[RequiredIndex]));
				}
			}
			if (Fields[2].Len() != 64)
			{
				Result.Errors.Add(FString::Printf(TEXT("Approved row %d requires a 64-character SHA-256."), LineIndex + 1));
			}
		}
	}
	Result.bValid = Result.Errors.IsEmpty();
	return Result;
}
