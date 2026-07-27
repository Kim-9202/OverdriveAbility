// Fill out your copyright notice in the Description page of Project Settings.


#include "OverdriveAbilityGraphPinFactory.h"
#include "SGraphPin_OverdriveAbilityNodes.h"
#include "K2Node_GetEffectContextFragment.h"

TSharedPtr<class SGraphPin> FOverdriveAbilityGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin && InPin->GetName() == TEXT("FragmentType") && Cast<UK2Node_GetEffectContextFragment>(InPin->GetOwningNode()))
	{
		return SNew(SGraphPin_ContextFragmentTypePin, InPin);
	}
	
	return nullptr;
}

