// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Factory_AbilityRouter.generated.h"

/**
 * 
 */
UCLASS()
class UFactory_AbilityRouter : public UFactory
{
	GENERATED_BODY()
protected:
	UFactory_AbilityRouter();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
