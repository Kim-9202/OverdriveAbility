// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityRouterEdge.generated.h"

class UOverdriveAbilityRouterNode;
class UOverdriveAbilityRouterGraph;
class UOverdriveAbilityRouterComponent;
class UOverdriveAbilityRouterEdgeCondition;

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterEdge : public UObject
{
	GENERATED_BODY()
	
public:
	UOverdriveAbilityRouterEdge(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterGraph> RouterGraph;

	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterNode> StartNode;

	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterNode> EndNode;

	// true = Press, false = Release. 이 엣지를 통과하는 데 필요한 입력 액션 종류.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RouterCondition", meta = (AllowPrivateAccess = "true"))
	bool bPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RouterCondition", meta = (AllowPrivateAccess = "true", Categories = "OverdriveAbility.AbilityRouter.InputType"))
	FGameplayTag InputTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RouterCondition", meta = (AllowPrivateAccess = "true"))
	FGameplayTagQuery ConditionQuery;

	// 태그로 표현되지 않는 추가 조건. 전부 true를 반환해야 엣지를 통과한다(빈 배열이면 통과).
	UPROPERTY(EditAnywhere, Instanced, Category = "RouterCondition", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UOverdriveAbilityRouterEdgeCondition>> EdgeConditions;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RouterCondition")
	bool IsPressed() const { return bPressed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RouterCondition")
	const FGameplayTag& GetInputTypeTag() const { return InputTypeTag; }

	const UOverdriveAbilityRouterGraph* GetRouterGraph() const { return RouterGraph; }
	const UOverdriveAbilityRouterNode* GetStartNode() const { return StartNode; }
	const UOverdriveAbilityRouterNode* GetEndNode() const { return EndNode; }

#if WITH_EDITOR
	void SetRouterGraph(UOverdriveAbilityRouterGraph* NewRouterGraph) { RouterGraph = NewRouterGraph; }
	void SetStartNode(UOverdriveAbilityRouterNode* NewStartNode) { StartNode = NewStartNode; }
	void SetEndNode(UOverdriveAbilityRouterNode* NewEndNode) { EndNode = NewEndNode; }
#endif

	// InStateTags = 라우터의 StateTagContainer + ASC 소유 태그(호출부에서 병합해 전달).
	// InRouterComponent는 판정에 라우터 런타임 상태가 필요할 때를 위한 확장 컨텍스트(현재 기본 구현은 미사용).
	bool CanEnterEndNode(const UOverdriveAbilityRouterComponent* InRouterComponent, bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const;
};

