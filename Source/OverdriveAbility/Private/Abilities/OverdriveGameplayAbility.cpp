// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/OverdriveGameplayAbility.h"
#include "Misc/DataValidation.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemStats.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Abilities/Fragments/OverdriveAbilityFragment.h"
#include "Abilities/Policies/OverdriveAbilityCooldownPolicy.h"
#include "Abilities/Policies/OverdriveAbilityCostPolicy.h"

#define LOCTEXT_NAMESPACE "OverdriveGameplayAbility"


namespace FOverdriveAbilitySystemTweaks
{
	int ClearAbilityTimers = 1;
	FAutoConsoleVariableRef CVarClearAbilityTimers(TEXT("AbilitySystem.ClearAbilityTimers"), FOverdriveAbilitySystemTweaks::ClearAbilityTimers, TEXT("Whether to call ClearAllTimersForObject as part of EndAbility call"), ECVF_Default);
}



UOverdriveGameplayAbility::UOverdriveGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivateOnGive = false;
}

void UOverdriveGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	for (UOverdriveAbilityFragment* Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->OnAvatarSet(ActorInfo, Spec);
	}
}

void UOverdriveGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	K2_PreActivate();

	for (UOverdriveAbilityFragment* Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->PreActivate(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}

	if (CooldownPolicy && CooldownPolicy->GetApplyPhase() == EOverdriveAbilityApplyPhase::OnActive)
	{
		CooldownPolicy->ApplyCooldown(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}

	if (CostPolicy && CostPolicy->GetApplyPhase() == EOverdriveAbilityApplyPhase::OnActive)
	{
		CostPolicy->ApplyCost(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}
}

void UOverdriveGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	for (UOverdriveAbilityFragment* Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	for (UOverdriveAbilityFragment* Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->PostActivate(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}
}

bool UOverdriveGameplayAbility::K2_CommitAbilityWithEvent(const FGameplayEventData& EventData)
{
	ensure(CurrentActorInfo);
	return CommitAbilityWithEvent(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData);
}

bool UOverdriveGameplayAbility::K2_CommitAbilityCooldownWithEvent(const FGameplayEventData& EventData, bool BroadcastCommitEvent, bool ForceCooldown)
{
	ensure(CurrentActorInfo);
	if (BroadcastCommitEvent)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured())
		{
			AbilitySystemComponent->NotifyAbilityCommit(this);
		}
	}
	return CommitAbilityCooldownWithEvent(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, ForceCooldown, EventData);
}

bool UOverdriveGameplayAbility::K2_CommitAbilityCostWithEvent(const FGameplayEventData& EventData, bool BroadcastCommitEvent)
{
	ensure(CurrentActorInfo);
	if (BroadcastCommitEvent)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured())
		{
			AbilitySystemComponent->NotifyAbilityCommit(this);
		}
	}
	return CommitAbilityCostWithEvent(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData);
}

bool UOverdriveGameplayAbility::K2_CheckAbilityCooldownWithEvent(const FGameplayEventData& EventData)
{
	ensure(CurrentActorInfo);
	return UAbilitySystemGlobals::Get().ShouldIgnoreCooldowns() || CheckCooldownWithEvent(CurrentSpecHandle, CurrentActorInfo, EventData);
}

bool UOverdriveGameplayAbility::K2_CheckAbilityCostWithEvent(const FGameplayEventData& EventData)
{
	ensure(CurrentActorInfo);
	return UAbilitySystemGlobals::Get().ShouldIgnoreCosts() || CheckCostWithEvent(CurrentSpecHandle, CurrentActorInfo, EventData);}

