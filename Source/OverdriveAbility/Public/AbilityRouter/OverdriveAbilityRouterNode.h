// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "OverdriveAbilityRouterNode.generated.h"

class UGameplayAbility;
class UOverdriveAbilityRouterEdge;
class UOverdriveAbilityRouterGraph;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class OVERDRIVEABILITY_API UOverdriveAbilityRouterNode : public UObject
{
	GENERATED_BODY()
	
public:
	UOverdriveAbilityRouterNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterGraph> RouterGraph;

	UPROPERTY()
	TArray<TObjectPtr<UOverdriveAbilityRouterNode>> ParentNodes;

	UPROPERTY()
	TArray<TObjectPtr<UOverdriveAbilityRouterNode>> ChildrenNodes;

	UPROPERTY()
	TMap<TObjectPtr<UOverdriveAbilityRouterNode>, TObjectPtr<UOverdriveAbilityRouterEdge>> EdgeMap;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "AbilityRouter")
	TSubclassOf<UGameplayAbility> GetAbilityToActivate() const;
	virtual TSubclassOf<UGameplayAbility> GetAbilityToActivate_Implementation() const {return nullptr;}

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AbilityRouter")
	UOverdriveAbilityRouterGraph* GetAbilityRouterGraph() const 
	{
		return RouterGraph;
	}

	bool IsLeafNode() const { return ChildrenNodes.IsEmpty(); }

	void GetParentNodes(TArray<const UOverdriveAbilityRouterNode*>& OutArray) const;

	void GetChildrenNodes(TArray<const UOverdriveAbilityRouterNode*>& OutArray) const;

	const UOverdriveAbilityRouterEdge* GetRouterEdge(const UOverdriveAbilityRouterNode* OtherNode) const;

	virtual const UOverdriveAbilityRouterNode* GetRouterNodeToActivate(const FGameplayTag& InInputTypeTag, bool bInPressed, const FGameplayTagContainer& InStateTags) const;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FLinearColor BackgroundColor;

	UPROPERTY()
	FText ContextMenuName;

	// 노드 제목 캐시. 값이 바뀌는 시점(각 노드의 PostEditChangeProperty 등)에 갱신한다. 디버깅용으로 멤버에 보관·직렬화하되, 디테일 패널에는 노출하지 않는다.
	UPROPERTY()
	FText CachedNodeTitle;
#endif

#if WITH_EDITOR
	// 캐시된 제목을 그대로 반환한다. 갱신은 각 노드가 값 변경 시점에 수행한다.
	FText GetNodeTitle() const { return CachedNodeTitle; }

	virtual bool CanCreateConnectionTo(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const;
	virtual bool CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const;
#endif

};

UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterNode_Root : public UOverdriveAbilityRouterNode
{
	GENERATED_BODY()

public:
	UOverdriveAbilityRouterNode_Root();

#if WITH_EDITOR
	virtual bool CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const;
#endif

};

/**
 * 두 번째 기회(전역 폴백) 진입점. CurrentNode 기준 순회가 실패했을 때 라우터가 여기서 다시 찾는다.
 * 어느 상태에서든 열려 있어야 하는 대쉬·점프 같은 어빌리티를 이 노드의 자식으로 매단다.
 * Root와 형제 관계다(상속 관계가 아니다).
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterNode_GlobalSecondChance : public UOverdriveAbilityRouterNode
{
	GENERATED_BODY()

public:
	UOverdriveAbilityRouterNode_GlobalSecondChance();

#if WITH_EDITOR
	// 진입점이므로 들어오는 연결을 받지 않는다(Root와 동일).
	virtual bool CanCreateConnectionFrom(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const override;
#endif

};

UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterNode_Simple : public UOverdriveAbilityRouterNode
{
	GENERATED_BODY()

public:
	UOverdriveAbilityRouterNode_Simple();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityRouter", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> NodeAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityRouter", meta = (AllowPrivateAccess = "true"))
	FText NodeTitle;

protected:
	virtual TSubclassOf<UGameplayAbility> GetAbilityToActivate_Implementation() const override { return NodeAbility; }

#if WITH_EDITOR
	// NodeTitle 변경 시 CachedNodeTitle을 갱신한다.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterNode_Proxy : public UOverdriveAbilityRouterNode
{
	GENERATED_BODY()

public:
	UOverdriveAbilityRouterNode_Proxy();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityRouter", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOverdriveAbilityRouterNode> TargetRouterNode;

protected:
	virtual const UOverdriveAbilityRouterNode* GetRouterNodeToActivate(const FGameplayTag& InInputTypeTag, bool bInPressed, const FGameplayTagContainer& InStateTags) const override;

#if WITH_EDITOR
public:
	UOverdriveAbilityRouterNode* GetTargetRouterNode() const { return TargetRouterNode; }

	// TargetRouterNode와 CachedNodeTitle을 함께 직접 갱신한다(트랜잭션·Modify는 호출부 책임).
	void SetTargetRouterNode(UOverdriveAbilityRouterNode* InTarget);

protected:
	// TargetRouterNode 변경 시 CachedNodeTitle을 갱신한다.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual bool CanCreateConnectionTo(const UOverdriveAbilityRouterNode* Other, FText& ErrorMessage) const;

private:
	// CachedNodeTitle 갱신 로직(PostEditChangeProperty/SetTargetRouterNode 공용).
	void UpdateCachedNodeTitle();
#endif
};
