// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/EdAbilityRouterGraphEdge.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"

UEdAbilityRouterGraphEdge::UEdAbilityRouterGraphEdge()
{
	bCanRenameNode = false;
}

void UEdAbilityRouterGraphEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdAbilityRouterGraphEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText();
}

void UEdAbilityRouterGraphEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		Modify();

		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdAbilityRouterGraphEdge::PrepareForCopying()
{
	RouterEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdAbilityRouterGraphEdge::CreateConnections(UEdAbilityRouterGraphNode* Start, UEdAbilityRouterGraphNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdAbilityRouterGraphNode* UEdAbilityRouterGraphEdge::GetStartNode() const
{
	if (GetInputPin()->LinkedTo.Num() > 0)
	{
		return Cast<UEdAbilityRouterGraphNode>(GetInputPin()->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdAbilityRouterGraphNode* UEdAbilityRouterGraphEdge::GetEndNode() const
{
	if (GetOutputPin()->LinkedTo.Num() > 0)
	{
		return Cast<UEdAbilityRouterGraphNode>(GetOutputPin()->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}