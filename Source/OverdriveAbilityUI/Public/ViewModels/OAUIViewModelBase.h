// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "OAUIViewModelBase.generated.h"

/**
 * OverdriveAbilityUI 모듈의 모든 ViewModel이 상속하는 루트 베이스.
 * OverdriveGrimoireUI 관례(UMVVMViewModelBase 파생 빈 베이스)를 따른다.
 */
UCLASS()
class OVERDRIVEABILITYUI_API UOAUIViewModelBase : public UMVVMViewModelBase
{
	GENERATED_BODY()
};
