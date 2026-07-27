// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"


UEdAbilityRouterGraphNode::UEdAbilityRouterGraphNode()
{
	bCanRenameNode = false;
}

UEdAbilityRouterGraphNode::~UEdAbilityRouterGraphNode()
{
}

void UEdAbilityRouterGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UEdAbilityRouterGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return RouterNode ? RouterNode->GetNodeTitle() : FText::GetEmpty();
}

void UEdAbilityRouterGraphNode::PrepareForCopying()
{
	RouterNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdAbilityRouterGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		// 출력 핀에서 끌어왔으면 이 노드가 자식, 입력 핀에서 끌어왔으면 부모가 된다.
		// 스키마가 같은 방향끼리의 연결을 거부하므로 반대 방향 핀을 골라야 한다.
		UEdGraphPin* TargetPin = (FromPin->Direction == EGPD_Output) ? GetInputPin() : GetOutputPin();

		if (GetSchema()->TryCreateConnection(FromPin, TargetPin))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}


FLinearColor UEdAbilityRouterGraphNode::GetBackgroundColor() const
{
	return RouterNode ? RouterNode->BackgroundColor : FLinearColor::Black;
}


UEdGraphPin* UEdAbilityRouterGraphNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdAbilityRouterGraphNode::GetOutputPin() const
{
	return Pins[1];
}

void UEdAbilityRouterGraphNode::PostEditUndo()
{
	Super::PostEditUndo();
}

bool UEdAbilityRouterGraphNode::CanUserDeleteNode() const
{
	// Root와 GlobalSecondChance는 형제 클래스다. 진입점 노드는 둘 다 지울 수 없다.
	return Cast<UOverdriveAbilityRouterNode_Root>(RouterNode) == nullptr
		&& Cast<UOverdriveAbilityRouterNode_GlobalSecondChance>(RouterNode) == nullptr;
}

bool UEdAbilityRouterGraphNode::CanDuplicateNode() const
{
	return Cast<UOverdriveAbilityRouterNode_Root>(RouterNode) == nullptr
		&& Cast<UOverdriveAbilityRouterNode_GlobalSecondChance>(RouterNode) == nullptr;
}
