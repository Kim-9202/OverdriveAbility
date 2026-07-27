// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "OverdriveAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
private:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};

