// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Policies/OverdriveAbilityCooldownPolicy.h"
#include "OverdriveAbilityCooldownPolicy_Default.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityCooldownPolicy_Default : public UOverdriveAbilityCooldownPolicy
{
	GENERATED_BODY()
	
private:
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData = nullptr) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown", meta = (ForceInlineRow, AllowPrivateAccess = "true"))
	TMap<FGameplayTag, float> NameSetByCallerMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown", meta = (ForceInlineRow, AllowPrivateAccess = "true"))
	TMap<FGameplayTag, float> TagSetByCallerMap;
};