void UOverdriveGameplayAbility::K2_ApplyCooldown()
{
	ensure(CurrentActorInfo);
	ApplyCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UOverdriveGameplayAbility::K2_ApplyCost()
{
	ensure(CurrentActorInfo);
	ApplyCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UOverdriveGameplayAbility::K2_ApplyCooldownWithEvent(const FGameplayEventData& EventData)
{
	ensure(CurrentActorInfo);
	ApplyCooldownWithEvent(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData);
}

void UOverdriveGameplayAbility::K2_ApplyCostWithEvent(const FGameplayEventData& EventData)
{
	ensure(CurrentActorInfo);
	ApplyCostWithEvent(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData);
}

bool UOverdriveGameplayAbility::CommitAbilityWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (!CommitCheckWithEvent(Handle, ActorInfo, ActivationInfo, EventData, OptionalRelevantTags))
	{
		return false;
	}

	CommitExecuteWithEvent(Handle, ActorInfo, ActivationInfo, EventData);

	K2_CommitExecuteWithEvent(EventData);

	ActorInfo->AbilitySystemComponent->NotifyAbilityCommit(this);

	return true;
}

bool UOverdriveGameplayAbility::CommitAbilityCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool ForceCooldown, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (UAbilitySystemGlobals::Get().ShouldIgnoreCooldowns())
	{
		return true;
	}

	if (!ForceCooldown)
	{
		// Last chance to fail (maybe we no longer have resources to commit since we after we started this ability activation)
		if (!CheckCooldownWithEvent(Handle, ActorInfo, EventData, OptionalRelevantTags))
		{
			return false;
		}
	}

	ApplyCooldownWithEvent(Handle, ActorInfo, ActivationInfo, EventData);
	return true;
}

bool UOverdriveGameplayAbility::CommitAbilityCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (UAbilitySystemGlobals::Get().ShouldIgnoreCosts())
	{
		return true;
	}

	// Last chance to fail (maybe we no longer have resources to commit since we after we started this ability activation)
	if (!CheckCostWithEvent(Handle, ActorInfo, EventData, OptionalRelevantTags))
	{
		return false;
	}

	ApplyCostWithEvent(Handle, ActorInfo, ActivationInfo, EventData);
	return true;
}

bool UOverdriveGameplayAbility::CommitCheckWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	const bool bValidHandle = Handle.IsValid();
	const bool bValidActorInfoPieces = (ActorInfo && (ActorInfo->AbilitySystemComponent != nullptr));
	const bool bValidSpecFound = bValidActorInfoPieces && (ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle) != nullptr);

	// Ensure that the ability spec is even valid before trying to process the commit
	if (!bValidHandle || !bValidActorInfoPieces || !bValidSpecFound)
	{
		ABILITY_LOG(Warning, TEXT("UGameplayAbility::CommitCheck provided an invalid handle or actor info or couldn't find ability spec: %s Handle Valid: %d ActorInfo Valid: %d Spec Not Found: %d"), *GetName(), bValidHandle, bValidActorInfoPieces, bValidSpecFound);
		return false;
	}

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();

	if (!AbilitySystemGlobals.ShouldIgnoreCooldowns() && !CheckCooldownWithEvent(Handle, ActorInfo, EventData, OptionalRelevantTags))
	{
		return false;
	}

	if (!AbilitySystemGlobals.ShouldIgnoreCosts() && !CheckCostWithEvent(Handle, ActorInfo, EventData, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UOverdriveGameplayAbility::CommitExecuteWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData)
{
	ApplyCooldownWithEvent(Handle, ActorInfo, ActivationInfo, EventData);

	ApplyCostWithEvent(Handle, ActorInfo, ActivationInfo, EventData);
}

bool UOverdriveGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CooldownGameplayEffectClass && CooldownPolicy)
	{
		return CooldownPolicy->CheckCooldown(Handle, ActorInfo, &CurrentEventData, OptionalRelevantTags);
	}
	return true;
}

void UOverdriveGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownGameplayEffectClass && CooldownPolicy)
	{
		CooldownPolicy->ApplyCooldown(Handle, ActorInfo, ActivationInfo, &CurrentEventData);
	}
}

bool UOverdriveGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CostGameplayEffectClass && CostPolicy)
	{
		return CostPolicy->CheckCost(Handle, ActorInfo, &CurrentEventData, OptionalRelevantTags);
	}

	return true;
}

void UOverdriveGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CostGameplayEffectClass && CostPolicy)
	{
		CostPolicy->ApplyCost(Handle, ActorInfo, ActivationInfo, &CurrentEventData);
	}
}

