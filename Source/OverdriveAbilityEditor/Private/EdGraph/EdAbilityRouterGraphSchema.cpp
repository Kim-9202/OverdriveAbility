// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraph/EdAbilityRouterGraphSchema.h"
#include "EdGraph/EdAbilityRouterGraphNode.h"
#include "EdGraph/EdAbilityRouterGraphNode_Proxy.h"
#include "EdGraph/EdAbilityRouterGraphEdge.h"
#include "EdGraph/EdAbilityRouterGraph.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "Framework/Commands/GenericCommands.h"
#include "ScopedTransaction.h"

#include "GraphEditorActions.h"

#define LOCTEXT_NAMESPACE "FOverdriveAbilityEditor"

int32 UEdAbilityRouterGraphSchema::CurrentCacheRefreshID = 0;

UEdGraphNode* FEdAbilityRouterGraphSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2f& Location, bool bSelectNewNode)
{
	if (NodeTemplate == nullptr)
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("AbilityRouterGraph_NewNode", "AbilityRouter Graph : New Node"));
	
	if (FromPin != nullptr)
	{
		FromPin->Modify();
	}
	ParentGraph->Modify();
	
	NodeTemplate->Rename(nullptr, ParentGraph);
	ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

	NodeTemplate->CreateNewGuid();
	NodeTemplate->PostPlacedNewNode();
	NodeTemplate->AllocateDefaultPins();
	NodeTemplate->AutowireNewNode(FromPin);

	NodeTemplate->NodePosX = Location.X;
	NodeTemplate->NodePosY = Location.Y;

	NodeTemplate->RouterNode->SetFlags(RF_Transactional);
	NodeTemplate->SetFlags(RF_Transactional);

	return NodeTemplate;
}

void FEdAbilityRouterGraphSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FEdAbilityRouterGraphSchemaAction_NewEdge::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2f& Location, bool bSelectNewNode)
{
	if (EdgeTemplate == nullptr)
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("AbilityRouterGraph_NewNode", "AbilityRouter Graph : New Node"));

	if (FromPin != nullptr)
	{
		FromPin->Modify();
	}
	ParentGraph->Modify();

	EdgeTemplate->Rename(nullptr, ParentGraph);
	ParentGraph->AddNode(EdgeTemplate, true, bSelectNewNode);

	EdgeTemplate->CreateNewGuid();
	EdgeTemplate->PostPlacedNewNode();
	EdgeTemplate->AllocateDefaultPins();
	EdgeTemplate->AutowireNewNode(FromPin);

	EdgeTemplate->NodePosX = Location.X;
	EdgeTemplate->NodePosY = Location.Y;

	EdgeTemplate->RouterEdge->SetFlags(RF_Transactional);
	EdgeTemplate->SetFlags(RF_Transactional);

	return EdgeTemplate;
}

void FEdAbilityRouterGraphSchemaAction_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(EdgeTemplate);
}

void UEdAbilityRouterGraphSchema::GetBreakLinkToSubMenuActions(UToolMenu* Menu, UEdGraphPin* InGraphPin)
{	
	// Make sure we have a unique name for every entry in the list
	TMap<FString, uint32> LinkTitleCount;

	FToolMenuSection& Section = Menu->FindOrAddSection("EdAbilityRouterGraphSchema_PinActions");

	// Add all the links we could break from
	for (auto& LinkedPin : InGraphPin->LinkedTo)
	{
		FText Title = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView);

		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);

		FText Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		Section.AddMenuEntry(NAME_None, Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject(this, &UEdAbilityRouterGraphSchema::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), LinkedPin)));
	}
}

EGraphType UEdAbilityRouterGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return EGraphType::GT_StateMachine;
}

// Graph에 TNode 타입 진입 노드가 이미 있으면 그것을, 없으면 만들어 반환한다.
// Root와 GlobalSecondChance는 형제 클래스라 Cast<TNode>로 서로를 잡지 않는다.
// 멱등하므로 신규 그래프 생성 시와 저장 시(Rebuild) 모두 같은 함수를 호출할 수 있다.
template <typename TNode>
static UOverdriveAbilityRouterNode* EnsureRouterEntryNode(UEdGraph& Graph, UOverdriveAbilityRouterGraph* RouterGraph, int32 PosY)
{
	for (UEdGraphNode* Node : Graph.Nodes)
	{
		const UEdAbilityRouterGraphNode* EdRouterNode = Cast<UEdAbilityRouterGraphNode>(Node);
		if (EdRouterNode == nullptr)
		{
			continue;
		}

		if (TNode* ExistingNode = Cast<TNode>(EdRouterNode->RouterNode))
		{
			return ExistingNode;
		}
	}

	UEdAbilityRouterGraphNode* EntryEdNode = NewObject<UEdAbilityRouterGraphNode>(&Graph, NAME_None, RF_Transactional);
	EntryEdNode->RouterNode = NewObject<TNode>(EntryEdNode, NAME_None, RF_Transactional);
	EntryEdNode->RouterNode->RouterGraph = RouterGraph;

	EntryEdNode->CreateNewGuid();
	EntryEdNode->PostPlacedNewNode();
	EntryEdNode->AllocateDefaultPins();

	EntryEdNode->NodePosX = 0;
	EntryEdNode->NodePosY = PosY;

	Graph.AddNode(EntryEdNode, true, false);

	return EntryEdNode->RouterNode;
}

void UEdAbilityRouterGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UOverdriveAbilityRouterGraph* RouterGraph = CastChecked<UOverdriveAbilityRouterGraph>(Graph.GetOuter());

	RouterGraph->RootRouterNode = EnsureRouterEntryNode<UOverdriveAbilityRouterNode_Root>(Graph, RouterGraph, 0);
	RouterGraph->GlobalSecondChanceRouterNode = EnsureRouterEntryNode<UOverdriveAbilityRouterNode_GlobalSecondChance>(Graph, RouterGraph, 250);
}

void UEdAbilityRouterGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UOverdriveAbilityRouterGraph* RouterGraph = CastChecked<UOverdriveAbilityRouterGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());

	const UEdGraphPin* FromPin = ContextMenuBuilder.FromPin;
	TSubclassOf<UOverdriveAbilityRouterNode> NodeClass = UOverdriveAbilityRouterNode::StaticClass();
	TSubclassOf<UOverdriveAbilityRouterNode> RootNodeClass = UOverdriveAbilityRouterNode_Root::StaticClass();
	TSubclassOf<UOverdriveAbilityRouterNode> GlobalSecondChanceNodeClass = UOverdriveAbilityRouterNode_GlobalSecondChance::StaticClass();

	const FText AddToolTip = LOCTEXT("AddAbilityRouterNodeTooltip", "Add node here");

	for (TObjectIterator<UClass> It; It; ++It)
	{
		// 진입점 노드(Root/GlobalSecondChance)는 그래프당 하나뿐이라 사용자가 직접 만들 수 없다.
		if (!IsValid(*It) || !It->IsChildOf(NodeClass) || It->HasAnyClassFlags(CLASS_Abstract)
			|| *It == RootNodeClass || *It == GlobalSecondChanceNodeClass)
		{
			continue;
		}

		FString ClassName = It->GetName();
		if (ClassName.StartsWith(TEXT("SKEL_")) || ClassName.StartsWith(TEXT("REINST_")))
		{
			continue;
		}

		FText Description = It->GetDefaultObject<UOverdriveAbilityRouterNode>()->ContextMenuName;

		TSharedPtr<FEdAbilityRouterGraphSchemaAction_NewNode> NewNodeAction = 
			MakeShareable<FEdAbilityRouterGraphSchemaAction_NewNode>(new FEdAbilityRouterGraphSchemaAction_NewNode(LOCTEXT("AddRouterNodeActionCategory","Add Router Node"), Description, AddToolTip, 0));
		
		UClass* EdNodeClass = UEdAbilityRouterGraphNode::StaticClass();
		if (It->IsChildOf(UOverdriveAbilityRouterNode_Proxy::StaticClass()))
		{
			EdNodeClass = UEdAbilityRouterGraphNode_Proxy::StaticClass();
		}

		UEdAbilityRouterGraphNode* NewEdRouterNode = NewObject<UEdAbilityRouterGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, EdNodeClass);
		NewEdRouterNode->RouterNode = NewObject<UOverdriveAbilityRouterNode>(NewEdRouterNode, *It);
		NewEdRouterNode->RouterNode->RouterGraph = RouterGraph;

		NewNodeAction->NodeTemplate = NewEdRouterNode;

		// 액션을 실제 메뉴에 등록해야 우클릭 메뉴에 노출된다.
		ContextMenuBuilder.AddAction(NewNodeAction);
	}
}

void UEdAbilityRouterGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context->Pin)
	{
		FToolMenuSection& Section = Menu->AddSection("AbilityRouterGraphSchemaNodeActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));

		if (Context->Pin->LinkedTo.Num() > 0)
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);

			if (Context->Pin->LinkedTo.Num() > 1)
			{
				Section.AddSubMenu(
					"BreakLinkTo",
					LOCTEXT("BreakLinkTo", "Break Link To..."),
					LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
					FNewToolMenuDelegate::CreateUObject((UEdAbilityRouterGraphSchema* const)this, &UEdAbilityRouterGraphSchema::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(Context->Pin)));
			}
			else
			{
				const_cast<UEdAbilityRouterGraphSchema*>(this)->GetBreakLinkToSubMenuActions(Menu, const_cast<UEdGraphPin*>(Context->Pin));
			}
		}
	}
	if (const UEdAbilityRouterGraphNode* RouterNode = Cast<UEdAbilityRouterGraphNode>(Context->Node); RouterNode)
	{
		FToolMenuSection& Section = Menu->AddSection("AbilityRouterGraphSchemaNodeActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));
		
		// 진입점 노드(Root/GlobalSecondChance)는 삭제·복제할 수 없으므로 해당 메뉴를 숨긴다.
		const bool bIsEntryNode = Cast<UOverdriveAbilityRouterNode_Root>(RouterNode->RouterNode) != nullptr
			|| Cast<UOverdriveAbilityRouterNode_GlobalSecondChance>(RouterNode->RouterNode) != nullptr;

		if (!bIsEntryNode)
		{
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		}

		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}


	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UEdAbilityRouterGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A == nullptr || B == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorNullPin", "Not a valid pin"));
	}

	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameDirection", "Can't connect two pins of the same direction"));
	}

	const UEdGraphPin* Out = nullptr;
	const UEdGraphPin* In = nullptr;
	SortPinsByDirection(A, B, Out, In);

	UEdAbilityRouterGraphNode* EdOutNode = Cast<UEdAbilityRouterGraphNode>(Out->GetOwningNode());
	UEdAbilityRouterGraphNode* EdInNode = Cast<UEdAbilityRouterGraphNode>(In->GetOwningNode());

	if (EdOutNode == nullptr || EdInNode == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UEdAbilityRouterGraphNode"));
	}

	if (EdOutNode->RouterNode == nullptr || EdInNode->RouterNode == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorNoRouterNode", "Node has no router node"));
	}

	// 이미 이어진 두 노드를 다시 잇지 않는다. 엣지가 중복되면 Rebuild가 ChildrenNodes에 같은
	// 노드를 두 번 넣고, EdgeMap이 먼저 만든 엣지를 덮어써 조건 평가에서 통째로 누락된다.
	if (AreNodesConnected(EdOutNode, EdInNode))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorDuplicateEdge", "These nodes are already connected"));
	}

	FText ErrorMessage;
	if (!EdOutNode->RouterNode->CanCreateConnectionTo(EdInNode->RouterNode, ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}

	if (!EdInNode->RouterNode->CanCreateConnectionFrom(EdOutNode->RouterNode, ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));

}

bool UEdAbilityRouterGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	// 엔진은 드래그 순서 그대로 핀을 넘긴다. CanCreateConnection과 같은 기준으로 다시 정렬한다.
	const UEdGraphPin* Out = nullptr;
	const UEdGraphPin* In = nullptr;
	SortPinsByDirection(A, B, Out, In);

	UEdAbilityRouterGraphNode* EdOutNode = Cast<UEdAbilityRouterGraphNode>(Out->GetOwningNode());
	UEdAbilityRouterGraphNode* EdInNode = Cast<UEdAbilityRouterGraphNode>(In->GetOwningNode());

	if (EdOutNode == nullptr || EdInNode == nullptr)
	{
		return false;
	}

	FVector2f InitPos((EdOutNode->NodePosX + EdInNode->NodePosX) / 2, (EdOutNode->NodePosY + EdInNode->NodePosY) / 2);

	UEdAbilityRouterGraphEdge* NewEdEdge = NewObject<UEdAbilityRouterGraphEdge>(EdOutNode->GetGraph());
	NewEdEdge->RouterEdge = NewObject<UOverdriveAbilityRouterEdge>(NewEdEdge);

	FEdAbilityRouterGraphSchemaAction_NewEdge NewEdgeAction;
	NewEdgeAction.EdgeTemplate = NewEdEdge;
	NewEdgeAction.PerformAction(EdOutNode->GetGraph(), nullptr, InitPos, false);

	NewEdEdge->CreateConnections(EdOutNode, EdInNode);

	return true;
}

void UEdAbilityRouterGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UEdAbilityRouterGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UEdAbilityRouterGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

void UEdAbilityRouterGraphSchema::SortPinsByDirection(const UEdGraphPin* A, const UEdGraphPin* B, const UEdGraphPin*& OutPin, const UEdGraphPin*& InPin)
{
	const bool bAIsOutput = (A->Direction == EGPD_Output);

	OutPin = bAIsOutput ? A : B;
	InPin = bAIsOutput ? B : A;
}

bool UEdAbilityRouterGraphSchema::AreNodesConnected(const UEdAbilityRouterGraphNode* StartNode, const UEdAbilityRouterGraphNode* EndNode)
{
	for (const UEdGraphPin* LinkedPin : StartNode->GetOutputPin()->LinkedTo)
	{
		const UEdGraphNode* ChildNode = LinkedPin->GetOwningNode();

		// 보통은 엣지 노드를 거쳐 자식에 닿는다. 엣지 없이 직접 이어진 경우도 함께 본다.
		if (const UEdAbilityRouterGraphEdge* EdEdge = Cast<UEdAbilityRouterGraphEdge>(ChildNode))
		{
			ChildNode = EdEdge->GetEndNode();
		}

		if (ChildNode == EndNode)
		{
			return true;
		}
	}

	return false;
}

