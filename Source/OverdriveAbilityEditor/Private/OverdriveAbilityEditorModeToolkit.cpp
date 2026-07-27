// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverdriveAbilityEditorModeToolkit.h"
#include "OverdriveAbilityEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "OverdriveAbilityEditorModeToolkit"

FOverdriveAbilityEditorModeToolkit::FOverdriveAbilityEditorModeToolkit()
{
}

void FOverdriveAbilityEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FOverdriveAbilityEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FOverdriveAbilityEditorModeToolkit::GetToolkitFName() const
{
	return FName("OverdriveAbilityEditorMode");
}

FText FOverdriveAbilityEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "OverdriveAbilityEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
