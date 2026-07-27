// Fill out your copyright notice in the Description page of Project Settings.


#include "OverdriveAbilityBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectAggregator.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/EnumProperty.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OverdriveAbilityBlueprintLibrary)

#define LOCTEXT_NAMESPACE "UOverdriveAbilityBlueprintLibrary"


void UOverdriveAbilityBlueprintLibrary::AddContextFragment(const FGameplayEffectContextHandle& Handle, const int32& InFragment)
{
	checkNoEntry();

}

void UOverdriveAbilityBlueprintLibrary::AddContextFragments(const FGameplayEffectContextHandle& Handle, const TArray<TInstancedStruct<FOverdriveEffectContextFragment>>& InFragments)
{
	if (Handle.IsValid())
	{
		FGameplayEffectContext* Context = const_cast<FGameplayEffectContext*>(Handle.Get());
		StaticCast<FOverdriveGameplayEffectContext*>(Context)->AddFragments(InFragments);
	}
}

void UOverdriveAbilityBlueprintLibrary::GetContextFragment(EOverdriveStructUtilsResult& ExecResult, const FGameplayEffectContextHandle& Handle, UScriptStruct* FragmentType, int32& Value)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UOverdriveAbilityBlueprintLibrary::execAddContextFragment)
{
	P_GET_STRUCT_REF(FGameplayEffectContextHandle, Handle);

	// Read wildcard Value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	const void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("Fragment_SetInvalidValueWarning", "Failed to resolve the Value for Set Fragment Struct Value")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		return;
	}
	if (!Handle.IsValid())
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("Fragment_HandleIsNotValid", "Effect Context Handle Is Invalid")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		return;
	}

	P_NATIVE_BEGIN;
	FOverdriveGameplayEffectContext* ContextObj = StaticCast<FOverdriveGameplayEffectContext*>(const_cast<FGameplayEffectContext*>(Handle.Get()));
	TInstancedStruct<FOverdriveEffectContextFragment> InstancedStruct;
	InstancedStruct.InitializeAsScriptStruct(ValueProp->Struct, (const uint8*)ValuePtr);

	if (InstancedStruct.IsValid())
	{
		ContextObj->AddFragment(MoveTemp(InstancedStruct));
	}
	P_NATIVE_END;
}

DEFINE_FUNCTION(UOverdriveAbilityBlueprintLibrary::execGetContextFragment)
{
	P_GET_ENUM_REF(EOverdriveStructUtilsResult, ExecResult);
	P_GET_STRUCT_REF(FGameplayEffectContextHandle, Handle);
	P_GET_OBJECTPTR_REF(UScriptStruct, FragmentType);

	// Read wildcard Value input.
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	ExecResult = EOverdriveStructUtilsResult::NotValid;

	if (!ValueProp || !ValuePtr || !Handle.IsValid() || FragmentType == nullptr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("Fragment_GetInvalidValueWarning", "Failed to resolve the Value for Get Instanced Struct Value")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		return;
	}

	P_NATIVE_BEGIN
	FGameplayEffectContext* ContextObj = const_cast<FGameplayEffectContext*>(Handle.Get());
	FOverdriveEffectContextFragment* StructMemory = StaticCast<FOverdriveGameplayEffectContext*>(ContextObj)->GetFragmentMemory(FragmentType);
	if(StructMemory)
	{
		ValueProp->Struct->CopyScriptStruct(ValuePtr, StructMemory);
		ExecResult = EOverdriveStructUtilsResult::Valid;
	}
	else
	{
		ExecResult = EOverdriveStructUtilsResult::NotValid;
	}
	P_NATIVE_END
}

bool UOverdriveAbilityBlueprintLibrary::EvaluateAttributeValueIgnoringEffects(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags, const TArray<FActiveGameplayEffectHandle>& EffectsToIgnore, float& OutValue)
{
	OutValue = 0.f;

	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
	{
		return false;
	}

	FGameplayEffectAttributeCaptureDefinition Capture(Attribute, EGameplayEffectAttributeCaptureSource::Source, true);

	FGameplayEffectAttributeCaptureSpec CaptureSpec(Capture);
	AbilitySystemComponent->CaptureAttributeForGameplayEffect(CaptureSpec);

	FAggregatorEvaluateParameters EvalParams;

	EvalParams.SourceTags = &SourceTags;
	EvalParams.TargetTags = &TargetTags;
	EvalParams.IgnoreHandles = EffectsToIgnore;

	return CaptureSpec.AttemptCalculateAttributeMagnitude(EvalParams, OutValue);
}

bool UOverdriveAbilityBlueprintLibrary::EvaluateAttributeValueIgnoringEffectsByQuery(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags, const FGameplayEffectQuery& IgnoreQuery, float& OutValue)
{
	OutValue = 0.f;

	if (!AbilitySystemComponent)
	{
		return false;
	}

	const TArray<FActiveGameplayEffectHandle> EffectsToIgnore = AbilitySystemComponent->GetActiveEffects(IgnoreQuery);

	return EvaluateAttributeValueIgnoringEffects(AbilitySystemComponent, Attribute, SourceTags, TargetTags, EffectsToIgnore, OutValue);
}

bool UOverdriveAbilityBlueprintLibrary::GetFilteredAttributeValue(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagRequirements& SourceTags, const FGameplayTagContainer& TargetTags, const TArray<FActiveGameplayEffectHandle>& HandlesToIgnore, float& OutValue)
{
	OutValue = 0.f;

	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
	{
		return false;
	}

	OutValue = AbilitySystemComponent->GetFilteredAttributeValue(Attribute, SourceTags, TargetTags, HandlesToIgnore);
	return true;
}

bool UOverdriveAbilityBlueprintLibrary::GetFilteredAttributeValueByQuery(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, const FGameplayTagRequirements& SourceTags, const FGameplayTagContainer& TargetTags, const FGameplayEffectQuery& IgnoreQuery, float& OutValue)
{
	OutValue = 0.f;

	if (!AbilitySystemComponent)
	{
		return false;
	}

	const TArray<FActiveGameplayEffectHandle> HandlesToIgnore = AbilitySystemComponent->GetActiveEffects(IgnoreQuery);

	return GetFilteredAttributeValue(AbilitySystemComponent, Attribute, SourceTags, TargetTags, HandlesToIgnore, OutValue);
}

#undef LOCTEXT_NAMESPACE

