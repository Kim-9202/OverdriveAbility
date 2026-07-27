// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_PairedAttribute.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

void UOAVM_PairedAttribute::SetCurrentAttribute(FGameplayAttribute InAttribute)
{
	if (CurrentAttribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(CurrentAttribute, InAttribute);

	RebuildBindings();
}

void UOAVM_PairedAttribute::SetMaxAttribute(FGameplayAttribute InAttribute)
{
	if (MaxAttribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(MaxAttribute, InAttribute);

	RebuildBindings();
}

void UOAVM_PairedAttribute::RebindDelegates()
{
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	if (CurrentAttribute.IsValid())
	{
		CurrentChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(this, &UOAVM_PairedAttribute::OnCurrentChanged);
	}

	if (MaxAttribute.IsValid())
	{
		MaxChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UOAVM_PairedAttribute::OnMaxChanged);
	}
}

void UOAVM_PairedAttribute::UnbindDelegates()
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
}

void UOAVM_PairedAttribute::RefreshValues()
{
	float NewCurrent = 0.f;
	float NewMax = 0.f;

	if (IsValid(AbilitySystem))
	{
		if (CurrentAttribute.IsValid() && AbilitySystem->HasAttributeSetForAttribute(CurrentAttribute))
		{
			NewCurrent = AbilitySystem->GetNumericAttribute(CurrentAttribute);
		}

		if (MaxAttribute.IsValid() && AbilitySystem->HasAttributeSetForAttribute(MaxAttribute))
		{
			NewMax = AbilitySystem->GetNumericAttribute(MaxAttribute);
		}
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentValue, NewCurrent);
	UE_MVVM_SET_PROPERTY_VALUE(MaxValue, NewMax);
	RecalcPercent();
}

void UOAVM_PairedAttribute::OnCurrentChanged(const FOnAttributeChangeData& Data)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentValue, Data.NewValue);
	RecalcPercent();
}

void UOAVM_PairedAttribute::OnMaxChanged(const FOnAttributeChangeData& Data)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxValue, Data.NewValue);
	RecalcPercent();
}

void UOAVM_PairedAttribute::RecalcPercent()
{
	const float NewPercent = (MaxValue > KINDA_SMALL_NUMBER) ? (CurrentValue / MaxValue) : 0.f;
	UE_MVVM_SET_PROPERTY_VALUE(Percent, NewPercent);
}
