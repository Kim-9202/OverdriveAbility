// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "ViewModels/OAUIViewModelBase.h"
#include "OAVM_ActiveEffect.generated.h"

class UAbilitySystemComponent;

/**
 * 활성 GameplayEffect 하나를 관찰하는 ViewModel.
 *
 * Stack/Duration/Inhibition/AssetTags 등 "상태"만 FieldNotify로 노출한다.
 * 매프레임 감소하는 남은시간·진행바는 VM이 Tick하지 않고, View가 GetTimeRemaining/GetProgress를
 * 자신의 Tick에서 호출해 계산한다.
 * 목록 관리(생성·제거)는 UOAVM_ActiveEffectList가 담당한다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_ActiveEffect : public UOAUIViewModelBase
{
	GENERATED_BODY()

public:
	/** 관찰할 이펙트 핸들을 주입하고 이벤트를 바인딩한다. ASC는 핸들에서 유도한다. */
	void InitializeFromHandle(FActiveGameplayEffectHandle InHandle);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FActiveGameplayEffectHandle GetHandle() const { return Handle; }

	/** 현재 월드 시간 기준 남은 시간(초). 무한 지속이면 -1. */
	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	float GetTimeRemaining() const;

	/** 진행도 0~1(남은시간/총지속). 무한 지속이면 1. */
	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	float GetProgress() const;

	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

protected:
	void BindEvents();
	void UnbindEvents();
	void RefreshFromEffect();

	void OnStackChanged(FActiveGameplayEffectHandle InHandle, int32 NewStackCount, int32 PreviousStackCount);
	void OnTimeChanged(FActiveGameplayEffectHandle InHandle, float NewStartTime, float NewDuration);
	void OnInhibitionChanged(FActiveGameplayEffectHandle InHandle, bool bInInhibited);

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	int32 StackCount = 0;
	int32 GetStackCount() const { return StackCount; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float StartWorldTime = 0.f;
	float GetStartWorldTime() const { return StartWorldTime; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float Duration = 0.f;
	float GetDuration() const { return Duration; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter = "IsInhibited", meta = (AllowPrivateAccess = "true"))
	bool bInhibited = false;
	bool IsInhibited() const { return bInhibited; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter = "IsInfinite", meta = (AllowPrivateAccess = "true"))
	bool bInfinite = false;
	bool IsInfinite() const { return bInfinite; }

	/** 이펙트의 Asset 태그. UI가 아이콘/이름을 태그로 매핑하는 데 쓴다. */
	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AssetTags;
	FGameplayTagContainer GetAssetTags() const { return AssetTags; }

	UPROPERTY()
	FActiveGameplayEffectHandle Handle;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	FDelegateHandle StackChangedHandle;
	FDelegateHandle TimeChangedHandle;
	FDelegateHandle InhibitionChangedHandle;
};
