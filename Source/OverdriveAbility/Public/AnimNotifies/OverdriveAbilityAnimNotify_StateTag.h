// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityAnimNotify_StateTag.generated.h"

class UAnimSequenceBase;
class USkeletalMeshComponent;

/**
 * AbilityRouter의 StateTag를 제거 → 추가 순으로 한 번에 교체하는 AnimNotify.
 * 콤보 창을 다음 단계로 넘길 때처럼 "이전 창을 닫고 새 창을 여는" 용도.
 *
 * 태그를 하나씩 바꾸면 그 사이에 입력 버퍼가 재평가돼 엉뚱한 엣지로 매칭될 수 있으므로
 * 반드시 컴포넌트의 배치 API(UpdateStateTags)를 통해 원자적으로 적용한다.
 */
UCLASS(EditInlineNew, Const, HideCategories = Object, CollapseCategories, Meta = (DisplayName = "Ability Router: Update State Tags"))
class OVERDRIVEABILITY_API UOverdriveAbilityAnimNotify_UpdateStateTag : public UAnimNotify
{
	GENERATED_BODY()

public:

	UOverdriveAbilityAnimNotify_UpdateStateTag(const FObjectInitializer& ObjectInitializer);

	//~ Begin UAnimNotify interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//~ End UAnimNotify interface

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:

	/** 먼저 제거할 상태 태그. 보유하지 않은 태그는 무시된다. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (AllowPrivateAccess = "true", Categories = "OverdriveAbility.AbilityRouter.State"))
	FGameplayTagContainer TagsToRemove;

	/** 제거 뒤에 추가할 상태 태그. 양쪽에 같은 태그가 있으면 최종적으로 남는다. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (AllowPrivateAccess = "true", Categories = "OverdriveAbility.AbilityRouter.State"))
	FGameplayTagContainer TagsToAdd;
};

/**
 * AbilityRouter의 StateTag를 통째로 덮어쓰는 AnimNotify.
 * 빈 컨테이너를 넣으면 현재 열린 창을 전부 닫는 용도로 쓸 수 있다.
 */
UCLASS(EditInlineNew, Const, HideCategories = Object, CollapseCategories, Meta = (DisplayName = "Ability Router: Set State Tags"))
class OVERDRIVEABILITY_API UOverdriveAbilityAnimNotify_SetStateTag : public UAnimNotify
{
	GENERATED_BODY()

public:

	UOverdriveAbilityAnimNotify_SetStateTag(const FObjectInitializer& ObjectInitializer);

	//~ Begin UAnimNotify interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//~ End UAnimNotify interface

private:

	/** 이 노티파이 이후의 상태 태그 전체. 기존 태그는 모두 버려진다. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (AllowPrivateAccess = "true", Categories = "OverdriveAbility.AbilityRouter.State"))
	FGameplayTagContainer NewStateTags;
};
