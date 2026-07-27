// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace OverdriveAbilityGameplayTags
{
	// AbilityRouter 카테고리 루트 태그. UPARAM/UPROPERTY meta=(Categories=...) 필터가
	// 이 서브트리만 노출하도록 NativeTag로 선언한다. 실제 leaf 태그는 에디터/ini에서 추가.
	OVERDRIVEABILITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityRouter_InputType);

	// AbilityRouter 상태 태그 루트. 엣지 ConditionQuery 매칭 및 StateTag Add/Remove API의
	// meta=(Categories=...) 필터가 이 서브트리만 노출하도록 한다.
	OVERDRIVEABILITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityRouter_State);
}
