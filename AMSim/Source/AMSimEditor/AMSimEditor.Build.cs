using UnrealBuildTool;

public class AMSimEditor : ModuleRules
{
	public AMSimEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AMSimGameplay",
			"AMSimSimulation",
			"AMSimUI",
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd"
		});
	}
}
