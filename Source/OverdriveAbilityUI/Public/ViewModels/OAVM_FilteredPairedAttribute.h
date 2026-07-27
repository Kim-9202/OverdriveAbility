// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ViewModels/OAVM_FilteredAttributeBase.h"
#include "OAVM_FilteredPairedAttribute.generated.h"

struct FOnAttributeChangeData;

/**
 * 두 Attribute(Current/Max)를 IgnoreEffectTags에 매칭되는 이펙트를 제외하고 관찰하며
 * Percent(Current/Max)를 파생 노출하는 ViewModel. 재평가는 이벤트 트리거로만 일어난다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_FilteredPairedAttribute : public UOAVM_FilteredAttributeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetCurrentAttribute(FGameplayAttribute InAttribute);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetMaxAttribute(FGameplayAttribute InAttribute);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayAttribute GetCurrentAttribute() const { return CurrentAttribute; }

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayAttribute GetMaxAttribute() const { return MaxAttribute; }

protected:
	//~ Begin UOAVM_AttributeBase Interface
	virtual void RebindDelegates() override;
	virtual void UnbindDelegates() override;
	virtual void RefreshValues() override;
	//~ End UOAVM_AttributeBase Interface

	/** 기저값·이펙트 변화 어느 쪽이든 전체 필터값을 재평가한다. */
	void OnObservedAttributeChanged(const FOnAttributeChangeData& Data);
	void RecalcPercent();

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float CurrentValue = 0.f;
	float GetCurrentValue() const { return CurrentValue; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float MaxValue = 0.f;
	float GetMaxValue() const { return MaxValue; }

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float Percent = 0.f;
	float GetPercent() const { return Percent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetCurrentAttribute", BlueprintGetter = "GetCurrentAttribute")
	FGameplayAttribute CurrentAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetMaxAttribute", BlueprintGetter = "GetMaxAttribute")
	FGameplayAttribute MaxAttribute;

	FDelegateHandle CurrentChangedHandle;
	FDelegateHandle MaxChangedHandle;
};
