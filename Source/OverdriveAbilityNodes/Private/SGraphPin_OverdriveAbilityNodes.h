// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

/**
 * 
 */
class OVERDRIVEABILITYNODES_API SGraphPin_ContextFragmentTypePin : public SGraphPin
{
public:
public:
    SLATE_BEGIN_ARGS(SGraphPin_ContextFragmentTypePin) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

    TArray<TWeakObjectPtr<UScriptStruct>> FilteredStructs;

    TWeakObjectPtr<UScriptStruct> SelectedStruct;
};