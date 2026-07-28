using UnrealBuildTool;

public class AMSimEditor : ModuleRules
{
	public AMSimEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AMSimGameplay",
			"AMSimSimulation",
			"AMSimUI",
			"AssetRegistry",
			"AssetTools",
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"Paper2D",
			"UMGEditor",
			"UnrealEd"
		});
	}
}
