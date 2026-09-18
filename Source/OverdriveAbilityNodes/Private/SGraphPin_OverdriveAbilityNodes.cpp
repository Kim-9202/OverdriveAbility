// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphPin_OverdriveAbilityNodes.h"
#include "Effects/OverdriveGameplayEffectTypes.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphSchema.h"
#include "ScopedTransaction.h"

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
                UEdGraphPin* PinObj = GetPinObj();
                if (PinObj == nullptr)
                {
                    return;
                }

                SelectedStruct = SelectedItem;

                // TrySetDefaultObject가 PinDefaultValueChanged를 불러 RefreshPin까지 이어진다.
                const FScopedTransaction Transaction(NSLOCTEXT("OverdriveAbilityNodes", "ChangeFragmentTypePinValue", "Change Fragment Type Pin Value"));
                PinObj->Modify();
                PinObj->GetSchema()->TrySetDefaultObject(*PinObj, SelectedItem.Get());
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
