// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "EdAbilityRouterGraphNode_Proxy.generated.h"

class SGraphEditor;

/**
 * Proxy 런타임 노드를 래핑하는 EdGraph 노드.
 * 더블클릭 시 Proxy가 가리키는 TargetRouterNode의 EdNode로 그래프 뷰를 이동시킨다.
 */
UCLASS()
class UEdAbilityRouterGraphNode_Proxy : public UEdAbilityRouterGraphNode
{
	GENERATED_BODY()

public:
	virtual void OnNodeDoubleClicked(TSharedPtr<SGraphEditor> InGraphEditor) override;
};
