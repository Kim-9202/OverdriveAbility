// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Fragments/OverdriveAbilityFragment.h"
#include "Abilities/OverdriveGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbilityTypes.h"

UOverdriveGameplayAbility* UOverdriveAbilityFragment::GetOwnerAbility() const
{
	return GetTypedOuter<UOverdriveGameplayAbility>();
}

