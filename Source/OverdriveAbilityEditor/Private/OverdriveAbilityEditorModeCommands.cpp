// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverdriveAbilityEditorModeCommands.h"
#include "OverdriveAbilityEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OverdriveAbilityEditorModeCommands"

FOverdriveAbilityEditorModeCommands::FOverdriveAbilityEditorModeCommands()
	: TCommands<FOverdriveAbilityEditorModeCommands>("OverdriveAbilityEditorMode",
		NSLOCTEXT("OverdriveAbilityEditorMode", "OverdriveAbilityEditorModeCommands", "OverdriveAbility Editor Mode"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FOverdriveAbilityEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FOverdriveAbilityEditorModeCommands::GetCommands()
{
	return FOverdriveAbilityEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
