// Some copyright should be here...

using UnrealBuildTool;

public class VivoxSubsystem : ModuleRules
{
	public VivoxSubsystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"OnlineSubsystem",
				"VivoxVoiceChat"
			}
			);
	}
}
