using UnrealBuildTool;

public class AMSimGameplay : ModuleRules
{
	public AMSimGameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		PublicDependencyModuleNames.AddRange(new[] { "AMSimSimulation", "Core", "CoreUObject", "Engine" });
		PrivateDependencyModuleNames.Add("Json");
	}
}
