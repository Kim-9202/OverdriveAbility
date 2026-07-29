// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OverdriveAbility : ModuleRules
{
	public OverdriveAbility(ReadOnlyTargetRules Target) : base(Target)
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
				"OverdriveAbility/Public/Abilities"
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// public 헤더가 UGameplayAbility 상속·GameplayTag/SpecHandle/ActiveGameplayEffectHandle를
				// 노출하므로 Public 의존성이어야 외부 모듈에서 include 시 컴파일/링크됨.
				"GameplayAbilities",
				"GameplayTasks",
				"GameplayTags",
				// 라우터 컴포넌트의 public 헤더가 TObjectPtr<const UInputAction>를 노출.
				"EnhancedInput",
				// AnimNotify public 헤더가 Animation/AnimNotifies/AnimNotify.h를 include.
				"Engine",
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Slate",
				"SlateCore",
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
