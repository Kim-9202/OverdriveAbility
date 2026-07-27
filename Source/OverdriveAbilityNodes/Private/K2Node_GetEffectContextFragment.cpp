// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetEffectContextFragment.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "OverdriveAbilityBlueprintLibrary.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "GraphEditorSettings.h"
#include "KismetCompiler.h"

namespace K2Node_GetEffectContextFragmentPinNames
{
	static const FName FragmentPinName = "Fragment";
	static const FName FragmentTypePinName = "FragmentType";
	static const FName ContextHandlePinName = "ContextHandle";

	static const FName ValidPinName = "Valid";
	static const FName InvalidPinName = "Invalid";
}

void UK2Node_GetEffectContextFragment::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);
	UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		auto CustomizeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, const FName FunctionName)
			{
				UK2Node_GetEffectContextFragment* Node = CastChecked<UK2Node_GetEffectContextFragment>(NewNode);
				UFunction* Function = UOverdriveAbilityBlueprintLibrary::StaticClass()->FindFunctionByName(FunctionName);
				check(Function);
				Node->SetFromFunction(Function);
			};

		// GetEffectContextFragment()
		UBlueprintNodeSpawner* GetNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(GetNodeSpawner != nullptr);
		GetNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UOverdriveAbilityBlueprintLibrary, GetContextFragment));
		ActionRegistrar.AddBlueprintAction(Action, GetNodeSpawner);
	}
}

void UK2Node_GetEffectContextFragment::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	RefreshPin();
}



void UK2Node_GetEffectContextFragment::PostReconstructNode()
{
	Super::PostReconstructNode();
	
	RefreshPin();
}

void UK2Node_GetEffectContextFragment::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	RefreshPin();
}

void UK2Node_GetEffectContextFragment::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	RefreshPin();
}

void UK2Node_GetEffectContextFragment::RefreshPin()
{
	UEdGraphPin* TypePin = GetFragmentTypePin();
	UEdGraphPin * OutPin = GetFragmentPin();

	// 출력 Fragment 핀의 현재 struct 타입이 FragmentType 입력에서 선택된 타입과
	// 다를 때만 갱신한다. (OutPin->DefaultObject는 출력핀이라 항상 null이므로 오비교였음.)
	if (OutPin->PinType.PinSubCategoryObject != TypePin->DefaultObject)
	{
		if (OutPin->SubPins.Num() > 0)
		{
			GetSchema()->RecombinePin(OutPin);
		}

		OutPin->PinType.PinSubCategoryObject = TypePin->DefaultObject;
		OutPin->PinType.PinCategory = (TypePin->DefaultObject == nullptr) ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;
	}
}

UEdGraphPin* UK2Node_GetEffectContextFragment::GetFragmentTypePin() const
{
	UEdGraphPin* Pin = FindPinChecked(K2Node_GetEffectContextFragmentPinNames::FragmentTypePinName);
	check(Pin)
	return Pin;
}

UEdGraphPin* UK2Node_GetEffectContextFragment::GetFragmentPin() const
{
	UEdGraphPin* Pin = FindPinChecked(K2Node_GetEffectContextFragmentPinNames::FragmentPinName);
	check(Pin)
	return Pin;
}

UEdGraphPin* UK2Node_GetEffectContextFragment::GetContextHandlePin() const
{
	UEdGraphPin* Pin = FindPinChecked(K2Node_GetEffectContextFragmentPinNames::ContextHandlePinName);
	check(Pin)
	return Pin;
}

UEdGraphPin* UK2Node_GetEffectContextFragment::GetValidPin() const
{
	UEdGraphPin* Pin = FindPinChecked(K2Node_GetEffectContextFragmentPinNames::ValidPinName);
	check(Pin)
	return Pin;
}

UEdGraphPin* UK2Node_GetEffectContextFragment::GetInvalidPin() const
{
	UEdGraphPin* Pin = FindPinChecked(K2Node_GetEffectContextFragmentPinNames::InvalidPinName);
	check(Pin)
	return Pin;
}

