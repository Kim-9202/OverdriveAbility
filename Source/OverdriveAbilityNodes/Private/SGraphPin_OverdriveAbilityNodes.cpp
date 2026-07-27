// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphPin_OverdriveAbilityNodes.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "UObject/ObjectMacros.h"
#include "K2Node_GetEffectContextFragment.h"

void SGraphPin_ContextFragmentTypePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

    // Fill filtered struct list
    for (TObjectIterator<UScriptStruct> It; It; ++It)
    {
        UScriptStruct* Struct = *It;
        if (Struct &&
            Struct->IsChildOf(FOverdriveEffectContextFragment::StaticStruct()) &&
            Struct != FOverdriveEffectContextFragment::StaticStruct())
        {
            FilteredStructs.Add(Struct);
        }
    }

    SelectedStruct = Cast<UScriptStruct>(InPin->DefaultObject);

    ChildSlot
        [
            GetDefaultValueWidget()  // 내부에서 FilteredStructs 사용
        ];
}

TSharedRef<SWidget> SGraphPin_ContextFragmentTypePin::GetDefaultValueWidget()
{
	return 
		SNew(SComboBox<TWeakObjectPtr<UScriptStruct>>)
		.OptionsSource(&FilteredStructs)
        .OnGenerateWidget_Lambda([](TWeakObjectPtr<UScriptStruct> InItem)
            {
                return SNew(STextBlock).Text(InItem->GetDisplayNameText());
            })
        .OnSelectionChanged_Lambda([this](TWeakObjectPtr<UScriptStruct> SelectedItem, ESelectInfo::Type)
            {
                SelectedStruct = SelectedItem;

                this->GetPinObj()->DefaultObject = SelectedItem.Get();
                StaticCast<UK2Node_GetEffectContextFragment*>(this->GetPinObj()->GetOwningNode())->RefreshPin();
            })
        .Content()
        [
            SNew(STextBlock).Text_Lambda([this]() -> FText
                {
                    return SelectedStruct.IsValid()
                        ? SelectedStruct->GetDisplayNameText()
                        : FText::FromString(TEXT("Select Struct"));
                })
        ];
}
