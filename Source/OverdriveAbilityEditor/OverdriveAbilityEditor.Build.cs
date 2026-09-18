// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OverdriveAbilityEditor : ModuleRules
{
	public OverdriveAbilityEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"OverdriveAbilityEditor/Public",
                "OverdriveAbilityEditor/Private"
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
				"OverdriveAbility",
            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"ApplicationCore",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd",
                "PropertyEditor",
                "ContentBrowser",
                "Kismet",
                "ToolMenus",
                "AssetTools",
                "AssetDefinition",
                "GraphEditor",
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
