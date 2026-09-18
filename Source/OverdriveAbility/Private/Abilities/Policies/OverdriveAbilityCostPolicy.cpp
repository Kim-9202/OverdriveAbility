// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Policies/OverdriveAbilityCostPolicy.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/OverdriveGameplayAbility.h"

UOverdriveGameplayAbility* UOverdriveAbilityCostPolicy::GetOwnerAbility() const
{
	return GetTypedOuter<UOverdriveGameplayAbility>();
}
