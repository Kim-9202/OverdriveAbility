// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityRouterEdgeCondition.generated.h"

class UOverdriveAbilityRouterComponent;

/**
 * 엣지 전이의 추가 조건. UOverdriveAbilityRouterEdge에 Instanced 배열로 매달며, 모든 조건이 true를 반환해야 엣지를 통과한다.
 * bPressed/InputTypeTag/ConditionQuery로 표현되지 않는 판정(어트리뷰트 값, 이동 상태 등)을 담당한다.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class OVERDRIVEABILITY_API UOverdriveAbilityRouterEdgeCondition : public UObject
{
	GENERATED_BODY()

public:
	// UOverdriveAbilityRouterEdge::CanEnterEndNode와 동일한 매개변수를 받는다.
	// InStateTags = 라우터의 StateTagContainer + ASC 소유 태그.
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "AbilityRouter")
	bool CanEnterEndNode(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const;
	virtual bool CanEnterEndNode_Implementation(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const { return true; }
};
