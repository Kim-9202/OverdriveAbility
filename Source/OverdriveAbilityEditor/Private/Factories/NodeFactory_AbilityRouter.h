// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <EdGraphUtilities.h>

class SGraphNode;

/**
 * 
 */
class FNodeFactory_AbilityRouter : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
