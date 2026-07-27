// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_AttributeBase.h"
#include "AbilitySystemComponent.h"

void UOAVM_AttributeBase::SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem)
{
	if (AbilitySystem == InAbilitySystem)
	{
		return;
	}

	UnbindDelegates();

	AbilitySystem = InAbilitySystem;

	RebuildBindings();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySystem);
}

void UOAVM_AttributeBase::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}

void UOAVM_AttributeBase::RebuildBindings()
{
	UnbindDelegates();

	if (IsValid(AbilitySystem))
	{
		RebindDelegates();
	}

	RefreshValues();
}
