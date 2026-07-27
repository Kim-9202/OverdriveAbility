// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "ViewModels/OAVM_AttributeBase.h"
#include "OAVM_FilteredAttributeBase.generated.h"

struct FGameplayEffectSpec;
struct FActiveGameplayEffect;

/**
 * 특정 GameplayEffect를 제외한 Attribute 값을 관찰하는 필터형 VM 공통 베이스.
 *
 * 필터값은 aggregator 재평가가 필요하고, "필터값 변경"을 직접 알려주는 델리게이트가
 * 없으므로 이벤트 트리거(이펙트 add/remove)로 재계산을 유발한다. 매프레임 폴링하지 않는다.
 * 태그 프로퍼티·이펙트 트리거 바인딩·무시 핸들 수집을 파생(단일/이중)이 공유한다.
 */
UCLASS(Abstract)
class OVERDRIVEABILITYUI_API UOAVM_FilteredAttributeBase : public UOAVM_AttributeBase
{
	GENERATED_BODY()

public:
	/** 필터값 산출에 쓰이는 Source 태그. 바뀌면 재계산한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetSourceTags(const FGameplayTagContainer& InSourceTags);

	/** 필터값 산출에 쓰이는 Target 태그. 바뀌면 재계산한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetTargetTags(const FGameplayTagContainer& InTargetTags);

	/** 이 태그를 모두 가진 활성 이펙트를 값 계산에서 제외한다. 바뀌면 재계산한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetIgnoreEffectTags(const FGameplayTagContainer& InIgnoreEffectTags);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayTagContainer GetSourceTags() const { return SourceTags; }

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayTagContainer GetTargetTags() const { return TargetTags; }

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayTagContainer GetIgnoreEffectTags() const { return IgnoreEffectTags; }

protected:
	/** 이펙트 add/remove 델리게이트를 바인딩한다(파생의 RebindDelegates에서 호출). */
	void BindEffectChangeDelegates();

	/** 이펙트 add/remove 델리게이트를 해제한다(파생의 UnbindDelegates에서 호출). */
	void UnbindEffectChangeDelegates();

	/** IgnoreEffectTags를 모두 가진 활성 이펙트 핸들 목록을 반환한다. */
	TArray<FActiveGameplayEffectHandle> GatherIgnoreHandles() const;

	void OnEffectAdded(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	void OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect);

	/** 필터값 산출에 쓰이는 Source 태그. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetSourceTags", BlueprintGetter = "GetSourceTags")
	FGameplayTagContainer SourceTags;

	/** 필터값 산출에 쓰이는 Target 태그. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetTargetTags", BlueprintGetter = "GetTargetTags")
	FGameplayTagContainer TargetTags;

	/** 이 태그를 모두 가진 활성 이펙트를 값 계산에서 제외한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetIgnoreEffectTags", BlueprintGetter = "GetIgnoreEffectTags")
	FGameplayTagContainer IgnoreEffectTags;

	FDelegateHandle EffectAddedHandle;
	FDelegateHandle EffectRemovedHandle;
};
