// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ViewModels/OAVM_AttributeBase.h"
#include "OAVM_PairedAttribute.generated.h"

struct FOnAttributeChangeData;

/**
 * 두 Attribute(Current/Max)를 필터 없이 관찰하고 Percent(Current/Max)를 파생 노출하는 ViewModel.
 * HP바처럼 비율이 필요한 경우를 위한 형태. 재계산은 나눗셈 한 번으로 저렴하다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_PairedAttribute : public UOAVM_AttributeBase
{
	GENERATED_BODY()

public:
	//~ CurrentAttribute/MaxAttribute의 접근자. UPROPERTY의 Setter/Getter(네이티브)와
	//~ BlueprintSetter/BlueprintGetter가 모두 이 함수들을 참조한다.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetCurrentAttribute(FGameplayAttribute InAttribute);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayAttribute GetCurrentAttribute() const { return CurrentAttribute; }

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetMaxAttribute(FGameplayAttribute InAttribute);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayAttribute GetMaxAttribute() const { return MaxAttribute; }

protected:
	//~ Begin UOAVM_AttributeBase Interface
	virtual void RebindDelegates() override;
	virtual void UnbindDelegates() override;
	virtual void RefreshValues() override;
	//~ End UOAVM_AttributeBase Interface

	void OnCurrentChanged(const FOnAttributeChangeData& Data);
	void OnMaxChanged(const FOnAttributeChangeData& Data);
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetCurrentAttribute", BlueprintGetter = "GetCurrentAttribute", meta = (AllowPrivateAccess = "true"))
	FGameplayAttribute CurrentAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetMaxAttribute", BlueprintGetter = "GetMaxAttribute", meta = (AllowPrivateAccess = "true"))
	FGameplayAttribute MaxAttribute;

	FDelegateHandle CurrentChangedHandle;
	FDelegateHandle MaxChangedHandle;
};
