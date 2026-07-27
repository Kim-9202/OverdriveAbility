// Fill out your copyright notice in the Description page of Project Settings.


#include "Customizations/OverdriveAbilityRouterNode_ProxyDetails.h"

#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "ScopedTransaction.h"

#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "OverdriveAbilityRouterNode_ProxyDetails"

TSharedRef<IDetailCustomization> FOverdriveAbilityRouterNode_ProxyDetails::MakeInstance()
{
	return MakeShared<FOverdriveAbilityRouterNode_ProxyDetails>();
}

void FOverdriveAbilityRouterNode_ProxyDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// TargetRouterNode 기본 위젯은 선택 상태와 무관하게 항상 숨긴다. 편집은 아래 커스텀 콤보박스로만 한다.
	// 런타임 노드를 직접 커스터마이즈하므로 TargetRouterNode가 최상위 프로퍼티 → FName+Class 오버로드로 확실히 숨겨진다.
	DetailBuilder.HideProperty(TEXT("TargetRouterNode"), UOverdriveAbilityRouterNode_Proxy::StaticClass());

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// 다중 선택 시에는 커스텀 콤보를 만들지 않는다(기본 위젯은 위에서 이미 숨김).
	if (ObjectsBeingCustomized.Num() != 1)
	{
		return;
	}

	ProxyNode = Cast<UOverdriveAbilityRouterNode_Proxy>(ObjectsBeingCustomized[0].Get());
	if (!ProxyNode.IsValid())
	{
		return;
	}

	BuildOptions();

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(TEXT("AbilityRouter"));
	Category.AddCustomRow(LOCTEXT("TargetRouterNodeSearch", "Target Router Node"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("TargetRouterNodeLabel", "Target Router Node"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(250.0f)
		[
			SNew(SComboBox<TSharedPtr<FRouterNodeOption>>)
			.OptionsSource(&Options)
			.InitiallySelectedItem(FindOptionForCurrentTarget())
			.OnGenerateWidget(this, &FOverdriveAbilityRouterNode_ProxyDetails::MakeOptionWidget)
			.OnSelectionChanged(this, &FOverdriveAbilityRouterNode_ProxyDetails::OnSelectionChanged)
			[
				SNew(STextBlock)
				.Text(this, &FOverdriveAbilityRouterNode_ProxyDetails::GetCurrentTargetText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void FOverdriveAbilityRouterNode_ProxyDetails::BuildOptions()
{
	Options.Reset();

	// 맨 앞에 비우기(None) 항목.
	TSharedRef<FRouterNodeOption> NoneOption = MakeShared<FRouterNodeOption>();
	NoneOption->Node = nullptr;
	NoneOption->DisplayText = LOCTEXT("NoneOption", "(None)");
	Options.Add(NoneOption);

	UOverdriveAbilityRouterNode* SelfRouterNode = ProxyNode.Get();
	if (SelfRouterNode == nullptr)
	{
		return;
	}

	const UOverdriveAbilityRouterGraph* RouterGraph = SelfRouterNode->GetAbilityRouterGraph();
	if (RouterGraph == nullptr)
	{
		return;
	}

	for (UOverdriveAbilityRouterNode* RouterNode : RouterGraph->AllRouterNodes)
	{
		if (RouterNode == nullptr)
		{
			continue;
		}

		// 자기 자신 제외 + 다른 Proxy 제외(Proxy→Proxy 체인 방지).
		if (RouterNode == SelfRouterNode || RouterNode->IsA<UOverdriveAbilityRouterNode_Proxy>())
		{
			continue;
		}

		TSharedRef<FRouterNodeOption> Option = MakeShared<FRouterNodeOption>();
		Option->Node = RouterNode;
		Option->DisplayText = RouterNode->GetNodeTitle();
		Options.Add(Option);
	}
}

TSharedPtr<FOverdriveAbilityRouterNode_ProxyDetails::FRouterNodeOption> FOverdriveAbilityRouterNode_ProxyDetails::FindOptionForCurrentTarget() const
{
	const UOverdriveAbilityRouterNode* CurrentTarget = ProxyNode.IsValid() ? ProxyNode->GetTargetRouterNode() : nullptr;

	for (const TSharedPtr<FRouterNodeOption>& Option : Options)
	{
		if (Option.IsValid() && Option->Node.Get() == CurrentTarget)
		{
			return Option;
		}
	}

	// 못 찾으면 None(첫 항목).
	return Options.Num() > 0 ? Options[0] : nullptr;
}

FText FOverdriveAbilityRouterNode_ProxyDetails::GetCurrentTargetText() const
{
	if (!ProxyNode.IsValid())
	{
		return LOCTEXT("NoneOption", "(None)");
	}

	UOverdriveAbilityRouterNode* CurrentTarget = ProxyNode->GetTargetRouterNode();
	return CurrentTarget ? CurrentTarget->GetNodeTitle() : LOCTEXT("NoneOption", "(None)");
}

TSharedRef<SWidget> FOverdriveAbilityRouterNode_ProxyDetails::MakeOptionWidget(TSharedPtr<FRouterNodeOption> InOption) const
{
	return SNew(STextBlock)
		.Text(InOption.IsValid() ? InOption->DisplayText : FText::GetEmpty())
		.Font(IDetailLayoutBuilder::GetDetailFont());
}

void FOverdriveAbilityRouterNode_ProxyDetails::OnSelectionChanged(TSharedPtr<FRouterNodeOption> NewSelection, ESelectInfo::Type SelectInfo)
{
	// 위젯 초기화 시점의 프로그램적 선택(Direct)은 무시하고 사용자 선택만 반영.
	if (SelectInfo == ESelectInfo::Direct || !NewSelection.IsValid() || !ProxyNode.IsValid())
	{
		return;
	}

	// PropertyHandle SetValue 대신 ProxyNode 내부(TargetRouterNode·CachedNodeTitle)를 직접 수정한다.
	const FScopedTransaction Transaction(LOCTEXT("SetProxyTargetRouterNode", "Set Proxy Target Router Node"));
	ProxyNode->SetTargetRouterNode(NewSelection->Node.Get()); // None 항목이면 nullptr

	// 노드 제목이 즉시 다시 그려지도록 시각화 캐시를 비운다.
	if (UOverdriveAbilityRouterGraph* RouterGraph = ProxyNode->GetAbilityRouterGraph())
	{
		if (RouterGraph->EdGraph)
		{
			RouterGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
		}
	}
}

#undef LOCTEXT_NAMESPACE
