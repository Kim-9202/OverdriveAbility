// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDefinitions/AssetDefinition_AbilityRouterGraph.h"
#include "Editor.h"
#include "MergeUtils.h"
#include "SDetailsDiff.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "ContentBrowserMenuContexts.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "OverdriveAbilityEditorModule.h"

#define LOCTEXT_NAMESPACE "FOverdriveGrimoireEditorModule"

namespace MenuExtension_AbilityRouterGraph
{
    static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, [] {
        UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
            {
                FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
                UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UOverdriveAbilityRouterGraph::StaticClass());

                FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
            }));
        });
}


UAssetDefinition_AbilityRouterGraph::UAssetDefinition_AbilityRouterGraph()
{
}

FText UAssetDefinition_AbilityRouterGraph::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_AbilityRouterGraph_Title", "AbilityRouterGraph");
}

TSoftClassPtr<UObject> UAssetDefinition_AbilityRouterGraph::GetAssetClass() const
{
	return UOverdriveAbilityRouterGraph::StaticClass();
}

EAssetCommandResult UAssetDefinition_AbilityRouterGraph::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	if (DiffArgs.OldAsset == nullptr && DiffArgs.NewAsset == nullptr)
	{
		return EAssetCommandResult::Unhandled;
	}

	const TSharedRef<SDetailsDiff> DetailsDiff = SDetailsDiff::CreateDiffWindow(DiffArgs.OldAsset, DiffArgs.NewAsset, DiffArgs.OldRevision, DiffArgs.NewRevision, UDataAsset::StaticClass());
	// allow users to edit NewAsset if it's a local asset
	if (DiffArgs.NewAsset != nullptr && !FPackageName::IsTempPackage(DiffArgs.NewAsset->GetPackage()->GetName()))
	{
		DetailsDiff->SetOutputObject(DiffArgs.NewAsset);
	}
	return EAssetCommandResult::Handled;
}

EAssetCommandResult UAssetDefinition_AbilityRouterGraph::Merge(const FAssetAutomaticMergeArgs& MergeArgs) const
{
	return MergeUtils::Merge(MergeArgs);
}

EAssetCommandResult UAssetDefinition_AbilityRouterGraph::Merge(const FAssetManualMergeArgs& MergeArgs) const
{
	return MergeUtils::Merge(MergeArgs);
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_AbilityRouterGraph::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = { FAssetCategoryPath(FOverdriveAbilityEditorModule::GetOverdrivePluginCategory(), FOverdriveAbilityEditorModule::GetAbilityCategory()) };
	return Categories;
}

FText UAssetDefinition_AbilityRouterGraph::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("AssetDefinition_AbilityRouterGraph_Description", "Data Asset that define a Grimoire, Use Ability and GameplayEffect by ReaderComponent");
}

EAssetCommandResult UAssetDefinition_AbilityRouterGraph::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	FOverdriveAbilityEditorModule& OverdriveAbilityEditorModule = FModuleManager::LoadModuleChecked<FOverdriveAbilityEditorModule>("OverdriveAbilityEditor");
	for (UOverdriveAbilityRouterGraph* RouterGraph : OpenArgs.LoadObjects<UOverdriveAbilityRouterGraph>())
	{
		OverdriveAbilityEditorModule.CreateAbilityRouterGraphAssetEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, RouterGraph);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE

