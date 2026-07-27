// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/AbilityRouterGraphEditorToolkit.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "EdGraph/AbilityRouterGraphAssetEditor.h"
#include "EdGraph/EdAbilityRouterGraph.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "EdGraph/EdAbilityRouterGraphEdge.h"

#include "EdGraph/EdGraphSchema.h"
#include "EdGraphUtilities.h"
#include "IDetailsView.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "AbilityRouterGraphEditorToolkit"


FAbilityRouterGraphEditorToolkit::FAbilityRouterGraphEditorToolkit(UAssetEditor* InOwningAssetEditor)
	: FBaseAssetToolkit(InOwningAssetEditor)
{


}

FAbilityRouterGraphEditorToolkit::~FAbilityRouterGraphEditorToolkit()
{
}

void FAbilityRouterGraphEditorToolkit::SetAbilityRouterGraph(UOverdriveAbilityRouterGraph* NewGraph)
{
	EditingRouterGraph = NewGraph;
}

void FAbilityRouterGraphEditorToolkit::CreateWidgets()
{	
	RegisterToolbar();
	CreateEditorModeManager();


	DetailsView = CreateDetailsView();

	// 디테일 패널에서 값이 바뀌면 스키마 시각화 캐시를 비워야 SNodeTitle이 제목을 다시 읽는다.
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FAbilityRouterGraphEditorToolkit::OnFinishedChangingProperties);

	RouterGraphWidget = CreateRouterGraphEditorWidget();
}

FText FAbilityRouterGraphEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Ability Router Graph");
}

FName FAbilityRouterGraphEditorToolkit::GetToolkitFName() const
{
	static const FName ToolkitName("AbilityRouterGraphEditor");
	return ToolkitName;
}

FString FAbilityRouterGraphEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Ability Router Graph ").ToString();
}

FLinearColor FAbilityRouterGraphEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

TSharedRef<SDockTab> FAbilityRouterGraphEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabID);

	TSharedPtr<SDockTab> RouterGraphTab = SNew(SDockTab)
		.Label(LOCTEXT("RouterGraphTabTitle", "Ability Router"))
		[
			RouterGraphWidget.ToSharedRef()
		];

	return RouterGraphTab.ToSharedRef();
}

TSharedRef<SGraphEditor> FAbilityRouterGraphEditorToolkit::CreateRouterGraphEditorWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_GenericGraph", "Generic Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAbilityRouterGraphEditorToolkit::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAbilityRouterGraphEditorToolkit::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(Cast<UAbilityRouterGraphAssetEditor>(OwningAssetEditor)->GetAbilityRouterGraph()->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FAbilityRouterGraphEditorToolkit::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::SelectAllNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::PasteNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::DuplicateNodes),
		FCanExecuteAction::CreateSP(this, &FAbilityRouterGraphEditorToolkit::CanDuplicateNodes)
	);
}

//////////////////////////////////////////////////////////////////////////
// graph editor command handlers

void FAbilityRouterGraphEditorToolkit::SelectAllNodes()
{
	if (RouterGraphWidget.IsValid())
	{
		RouterGraphWidget->SelectAllNodes();
	}
}

bool FAbilityRouterGraphEditorToolkit::CanSelectAllNodes() const
{
	return RouterGraphWidget.IsValid();
}

void FAbilityRouterGraphEditorToolkit::DeleteSelectedNodes()
{
	if (!RouterGraphWidget.IsValid())
	{
		return;
	}

	UEdGraph* EdGraph = RouterGraphWidget->GetCurrentGraph();
	if (EdGraph == nullptr)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("DeleteSelectedNode", "Delete Selected Node"));
	EdGraph->Modify();

	const FGraphPanelSelectionSet SelectedNodes = RouterGraphWidget->GetSelectedNodes();
	RouterGraphWidget->ClearSelectionSet();

	for (UObject* SelectedObject : SelectedNodes)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(SelectedObject);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
		{
			continue;
		}

		// 앞서 지운 노드에 딸려 엣지가 이미 스스로 제거됐을 수 있다.
		if (!EdGraph->Nodes.Contains(EdNode))
		{
			continue;
		}

		EdNode->Modify();

		// DestroyNode()가 타는 BreakAllNodeLinks()는 bNotifyNodes=false라
		// PinConnectionListChanged가 발생하지 않아 엣지가 스스로 정리되지 않는다.
		// 엣지 자신에게 부르면 첫 핀을 끊는 순간 DestroyNode()가 호출되는데
		// BreakNodeLinks 루프가 남은 핀을 계속 처리해 중복 호출된다.
		if (Cast<UEdAbilityRouterGraphNode>(EdNode) != nullptr)
		{
			if (const UEdGraphSchema* Schema = EdNode->GetSchema())
			{
				Schema->BreakNodeLinks(*EdNode);
			}
		}

		EdNode->DestroyNode();
	}
}

