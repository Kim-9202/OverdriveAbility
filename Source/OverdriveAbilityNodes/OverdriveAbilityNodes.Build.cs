// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OverdriveAbilityNodes : ModuleRules
{
	public OverdriveAbilityNodes(ReadOnlyTargetRules Target) : base(Target)
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
				
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...


            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
                "SlateCore",
				"InputCore",

				// ... add private dependencies that you statically link with here ...	
				"OverdriveAbility",
				"GameplayAbilities",

                "BlueprintGraph",   // UK2Node 클래스
                "KismetCompiler",   // 노드 컴파일
                "Kismet",           // 블루프린트 에디터 유틸리티
                "GraphEditor",       // 그래프 UI
				"UnrealEd"
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
