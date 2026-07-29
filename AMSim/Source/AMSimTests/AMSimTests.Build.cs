using UnrealBuildTool;

public class AMSimTests : ModuleRules
{
	public AMSimTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AMSimGameplay",
			"AMSimEditor",
			"AMSimSimulation",
			"AMSimUI",
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"SlateCore"
		});
	}
}