FConnectionDrawingPolicy* UEdAbilityRouterGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_AbilityRouterGraph(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UEdAbilityRouterGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

UEdGraphPin* UEdAbilityRouterGraphSchema::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdAbilityRouterGraphNode* EdNode = Cast<UEdAbilityRouterGraphNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
		return EdNode->GetOutputPin();
	case EGPD_Output:
		return EdNode->GetInputPin();
	default:
		return nullptr;
	}
}

bool UEdAbilityRouterGraphSchema::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdAbilityRouterGraphNode>(InTargetNode) != nullptr;
}

bool UEdAbilityRouterGraphSchema::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdAbilityRouterGraphSchema::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdAbilityRouterGraphSchema::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

/////////////////////////// FConnectionDrawingPolicy_AbilityRouterGraph ///////////////////////////////////
void FConnectionDrawingPolicy_AbilityRouterGraph::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}
}

void FConnectionDrawingPolicy_AbilityRouterGraph::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{	
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FConnectionDrawingPolicy_AbilityRouterGraph::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2f StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2f EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2f SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2f StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2f EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

void FConnectionDrawingPolicy_AbilityRouterGraph::DrawSplineWithArrow(const FVector2f& StartPoint, const FVector2f& EndPoint, const FConnectionParams& Params)
{
	const FVector2f& P0 = Params.bUserFlag1 ? EndPoint : StartPoint;
	const FVector2f& P1 = Params.bUserFlag1 ? StartPoint : EndPoint;

	Internal_DrawLineWithArrow(P0, P1, Params);
}

void FConnectionDrawingPolicy_AbilityRouterGraph::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2f& StartPoint, const FVector2f& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	}
	else
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
	}
}

FVector2f FConnectionDrawingPolicy_AbilityRouterGraph::ComputeSplineTangent(const FVector2f& Start, const FVector2f& End) const
{
	const FVector2f Delta = End - Start;
	const FVector2f NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}

void FConnectionDrawingPolicy_AbilityRouterGraph::DetermineLinkGeometry(FArrangedChildren& ArrangedNodes, TSharedRef<SWidget>& OutputPinWidget, UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FArrangedWidget*& StartWidgetGeometry, FArrangedWidget*& EndWidgetGeometry)
{
	if (UEdAbilityRouterGraphEdge* EdgeNode = Cast<UEdAbilityRouterGraphEdge>(InputPin->GetOwningNode()))
	{
		UEdAbilityRouterGraphNode* Start = EdgeNode->GetStartNode();
		UEdAbilityRouterGraphNode* End = EdgeNode->GetEndNode();
		if (Start != nullptr && End != nullptr)
		{
			int32* StartNodeIndex = NodeWidgetMap.Find(Start);
			int32* EndNodeIndex = NodeWidgetMap.Find(End);
			if (StartNodeIndex != nullptr && EndNodeIndex != nullptr)
			{
				StartWidgetGeometry = &(ArrangedNodes[*StartNodeIndex]);
				EndWidgetGeometry = &(ArrangedNodes[*EndNodeIndex]);
			}
		}
	}
	else
	{
		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);

		if (TSharedPtr<SGraphPin>* pTargetWidget = PinToPinWidgetMap.Find(InputPin))
		{
			TSharedRef<SGraphPin> InputWidget = (*pTargetWidget).ToSharedRef();
			EndWidgetGeometry = PinGeometries->Find(InputWidget);
		}
	}
}

void FConnectionDrawingPolicy_AbilityRouterGraph::Internal_DrawLineWithArrow(const FVector2f& StartAnchorPoint, const FVector2f& EndAnchorPoint, const FConnectionParams& Params)
{	//@TODO: Should this be scaled by zoom factor?
	const float LineSeparationAmount = 4.5f;

	const FVector2f DeltaPos = EndAnchorPoint - StartAnchorPoint;
	const FVector2f UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2f Normal = FVector2f(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2f DirectionBias = Normal * LineSeparationAmount;
	const FVector2f LengthBias = ArrowRadius.X * UnitDelta;
	const FVector2f StartPoint = StartAnchorPoint + DirectionBias + LengthBias;
	const FVector2f EndPoint = EndAnchorPoint + DirectionBias - LengthBias;

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, Params);

	// Draw the arrow
	const FVector2f ArrowDrawPos = EndPoint - ArrowRadius;
	const float AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2f>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
	);
}


#undef LOCTEXT_NAMESPACE

