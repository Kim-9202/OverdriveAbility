// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Policies/OverdriveAbilityCooldownPolicy_Stack.h"
#include "Abilities/OverdriveGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "Effects/Fragments/OverdriveEffectContextFragment_Cooldown.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "OverdriveAbilityCooldownPolicy_Stack"

bool UOverdriveAbilityCooldownPolicy_Stack::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ensure(ActorInfo))
	{
		return true;
	}
	if (!CooldownEffectHandle.IsValid())
	{
		return true;
	}

	UOverdriveGameplayAbility* OwnerAbility = GetOwnerAbility();
	if (!ensure(OwnerAbility))
	{
		return true;
	}

	const FGameplayTagContainer* CooldownTags = OwnerAbility->GetCooldownTags();

	if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		if (const FActiveGameplayEffect* ActiveCooldownEffect = AbilitySystemComponent->GetActiveGameplayEffect(CooldownEffectHandle))
		{
			if (ActiveCooldownEffect->Spec.GetStackCount() >= ActiveCooldownEffect->Spec.Def->GetStackLimitCount())
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
		else
		{
			CooldownEffectHandle.Invalidate();
		}
	}
	
	return true;
}

void UOverdriveAbilityCooldownPolicy_Stack::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData) const
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

#if WITH_EDITOR
EDataValidationResult UOverdriveAbilityCooldownPolicy_Stack::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	const UOverdriveGameplayAbility* OwnerAbility = GetOwnerAbility();
	if (OwnerAbility == nullptr)
	{
		return Result;
	}

	const UGameplayEffect* CooldownEffect = OwnerAbility->GetCooldownGameplayEffect();
	if (CooldownEffect == nullptr)
	{
		return Result;
	}

	if (CooldownEffect->GetStackingType() == EGameplayEffectStackingType::None)
	{
		Context.AddError(LOCTEXT("StackingTypeIsNone", "Stack 쿨다운 정책은 스택되는 쿨다운 GE가 필요합니다. 쿨다운 GameplayEffect의 StackingType을 None이 아닌 값으로 설정하세요."));

		Result = EDataValidationResult::Invalid;
	}

	if (CooldownEffect->GetStackLimitCount() <= 0)
	{
		Context.AddError(LOCTEXT("StackLimitCountIsUnlimited", "쿨다운 GameplayEffect의 StackLimitCount가 0 이하(무제한)입니다. Stack 쿨다운 정책은 한도가 있어야 동작하므로 1 이상으로 설정하세요."));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE

