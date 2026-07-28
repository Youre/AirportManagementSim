// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AMSim : ModuleRules
{
	public AMSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AMSimGameplay",
			"AMSimUI",
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CommonUI",
			"EnhancedInput",
			"InputCore",
			"Paper2D",
			"Slate",
			"SlateCore",
			"UMG"
		});
	}
}
