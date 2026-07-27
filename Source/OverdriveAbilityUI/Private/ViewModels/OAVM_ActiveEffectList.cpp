// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_ActiveEffectList.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "ViewModels/OAVM_ActiveEffect.h"

void UOAVM_ActiveEffectList::SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystem)
{
	if (AbilitySystem.Get() == InAbilitySystem)
	{
		return;
	}

	UnbindSourceDelegates();

	AbilitySystem = InAbilitySystem;

	if (AbilitySystem.IsValid())
	{
		BindSourceDelegates();
	}

	SyncItems();
}

UAbilitySystemComponent* UOAVM_ActiveEffectList::GetAbilitySystemComponent() const
{
	return AbilitySystem.Get();
}

void UOAVM_ActiveEffectList::SetFilterTags(const FGameplayTagContainer& InFilterTags)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(FilterTags, InFilterTags))
	{
		SyncItems();
	}
}

void UOAVM_ActiveEffectList::BeginDestroy()
{
	UnbindSourceDelegates();

	Super::BeginDestroy();
}

void UOAVM_ActiveEffectList::BindSourceDelegates()
{
	if (!AbilitySystem.IsValid())
	{
		return;
	}

	EffectAddedHandle = AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UOAVM_ActiveEffectList::OnEffectAdded);
	EffectRemovedHandle = AbilitySystem->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UOAVM_ActiveEffectList::OnEffectRemoved);
}

void UOAVM_ActiveEffectList::UnbindSourceDelegates()
{
	if (AbilitySystem.IsValid())
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

void UOAVM_ActiveEffectList::SyncItems()
{
	bool bChanged = false;

	// 필터에서 벗어났거나 제거된 항목을 걷어낸다.
	for (int32 Index = Items.Num() - 1; Index >= 0; --Index)
	{
		const UOAVM_ActiveEffect* Item = Items[Index];
		if (!Item || !MatchesFilter(Item->GetHandle()))
		{
			Items.RemoveAt(Index);
			bChanged = true;
		}
	}

	// 새로 매칭된 이펙트에 대한 항목을 추가한다.
	if (AbilitySystem.IsValid())
	{
		UAbilitySystemComponent* ASC = AbilitySystem.Get();
		const TArray<FActiveGameplayEffectHandle> AllHandles = ASC->GetActiveGameplayEffects().GetAllActiveEffectHandles();

		for (const FActiveGameplayEffectHandle& EffectHandle : AllHandles)
		{
			if (MatchesFilter(EffectHandle) && !HasItemForHandle(EffectHandle))
			{
				UOAVM_ActiveEffect* NewItem = NewObject<UOAVM_ActiveEffect>(this);
				NewItem->InitializeFromHandle(EffectHandle);
				Items.Add(NewItem);
				bChanged = true;
			}
		}
	}

	if (bChanged)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
}

bool UOAVM_ActiveEffectList::MatchesFilter(FActiveGameplayEffectHandle InHandle) const
{
	if (!AbilitySystem.IsValid())
	{
		return false;
	}

	const FActiveGameplayEffect* ActiveEffect = AbilitySystem->GetActiveGameplayEffect(InHandle);
	if (!ActiveEffect)
	{
		return false;
	}

	if (FilterTags.IsEmpty())
	{
		return true;
	}

	FGameplayTagContainer AssetTags;
	ActiveEffect->Spec.GetAllAssetTags(AssetTags);
	return AssetTags.HasAll(FilterTags);
}

bool UOAVM_ActiveEffectList::HasItemForHandle(FActiveGameplayEffectHandle InHandle) const
{
	for (const UOAVM_ActiveEffect* Item : Items)
	{
		if (Item && Item->GetHandle() == InHandle)
		{
			return true;
		}
	}

	return false;
}

void UOAVM_ActiveEffectList::OnEffectAdded(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle InHandle)
{
	SyncItems();
}

void UOAVM_ActiveEffectList::OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect)
{
	SyncItems();
}