bool UOverdriveGameplayAbility::CheckCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CooldownGameplayEffectClass && CooldownPolicy)
	{
		return CooldownPolicy->CheckCooldown(Handle, ActorInfo, &EventData, OptionalRelevantTags);
	}

	// 쿨다운 GE/정책이 없으면 쿨다운 제약 자체가 없음 = 통과.
	// (비-Event판 CheckCooldown, CheckCostWithEvent와 일관되게 true.)
	return true;
}

void UOverdriveGameplayAbility::ApplyCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData) const
{
	if (CooldownGameplayEffectClass && CooldownPolicy)
	{
		CooldownPolicy->ApplyCooldown(Handle, ActorInfo, ActivationInfo, &EventData);
	}
}

bool UOverdriveGameplayAbility::CheckCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CostGameplayEffectClass && CostPolicy)
	{
		return CostPolicy->CheckCost(Handle, ActorInfo, &EventData, OptionalRelevantTags);
	}

	return true;
}

void UOverdriveGameplayAbility::ApplyCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData) const
{
	if (CostGameplayEffectClass && CostPolicy)
	{
		CostPolicy->ApplyCost(Handle, ActorInfo, ActivationInfo, &EventData);
	}
}

void UOverdriveGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Give 시 자동 실행 유무
	if (ActorInfo && !Spec.IsActive() && bAutoActivateOnGive)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UOverdriveGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsEndAbilityValid(Handle, ActorInfo))
	{
		return;
	}

	if (ScopeLockCount <= 0)
	{
		OnEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}

	Super::EndAbility(Handle,ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOverdriveGameplayAbility::OnEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	for (UOverdriveAbilityFragment* Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}

	if (CooldownPolicy && CooldownPolicy->GetApplyPhase() == EOverdriveAbilityApplyPhase::OnEnd)
	{
		CooldownPolicy->ApplyCooldown(Handle, ActorInfo, ActivationInfo, &CurrentEventData);
	}

	if (CostPolicy && CostPolicy->GetApplyPhase() == EOverdriveAbilityApplyPhase::OnEnd)
	{
		CostPolicy->ApplyCost(Handle, ActorInfo, ActivationInfo, &CurrentEventData);
	}
}

#if WITH_EDITOR
EDataValidationResult UOverdriveGameplayAbility::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult SuperResult = Super::IsDataValid(Context);

	EDataValidationResult Result = EDataValidationResult::Valid;

	for (int i = 0; i <Fragments.Num(); i++)
	{
		const UOverdriveAbilityFragment* Fragment = Fragments[i];
		if (!Fragment)
		{
			FText NullErrorText = FText::FormatOrdered(LOCTEXT("FragmentIsNull", "Fragment {0} Index Is null"), i);
			Context.AddError(NullErrorText);

			Result = EDataValidationResult::Invalid;

			continue;
		}

		Result = CombineDataValidationResults(Result, Fragment->IsDataValid(Context));
	}

	if (CooldownGameplayEffectClass)
	{
		if (CooldownPolicy == nullptr)
		{
			Context.AddError(LOCTEXT("CooldownPolicyIsNull", "CooldownPolicy Is null"));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			Result = CombineDataValidationResults(Result, CooldownPolicy->IsDataValid(Context));
		}
	}

	if (CostGameplayEffectClass)
	{
		if (CostPolicy == nullptr)
		{
			Context.AddError(LOCTEXT("CostPolicyIsNull", "CostPolicy Is null"));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			Result = CombineDataValidationResults(Result, CostPolicy->IsDataValid(Context));
		}
	}

	return CombineDataValidationResults(SuperResult, Result);
}

#endif

UOverdriveAbilityFragment* UOverdriveGameplayAbility::GetAbilityFragmentByClass(TSubclassOf<UOverdriveAbilityFragment> FragmentClass) const
{
	for (auto& Fragment : Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		if (Fragment->GetClass() == FragmentClass)
		{
			return Fragment;
		}
	}

	return nullptr;
}
