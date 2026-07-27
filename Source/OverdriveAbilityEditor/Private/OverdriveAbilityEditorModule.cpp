// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverdriveAbilityEditorModule.h"
#include "OverdriveAbilityEditorModeCommands.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "EdGraph/AbilityRouterGraphAssetEditor.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "Customizations/OverdriveAbilityRouterNode_ProxyDetails.h"
#include "Factories/NodeFactory_AbilityRouter.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "OverdriveAbilityEditorModule"

DEFINE_LOG_CATEGORY(LogOverdriveAbilityEditor)

const FText FOverdriveAbilityEditorModule::OverdrivePluginCategory = LOCTEXT("OverdrivePluginsAssetCategory", "Overdrive Plugins");
const FText FOverdriveAbilityEditorModule::AbilityCategory = LOCTEXT("Ability Category", "Overdrive Ability");

void FOverdriveAbilityEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FOverdriveAbilityEditorModeCommands::Register();

	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// 런타임 Proxy 노드의 TargetRouterNode를 "같은 그래프 형제 노드" 콤보박스로 대체.
	PropertyEditor.RegisterCustomClassLayout(
		UOverdriveAbilityRouterNode_Proxy::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FOverdriveAbilityRouterNode_ProxyDetails::MakeInstance));
	PropertyEditor.NotifyCustomizationModuleChanged();

	// 커스텀 노드 위젯(SEdAbilityRouterGraphNode)을 쓰려면 비주얼 노드 팩토리를 등록해야 한다.
	// 미등록 시 엔진 기본 노드 위젯이 그려져 일반 핀이 그대로 보인다.
	GraphNodeFactory_AbilityRouter = MakeShareable(new FNodeFactory_AbilityRouter());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory_AbilityRouter);
}

void FOverdriveAbilityEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (GraphNodeFactory_AbilityRouter.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphNodeFactory_AbilityRouter);
		GraphNodeFactory_AbilityRouter.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditor.UnregisterCustomClassLayout(UOverdriveAbilityRouterNode_Proxy::StaticClass()->GetFName());
		PropertyEditor.NotifyCustomizationModuleChanged();
	}


	FOverdriveAbilityEditorModeCommands::Unregister();
}

const FText& FOverdriveAbilityEditorModule::GetOverdrivePluginCategory()
{
	return OverdrivePluginCategory;
}

const FText& FOverdriveAbilityEditorModule::GetAbilityCategory()
{
	return AbilityCategory;
}

UAbilityRouterGraphAssetEditor* FOverdriveAbilityEditorModule::CreateAbilityRouterGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UOverdriveAbilityRouterGraph* AbilityRouterGraph)
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	UAbilityRouterGraphAssetEditor* AssetEditor = NewObject<UAbilityRouterGraphAssetEditor>(AssetEditorSubsystem, NAME_None, RF_Transient);
	AssetEditor->Initialize(AbilityRouterGraph);

	return AssetEditor;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOverdriveAbilityEditorModule, OverdriveAbilityEditor)