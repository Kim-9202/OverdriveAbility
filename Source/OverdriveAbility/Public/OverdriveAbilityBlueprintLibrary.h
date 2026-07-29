// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "OverdriveAbilityBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
class UOverdriveAbilityRouterComponent;

UENUM()
enum class EOverdriveStructUtilsResult : uint8
{
	Valid,
	NotValid,
};

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** 액터의 AbilityRouter 컴포넌트를 반환한다. 라우터는 Pawn에 붙는 것이 전제이며, 조회 지점을 여기로 단일화한다. */
	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|Router", meta = (DefaultToSelf = "Actor"))
	static UOverdriveAbilityRouterComponent* GetAbilityRouterComponent(AActor* Actor);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "OverdriveAbility|Context", meta = (CustomStructureParam = "InFragment", BlueprintInternalUseOnly = "true"))
	static void AddContextFragment(const FGameplayEffectContextHandle& Handle, const int32& InFragment);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Context", meta = (CustomStructureParam = "InFragments"))
	static void AddContextFragments(const FGameplayEffectContextHandle& Handle, const TArray<TInstancedStruct<FOverdriveEffectContextFragment>>& InFragment);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "OverdriveAbility|Context", meta = (CustomStructureParam = "Fragment", ExpandEnumAsExecs = "ExecResult", BlueprintInternalUseOnly = "true"))
	static void GetContextFragment(EOverdriveStructUtilsResult& ExecResult, const FGameplayEffectContextHandle& Handle, UScriptStruct* FragmentType, int32& Fragment);

	DECLARE_FUNCTION(execGetContextFragment);
	DECLARE_FUNCTION(execAddContextFragment);

	/** 지정한 활성 GameplayEffect 핸들들을 무시하고, Source/Target 태그 필터를 적용한 어트리뷰트 값을 OutValue로 내보낸다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Attribute", meta = (ExpandBoolAsExecs = "ReturnValue", AutoCreateRefTerm = "SourceTags,TargetTags,EffectsToIgnore"))
	static bool EvaluateAttributeValueIgnoringEffects(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags, const TArray<FActiveGameplayEffectHandle>& EffectsToIgnore, float& OutValue);

	/** 쿼리(이펙트 클래스·태그 등)에 매칭되는 활성 GameplayEffect들을 무시하고, Source/Target 태그 필터를 적용한 어트리뷰트 값을 OutValue로 내보낸다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Attribute", meta = (ExpandBoolAsExecs = "ReturnValue", AutoCreateRefTerm = "SourceTags,TargetTags,IgnoreQuery"))
	static bool EvaluateAttributeValueIgnoringEffectsByQuery(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags, const FGameplayEffectQuery& IgnoreQuery, float& OutValue);

	/** UAbilitySystemComponent::GetFilteredAttributeValue의 BP 래퍼.
	 *  SourceTags 필터(RequireTags만 사용, TagQuery/IgnoreTags는 엔진에서 무시됨)와
	 *  TargetTags를 적용하고, HandlesToIgnore의 활성 이펙트를 제외한 어트리뷰트 값을 OutValue로 내보낸다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Attribute", meta = (ExpandBoolAsExecs = "ReturnValue", AutoCreateRefTerm = "SourceTags,TargetTags,HandlesToIgnore"))
	static bool GetFilteredAttributeValue(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagRequirements& SourceTags, const FGameplayTagContainer& TargetTags, const TArray<FActiveGameplayEffectHandle>& HandlesToIgnore, float& OutValue);

	/** 쿼리에 매칭되는 활성 GameplayEffect들을 조회해 무시 목록으로 넘기는 GetFilteredAttributeValue 변형. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Attribute", meta = (ExpandBoolAsExecs = "ReturnValue", AutoCreateRefTerm = "SourceTags,TargetTags,IgnoreQuery"))
	static bool GetFilteredAttributeValueByQuery(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagRequirements& SourceTags, const FGameplayTagContainer& TargetTags, const FGameplayEffectQuery& IgnoreQuery, float& OutValue);
};

