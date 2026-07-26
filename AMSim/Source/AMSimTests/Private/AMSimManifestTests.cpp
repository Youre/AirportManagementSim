#include "AMSimImportManifestValidator.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimManifestTemplateTest,
	"AMSim.Phase0.Content.ImportManifestTemplate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimManifestTemplateTest::RunTest(const FString& Parameters)
{
	const FString Path = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), TEXT("../docs/planning/30-content-and-assets/aircraft-import-manifest-template.csv")));
	FString Csv;
	TestTrue(TEXT("Manifest template is readable"), FFileHelper::LoadFileToString(Csv, *Path));
	const FAMSimManifestValidation Validation = FAMSimImportManifestValidator::Validate(Csv);
	TestTrue(TEXT("Empty template is structurally valid"), Validation.bValid);
	TestEqual(TEXT("Template approves no assets"), Validation.DataRowCount, 0);
	return true;
}

#endif
