// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/AbilityRouterGraphAssetEditor.h"
#include "EdGraph/AbilityRouterGraphEditorToolkit.h"
#include "EdGraph/EdAbilityRouterGraph.h"
#include "EdGraph/EdAbilityRouterGraphSchema.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"

void UAbilityRouterGraphAssetEditor::Initialize(UOverdriveAbilityRouterGraph* InRouterGraph)
{
	AbilityRouterGraph = InRouterGraph;

	if (AbilityRouterGraph != nullptr)
	{
		bool bUpgradedAsset = false;

		// EdGraph가 null이면(신규 생성 또는 과거 저장 시 누락) 여기서 만든다.
		// null인 채로 SGraphEditor에 넘기면 SGraphPanel::Construct에서 크래시.
		if (AbilityRouterGraph->EdGraph == nullptr)
		{
			AbilityRouterGraph->EdGraph = FBlueprintEditorUtils::CreateNewGraph(
				AbilityRouterGraph,
				NAME_None,
				UEdAbilityRouterGraph::StaticClass(),
				UEdAbilityRouterGraphSchema::StaticClass());
			AbilityRouterGraph->EdGraph->bAllowDeletion = false;

			bUpgradedAsset = true;
		}

		// CreateNewGraph는 기본 노드를 만들지 않는다. 또 GlobalSecondChance 도입 전에 저장된
		// 에셋에는 그 노드가 없다. CreateDefaultNodesForGraph는 없는 진입 노드만 채우므로
		// 신규·기존 구분 없이 항상 호출한다. 툴킷이 SGraphEditor를 만들기 전이라 즉시 그려진다.
		UEdGraph* EdGraph = AbilityRouterGraph->EdGraph;
		const int32 NodeCountBeforeDefaults = EdGraph->Nodes.Num();

		EdGraph->GetSchema()->CreateDefaultNodesForGraph(*EdGraph);

		// AddNode는 더티 처리를 하지 않는다. 멱등 호출이라 실제로 채워진 경우에만 찍어야
		// 에셋을 열기만 해도 더티가 되는 일을 피할 수 있다.
		if (bUpgradedAsset || EdGraph->Nodes.Num() != NodeCountBeforeDefaults)
		{
			AbilityRouterGraph->MarkPackageDirty();
		}
	}

	Super::Initialize();
}

void UAbilityRouterGraphAssetEditor::GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit)
{
	InObjectsToEdit.Add(AbilityRouterGraph);
}

TSharedPtr<FBaseAssetToolkit> UAbilityRouterGraphAssetEditor::CreateToolkit()
{
	TSharedPtr<FAbilityRouterGraphEditorToolkit> RouterToolkit = MakeShared<FAbilityRouterGraphEditorToolkit>(this);
	RouterToolkit->SetAbilityRouterGraph(AbilityRouterGraph);

	return RouterToolkit;
}
