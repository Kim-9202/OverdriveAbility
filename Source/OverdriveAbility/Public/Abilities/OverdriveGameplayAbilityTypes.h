// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverdriveGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EOverdriveAbilityApplyPhase : uint8
{
	None,
	OnActive,
	OnEnd,
	MAX
};
