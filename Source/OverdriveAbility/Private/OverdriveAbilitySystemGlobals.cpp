// Fill out your copyright notice in the Description page of Project Settings.


#include "OverdriveAbilitySystemGlobals.h"
#include "Effects/OverdriveGameplayEffectTypes.h"

FGameplayEffectContext* UOverdriveAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FOverdriveGameplayEffectContext();
}

