// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverdriveAbilityNodesModule.h"

void FOverdriveAbilityNodesModule::StartupModule()
{
    NodePinFactory = MakeShareable(new FOverdriveAbilityGraphPinFactory());
    FEdGraphUtilities::RegisterVisualPinFactory(NodePinFactory);
}

void FOverdriveAbilityNodesModule::ShutdownModule()
{
    if (NodePinFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualPinFactory(NodePinFactory);
    }
}

IMPLEMENT_MODULE(FOverdriveAbilityNodesModule, OverdriveAbilityNodes)


