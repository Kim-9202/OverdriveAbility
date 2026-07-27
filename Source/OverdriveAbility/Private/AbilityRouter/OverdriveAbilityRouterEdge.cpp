// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdgeCondition.h"

UOverdriveAbilityRouterEdge::UOverdriveAbilityRouterEdge(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UOverdriveAbilityRouterEdge::CanEnterEndNode(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const
{
	if (bPressed != bInPressed || InputTypeTag != InInputTypeTag)
	{
		return false;
	}

	if (!ConditionQuery.IsEmpty() && !ConditionQuery.Matches(InStateTags))
	{
		return false;
	}

	// 태그 쿼리보다 비싼 검사(가상 호출·BP 실행)라 마지막에 돌린다.
	for (const UOverdriveAbilityRouterEdgeCondition* EdgeCondition : EdgeConditions)
	{
		// 에디터에서 클래스를 고르지 않은 빈 슬롯이 남을 수 있다.
		if (EdgeCondition == nullptr)
		{
			continue;
		}

		if (!EdgeCondition->CanEnterEndNode(InRouterComponent, bInPressed, InInputTypeTag, InStateTags))
		{
			return false;
		}
	}

	return true;
}

