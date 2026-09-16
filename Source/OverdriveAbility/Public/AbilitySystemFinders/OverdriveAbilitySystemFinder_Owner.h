// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemFinders/OverdriveAbilitySystemFinder.h"
#include "OverdriveAbilitySystemFinder_Owner.generated.h"

/**
 *
 */
UCLASS(BlueprintType, DisplayName = "Find From Owner")
class OVERDRIVEABILITY_API UOverdriveAbilitySystemFinder_Owner : public UOverdriveAbilitySystemFinder
{
	GENERATED_BODY()

protected:
	virtual UAbilitySystemComponent* FindAbilitySystem_Implementation(UActorComponent* InComponent) const override;
};
