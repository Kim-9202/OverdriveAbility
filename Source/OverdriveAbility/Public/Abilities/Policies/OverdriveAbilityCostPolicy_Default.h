// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Policies/OverdriveAbilityCostPolicy.h"
#include "OverdriveAbilityCostPolicy_Default.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityCostPolicy_Default : public UOverdriveAbilityCostPolicy
{
	GENERATED_BODY()
	
private:
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData = nullptr) const override;
};

