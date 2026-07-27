// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_SingleAttribute.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

void UOAVM_SingleAttribute::SetAttribute(FGameplayAttribute InAttribute)
{
	if (Attribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(Attribute, InAttribute);

	RebuildBindings();
}

void UOAVM_SingleAttribute::RebindDelegates()
{
	if (!IsValid(AbilitySystem) || !Attribute.IsValid())
	{
		return;
	}

	AttributeChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UOAVM_SingleAttribute::OnAttributeChanged);
}

void UOAVM_SingleAttribute::UnbindDelegates()
{
	if (IsValid(AbilitySystem) && Attribute.IsValid() && AttributeChangedHandle.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(AttributeChangedHandle);
	}

	AttributeChangedHandle.Reset();
}

void UOAVM_SingleAttribute::RefreshValues()
{
	float NewValue = 0.f;

	if (IsValid(AbilitySystem) && Attribute.IsValid() && AbilitySystem->HasAttributeSetForAttribute(Attribute))
	{
		NewValue = AbilitySystem->GetNumericAttribute(Attribute);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Value, NewValue);
}

void UOAVM_SingleAttribute::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	UE_MVVM_SET_PROPERTY_VALUE(Value, Data.NewValue);
}
