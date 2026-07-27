// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_AddContextFragment.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "OverdriveAbilityBlueprintLibrary.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "GraphEditorSettings.h"
#include "KismetCompiler.h"
#include "Effects/OverdriveGameplayEffectTypes.h"

void UK2Node_AddContextFragment::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);
	UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		auto CustomizeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, const FName FunctionName)
			{
				UK2Node_AddContextFragment* Node = CastChecked<UK2Node_AddContextFragment>(NewNode);
				UFunction* Function = UOverdriveAbilityBlueprintLibrary::StaticClass()->FindFunctionByName(FunctionName);
				check(Function);
				Node->SetFromFunction(Function);
			};

		// AddEffectContextFragment()
		UBlueprintNodeSpawner* GetNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(GetNodeSpawner != nullptr);
		GetNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UOverdriveAbilityBlueprintLibrary, AddContextFragment));
		ActionRegistrar.AddBlueprintAction(Action, GetNodeSpawner);
	}
}

bool UK2Node_AddContextFragment::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	const UEdGraphPin* InFragmentPin = FindPinChecked(FName(TEXT("InFragment")));

	if (MyPin == InFragmentPin)
	{
		if (OtherPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct || !Cast<UStruct>(OtherPin->PinType.PinSubCategoryObject)->IsChildOf(FOverdriveEffectContextFragment::StaticStruct()))
		{
			OutReason = TEXT("InFragmentPin must be Child Of FOverdriveEffectContextFragment.");
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
	}
}

