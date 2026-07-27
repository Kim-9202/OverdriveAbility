// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityRouterGraph.generated.h"

class UOverdriveAbilityRouterNode;

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterGraph : public UObject
{
	GENERATED_BODY()
public:
	UOverdriveAbilityRouterGraph(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:	
	UPROPERTY()
	TArray<TObjectPtr<UOverdriveAbilityRouterNode>> AllRouterNodes;

	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterNode> RootRouterNode;

	// 전역 폴백 진입점. 로컬 순회가 실패했을 때 라우터가 여기서 한 번 더 찾는다.
	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterNode> GlobalSecondChanceRouterNode;

	const UOverdriveAbilityRouterNode* GetRootRouterNode() const { return RootRouterNode; }

	const UOverdriveAbilityRouterNode* GetGlobalSecondChanceRouterNode() const { return GlobalSecondChanceRouterNode; }

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UEdGraph> EdGraph;

#endif
};