bool FAbilityRouterGraphEditorToolkit::CanDeleteNodes() const
{
	if (!RouterGraphWidget.IsValid())
	{
		return false;
	}

	for (UObject* SelectedObject : RouterGraphWidget->GetSelectedNodes())
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(SelectedObject);
		if (EdNode != nullptr && EdNode->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FAbilityRouterGraphEditorToolkit::DeleteSelectedDuplicatableNodes()
{
	if (!RouterGraphWidget.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = RouterGraphWidget->GetSelectedNodes();
	RouterGraphWidget->ClearSelectionSet();

	// 복제 가능한 노드만 다시 선택해 삭제하고, 나머지는 선택 상태로 복원한다.
	FGraphPanelSelectionSet RemainingNodes;
	for (UObject* SelectedObject : OldSelectedNodes)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(SelectedObject);
		if (EdNode != nullptr && EdNode->CanDuplicateNode())
		{
			RouterGraphWidget->SetNodeSelection(EdNode, true);
		}
		else
		{
			RemainingNodes.Add(SelectedObject);
		}
	}

	DeleteSelectedNodes();

	RouterGraphWidget->ClearSelectionSet();
	for (UObject* RemainingObject : RemainingNodes)
	{
		if (UEdGraphNode* EdNode = Cast<UEdGraphNode>(RemainingObject))
		{
			RouterGraphWidget->SetNodeSelection(EdNode, true);
		}
	}
}

void FAbilityRouterGraphEditorToolkit::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAbilityRouterGraphEditorToolkit::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAbilityRouterGraphEditorToolkit::CopySelectedNodes()
{
	if (!RouterGraphWidget.IsValid())
	{
		return;
	}

	FGraphPanelSelectionSet SelectedNodes = RouterGraphWidget->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*SelectedIter);
		if (EdNode == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		// 엣지는 양 끝 노드가 모두 복사 대상일 때만 포함한다(끊긴 엣지 방지).
		if (UEdAbilityRouterGraphEdge* EdEdge = Cast<UEdAbilityRouterGraphEdge>(EdNode))
		{
			UEdAbilityRouterGraphNode* StartNode = EdEdge->GetStartNode();
			UEdAbilityRouterGraphNode* EndNode = EdEdge->GetEndNode();
			if (StartNode == nullptr || EndNode == nullptr
				|| !SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}

		EdNode->PrepareForCopying();
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FAbilityRouterGraphEditorToolkit::CanCopyNodes() const
{
	if (!RouterGraphWidget.IsValid())
	{
		return false;
	}

	for (UObject* SelectedObject : RouterGraphWidget->GetSelectedNodes())
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(SelectedObject);
		if (EdNode != nullptr && EdNode->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FAbilityRouterGraphEditorToolkit::PasteNodes()
{
	if (RouterGraphWidget.IsValid())
	{
		PasteNodesHere(RouterGraphWidget->GetPasteLocation2f());
	}
}

void FAbilityRouterGraphEditorToolkit::PasteNodesHere(const FVector2f& Location)
{
	if (!RouterGraphWidget.IsValid())
	{
		return;
	}

	UEdGraph* EdGraph = RouterGraphWidget->GetCurrentGraph();
	if (EdGraph == nullptr)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("PasteNode", "Paste Node"));
	EdGraph->Modify();

	RouterGraphWidget->ClearSelectionSet();

	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

	// 붙여넣은 노드들의 평균 위치를 구해 마우스 위치 기준으로 재배치한다.
	FVector2f AvgNodePosition(0.f, 0.f);
	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		AvgNodePosition.X += PastedNode->NodePosX;
		AvgNodePosition.Y += PastedNode->NodePosY;
	}

	if (PastedNodes.Num() > 0)
	{
		const float InvNumNodes = 1.0f / static_cast<float>(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		RouterGraphWidget->SetNodeSelection(PastedNode, true);

		PastedNode->NodePosX = static_cast<int32>((PastedNode->NodePosX - AvgNodePosition.X) + Location.X);
		PastedNode->NodePosY = static_cast<int32>((PastedNode->NodePosY - AvgNodePosition.Y) + Location.Y);

		PastedNode->SnapToGrid(16);
		PastedNode->CreateNewGuid();
		PastedNode->PostPasteNode();
	}

	RouterGraphWidget->NotifyGraphChanged();

	if (UObject* GraphOwner = EdGraph->GetOuter())
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FAbilityRouterGraphEditorToolkit::CanPasteNodes() const
{
	if (!RouterGraphWidget.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(RouterGraphWidget->GetCurrentGraph(), ClipboardContent);
}

void FAbilityRouterGraphEditorToolkit::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAbilityRouterGraphEditorToolkit::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FAbilityRouterGraphEditorToolkit::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	// 디테일은 EdNode가 아니라 런타임 노드/엣지를 표시한다(엔진 ObjectTreeGraph 관례).
	TArray<UObject*> Selection;
	for (UObject* SelectionEntry : NewSelection)
	{
		if (UEdAbilityRouterGraphNode* EdNode = Cast<UEdAbilityRouterGraphNode>(SelectionEntry))
		{
			if (EdNode->RouterNode)
			{
				Selection.Add(EdNode->RouterNode);
			}
		}
		else if (UEdAbilityRouterGraphEdge* EdEdge = Cast<UEdAbilityRouterGraphEdge>(SelectionEntry))
		{
			if (EdEdge->RouterEdge)
			{
				Selection.Add(EdEdge->RouterEdge);
			}
		}
	}

	if (NewSelection.Num() == 0)
	{
		// 선택 없음 → 그래프 표시.
		DetailsView->SetObject(EditingRouterGraph);
	}
	else if (NewSelection.Num() == 1 && Selection.Num() == 1)
	{
		// 단일 노드/엣지 → 해당 런타임 객체 표시.
		DetailsView->SetObjects(Selection);
	}
	else
	{
		// 다중 선택 등 → 아무것도 표시하지 않는다.
		DetailsView->SetObjects(TArray<UObject*>());
	}
}

void FAbilityRouterGraphEditorToolkit::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	if (UEdAbilityRouterGraphNode* EdNode = Cast<UEdAbilityRouterGraphNode>(Node))
	{
		// 노드 타입별 가상 동작으로 위임 (Proxy → Target 노드로 JumpToNode)
		EdNode->OnNodeDoubleClicked(RouterGraphWidget);
	}
}

void FAbilityRouterGraphEditorToolkit::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingRouterGraph == nullptr || EditingRouterGraph->EdGraph == nullptr)
	{
		return;
	}

	EditingRouterGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}


#undef LOCTEXT_NAMESPACE