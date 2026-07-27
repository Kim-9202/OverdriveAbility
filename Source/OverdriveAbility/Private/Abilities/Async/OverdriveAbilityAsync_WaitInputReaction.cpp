// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/OverdriveAbilityAsync_WaitInputReaction.h"
#include "Components/OverdriveAbilityRouterComponent.h"
#include "UObject/Package.h"

UOverdriveAbilityAsync_WaitInputReaction* UOverdriveAbilityAsync_WaitInputReaction::WaitInputReactionOnActor(AActor* TargetActor,
	bool bPressed, FGameplayTag InputTypeTag, bool bTriggerOnce, bool bBroadcastIfAlreadyInState)
{
	UPackage* TransientPackage = GetTransientPackage();
	FName UniqueActionName = NAME_None;

	if (TargetActor != nullptr)
	{
		const UClass* ThisClass = UOverdriveAbilityAsync_WaitInputReaction::StaticClass();
		const FName ActionNameConcat = FName(*FString::Printf(TEXT("%s_%s"), *TargetActor->GetName(), *ThisClass->GetName()));
		UniqueActionName = ::MakeUniqueObjectName(TransientPackage, ThisClass, ActionNameConcat);
	}

	UOverdriveAbilityAsync_WaitInputReaction* MyObj = NewObject<UOverdriveAbilityAsync_WaitInputReaction>(TransientPackage, UniqueActionName);
	MyObj->SetAbilityActor(TargetActor);
	MyObj->TargetActor = TargetActor;
	MyObj->bWaitPressed = bPressed;
	MyObj->InputTypeTag = InputTypeTag;
	MyObj->bTriggerOnce = bTriggerOnce;
	MyObj->bBroadcastIfAlreadyInState = bBroadcastIfAlreadyInState;

	return MyObj;
}

void UOverdriveAbilityAsync_WaitInputReaction::Activate()
{
	Super::Activate();

	RouterComponent = TargetActor.IsValid() ? TargetActor->FindComponentByClass<UOverdriveAbilityRouterComponent>() : nullptr;

	if (!RouterComponent.IsValid())
	{
		EndAction();
		return;
	}

	ReactionHandle = RouterComponent->GetInputReactionDelegate(bWaitPressed, InputTypeTag)
		.AddUObject(this, &UOverdriveAbilityAsync_WaitInputReaction::HandleInputReaction);

	// 이미 대기 중인 상태(Press/Release)와 일치하면 즉시 발화.
	if (bBroadcastIfAlreadyInState && RouterComponent->IsInputPressed(InputTypeTag) == bWaitPressed)
	{
		HandleInputReaction(bWaitPressed, InputTypeTag);
	}
}

void UOverdriveAbilityAsync_WaitInputReaction::HandleInputReaction(bool bInPressed, const FGameplayTag& InInputTypeTag)
{
	if (ShouldBroadcastDelegates())
	{
		OnInputReaction.Broadcast(bInPressed, InInputTypeTag);
	}

	if (bTriggerOnce)
	{
		EndAction();
	}
}

void UOverdriveAbilityAsync_WaitInputReaction::EndAction()
{
	if (RouterComponent.IsValid() && ReactionHandle.IsValid())
	{
		RouterComponent->GetInputReactionDelegate(bWaitPressed, InputTypeTag).Remove(ReactionHandle);
		ReactionHandle.Reset();
	}
	RouterComponent.Reset();

	Super::EndAction();
}
