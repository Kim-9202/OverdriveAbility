// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

/**
 * 
 */
struct OVERDRIVEABILITYNODES_API FOverdriveAbilityGraphPinFactory : public FGraphPanelPinFactory
{
public:
	FOverdriveAbilityGraphPinFactory()
	{ }
	~FOverdriveAbilityGraphPinFactory()
	{ }

protected:
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};

