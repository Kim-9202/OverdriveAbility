// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"

UOverdriveAbilityRouterEdge::UOverdriveAbilityRouterEdge(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UOverdriveAbilityRouterEdge::CanEnterEndNode(bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const
{
	if (bPressed != bInPressed || InputTypeTag != InInputTypeTag)
	{
		return false;
	}

	if (ConditionQuery.IsEmpty())
	{
		return true;
	}

	return ConditionQuery.Matches(InStateTags);
}

