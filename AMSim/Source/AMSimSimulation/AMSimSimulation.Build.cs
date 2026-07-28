using UnrealBuildTool;

public class AMSimSimulation : ModuleRules
{
	public AMSimSimulation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject" });
	}
}
