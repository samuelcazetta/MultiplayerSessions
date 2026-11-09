// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerSessions : ModuleRules
{
	public MultiplayerSessions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { });
				
		PrivateIncludePaths.AddRange(new string[] { });
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"UMG"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",

				// Online and Steam
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"OnlineSubsystemSteam",
				"SteamSockets"
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}