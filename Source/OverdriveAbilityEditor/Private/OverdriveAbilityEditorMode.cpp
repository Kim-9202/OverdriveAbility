// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverdriveAbilityEditorMode.h"
#include "OverdriveAbilityEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "OverdriveAbilityEditorModeCommands.h"
#include "Modules/ModuleManager.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/OverdriveAbilitySimpleTool.h"
#include "Tools/OverdriveAbilityInteractiveTool.h"

// step 2: register a ToolBuilder in FOverdriveAbilityEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "OverdriveAbilityEditorMode"

const FEditorModeID UOverdriveAbilityEditorMode::EM_OverdriveAbilityEditorModeId = TEXT("EM_OverdriveAbilityEditorMode");

FString UOverdriveAbilityEditorMode::SimpleToolName = TEXT("OverdriveAbilityEditor_ActorInfoTool");
FString UOverdriveAbilityEditorMode::InteractiveToolName = TEXT("OverdriveAbilityEditor_MeasureDistanceTool");


UOverdriveAbilityEditorMode::UOverdriveAbilityEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UOverdriveAbilityEditorMode::EM_OverdriveAbilityEditorModeId,
		LOCTEXT("ModeName", "OverdriveAbilityEditor"),
		FSlateIcon(),
		true);
}


UOverdriveAbilityEditorMode::~UOverdriveAbilityEditorMode()
{
}


void UOverdriveAbilityEditorMode::ActorSelectionChangeNotify()
{
}

void UOverdriveAbilityEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FOverdriveAbilityEditorModeCommands& SampleToolCommands = FOverdriveAbilityEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UOverdriveAbilitySimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UOverdriveAbilityInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UOverdriveAbilityEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FOverdriveAbilityEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UOverdriveAbilityEditorMode::GetModeCommands() const
{
	return FOverdriveAbilityEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
