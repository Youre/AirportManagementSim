using UnrealBuildTool;

public class AMSimGameplay : ModuleRules
{
	public AMSimGameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[] { "AMSimSimulation", "Core", "CoreUObject", "Engine" });
		PrivateDependencyModuleNames.Add("Json");
	}
}
