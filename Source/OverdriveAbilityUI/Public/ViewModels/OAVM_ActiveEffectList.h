// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "ViewModels/OAUIViewModelBase.h"
#include "OAVM_ActiveEffectList.generated.h"

class UAbilitySystemComponent;
class UOAVM_ActiveEffect;
struct FGameplayEffectSpec;
struct FActiveGameplayEffect;

/**
 * FilterTags에 매칭되는 활성 GameplayEffect들을 항목 ViewModel 목록으로 노출하는 컨테이너 ViewModel.
 *
 * 이펙트 add/remove 델리게이트를 구독해, 발화 시 전체 활성 이펙트를 diff 하여 Items를 증분 갱신한다.
 * 기존 항목 VM은 재사용하고 신규만 생성/제거하므로 UI 바인딩이 유지된다. 항목 VM은 이 컨테이너가 소유한다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_ActiveEffectList : public UOAUIViewModelBase
{
	GENERATED_BODY()

public:
	/** 관찰할 ASC를 주입한다. 바뀌면 재구독 후 목록을 재구성한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystem);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** 이 태그를 모두 가진 이펙트만 목록에 포함한다(비어 있으면 전체). 바뀌면 목록을 재구성한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetFilterTags(const FGameplayTagContainer& InFilterTags);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayTagContainer GetFilterTags() const { return FilterTags; }

	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

protected:
	void BindSourceDelegates();
	void UnbindSourceDelegates();

	/** 현재 활성 이펙트를 필터링해 Items를 증분 동기화한다. */
	void SyncItems();

	bool MatchesFilter(FActiveGameplayEffectHandle InHandle) const;
	bool HasItemForHandle(FActiveGameplayEffectHandle InHandle) const;

	void OnEffectAdded(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle InHandle);
	void OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect);

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UOAVM_ActiveEffect>> Items;
	TArray<TObjectPtr<UOAVM_ActiveEffect>> GetItems() const { return Items; }

	/** 이 태그를 모두 가진 이펙트만 목록에 포함한다(비어 있으면 전체). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetFilterTags", BlueprintGetter = "GetFilterTags")
	FGameplayTagContainer FilterTags;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	FDelegateHandle EffectAddedHandle;
	FDelegateHandle EffectRemovedHandle;
};
