// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "SNodePanel.h"
#include "SGraphNode.h"

class UEdAbilityRouterGraphEdge;

/**
 * 엣지(전이) 노드의 Slate 위젯. GenericGraph의 SEdNode_GenericGraphEdge 패턴을 따른다.
 * 두 노드 사이에 전이 아이콘을 배치하고(2nd pass layout), Press/Release에 따라 색을 달리한다.
 */
class OVERDRIVEABILITYEDITOR_API SEdAbilityRouterGraphEdge : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdAbilityRouterGraphEdge) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdAbilityRouterGraphEdge* InNode);

	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& NodeToWidgetLookup) const override;

	virtual void UpdateGraphNode() override;

	// 시작/끝 노드 사이에 위젯을 배치한다. NodeIndex/MaxNodes는 같은 두 노드 사이 다중 엣지 오프셋 계산용.
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

protected:
	FSlateColor GetEdgeColor() const;

	const FSlateBrush* GetEdgeImage() const;

	EVisibility GetEdgeImageVisibility() const;
	EVisibility GetEdgeTitleVisbility() const;

	// InputTypeTag의 LeafTag와 Press/Release(▼/▲)를 한 줄로 합친 라벨 텍스트.
	FText GetEdgeLabelText() const;
};
