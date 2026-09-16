// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFinders/OverdriveAbilitySystemFinder_Owner.h"
#include "AbilitySystemGlobals.h"
#include "Components/ActorComponent.h"

UAbilitySystemComponent* UOverdriveAbilitySystemFinder_Owner::FindAbilitySystem_Implementation(UActorComponent* InComponent) const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InComponent->GetOwner());
}
