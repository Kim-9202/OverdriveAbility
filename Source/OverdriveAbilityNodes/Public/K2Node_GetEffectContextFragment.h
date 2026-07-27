// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_GetEffectContextFragment.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITYNODES_API UK2Node_GetEffectContextFragment : public UK2Node_CallFunction
{
	GENERATED_BODY()
	
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

protected:
	//~ UK2Node interface
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UK2Node interface

	//~ UEdGraphNode interface
	virtual void PostReconstructNode() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* ChangedPin) override;
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode interface

	//~ UK2Node_CallFunction interface
	virtual bool CanToggleNodePurity() const override { return false; }
	//~ End UK2Node_CallFunction interface

public:
	void RefreshPin();

	UEdGraphPin* GetFragmentTypePin() const;
	UEdGraphPin* GetFragmentPin() const;
	UEdGraphPin* GetContextHandlePin() const;

	UEdGraphPin* GetValidPin() const;
	UEdGraphPin* GetInvalidPin() const;
};
