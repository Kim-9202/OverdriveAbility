// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_FilteredAttributeBase.h"
#include "AbilitySystemComponent.h"

void UOAVM_FilteredAttributeBase::SetSourceTags(const FGameplayTagContainer& InSourceTags)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(SourceTags, InSourceTags))
	{
		RefreshValues();
	}
}

void UOAVM_FilteredAttributeBase::SetTargetTags(const FGameplayTagContainer& InTargetTags)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(TargetTags, InTargetTags))
	{
		RefreshValues();
	}
}

void UOAVM_FilteredAttributeBase::SetIgnoreEffectTags(const FGameplayTagContainer& InIgnoreEffectTags)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(IgnoreEffectTags, InIgnoreEffectTags))
	{
		RefreshValues();
	}
}

void UOAVM_FilteredAttributeBase::BindEffectChangeDelegates()
{
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	EffectAddedHandle = AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UOAVM_FilteredAttributeBase::OnEffectAdded);
	EffectRemovedHandle = AbilitySystem->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UOAVM_FilteredAttributeBase::OnEffectRemoved);
}

void UOAVM_FilteredAttributeBase::UnbindEffectChangeDelegates()
{
	if (IsValid(AbilitySystem))
	{
		if (EffectAddedHandle.IsValid())
		{
			AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.Remove(EffectAddedHandle);
		}
		if (EffectRemovedHandle.IsValid())
		{
			AbilitySystem->OnAnyGameplayEffectRemovedDelegate().Remove(EffectRemovedHandle);
		}
	}

	EffectAddedHandle.Reset();
	EffectRemovedHandle.Reset();
}

TArray<FActiveGameplayEffectHandle> UOAVM_FilteredAttributeBase::GatherIgnoreHandles() const
{
	if (!IsValid(AbilitySystem) || IgnoreEffectTags.IsEmpty())
	{
		return TArray<FActiveGameplayEffectHandle>();
	}

	return AbilitySystem->GetActiveEffectsWithAllTags(IgnoreEffectTags);
}

void UOAVM_FilteredAttributeBase::OnEffectAdded(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	RefreshValues();
}

void UOAVM_FilteredAttributeBase::OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect)
{
	RefreshValues();
}
