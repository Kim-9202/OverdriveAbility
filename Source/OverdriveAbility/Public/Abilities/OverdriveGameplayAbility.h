// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OverdriveGameplayAbility.generated.h"

class UOverdriveAbilityFragment;
class UOverdriveAbilityCooldownPolicy;
class UOverdriveAbilityCostPolicy;

/**
 * 
 */
UCLASS(Abstract)
class OVERDRIVEABILITY_API UOverdriveGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UOverdriveGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// UGameplayAbility Interface begin
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// PreActivate 시 Fragment PreActivate 이전에 호출되는 BP 확장 이벤트.
	UFUNCTION(BlueprintImplementableEvent, Category = "OverdriveAbility", DisplayName = "PreActivate", meta = (ScriptName = "PreActivate"))
	void K2_PreActivate();

	// --------------------------------------
	//	CommitAbility
	// --------------------------------------

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit", DisplayName = "CommitAbilityWithEvent", meta = (ScriptName = "CommitAbilityWithEvent"))
	virtual bool K2_CommitAbilityWithEvent(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit", DisplayName = "CommitAbilityCooldownWithEvent", meta = (ScriptName = "CommitAbilityCooldownWithEvent"))
	virtual bool K2_CommitAbilityCooldownWithEvent(const FGameplayEventData& EventData, bool BroadcastCommitEvent = false, bool ForceCooldown = false);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit", DisplayName = "CommitAbilityCostWithEvent", meta = (ScriptName = "CommitAbilityCostWithEvent"))
	virtual bool K2_CommitAbilityCostWithEvent(const FGameplayEventData& EventData, bool BroadcastCommitEvent = false);
	
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit", DisplayName = "CheckAbilityCooldownWithEvent", meta = (ScriptName = "CheckAbilityCooldownWithEvent"))
	virtual bool K2_CheckAbilityCooldownWithEvent(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit", DisplayName = "CheckAbilityCostWithEvent", meta = (ScriptName = "CheckAbilityCostWithEvent"))
	virtual bool K2_CheckAbilityCostWithEvent(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit")
	void K2_ApplyCooldown();

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit")
	void K2_ApplyCost();

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit")
	void K2_ApplyCooldownWithEvent(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Commit")
	void K2_ApplyCostWithEvent(const FGameplayEventData& EventData);

	virtual bool CommitAbilityWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);
	virtual bool CommitAbilityCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool ForceCooldown, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);
	virtual bool CommitAbilityCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);

	virtual bool CommitCheckWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);

	UFUNCTION(BlueprintImplementableEvent, Category = "OverdriveAbility", DisplayName = "CommitExecuteWithEvent", meta = (ScriptName = "CommitExecuteWithEvent"))
	void K2_CommitExecuteWithEvent(const FGameplayEventData& EventData);

	virtual void CommitExecuteWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData);


	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	virtual void ApplyCooldownWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData) const;
	virtual bool CheckCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData& EventData, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	virtual void ApplyCostWithEvent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData& EventData) const;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// UGameplayAbility Interface end

	virtual void OnEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Fragment", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UOverdriveAbilityFragment>> Fragments;

public:
	UFUNCTION(BlueprintPure, Category = "Fragment", meta = (DeterminesOutputType = "FragmentClass"))
	UOverdriveAbilityFragment* GetAbilityFragmentByClass(TSubclassOf<UOverdriveAbilityFragment> FragmentClass) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	bool bAutoActivateOnGive;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Cooldowns", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOverdriveAbilityCooldownPolicy> CooldownPolicy;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Costs", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOverdriveAbilityCostPolicy> CostPolicy;
};

