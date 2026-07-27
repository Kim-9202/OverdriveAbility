// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/EdAbilityRouterGraphNode_Proxy.h"
#include "EdGraph/EdAbilityRouterGraph.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"

#include "GraphEditor.h"

void UEdAbilityRouterGraphNode_Proxy::OnNodeDoubleClicked(TSharedPtr<SGraphEditor> InGraphEditor)
{
	if (!InGraphEditor.IsValid())
	{
		return;
	}

	UOverdriveAbilityRouterNode_Proxy* ProxyNode = Cast<UOverdriveAbilityRouterNode_Proxy>(RouterNode);
	if (ProxyNode == nullptr)
	{
		return;
	}

	UOverdriveAbilityRouterNode* TargetRouterNode = ProxyNode->GetTargetRouterNode();
	if (TargetRouterNode == nullptr)
	{
		// Target 미설정 → no-op
		return;
	}

	UEdAbilityRouterGraph* EdGraph = Cast<UEdAbilityRouterGraph>(GetGraph());
	if (EdGraph == nullptr)
	{
		return;
	}

	if (TObjectPtr<UEdAbilityRouterGraphNode>* TargetEdNode = EdGraph->RouterNodeMap.Find(TargetRouterNode))
	{
		if (*TargetEdNode)
		{
			// 뷰를 Target 노드 중앙으로 이동 + 선택
			InGraphEditor->JumpToNode(*TargetEdNode);
		}
	}
}
