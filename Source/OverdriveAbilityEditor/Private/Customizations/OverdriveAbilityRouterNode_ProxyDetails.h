// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/WeakObjectPtr.h"
#include "Types/SlateEnums.h"

class SWidget;
class IDetailLayoutBuilder;
class UOverdriveAbilityRouterNode;
class UOverdriveAbilityRouterNode_Proxy;

/**
 * 런타임 Proxy 노드(UOverdriveAbilityRouterNode_Proxy)용 디테일 커스터마이징.
 * 디테일 패널은 EdNode가 아니라 런타임 노드를 직접 표시한다(엔진 ObjectTreeGraph 관례).
 * 기본 오브젝트 피커는 같은 그래프의 형제 노드를 나열하지 못하므로,
 * TargetRouterNode를 "같은 그래프의 형제 노드 목록" 콤보박스로 대체한다.
 * 후보에서 자기 자신과 다른 Proxy 노드는 제외한다.
 */
class FOverdriveAbilityRouterNode_ProxyDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	/** 콤보박스 한 항목. 노드(약참조)와 표시 텍스트. nullptr 노드는 "(None)" 항목. */
	struct FRouterNodeOption
	{
		TWeakObjectPtr<UOverdriveAbilityRouterNode> Node;
		FText DisplayText;
	};

	/** 런타임 그래프의 노드들에서 후보 목록을 만든다(자기 자신·Proxy 제외, 맨 앞에 None). */
	void BuildOptions();

	/** 현재 TargetRouterNode에 해당하는 옵션을 찾아 콤보 선택값을 맞춘다. */
	TSharedPtr<FRouterNodeOption> FindOptionForCurrentTarget() const;

	FText GetCurrentTargetText() const;

	TSharedRef<SWidget> MakeOptionWidget(TSharedPtr<FRouterNodeOption> InOption) const;

	void OnSelectionChanged(TSharedPtr<FRouterNodeOption> NewSelection, ESelectInfo::Type SelectInfo);

	/** Proxy 노드(약참조). 커스터마이징 수명은 패널 수명에 종속. */
	TWeakObjectPtr<UOverdriveAbilityRouterNode_Proxy> ProxyNode;

	/** 콤보박스 항목 소스. */
	TArray<TSharedPtr<FRouterNodeOption>> Options;
};
