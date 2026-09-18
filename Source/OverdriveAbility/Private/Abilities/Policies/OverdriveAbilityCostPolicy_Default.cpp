// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Policies/OverdriveAbilityCostPolicy_Default.h"
#include "Abilities/OverdriveGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool UOverdriveAbilityCostPolicy_Default::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	UOverdriveGameplayAbility* OwnerAbility = GetOwnerAbility();
	if (!ensure(OwnerAbility))
	{
		return true;
	}

	int32 AbilityLevel = OwnerAbility->GetAbilityLevel(Handle, ActorInfo);

	UGameplayEffect* CostGE = OwnerAbility->GetCostGameplayEffect();
	if (CostGE)
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

		if (ensure(AbilitySystemComponent))
		{
			if (!AbilitySystemComponent->CanApplyAttributeModifiers(CostGE, AbilityLevel, OwnerAbility->MakeEffectContext(Handle, ActorInfo)))
			{
				const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

				if (OptionalRelevantTags && CostTag.IsValid())
				{
					OptionalRelevantTags->AddTag(CostTag);
				}
				return false;
			}
		}
	}
	return true;
}

void UOverdriveAbilityCostPolicy_Default::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData) const
{
	if (!ensure(ActorInfo))
	{
		return;
	}

	UOverdriveGameplayAbility* OwnerAbility = GetOwnerAbility();
	if (!ensure(OwnerAbility))
	{
		return;
	}

	int32 AbilityLevel = OwnerAbility->GetAbilityLevel(Handle, ActorInfo);

	UGameplayEffect* CostGE = OwnerAbility->GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle = OwnerAbility->MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGE->GetClass(), AbilityLevel);

		if (!SpecHandle.IsValid() || !OwnerAbility->HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		{
			return;
		}

		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		if (!ensure(AbilitySystemComponent))
		{
			return;
		}
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), AbilitySystemComponent->GetPredictionKeyForNewAction());
	}
}

