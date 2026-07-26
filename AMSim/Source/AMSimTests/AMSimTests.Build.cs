using UnrealBuildTool;

public class AMSimTests : ModuleRules
{
	public AMSimTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AMSimGameplay",
			"AMSimEditor",
			"AMSimSimulation",
			"AMSimUI",
			"Core",
			"CoreUObject",
			"Engine",
			"SlateCore"
		});
	}
}
