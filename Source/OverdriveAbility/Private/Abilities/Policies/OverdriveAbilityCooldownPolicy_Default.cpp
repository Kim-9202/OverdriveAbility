// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Policies/OverdriveAbilityCooldownPolicy_Default.h"
#include "Abilities/OverdriveGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "OverdriveAbilityBlueprintLibrary.h"
#include "Effects/Fragments/OverdriveEffectContextFragment_Cooldown.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool UOverdriveAbilityCooldownPolicy_Default::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ensure(ActorInfo))
	{
		return true;
	}

	UOverdriveGameplayAbility* OwnerAbility = GetOwnerAbility();
	if (!ensure(OwnerAbility))
	{
		return true;
	}

	const FGameplayTagContainer* CooldownTags = OwnerAbility->GetCooldownTags();
	if (CooldownTags && !CooldownTags->IsEmpty())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			if (AbilitySystemComponent->HasAnyMatchingGameplayTags(*CooldownTags))
			{
				if (OptionalRelevantTags)
				{
					const FGameplayTag& FailCooldownTag = UAbilitySystemGlobals::Get().ActivateFailCooldownTag;
					if (FailCooldownTag.IsValid())
					{
						OptionalRelevantTags->AddTag(FailCooldownTag);
					}

					// Let the caller know which tags were blocking
					OptionalRelevantTags->AppendMatchingTags(AbilitySystemComponent->GetOwnedGameplayTags(), *CooldownTags);
				}

				return false;
			}
		}
	}
	return true;
}

void UOverdriveAbilityCooldownPolicy_Default::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData) const
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

	UGameplayEffect* EffectObject = OwnerAbility->GetCooldownGameplayEffect();
	int32 AbilityLevel = OwnerAbility->GetAbilityLevel(Handle, ActorInfo);

	if (EffectObject)
	{
		FGameplayEffectSpecHandle SpecHandle = OwnerAbility->MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, EffectObject->GetClass(), AbilityLevel);
	
		if (!SpecHandle.IsValid() || !OwnerAbility->HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		{
			return;
		}

		for (auto& [DataName, Magnitude] : NameSetByCallerMap)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(DataName, Magnitude);
		}

		for (auto& [DataTag, Magnitude] : TagSetByCallerMap)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
		}

		if (EventData && EventData->ContextHandle.IsValid())
		{
			const FOverdriveGameplayEffectContext* OverdriveAbilityContext = StaticCast<const FOverdriveGameplayEffectContext*>(EventData->ContextHandle.Get());

			if (const FOverdriveEffectContextFragment_Cooldown* CooldownMemory = StaticCast<const FOverdriveEffectContextFragment_Cooldown*>(OverdriveAbilityContext->GetFragmentMemory(FOverdriveEffectContextFragment_Cooldown::StaticStruct())))
			{
				if (CooldownMemory->bAbsoluteCoolTime)
				{
					SpecHandle.Data->SetDuration(CooldownMemory->AbsoluteCoolTime, true);
				}
				else
				{
					for (auto& [DataName, Magnitude] : CooldownMemory->NameSetByCallerMap)
					{
						SpecHandle.Data->SetSetByCallerMagnitude(DataName, Magnitude);
					}

					for (auto& [DataTag, Magnitude] : CooldownMemory->TagSetByCallerMap)
					{
						SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
					}
				}
			}
		}

		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		if (!ensure(AbilitySystemComponent))
		{
			return;
		}
		CooldownEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), AbilitySystemComponent->GetPredictionKeyForNewAction());
	}
}
