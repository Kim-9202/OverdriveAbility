// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFinders/OverdriveAbilitySystemFinder.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UOverdriveAbilitySystemFinder::StartFind()
{
	if (RetryTimerHandle.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		FinishFind(nullptr);
		return;
	}

	World->GetTimerManager().SetTimer(RetryTimerHandle, this, &UOverdriveAbilitySystemFinder::TryFind, RetryPeriod, true);

	TryFind();
}

void UOverdriveAbilitySystemFinder::StopFind()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RetryTimerHandle);
	}

	RetryTimerHandle.Invalidate();
	CurrentAttemptCount = 0;
}

UAbilitySystemComponent* UOverdriveAbilitySystemFinder::FindAbilitySystem_Implementation(UActorComponent* InComponent) const
{
	return nullptr;
}

void UOverdriveAbilitySystemFinder::TryFind()
{
	UActorComponent* OwnerComponent = WeakOwnerComponent.Get();

	if (!IsValid(OwnerComponent))
	{
		FinishFind(nullptr);
		return;
	}

	++CurrentAttemptCount;

	if (UAbilitySystemComponent* FoundAbilitySystem = FindAbilitySystem(OwnerComponent))
	{
		FinishFind(FoundAbilitySystem);
		return;
	}

	if (CurrentAttemptCount >= MaxAttemptCount)
	{
		FinishFind(nullptr);
	}
}

void UOverdriveAbilitySystemFinder::FinishFind(UAbilitySystemComponent* FoundAbilitySystem)
{
	StopFind();

	if (IsValid(FoundAbilitySystem))
	{
		OnFound.ExecuteIfBound(FoundAbilitySystem);
	}
	else
	{
		OnFailed.ExecuteIfBound();
	}
}
