// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "OverdriveAbilityGraphPinFactory.h"

class FOverdriveAbilityNodesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FOverdriveAbilityGraphPinFactory> NodePinFactory;
};

