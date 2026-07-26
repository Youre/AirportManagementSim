#pragma once

#include "CoreMinimal.h"

class UWorld;

class IAMSimSpeechProvider
{
public:
	virtual ~IAMSimSpeechProvider() = default;
	virtual bool Initialize() = 0;
	virtual void Speak(UWorld* World, const FString& Caption) = 0;
	virtual void Shutdown() = 0;
	virtual FName GetProviderId() const = 0;
};

AMSIMUI_API TUniquePtr<IAMSimSpeechProvider> CreateAMSimLocalSpeechProvider();
