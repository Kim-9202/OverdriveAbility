// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "Abilities/GameplayAbility.h"

UOverdriveAbilityRouterNode::UOverdriveAbilityRouterNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	BackgroundColor = FLinearColor(0.8f, 0.8f, 0.8f, 0.8f);
#endif
}

void UOverdriveAbilityRouterNode::GetParentNodes(TArray<const UOverdriveAbilityRouterNode*>& OutArray) const
{
	OutArray.Reset();
	OutArray.Reserve(ParentNodes.Num());
	OutArray.Append(ParentNodes);
}

void UOverdriveAbilityRouterNode::GetChildrenNodes(TArray<const UOverdriveAbilityRouterNode*>& OutArray) const
{
	OutArray.Reset();
	OutArray.Reserve(ChildrenNodes.Num());
	OutArray.Append(ChildrenNodes);
}

const UOverdriveAbilityRouterEdge* UOverdriveAbilityRouterNode::GetRouterEdge(const UOverdriveAbilityRouterNode* OtherNode) const
{
	return EdgeMap.Contains(OtherNode) ? EdgeMap[OtherNode] : nullptr;
}

const UOverdriveAbilityRouterNode* UOverdriveAbilityRouterNode::GetRouterNodeToActivate(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const
{
	return this;
}


#if WITH_EDITOR
bool UOverdriveAbilityRouterNode::CanCreateConnectionTo(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const
{
	return true;
}

bool UOverdriveAbilityRouterNode::CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const
{
	return true;
}
#endif

UOverdriveAbilityRouterNode_Simple::UOverdriveAbilityRouterNode_Simple()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = FText::FromString(TEXT("Simple Node"));
	CachedNodeTitle = FText::FromString(TEXT("Simple"));
#endif
}


#if WITH_EDITOR
void UOverdriveAbilityRouterNode_Simple::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UOverdriveAbilityRouterNode_Simple, NodeTitle))
	{
		CachedNodeTitle = NodeTitle.IsEmpty() ? FText::FromString(TEXT("Simple")) : NodeTitle;
	}
}
#endif

const UOverdriveAbilityRouterNode* UOverdriveAbilityRouterNode_Proxy::GetRouterNodeToActivate(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const
{
	return TargetRouterNode;
}

UOverdriveAbilityRouterNode_Proxy::UOverdriveAbilityRouterNode_Proxy()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = FText::FromString(TEXT("Proxy Node"));
	CachedNodeTitle = FText::FromString(TEXT("Not Found Proxy Target"));
#endif
}


#if WITH_EDITOR
void UOverdriveAbilityRouterNode_Proxy::UpdateCachedNodeTitle()
{
	CachedNodeTitle = TargetRouterNode
		? FText::Format(
			NSLOCTEXT("OverdriveAbilityRouter", "ProxyToTitle", "Proxy To {0}"),
			TargetRouterNode->GetNodeTitle())
		: FText::FromString(TEXT("Not Found Proxy Target"));
}

void UOverdriveAbilityRouterNode_Proxy::SetTargetRouterNode(UOverdriveAbilityRouterNode* InTarget)
{
	if (TargetRouterNode == InTarget)
	{
		return;
	}

	Modify();                       // undo/redo·더티 처리
	TargetRouterNode = InTarget;    // 내부 직접 수정
	UpdateCachedNodeTitle();        // CachedNodeTitle 직접 수정
}

void UOverdriveAbilityRouterNode_Proxy::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UOverdriveAbilityRouterNode_Proxy, TargetRouterNode))
	{
		UpdateCachedNodeTitle();
	}
}
bool UOverdriveAbilityRouterNode_Proxy::CanCreateConnectionTo(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const
{
	return false;
}
#endif

UOverdriveAbilityRouterNode_Root::UOverdriveAbilityRouterNode_Root()
{
#if WITH_EDITORONLY_DATA
	CachedNodeTitle = FText::FromString(TEXT("Root"));
#endif
}

#if WITH_EDITOR
bool UOverdriveAbilityRouterNode_Root::CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const
{
	return false;
}

#endif

UOverdriveAbilityRouterNode_GlobalSecondChance::UOverdriveAbilityRouterNode_GlobalSecondChance()
{
#if WITH_EDITORONLY_DATA
	CachedNodeTitle = FText::FromString(TEXT("Global Second Chance"));
	BackgroundColor = FLinearColor(0.9f, 0.5f, 0.1f, 0.8f);
#endif
}

#if WITH_EDITOR
bool UOverdriveAbilityRouterNode_GlobalSecondChance::CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const
{
	return false;
}

#endif
