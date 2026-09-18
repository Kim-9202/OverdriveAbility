// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_AddContextFragment.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITYNODES_API UK2Node_AddContextFragment : public UK2Node_CallFunction
{
	GENERATED_BODY()
	

protected:
	//~ Begin UEdGraphNode Interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	//~ End UEdGraphNode Interface

	//~ UK2Node interface
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	//~ End UK2Node interface

	//~ UK2Node_CallFunction interface
	virtual bool CanToggleNodePurity() const override { return false; }
	//~ End UK2Node_CallFunction interface

};
