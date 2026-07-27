// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_FilteredSingleAttribute.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "OverdriveAbilityBlueprintLibrary.h"

void UOAVM_FilteredSingleAttribute::SetAttribute(FGameplayAttribute InAttribute)
{
	if (Attribute == InAttribute)
	{
		return;
	}

	UnbindDelegates();

	UE_MVVM_SET_PROPERTY_VALUE(Attribute, InAttribute);

	RebuildBindings();
}

void UOAVM_FilteredSingleAttribute::RebindDelegates()
{
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	if (Attribute.IsValid())
	{
		AttributeChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UOAVM_FilteredSingleAttribute::OnAttributeChanged);
	}

	BindEffectChangeDelegates();
}

void UOAVM_FilteredSingleAttribute::UnbindDelegates()
{
	if (IsValid(AbilitySystem) && Attribute.IsValid() && AttributeChangedHandle.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(AttributeChangedHandle);
	}

	AttributeChangedHandle.Reset();

	UnbindEffectChangeDelegates();
}

void UOAVM_FilteredSingleAttribute::RefreshValues()
{
	float NewValue = 0.f;

	if (IsValid(AbilitySystem) && Attribute.IsValid())
	{
		UOverdriveAbilityBlueprintLibrary::EvaluateAttributeValueIgnoringEffects(AbilitySystem, Attribute, SourceTags, TargetTags, GatherIgnoreHandles(), NewValue);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Value, NewValue);
}

void UOAVM_FilteredSingleAttribute::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	// 필터값은 NewValue와 다를 수 있으므로 재평가한다.
	RefreshValues();
}
