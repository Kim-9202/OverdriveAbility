// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdAbilityRouterGraphNode.generated.h"

class UOverdriveAbilityRouterNode;
class SEdAbilityRouterGraphNode;
class SGraphEditor;

/**
 * 
 */
UCLASS()
class UEdAbilityRouterGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
public:
	UEdAbilityRouterGraphNode();
	virtual ~UEdAbilityRouterGraphNode();

	UPROPERTY(VisibleAnywhere, Instanced, Category = "AbilityRouter")
	TObjectPtr<UOverdriveAbilityRouterNode> RouterNode;

	SEdAbilityRouterGraphNode* SEdRouterNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	// 루트 노드는 삭제/복제(복사·붙여넣기) 대상에서 제외한다.
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

	virtual void PostEditUndo() override;

	// 노드 더블클릭 시 동작. 기본은 no-op. 서브클래스가 오버라이드한다.
	virtual void OnNodeDoubleClicked(TSharedPtr<SGraphEditor> InGraphEditor) {}
};
