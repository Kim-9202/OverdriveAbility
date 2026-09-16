// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/OverdriveGameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityCostPolicy.generated.h"

class UOverdriveGameplayAbility;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;
struct FGameplayTagContainer;
struct FGameplayEventData;

/**
 * 
 */
UCLASS(Abstract, DefaultToInstanced, Blueprintable, BlueprintType, EditInlineNew, Transient, CollapseCategories)
class OVERDRIVEABILITY_API UOverdriveAbilityCostPolicy : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* EventData = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const { return false; }
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* EventData = nullptr) const {}

	UOverdriveGameplayAbility* GetOwnerAbility() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase", meta = (AllowPrivateAccess = "true"))
	EOverdriveAbilityApplyPhase ApplyPhase;

public:
	EOverdriveAbilityApplyPhase GetApplyPhase() const {return ApplyPhase;}
};

