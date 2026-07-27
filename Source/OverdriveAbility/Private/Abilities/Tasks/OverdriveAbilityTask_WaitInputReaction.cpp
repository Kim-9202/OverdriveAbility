// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/OverdriveAbilityTask_WaitInputReaction.h"
#include "Components/OverdriveAbilityRouterComponent.h"
#include "AbilitySystemComponent.h"

UOverdriveAbilityTask_WaitInputReaction* UOverdriveAbilityTask_WaitInputReaction::WaitInputReaction(UGameplayAbility* OwningAbility,
	bool bPressed, FGameplayTag InputTypeTag, AActor* OptionalExternalTarget, bool bTriggerOnce, bool bBroadcastIfAlreadyInState, FName TaskInstanceName)
{
	UOverdriveAbilityTask_WaitInputReaction* MyObj = NewAbilityTask<UOverdriveAbilityTask_WaitInputReaction>(OwningAbility, TaskInstanceName);
	MyObj->bWaitPressed = bPressed;
	MyObj->InputTypeTag = InputTypeTag;
	MyObj->ExternalTarget = OptionalExternalTarget;
	MyObj->bTriggerOnce = bTriggerOnce;
	MyObj->bBroadcastIfAlreadyInState = bBroadcastIfAlreadyInState;

	return MyObj;
}

void UOverdriveAbilityTask_WaitInputReaction::Activate()
{
	AActor* TargetActor = ExternalTarget.IsValid() ? ExternalTarget.Get()
		: (AbilitySystemComponent.IsValid() ? AbilitySystemComponent->GetAvatarActor() : nullptr);

	RouterComponent = TargetActor ? TargetActor->FindComponentByClass<UOverdriveAbilityRouterComponent>() : nullptr;

	if (!RouterComponent.IsValid())
	{
		EndTask();
		return;
	}

	ReactionHandle = RouterComponent->GetInputReactionDelegate(bWaitPressed, InputTypeTag)
		.AddUObject(this, &UOverdriveAbilityTask_WaitInputReaction::HandleInputReaction);

	// 이미 대기 중인 상태(Press/Release)와 일치하면 즉시 발화.
	if (bBroadcastIfAlreadyInState && RouterComponent->IsInputPressed(InputTypeTag) == bWaitPressed)
	{
		HandleInputReaction(bWaitPressed, InputTypeTag);
	}
}

void UOverdriveAbilityTask_WaitInputReaction::HandleInputReaction(bool bInPressed, const FGameplayTag& InInputTypeTag)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnInputReaction.Broadcast(bInPressed, InInputTypeTag);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UOverdriveAbilityTask_WaitInputReaction::OnDestroy(bool bInOwnerFinished)
{
	if (RouterComponent.IsValid() && ReactionHandle.IsValid())
	{
		RouterComponent->GetInputReactionDelegate(bWaitPressed, InputTypeTag).Remove(ReactionHandle);
		ReactionHandle.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}
