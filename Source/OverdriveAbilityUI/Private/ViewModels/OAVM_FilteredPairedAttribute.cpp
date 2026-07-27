// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_FilteredPairedAttribute.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "OverdriveAbilityBlueprintLibrary.h"

void UOAVM_FilteredPairedAttribute::SetCurrentAttribute(FGameplayAttribute InAttribute)
{
	if (CurrentAttribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(CurrentAttribute, InAttribute);

	RebuildBindings();
}

void UOAVM_FilteredPairedAttribute::SetMaxAttribute(FGameplayAttribute InAttribute)
{
	if (MaxAttribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(MaxAttribute, InAttribute);

	RebuildBindings();
}

void UOAVM_FilteredPairedAttribute::RebindDelegates()
{
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	if (CurrentAttribute.IsValid())
	{
		CurrentChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(this, &UOAVM_FilteredPairedAttribute::OnObservedAttributeChanged);
	}

	if (MaxAttribute.IsValid())
	{
		MaxChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UOAVM_FilteredPairedAttribute::OnObservedAttributeChanged);
	}

	BindEffectChangeDelegates();
}

void UOAVM_FilteredPairedAttribute::UnbindDelegates()
{
	if (IsValid(AbilitySystem))
	{
		if (CurrentAttribute.IsValid() && CurrentChangedHandle.IsValid())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).Remove(CurrentChangedHandle);
		}

		if (MaxAttribute.IsValid() && MaxChangedHandle.IsValid())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).Remove(MaxChangedHandle);
		}
	}

	CurrentChangedHandle.Reset();
	MaxChangedHandle.Reset();

	UnbindEffectChangeDelegates();
}

void UOAVM_FilteredPairedAttribute::RefreshValues()
{
	float NewCurrent = 0.f;
	float NewMax = 0.f;

	if (IsValid(AbilitySystem))
	{
		UAbilitySystemComponent* ASC = AbilitySystem;
		const TArray<FActiveGameplayEffectHandle> IgnoreHandles = GatherIgnoreHandles();

		if (CurrentAttribute.IsValid())
		{
			UOverdriveAbilityBlueprintLibrary::EvaluateAttributeValueIgnoringEffects(ASC, CurrentAttribute, SourceTags, TargetTags, IgnoreHandles, NewCurrent);
		}

		if (MaxAttribute.IsValid())
		{
			UOverdriveAbilityBlueprintLibrary::EvaluateAttributeValueIgnoringEffects(ASC, MaxAttribute, SourceTags, TargetTags, IgnoreHandles, NewMax);
		}
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentValue, NewCurrent);
	UE_MVVM_SET_PROPERTY_VALUE(MaxValue, NewMax);
	RecalcPercent();
}

void UOAVM_FilteredPairedAttribute::OnObservedAttributeChanged(const FOnAttributeChangeData& Data)
{
	// 필터값은 개별 NewValue와 다를 수 있으므로 전체 재평가한다.
	RefreshValues();
}

void UOAVM_FilteredPairedAttribute::RecalcPercent()
{
	const float NewPercent = (MaxValue > KINDA_SMALL_NUMBER) ? (CurrentValue / MaxValue) : 0.f;
	UE_MVVM_SET_PROPERTY_VALUE(Percent, NewPercent);
}
