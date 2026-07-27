// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/NodeFactory_AbilityRouter.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "EdGraph/EdAbilityRouterGraphEdge.h"
#include "EdGraph/SEdAbilityRouterGraphNode.h"
#include "EdGraph/SEdAbilityRouterGraphEdge.h"

TSharedPtr<SGraphNode> FNodeFactory_AbilityRouter::CreateNode(UEdGraphNode* Node) const
{
	if (UEdAbilityRouterGraphNode* RouterNode = Cast<UEdAbilityRouterGraphNode>(Node))
	{
		return SNew(SEdAbilityRouterGraphNode, RouterNode);
	}
	else if (UEdAbilityRouterGraphEdge* RouterEdge = Cast<UEdAbilityRouterGraphEdge>(Node))
	{
		return SNew(SEdAbilityRouterGraphEdge, RouterEdge);
	}

	return nullptr;
}
