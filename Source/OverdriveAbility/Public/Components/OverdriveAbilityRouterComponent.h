// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "OverdriveAbilityRouterComponent.generated.h"

class UOverdriveAbilityRouterNode;
class UOverdriveAbilityRouterGraph;
class UOverdriveAbilitySystemComponent;
class UGameplayAbility;
class UAbilitySystemComponent;
class UInputAction;
class UEnhancedInputComponent;
class APawn;
class AController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOverdriveAbilityInputReactionDelegate, bool /*bPressed*/, const FGameplayTag& /*InputTypeTag*/);

// 에디터에서 InputAction ↔ 라우터 InputType 태그를 묶는 행(row).
USTRUCT(BlueprintType)
struct OVERDRIVEABILITY_API FOverdriveAbilityRouterInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "OverdriveAbility|Input")
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "OverdriveAbility|Input", meta = (Categories = "OverdriveAbility.AbilityRouter.InputType"))
	FGameplayTag InputTypeTag;
};

// (bPressed, InputType)을 TMap 키로 쓰기 위한 경량 구조체. 리플렉션 불필요(BP 미노출).
struct FOverdriveAbilityInputKey
{
	bool bPressed = false;
	FGameplayTag InputTypeTag;

	bool operator==(const FOverdriveAbilityInputKey& Other) const
	{
		return bPressed == Other.bPressed && InputTypeTag == Other.InputTypeTag;
	}

	friend uint32 GetTypeHash(const FOverdriveAbilityInputKey& Key)
	{
		return HashCombine(GetTypeHash(Key.bPressed ? 1 : 0), GetTypeHash(Key.InputTypeTag));
	}
};

// 매칭 실패한 Press 입력을 짧은 시간 보관하는 단일 슬롯. 리플렉션 불필요(값 타입, BP 미노출).
struct FOverdriveAbilityBufferedInput
{
	FGameplayTag InputTag;
	double Timestamp = 0.0;

	bool IsSet() const { return InputTag.IsValid(); }
	void Reset()
	{
		InputTag = FGameplayTag();
		Timestamp = 0.0;
	}
};

/**
 *
 */
