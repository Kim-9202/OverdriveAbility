// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "SNodePanel.h"
#include "SGraphNode.h"

class UEdAbilityRouterGraphNode;

/**
 * 
 */
class OVERDRIVEABILITYEDITOR_API SEdAbilityRouterGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdAbilityRouterGraphNode) 
	{}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdAbilityRouterGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

protected:
	virtual TSharedRef<SWidget> CreateRouterNodeBody();

private:
	TSharedPtr<SErrorText> ErrorText;
};
