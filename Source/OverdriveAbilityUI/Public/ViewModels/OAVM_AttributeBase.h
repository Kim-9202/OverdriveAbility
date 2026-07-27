// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewModels/OAUIViewModelBase.h"
#include "OAVM_AttributeBase.generated.h"

class UAbilitySystemComponent;

/**
 * Attribute 관찰 ViewModel 공통 베이스.
 *
 * ASC를 주입받아 델리게이트를 재바인딩하는 골격을 제공한다. 어떤 델리게이트를 걸고 어떤
 * 값을 pull 할지는 파생 클래스가 RebindDelegates/UnbindDelegates/RefreshValues로 구현한다.
 * VM은 매프레임 Tick/Timer를 돌리지 않는다(값 계산은 View 측 책임).
 */
UCLASS(Abstract)
class OVERDRIVEABILITYUI_API UOAVM_AttributeBase : public UOAUIViewModelBase
{
	GENERATED_BODY()

public:
	//~ AbilitySystem의 접근자. UPROPERTY의 Setter/Getter(네이티브)와 BlueprintSetter/BlueprintGetter가
	//~ 모두 이 함수들을 참조하므로, 디테일 패널·BP·C++ 어느 경로로 써도 세터를 경유한다.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|UI")
	void SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem);

	UFUNCTION(BlueprintPure, Category = "OverdriveAbility|UI")
	UAbilitySystemComponent* GetAbilitySystem() const { return AbilitySystem; }

	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

protected:
	/** ASC 유효 시 필요한 델리게이트를 바인딩한다. */
	virtual void RebindDelegates() {}

	/** 걸어둔 모든 델리게이트를 해제한다. 중복 호출돼도 안전해야 한다. */
	virtual void UnbindDelegates() {}

	/** 현재 ASC/Attribute 기준으로 FieldNotify 값을 다시 읽어온다. */
	virtual void RefreshValues() {}

	/** 관찰 대상이 바뀐 뒤 바인딩과 값을 다시 맞춘다. 대입 전에 UnbindDelegates를 먼저 부를 것. */
	void RebuildBindings();

	UPROPERTY(BlueprintReadWrite, Category = "OverdriveAbility|UI", FieldNotify, Setter, Getter, BlueprintSetter = "SetAbilitySystem", BlueprintGetter = "GetAbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
};
