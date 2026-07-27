// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "OverdriveAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
protected:
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};

