// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFinders/OverdriveAbilitySystemFinder_Controller.h"
#include "AbilitySystemGlobals.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

UAbilitySystemComponent* UOverdriveAbilitySystemFinder_Controller::FindAbilitySystem_Implementation(UActorComponent* InComponent) const
{
	AActor* OwnerActor = InComponent->GetOwner();

	AController* TargetController = Cast<AController>(OwnerActor);

	if (const APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		TargetController = OwnerPawn->GetController();
	}
	else if (const APlayerState* OwnerPlayerState = Cast<APlayerState>(OwnerActor))
	{
		TargetController = OwnerPlayerState->GetOwningController();
	}

	if (!IsValid(TargetController))
	{
		return nullptr;
	}

	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetController);
}
