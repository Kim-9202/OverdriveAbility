// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "OverdriveAbilityTask_WaitInputReaction.generated.h"

class UOverdriveAbilityRouterComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOverdriveAbilityTaskInputReactionDelegate, bool, bPressed, FGameplayTag, InputTypeTag);

/**
 * 라우터 컴포넌트의 (bPressed, InputType) 입력 반응을 어빌리티 실행 중 대기/구독하는 AbilityTask.
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityTask_WaitInputReaction : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOverdriveAbilityTaskInputReactionDelegate OnInputReaction;

	// OptionalExternalTarget 미지정 시 어빌리티 아바타 액터에서 라우터 컴포넌트를 찾는다.
	// bBroadcastIfAlreadyInState: 활성화 시점에 이미 그 상태(Press/Release)면 즉시 OnInputReaction을 발화.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "InputTypeTag"))
	static UOverdriveAbilityTask_WaitInputReaction* WaitInputReaction(UGameplayAbility* OwningAbility,
		bool bPressed,
		UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) FGameplayTag InputTypeTag,
		AActor* OptionalExternalTarget = nullptr, bool bTriggerOnce = false, bool bBroadcastIfAlreadyInState = false, FName TaskInstanceName = NAME_None);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void HandleInputReaction(bool bInPressed, const FGameplayTag& InInputTypeTag);

	bool bWaitPressed = false;
	FGameplayTag InputTypeTag;
	bool bTriggerOnce = false;
	bool bBroadcastIfAlreadyInState = false;

	TWeakObjectPtr<AActor> ExternalTarget;
	TWeakObjectPtr<UOverdriveAbilityRouterComponent> RouterComponent;
	FDelegateHandle ReactionHandle;
};