UCLASS()
class OVERDRIVEABILITY_API UOverdriveAbilityRouterComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:
	UOverdriveAbilityRouterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer.Reset();
		TagContainer.AppendTags(StateTagContainer);
	}

	// bPressed: true = Press, false = Release.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility", meta = (AutoCreateRefTerm = "InputTypeTag"))
	void AbilityInputActionEvent(
		bool bPressed,
		UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) const FGameplayTag& InputTypeTag);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|InputBlock", meta = (AutoCreateRefTerm = "InputTypeTag"))
	void BlockInputTag(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) const FGameplayTag& InputTypeTag);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|InputBlock", meta = (AutoCreateRefTerm = "InputTypeTag"))
	void UnblockInputTag(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) const FGameplayTag& InputTypeTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OverdriveAbility|InputBlock", meta = (AutoCreateRefTerm = "InputTypeTag"))
	bool IsInputTypeBlocked(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) const FGameplayTag& InputTypeTag) const;

	// 상태 태그 추가/제거. 엣지 ConditionQuery가 이 컨테이너로 콤보 창을 판정한다. 변경 시 버퍼 입력을 재평가.
	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|State", meta = (AutoCreateRefTerm = "StateTag"))
	void AddStateTag(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.State")) const FGameplayTag& StateTag);

	UFUNCTION(BlueprintCallable, Category = "OverdriveAbility|State", meta = (AutoCreateRefTerm = "StateTag"))
	void RemoveStateTag(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.State")) const FGameplayTag& StateTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OverdriveAbility|State", meta = (AutoCreateRefTerm = "StateTag"))
	bool HasStateTag(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.State")) const FGameplayTag& StateTag) const;

	// (bPressed, InputType) 조합에 대한 입력 반응 델리게이트를 반환(없으면 생성). 바인딩 시 해당 입력은 그래프 순회 대신 델리게이트만 실행.
	FOverdriveAbilityInputReactionDelegate& GetInputReactionDelegate(bool bPressed, const FGameplayTag& InputTypeTag);

	// 해당 InputType이 현재 눌려 있는(Press) 상태인지 반환. 태스크의 즉시-Broadcast 판정 등에 사용.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OverdriveAbility", meta = (AutoCreateRefTerm = "InputTypeTag"))
	bool IsInputPressed(UPARAM(meta = (Categories = "OverdriveAbility.AbilityRouter.InputType")) const FGameplayTag& InputTypeTag) const;

	UAbilitySystemComponent* GetAbilitySystem() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// 매칭 파이프라인(block → reaction delegate → 그래프 순회·발동). 어빌리티가 실제 발동되면 true.
	bool ProcessInput(bool bPressed, const FGameplayTag& InputTypeTag);

	// StartNode의 자식 중 엣지 조건을 만족하는 노드를 찾아 Proxy 체인까지 해소한 결과를 반환. 없으면 nullptr.
	// ProcessInput이 로컬(CurrentNode)과 전역 폴백(GlobalSecondChance) 두 시작점으로 각각 호출한다.
	// InStateTags는 ProcessInput이 한 번 만들어 두 호출에 공유하는 병합 태그(StateTagContainer + ASC 소유 태그).
	const UOverdriveAbilityRouterNode* FindNodeToActivate(const UOverdriveAbilityRouterNode* StartNode, bool bPressed, const FGameplayTag& InputTypeTag, const FGameplayTagContainer& InStateTags) const;

	// 폰 재시작(=InputComponent 생성 직후) / 빙의 변경 훅. 다이나믹 델리게이트라 UFUNCTION 필수.
	UFUNCTION()
	void OnPawnRestarted(APawn* Pawn);

	UFUNCTION()
	void OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void BindInputActions();
	void UnbindInputActions();

	// 바인딩이 끊긴 뒤 남는 Press/버퍼 상태를 비운다.
	void ResetPendingInputState();

	// BindAction 페이로드로 (bPressed, InputTypeTag)를 받아 AbilityInputActionEvent로 넘긴다.
	void HandleAbilityInputAction(bool bPressed, FGameplayTag InputTypeTag);

	UPROPERTY(EditAnywhere, Category = "OverdriveAbility|Input", meta = (TitleProperty = "InputTypeTag"))
	TArray<FOverdriveAbilityRouterInputAction> InputActions;

	// 빙의 해제 시 엔진이 먼저 파괴하므로 약참조로 잡는다.
	TWeakObjectPtr<UEnhancedInputComponent> BoundInputComponent;
	TArray<uint32> BoundBindingHandles;

	// 보관 중인 Press를 재평가한다. 만료 시 폐기, 매칭 성공 시 소비, 실패 시 만료까지 보존.
	void FlushInputBuffer();

	// 입력 버퍼 유지 시간(초).
	UPROPERTY(EditAnywhere, Category = "OverdriveAbility|InputBuffer", meta = (ClampMin = "0.0"))
	float InputBufferWindow = 0.2f;

	// 매칭 실패한 마지막 Press 단일 슬롯.
	FOverdriveAbilityBufferedInput BufferedInput;

	TWeakObjectPtr<UAbilitySystemComponent> WeakAbilitySystem;
	TWeakObjectPtr<UGameplayAbility> CurrentAbilityInstance;

	FGameplayTagContainer StateTagContainer;

	// 현재 Press 상태인 InputType들의 집합. 중복 Press/Release를 ensure로 거르고 즉시-Broadcast 판정에 사용.
	FGameplayTagContainer PressedInputTags;

	FGameplayTagCountContainer InputBlockTagContainer;

	TMap<FOverdriveAbilityInputKey, FOverdriveAbilityInputReactionDelegate> InputReactionDelegates;

	TWeakObjectPtr<const UOverdriveAbilityRouterNode> CurrentNode;
	FGameplayAbilitySpecHandle CurrentSpecHandle;

	UPROPERTY(EditAnywhere)
	const TObjectPtr<UOverdriveAbilityRouterGraph> AbilityGraph;

	FDelegateHandle OnCurrentAbilityEndedHandle;

	UFUNCTION()
	void OnCurrentAbilityEnded(UGameplayAbility* EndedAbility);

	void InitializeRouterComponent();

	void InitializeAbilitySystem();
};

