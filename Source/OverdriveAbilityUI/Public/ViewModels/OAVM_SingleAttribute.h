// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ViewModels/OAVM_AttributeBase.h"
#include "OAVM_SingleAttribute.generated.h"

struct FOnAttributeChangeData;

/**
 * 단일 Attribute를 필터 없이 관찰하는 ViewModel.
 *
 * GetGameplayAttributeValueChangeDelegate에 바인딩해 최종값(NewValue)을 그대로
 * 노출한다. 재계산이 없는 순수 이벤트 push(O(1)).
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_SingleAttribute : public UOAVM_AttributeBase
{
	GENERATED_BODY()

public:
	/** 관찰 대상 Attribute를 설정한다. 바뀌면 재바인딩한다. */
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetAttribute(FGameplayAttribute InAttribute);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	FGameplayAttribute GetAttribute() const { return Attribute; }

protected:
	//~ Begin UOAVM_AttributeBase Interface
	virtual void RebindDelegates() override;
	virtual void UnbindDelegates() override;
	virtual void RefreshValues() override;
	//~ End UOAVM_AttributeBase Interface

	void OnAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintReadOnly, Category = "OverdriveAbility|UI", FieldNotify, Getter, meta = (AllowPrivateAccess = "true"))
	float Value = 0.f;
	float GetValue() const { return Value; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetAttribute", BlueprintGetter = "GetAttribute")
	FGameplayAttribute Attribute;

	FDelegateHandle AttributeChangedHandle;
};
