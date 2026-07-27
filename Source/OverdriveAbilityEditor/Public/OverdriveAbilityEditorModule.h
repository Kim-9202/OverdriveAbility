// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOverdriveAbilityEditor, Log, All);

class UAbilityRouterGraphAssetEditor;
class UOverdriveAbilityRouterGraph;
struct FGraphPanelNodeFactory;

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FOverdriveAbilityEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static const FText& GetOverdrivePluginCategory();

	static const FText& GetAbilityCategory();

	virtual UAbilityRouterGraphAssetEditor* CreateAbilityRouterGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UOverdriveAbilityRouterGraph* AbilityRouterGraph);

private:
	static const FText OverdrivePluginCategory;
	static const FText AbilityCategory;

	/** AbilityRouter 노드용 비주얼 노드 팩토리 (등록해야 커스텀 SGraphNode가 적용됨) */
	TSharedPtr<FGraphPanelNodeFactory> GraphNodeFactory_AbilityRouter;
};
