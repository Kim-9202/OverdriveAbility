// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdAbilityRouterGraphEdge.generated.h"

class UEdAbilityRouterGraph;
class UOverdriveAbilityRouterEdge;
class UEdAbilityRouterGraphNode;

/**
 * 
 */
UCLASS()
class UEdAbilityRouterGraphEdge : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UEdAbilityRouterGraphEdge();

	UPROPERTY()
	TObjectPtr<UEdAbilityRouterGraph> EdRouterGraph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "AbilityRouterGraph")
	TObjectPtr<UOverdriveAbilityRouterEdge> RouterEdge;

	UEdGraphPin* GetInputPin() const { return Pins[0]; }
	UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	UEdAbilityRouterGraphNode* GetStartNode() const;
	UEdAbilityRouterGraphNode* GetEndNode() const;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	void CreateConnections(UEdAbilityRouterGraphNode* Start, UEdAbilityRouterGraphNode* End);
};
