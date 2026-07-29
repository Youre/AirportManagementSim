#pragma once

#include "AMSimPhase1Types.h"

namespace AMSim
{
	struct FContextHelpDefinition
	{
		FName Id;
		FString Eyebrow;
		FString Title;
		FString Body;
		FString NextAction;
	};

	AMSIMSIMULATION_API const TArray<FContextHelpDefinition>&
	GetPhase1ContextHelpCatalog();

	AMSIMSIMULATION_API const FContextHelpDefinition*
	FindPhase1ContextHelp(FName HelpId);

	AMSIMSIMULATION_API const FContextHelpDefinition*
	SelectPhase1ContextHelp(const FPhase1State& State);
}
