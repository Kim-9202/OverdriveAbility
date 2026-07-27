// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "UObject/ObjectSaveContext.h"
#include "EdAbilityRouterGraph.generated.h"

class UOverdriveAbilityRouterGraph;
class UOverdriveAbilityRouterNode;
class UOverdriveAbilityRouterEdge;
class UEdAbilityRouterGraphNode;
class UEdAbilityRouterGraphEdge;
/**
 * 
 */
UCLASS()
class OVERDRIVEABILITYEDITOR_API UEdAbilityRouterGraph : public UEdGraph
{
	GENERATED_BODY()
	
public:
	UEdAbilityRouterGraph();
	virtual ~UEdAbilityRouterGraph();

	void RebuildAbilityRouterGraph();

	UOverdriveAbilityRouterGraph* GetAbilityRouterGraph() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	// 저장 직전에 EdGraph → 런타임 라우터 그래프를 동기화한다.
	// 툴킷의 저장 커맨드만 잡으면 Content Browser 저장·Save All 경로가 빠진다.
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UOverdriveAbilityRouterNode>, TObjectPtr<UEdAbilityRouterGraphNode>> RouterNodeMap;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UOverdriveAbilityRouterEdge>, TObjectPtr<UEdAbilityRouterGraphEdge>> RouterEdgeMap;

protected:
	void ClearGraph();

	void SortRouterNodes();
};
