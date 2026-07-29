// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/OverdriveAbilityAnimNotify_StateTag.h"
#include "Components/OverdriveAbilityRouterComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "OverdriveAbilityBlueprintLibrary.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(OverdriveAbilityAnimNotify_StateTag)

#define LOCTEXT_NAMESPACE "OverdriveAbilityAnimNotify_StateTag"

UOverdriveAbilityAnimNotify_UpdateStateTag::UOverdriveAbilityAnimNotify_UpdateStateTag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(99, 192, 255, 255);
#endif
}

void UOverdriveAbilityAnimNotify_UpdateStateTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// 엔진 노티파이 관례대로 Super는 부르지 않는다(블루프린트 Received_Notify 재호출 방지).
	if (MeshComp == nullptr)
	{
		return;
	}

	// 프리뷰 액터나 라우터를 갖지 않은 액터에서는 null이 정상이므로 조용히 무시한다.
	UOverdriveAbilityRouterComponent* RouterComponent = UOverdriveAbilityBlueprintLibrary::GetAbilityRouterComponent(MeshComp->GetOwner());
	if (RouterComponent == nullptr)
	{
		return;
	}

	RouterComponent->UpdateStateTags(TagsToRemove, TagsToAdd);
}

#if WITH_EDITOR
EDataValidationResult UOverdriveAbilityAnimNotify_UpdateStateTag::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// 설정 실수일 뿐 에셋이 깨진 것은 아니므로 Warning만 내고 Result는 유효한 채로 둔다.
	if (TagsToRemove.IsEmpty() && TagsToAdd.IsEmpty())
	{
		Context.AddWarning(LOCTEXT("UpdateStateTag_NoTags", "제거·추가 태그가 모두 비어 있어 이 노티파이는 아무 것도 하지 않습니다."));
	}

	return Result;
}
#endif

UOverdriveAbilityAnimNotify_SetStateTag::UOverdriveAbilityAnimNotify_SetStateTag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(99, 255, 192, 255);
#endif
}

void UOverdriveAbilityAnimNotify_SetStateTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// 엔진 노티파이 관례대로 Super는 부르지 않는다(블루프린트 Received_Notify 재호출 방지).
	if (MeshComp == nullptr)
	{
		return;
	}

	// 프리뷰 액터나 라우터를 갖지 않은 액터에서는 null이 정상이므로 조용히 무시한다.
	UOverdriveAbilityRouterComponent* RouterComponent = UOverdriveAbilityBlueprintLibrary::GetAbilityRouterComponent(MeshComp->GetOwner());
	if (RouterComponent == nullptr)
	{
		return;
	}

	RouterComponent->SetStateTags(NewStateTags);
}

#undef LOCTEXT_NAMESPACE
