// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/EdAbilityRouterGraph.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "EdGraph/EdAbilityRouterGraphEdge.h"

#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"

#include "OverdriveAbilityEditorModule.h"

UEdAbilityRouterGraph::UEdAbilityRouterGraph()
{

}

UEdAbilityRouterGraph::~UEdAbilityRouterGraph()
{
}

void UEdAbilityRouterGraph::RebuildAbilityRouterGraph()
{
	UE_LOG(LogOverdriveAbilityEditor, Verbose, TEXT("%hs::Rebuild Ability Router Graph"), __FUNCTION__);

	UOverdriveAbilityRouterGraph* RouterGraph = GetAbilityRouterGraph();

	ClearGraph();

	for (auto& Node : Nodes)
	{
		if (UEdAbilityRouterGraphNode* EdRouterNode = Cast<UEdAbilityRouterGraphNode>(Node))
		{
			UOverdriveAbilityRouterNode* RouterNode = EdRouterNode->RouterNode;
			if (RouterNode == nullptr)
			{
				continue;
			}

			RouterNodeMap.Add(RouterNode, EdRouterNode);
			RouterGraph->AllRouterNodes.Add(RouterNode);

			// Root와 GlobalSecondChance는 형제 클래스라 두 Cast가 배타적이다(검사 순서 무관).
			if (UOverdriveAbilityRouterNode_Root* RouterRootNode = Cast<UOverdriveAbilityRouterNode_Root>(RouterNode))
			{
				RouterGraph->RootRouterNode = RouterRootNode;
			}
			else if (UOverdriveAbilityRouterNode_GlobalSecondChance* GlobalSecondChanceNode = Cast<UOverdriveAbilityRouterNode_GlobalSecondChance>(RouterNode))
			{
				RouterGraph->GlobalSecondChanceRouterNode = GlobalSecondChanceNode;
			}

			RouterNode->RouterGraph = RouterGraph;

			// 이미 그래프 소유면 건드리지 않는다. Rebuild는 저장 직전(PreSave)에도 도는데,
			// 그 시점의 불필요한 Rename은 피하는 편이 안전하다.
			if (RouterNode->GetOuter() != RouterGraph)
			{
				RouterNode->Rename(nullptr, RouterGraph, REN_DontCreateRedirectors | REN_DoNotDirty);
			}

			UEdGraphPin* OutputPin = EdRouterNode->GetOutputPin();

			for (auto& LinkedToPin : OutputPin->LinkedTo)
			{
				UOverdriveAbilityRouterNode* ChildNode = nullptr;
				if (UEdAbilityRouterGraphNode* EdChildNode = Cast<UEdAbilityRouterGraphNode>(LinkedToPin->GetOwningNode()))
				{
					ChildNode = EdChildNode->RouterNode;
				}
				else if (UEdAbilityRouterGraphEdge* EdChildEdge = Cast<UEdAbilityRouterGraphEdge>(LinkedToPin->GetOwningNode()))
				{
					if (UEdAbilityRouterGraphNode* EdEdgeEndNode = EdChildEdge->GetEndNode())
					{
						ChildNode = EdEdgeEndNode->RouterNode;
					}
				}

				if (ensure(ChildNode))
				{
					RouterNode->ChildrenNodes.Add(ChildNode);

					ChildNode->ParentNodes.Add(RouterNode);
				}
			}
		}
		else if (UEdAbilityRouterGraphEdge* EdRouterEdge = Cast<UEdAbilityRouterGraphEdge>(Node))
		{
			UEdAbilityRouterGraphNode* StartNode = EdRouterEdge->GetStartNode();
			UEdAbilityRouterGraphNode* EndNode = EdRouterEdge->GetEndNode();
			UOverdriveAbilityRouterEdge* RouterEdge = EdRouterEdge->RouterEdge;

			if (!ensure(StartNode != nullptr && EndNode != nullptr && RouterEdge != nullptr))
			{
				continue;
			}

			RouterEdgeMap.Add(RouterEdge, EdRouterEdge);

			RouterEdge->SetRouterGrpah(RouterGraph);

			if (RouterEdge->GetOuter() != RouterGraph)
			{
				RouterEdge->Rename(nullptr, RouterGraph, REN_DontCreateRedirectors | REN_DoNotDirty);
			}

			RouterEdge->SetStartNode(StartNode->RouterNode);
			RouterEdge->SetEndNode(EndNode->RouterNode);
			StartNode->RouterNode->EdgeMap.Add(EndNode->RouterNode, RouterEdge);
		}
	}

	SortRouterNodes();
}

UOverdriveAbilityRouterGraph* UEdAbilityRouterGraph::GetAbilityRouterGraph() const
{
	return CastChecked<UOverdriveAbilityRouterGraph>(GetOuter());
}

bool UEdAbilityRouterGraph::Modify(bool bAlwaysMarkDirty)
{
	bool bResult = Super::Modify(bAlwaysMarkDirty);

	GetAbilityRouterGraph()->Modify();

	for (auto& Node : Nodes)
	{
		Node->Modify();
	}

	return bResult;
}

void UEdAbilityRouterGraph::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}

void UEdAbilityRouterGraph::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	// 쿠킹 시점엔 이미 빌드된 런타임 데이터를 그대로 쓴다.
	if (SaveContext.IsCooking())
	{
		return;
	}

	RebuildAbilityRouterGraph();
}

void UEdAbilityRouterGraph::ClearGraph()
{
	UOverdriveAbilityRouterGraph* RouterGraph = GetAbilityRouterGraph();

	// AllRouterNodes가 아니라 EdGraph의 노드를 기준으로 리셋한다.
	// AllRouterNodes는 직전 Rebuild 결과라, 붙여넣기·Undo로 새로 들어온 노드는 거기에 없다.
	// 그런 노드는 직렬화된 ChildrenNodes/ParentNodes를 그대로 들고 있어 스테일 링크가 남는다.
	for (auto& Node : Nodes)
	{
		UEdAbilityRouterGraphNode* EdRouterNode = Cast<UEdAbilityRouterGraphNode>(Node);
		if (EdRouterNode == nullptr)
		{
			continue;
		}

		if (UOverdriveAbilityRouterNode* RouterNode = EdRouterNode->RouterNode)
		{
			RouterNode->ParentNodes.Reset();
			RouterNode->ChildrenNodes.Reset();
			RouterNode->EdgeMap.Reset();
		}
	}

	RouterGraph->AllRouterNodes.Reset();
	RouterGraph->RootRouterNode = nullptr;
	RouterGraph->GlobalSecondChanceRouterNode = nullptr;

	RouterNodeMap.Reset();
	RouterEdgeMap.Reset();
}

void UEdAbilityRouterGraph::SortRouterNodes()
{
	TArray<UOverdriveAbilityRouterNode*> RouterNodeArray;

	auto Comp = [&](const UOverdriveAbilityRouterNode& U, const UOverdriveAbilityRouterNode& D)
		{
			UEdAbilityRouterGraphNode* EdNode_UNode = RouterNodeMap[&U];
			UEdAbilityRouterGraphNode* EdNode_DNode = RouterNodeMap[&D];
			return EdNode_UNode->NodePosY < EdNode_DNode->NodePosY;
		};

	for(auto& RouterNodeElement : RouterNodeMap)
	{
		UOverdriveAbilityRouterNode* RouterNode = RouterNodeElement.Key;

		RouterNode->ParentNodes.Sort(Comp);
		RouterNode->ChildrenNodes.Sort(Comp);
	}
}

