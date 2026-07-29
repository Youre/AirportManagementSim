#pragma once

#include "UObject/Object.h"
#include "AMSimAccessibilityProfile.generated.h"

UCLASS(Config=GameUserSettings, DefaultConfig)
class AMSIMUI_API UAMSimAccessibilityProfile final : public UObject
{
	GENERATED_BODY()

public:
	static UAMSimAccessibilityProfile* Get();
	static const TArray<float>& SupportedScales();
	static float NormalizeInterfaceScale(float RequestedScale);

	void ApplySavedSettings();
	void SetInterfaceScale(float RequestedScale);
	void ToggleReducedMotion();
	void ResetToDefaults();

	float GetInterfaceScale() const { return InterfaceScale; }
	bool HasSavedInterfaceScale() const
	{
		return bHasSavedInterfaceScale;
	}
	bool IsReducedMotionEnabled() const { return bReducedMotion; }
	bool AreCaptionsEnabled() const { return true; }
	bool IsCameraShakeEnabled() const { return false; }

private:
	void ApplyInterfaceScale() const;
	void SaveLocalProfile();

	UPROPERTY(Config)
	float InterfaceScale = 1.0f;

	UPROPERTY(Config)
	bool bHasSavedInterfaceScale = false;

	UPROPERTY(Config)
	bool bReducedMotion = true;
};
