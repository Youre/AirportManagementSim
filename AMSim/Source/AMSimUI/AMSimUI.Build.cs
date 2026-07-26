using UnrealBuildTool;

public class AMSimUI : ModuleRules
{
	public AMSimUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[]
		{
			"AMSimGameplay",
			"AMSimSimulation",
			"CommonUI",
			"Core",
			"CoreUObject",
			"Engine",
			"EnhancedInput",
			"Paper2D",
			"UMG"
		});
		PrivateDependencyModuleNames.AddRange(new[] { "InputCore", "Slate", "SlateCore" });
	}
}
