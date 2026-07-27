// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/Factory_AbilityRouter.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"

UFactory_AbilityRouter::UFactory_AbilityRouter()
{
	SupportedClass = UOverdriveAbilityRouterGraph::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UFactory_AbilityRouter::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(UOverdriveAbilityRouterGraph::StaticClass()));

	UOverdriveAbilityRouterGraph* NewGraph = NewObject<UOverdriveAbilityRouterGraph>(InParent, InClass, InName, Flags | RF_Transactional, Context);

	return NewGraph;
}
