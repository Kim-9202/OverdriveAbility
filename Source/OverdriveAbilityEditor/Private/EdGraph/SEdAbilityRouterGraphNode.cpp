// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/SEdAbilityRouterGraphNode.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "EdGraph/AbilityRouterGraphDragDropAction.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "FOverdriveAbilityEditor"

//////////////////////////////////////////////////////////////////////////
class SAbilityRouterGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SAbilityRouterGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SAbilityRouterGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SAbilityRouterGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SAbilityRouterGraphPin::OnPinMouseDown)
			.Cursor(this, &SAbilityRouterGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FLinearColor(0.02f, 0.02f, 0.02f);
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		TArray<FGraphPinHandle> PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FAbilityRouterGraphDragDropAction::New(InGraphPanel, PinHandles);
	}

};


//////////////////////////////////////////////////////////////////////////


void SEdAbilityRouterGraphNode::Construct(const FArguments& InArgs, UEdAbilityRouterGraphNode* InNode)
{
	GraphNode = InNode;
	IsEditable = false;

	UpdateGraphNode();
	if (InNode)
	{
		InNode->SEdRouterNode = this;
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdAbilityRouterGraphNode::UpdateGraphNode()
{
	static const FMargin NodePadding = FMargin(5);
	static const FMargin NamePadding = FMargin(2);

	static const FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);

	InputPins.Reset();
	OutputPins.Reset();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	GetOrAddSlot(ENodeZone::Center)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Center)
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(0.0f)
		.BorderBackgroundColor(this, &SEdAbilityRouterGraphNode::GetBorderBackgroundColor)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(8.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
				.BorderBackgroundColor(TitleShadowColor)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Visibility(EVisibility::SelfHitTestInvisible)
				.Padding(6.0f)
				[
					CreateRouterNodeBody()
				]
			]
		]
	];
	
	CreatePinWidgets();
}

void SEdAbilityRouterGraphNode::CreatePinWidgets()
{
	UEdAbilityRouterGraphNode* StateNode = CastChecked<UEdAbilityRouterGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SAbilityRouterGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}

}

void SEdAbilityRouterGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

TSharedRef<SWidget> SEdAbilityRouterGraphNode::CreateRouterNodeBody()
{
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	TSharedPtr<SVerticalBox> OutNodeBody;

	SAssignNew(OutNodeBody, SVerticalBox)
	+SVerticalBox::Slot()
	.AutoHeight()
	.VAlign(EVerticalAlignment::VAlign_Center)
	.HAlign(EHorizontalAlignment::HAlign_Center)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4.0f, 0.0f)
		[
			SAssignNew(ErrorText, SErrorText)
			.BackgroundColor(this, &SEdAbilityRouterGraphNode::GetErrorColor)
			.ToolTipText(this, &SEdAbilityRouterGraphNode::GetErrorMsgToolTip)
		]

		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(InlineEditableText, SInlineEditableTextBlock)
				.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
				.OnVerifyTextChanged(this, &SEdAbilityRouterGraphNode::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &SEdAbilityRouterGraphNode::OnNameTextCommited)
				.IsReadOnly(this, &SEdAbilityRouterGraphNode::IsNameReadOnly)
				.IsSelected(this, &SEdAbilityRouterGraphNode::IsSelectedExclusively)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				NodeTitle.ToSharedRef()
			]
		]
	];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);

	return OutNodeBody.ToSharedRef();
}

bool SEdAbilityRouterGraphNode::IsNameReadOnly() const
{
	return true;
}

FSlateColor SEdAbilityRouterGraphNode::GetBorderBackgroundColor() const
{
	UEdAbilityRouterGraphNode* MyNode = CastChecked<UEdAbilityRouterGraphNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : FLinearColor(0.0f, 0.22f, 0.4f);
}

FSlateColor SEdAbilityRouterGraphNode::GetBackgroundColor() const
{
	return FLinearColor(0.1f, 0.1f, 0.1f);
}

EVisibility SEdAbilityRouterGraphNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE