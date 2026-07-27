// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/OverdriveGameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "OverdriveAbilityCooldownPolicy.generated.h"

class UOverdriveGameplayAbility;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;
struct FGameplayTagContainer;
struct FGameplayEventData;

/**
 * 
 */
UCLASS(Abstract, DefaultToInstanced, NotBlueprintable, BlueprintType, EditInlineNew, Transient)
class OVERDRIVEABILITY_API UOverdriveAbilityCooldownPolicy : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const {return false;}
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData = nullptr) const {}

	UOverdriveGameplayAbility* GetOwnerAbility() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Phase", meta = (AllowPrivateAccess = "true"))
	EOverdriveAbilityApplyPhase ApplyPhase;

public:
	EOverdriveAbilityApplyPhase GetApplyPhase() const { return ApplyPhase; }

protected:
	mutable FActiveGameplayEffectHandle CooldownEffectHandle;
};

