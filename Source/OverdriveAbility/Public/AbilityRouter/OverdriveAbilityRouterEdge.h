// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityRouterEdge.generated.h"

class UOverdriveAbilityRouterNode;
class UOverdriveAbilityRouterGraph;

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

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RouterCondition")
	bool IsPressed() const { return bPressed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RouterCondition")
	const FGameplayTag& GetInputTypeTag() const { return InputTypeTag; }

	const UOverdriveAbilityRouterGraph* GetRouterGraph() const { return RouterGraph; }
	const UOverdriveAbilityRouterNode* GetStartNode() const { return StartNode; }
	const UOverdriveAbilityRouterNode* GetEndNode() const { return EndNode; }

#if WITH_EDITOR
	void SetRouterGrpah(UOverdriveAbilityRouterGraph* NewRouterGraph) { RouterGraph = NewRouterGraph; }
	void SetStartNode(UOverdriveAbilityRouterNode* NewStartNode) { StartNode = NewStartNode; }
	void SetEndNode(UOverdriveAbilityRouterNode* NewEndNode) { EndNode = NewEndNode; }
#endif

	bool CanEnterEndNode(bool bInPressed, const FGameplayTag& InInputTypeTag, const FGameplayTagContainer& InStateTags) const;
};

