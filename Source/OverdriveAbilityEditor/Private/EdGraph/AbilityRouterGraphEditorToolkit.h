// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tools/BaseAssetToolkit.h"

class UAssetEditor;
class UOverdriveAbilityRouterGraph;
class UOverdriveAbilityRouterNode;

/**
 * 
 */
class FAbilityRouterGraphEditorToolkit : public FBaseAssetToolkit
{
public:
	FAbilityRouterGraphEditorToolkit(UAssetEditor* InOwningAssetEditor);
	~FAbilityRouterGraphEditorToolkit();

	void SetAbilityRouterGraph(UOverdriveAbilityRouterGraph* NewGraph);

protected:
	// EdGraph → 런타임 그래프 동기화는 UEdAbilityRouterGraph::PreSave가 담당한다.
	// 툴킷 저장 커맨드만 잡으면 Content Browser 저장·Save All 경로가 빠진다.

	// FBaseAssetToolkit interface
	virtual void CreateWidgets() override;
//	virtual void RegisterToolbar() override;
//	virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;
//	virtual void PostInitAssetEditor() override;
//	virtual void PostRegenerateMenusAndToolbars() override;
//
	// IToolkit interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

private:
	virtual TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args) override;
	TSharedRef<SGraphEditor> CreateRouterGraphEditorWidget();

	void CreateCommandList();

	//////////////////////////////////////////////////////////////////////////
	// graph editor command handlers
	void SelectAllNodes();
	bool CanSelectAllNodes() const;

	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicatableNodes();

	void CutSelectedNodes();
	bool CanCutNodes() const;

	void CopySelectedNodes();
	bool CanCopyNodes() const;

	void PasteNodes();
	void PasteNodesHere(const FVector2f& Location);
	bool CanPasteNodes() const;

	void DuplicateNodes();
	bool CanDuplicateNodes() const;

	//void OnJumpToNode(UEdGraphNode* Node, FName PinName);

	bool bIsInitialized = false;

	TSharedPtr<SGraphEditor> RouterGraphWidget;
	TSharedPtr<FUICommandList> GraphEditorCommands;

	UOverdriveAbilityRouterGraph* EditingRouterGraph = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// graph editor event
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	void OnNodeDoubleClicked(UEdGraphNode* Node);

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

	//void OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);
};
