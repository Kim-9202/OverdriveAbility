// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/SEdAbilityRouterGraphEdge.h"
#include "EdGraph/EdAbilityRouterGraphEdge.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "ConnectionDrawingPolicy.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SAbilityRouterGraphEdge"

void SEdAbilityRouterGraphEdge::Construct(const FArguments& InArgs, UEdAbilityRouterGraphEdge* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
}

bool SEdAbilityRouterGraphEdge::RequiresSecondPassLayout() const
{
	return true;
}

void SEdAbilityRouterGraphEdge::PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& NodeToWidgetLookup) const
{
	UEdAbilityRouterGraphEdge* EdgeNode = CastChecked<UEdAbilityRouterGraphEdge>(GraphNode);

	FGeometry StartGeom;
	FGeometry EndGeom;

	UEdAbilityRouterGraphNode* Start = EdgeNode->GetStartNode();
	UEdAbilityRouterGraphNode* End = EdgeNode->GetEndNode();
	if (Start != nullptr && End != nullptr)
	{
		const TSharedRef<SNode>* pFromWidget = NodeToWidgetLookup.Find(Start);
		const TSharedRef<SNode>* pToWidget = NodeToWidgetLookup.Find(End);
		if (pFromWidget != nullptr && pToWidget != nullptr)
		{
			const TSharedRef<SNode>& FromWidget = *pFromWidget;
			const TSharedRef<SNode>& ToWidget = *pToWidget;

			StartGeom = FGeometry(FVector2D(Start->NodePosX, Start->NodePosY), FVector2D::ZeroVector, FromWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(End->NodePosX, End->NodePosY), FVector2D::ZeroVector, ToWidget->GetDesiredSize(), 1.0f);
		}
	}

	PositionBetweenTwoNodes(StartGeom, EndGeom);
}

void SEdAbilityRouterGraphEdge::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	// RouterEdge에는 이름 개념이 없으므로 기본 동작만 수행한다.
	SGraphNode::OnNameTextCommited(InText, CommitInfo);
}

void SEdAbilityRouterGraphEdge::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
				.ColorAndOpacity(this, &SEdAbilityRouterGraphEdge::GetEdgeColor)
			]
			+ SOverlay::Slot()
			.Padding(FMargin(4.0f, 4.0f, 4.0f, 4.0f))
			[
				SNew(SVerticalBox)
				// 전이 아이콘.
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SImage)
					.Image(this, &SEdAbilityRouterGraphEdge::GetEdgeImage)
					.Visibility(this, &SEdAbilityRouterGraphEdge::GetEdgeImageVisibility)
				]
				// LeafTag + Press/Release 라벨.
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::Black)
					.Justification(ETextJustify::Center)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.Text(this, &SEdAbilityRouterGraphEdge::GetEdgeLabelText)
				]
				// 엣지는 이름이 없으므로 숨겨 두지만, SGraphNode 내부 참조를 위해 생성은 유지한다.
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(InlineEditableText, SInlineEditableTextBlock)
					.Visibility(EVisibility::Collapsed)
					.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
					.OnTextCommitted(this, &SEdAbilityRouterGraphEdge::OnNameTextCommited)
				]
			]
		];
}

void SEdAbilityRouterGraphEdge::PositionBetweenTwoNodes(const FGeometry& StartGeom, const FGeometry& EndGeom) const
{
	// 두 박스의 중간 지점을 시드로 잡는다.
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// 두 박스에서 (근사적으로) 가장 가까운 점을 찾는다.
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// 연결선의 중간 지점에, 선 방향에 수직으로 띄워서 위치시킨다.
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize);

	GraphNode->NodePosX = static_cast<int32>(NewCorner.X);
	GraphNode->NodePosY = static_cast<int32>(NewCorner.Y);
}

FSlateColor SEdAbilityRouterGraphEdge::GetEdgeColor() const
{
	UEdAbilityRouterGraphEdge* EdgeNode = CastChecked<UEdAbilityRouterGraphEdge>(GraphNode);
	if (EdgeNode->RouterEdge != nullptr)
	{
		// Press = 초록 계열, Release = 주황 계열로 입력 종류를 구분한다.
		return EdgeNode->RouterEdge->IsPressed()
			? FLinearColor(0.2f, 0.8f, 0.25f, 1.0f)
			: FLinearColor(0.9f, 0.5f, 0.1f, 1.0f);
	}
	return FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);
}

const FSlateBrush* SEdAbilityRouterGraphEdge::GetEdgeImage() const
{
	return FAppStyle::GetBrush("Graph.TransitionNode.Icon");
}

EVisibility SEdAbilityRouterGraphEdge::GetEdgeImageVisibility() const
{
	// RouterEdge에는 타이틀 개념이 없으므로 항상 아이콘을 표시한다.
	return EVisibility::Visible;
}

EVisibility SEdAbilityRouterGraphEdge::GetEdgeTitleVisbility() const
{
	// 타이틀을 그리지 않는다(엣지는 이름이 없음).
	return EVisibility::Collapsed;
}

FText SEdAbilityRouterGraphEdge::GetEdgeLabelText() const
{
	UEdAbilityRouterGraphEdge* EdgeNode = CastChecked<UEdAbilityRouterGraphEdge>(GraphNode);
	const UOverdriveAbilityRouterEdge* Edge = EdgeNode->RouterEdge;
	if (Edge == nullptr)
	{
		return FText::GetEmpty();
	}

	// LeafTag: 전체 태그 문자열의 마지막 '.' 이후 부분.
	FString Leaf;
	const FGameplayTag& InputTag = Edge->GetInputTypeTag();
	if (InputTag.IsValid())
	{
		const FString Full = InputTag.GetTagName().ToString();
		int32 DotIdx = INDEX_NONE;
		Leaf = Full.FindLastChar(TEXT('.'), DotIdx) ? Full.RightChop(DotIdx + 1) : Full;
	}
	else
	{
		Leaf = TEXT("(None)");
	}

	// U+25BC(BLACK DOWN TRIANGLE) = Press, U+25B2(BLACK UP TRIANGLE) = Release.
	// 코드 포인트로 직접 만들어 소스 파일 인코딩 의존성을 제거한다.
	const TCHAR Arrow = Edge->IsPressed() ? static_cast<TCHAR>(0x25BC) : static_cast<TCHAR>(0x25B2);
	return FText::FromString(FString::Printf(TEXT("%c %s"), Arrow, *Leaf));
}

#undef LOCTEXT_NAMESPACE
