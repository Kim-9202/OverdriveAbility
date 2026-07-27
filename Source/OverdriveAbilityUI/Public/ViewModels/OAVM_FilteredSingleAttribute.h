// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ViewModels/OAVM_FilteredAttributeBase.h"
#include "OAVM_FilteredSingleAttribute.generated.h"

struct FOnAttributeChangeData;

/**
 * 단일 Attribute를 IgnoreEffectTags에 매칭되는 이펙트를 제외하고 관찰하는 ViewModel.
 * 값은 aggregator 재평가로 산출하며, 재평가는 이벤트 트리거(기저값 변경·이펙트 add/remove)로만 일어난다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAVM_FilteredSingleAttribute : public UOAVM_FilteredAttributeBase
{
	GENERATED_BODY()

public:
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
