// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OverdriveAbilityUI : ModuleRules
{
	public OverdriveAbilityUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ViewModel(UMVVMViewModelBase)·FieldNotify 매크로.
				"ModelViewViewModel",
				// public 헤더가 UOverdriveAbilityBlueprintLibrary/GAS 타입을 노출.
				"OverdriveAbility",
				// public 헤더가 FGameplayAttribute/FActiveGameplayEffectHandle/UAbilitySystemComponent를 노출.
				"GameplayAbilities",
				// public 헤더가 FGameplayTagContainer를 멤버로 노출.
				"GameplayTags",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
			}
		);
	}
}
