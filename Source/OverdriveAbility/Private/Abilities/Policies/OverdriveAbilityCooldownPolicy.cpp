// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Policies/OverdriveAbilityCooldownPolicy.h"
#include "Abilities/OverdriveGameplayAbility.h"

UOverdriveGameplayAbility* UOverdriveAbilityCooldownPolicy::GetOwnerAbility() const
{
	return GetTypedOuter<UOverdriveGameplayAbility>();
}
