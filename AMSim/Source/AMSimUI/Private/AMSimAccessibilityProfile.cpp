#include "AMSimAccessibilityProfile.h"

#include "Engine/UserInterfaceSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"

UAMSimAccessibilityProfile* UAMSimAccessibilityProfile::Get()
{
	return GetMutableDefault<UAMSimAccessibilityProfile>();
}

const TArray<float>& UAMSimAccessibilityProfile::SupportedScales()
{
	static const TArray<float> Scales = {
		1.0f,
		1.25f,
		1.5f,
		1.75f,
		2.0f
	};
	return Scales;
}

void UAMSimAccessibilityProfile::ApplySavedSettings()
{
	const FString CommandLine(FCommandLine::Get());
	if (!bHasSavedInterfaceScale ||
		CommandLine.Contains(TEXT("ApplicationScale=")) ||
		CommandLine.Contains(TEXT("AMSimIgnoreUserProfile")))
	{
		return;
	}
	SetInterfaceScale(InterfaceScale);
}

void UAMSimAccessibilityProfile::SetInterfaceScale(
	const float RequestedScale)
{
	InterfaceScale = NormalizeInterfaceScale(RequestedScale);
	bHasSavedInterfaceScale = true;
	ApplyInterfaceScale();
	SaveLocalProfile();
}

float UAMSimAccessibilityProfile::NormalizeInterfaceScale(
	const float RequestedScale)
{
	const TArray<float>& Scales = SupportedScales();
	float Result = Scales[0];
	float BestDistance =
		FMath::Abs(RequestedScale - Result);
	for (const float Candidate : Scales)
	{
		const float Distance =
			FMath::Abs(RequestedScale - Candidate);
		if (Distance < BestDistance)
		{
			Result = Candidate;
			BestDistance = Distance;
		}
	}
	return Result;
}

void UAMSimAccessibilityProfile::ToggleReducedMotion()
{
	bReducedMotion = !bReducedMotion;
	SaveLocalProfile();
}

void UAMSimAccessibilityProfile::ResetToDefaults()
{
	InterfaceScale = 1.0f;
	bHasSavedInterfaceScale = true;
	bReducedMotion = true;
	ApplyInterfaceScale();
	SaveLocalProfile();
}

void UAMSimAccessibilityProfile::ApplyInterfaceScale() const
{
	if (UUserInterfaceSettings* Settings =
		GetMutableDefault<UUserInterfaceSettings>())
	{
		Settings->ApplicationScale = InterfaceScale;
	}
}

void UAMSimAccessibilityProfile::SaveLocalProfile()
{
	SaveConfig(CPF_Config, *GGameUserSettingsIni);
}
