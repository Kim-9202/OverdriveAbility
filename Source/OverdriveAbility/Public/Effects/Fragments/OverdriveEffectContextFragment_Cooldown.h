// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveEffectContextFragment_Cooldown.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Cooldown"))
struct OVERDRIVEABILITY_API FOverdriveEffectContextFragment_Cooldown : public FOverdriveEffectContextFragment
{
	GENERATED_BODY()
public:
	FOverdriveEffectContextFragment_Cooldown();
	~FOverdriveEffectContextFragment_Cooldown();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bAbsoluteCoolTime;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float AbsoluteCoolTime;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (ForceInlineRow))
	TMap<FName, float> NameSetByCallerMap;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (ForceInlineRow))
	TMap<FGameplayTag, float> TagSetByCallerMap;
};

