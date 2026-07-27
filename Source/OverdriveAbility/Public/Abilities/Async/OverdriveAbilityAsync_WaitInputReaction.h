// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/Async/AbilityAsync.h"
#include "OverdriveAbilityAsync_WaitInputReaction.generated.h"

class UOverdriveAbilityRouterComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOverdriveAbilityAsyncInputReactionDelegate, bool, bPressed, FGameplayTag, InputTypeTag);

/**
 * 라우터 컴포넌트의 (bPressed, InputType) 입력 반응을 임의 블루프린트에서 대기/구독하는 AbilityAsync.
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityAsync_WaitInputReaction : public UAbilityAsync
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOverdriveAbilityAsyncInputReactionDelegate OnInputReaction;

	// bBroadcastIfAlreadyInState: 활성화 시점에 이미 그 상태(Press/Release)면 즉시 OnInputReaction을 발화.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Async",
		meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "InputTypeTag"))
	static UOverdriveAbilityAsync_WaitInputReaction* WaitInputReactionOnActor(AActor* TargetActor,
		bool bPressed,
		UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) FGameplayTag InputTypeTag,
		bool bTriggerOnce = false, bool bBroadcastIfAlreadyInState = false);

protected:
	virtual void Activate() override;
	virtual void EndAction() override;

private:
	void HandleInputReaction(bool bInPressed, const FGameplayTag& InInputTypeTag);

	bool bWaitPressed = false;
	FGameplayTag InputTypeTag;
	bool bTriggerOnce = false;
	bool bBroadcastIfAlreadyInState = false;

	TWeakObjectPtr<AActor> TargetActor;
	TWeakObjectPtr<UOverdriveAbilityRouterComponent> RouterComponent;
	FDelegateHandle ReactionHandle;
};
