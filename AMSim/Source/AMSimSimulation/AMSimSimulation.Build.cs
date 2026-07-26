using UnrealBuildTool;

public class AMSimSimulation : ModuleRules
{
	public AMSimSimulation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject" });
	}
}
