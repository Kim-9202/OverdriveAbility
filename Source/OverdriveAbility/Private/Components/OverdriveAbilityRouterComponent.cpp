// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OverdriveAbilityRouterComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityRouter/OverdriveAbilityRouterGraph.h"
#include "AbilityRouter/OverdriveAbilityRouterNode.h"
#include "AbilityRouter/OverdriveAbilityRouterEdge.h"
#include "AbilitySystemFinders/OverdriveAbilitySystemFinder.h"
#include "AbilitySystemFinders/OverdriveAbilitySystemFinder_Owner.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"

UOverdriveAbilityRouterComponent::UOverdriveAbilityRouterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemFinderClass = UOverdriveAbilitySystemFinder_Owner::StaticClass();
}

void UOverdriveAbilityRouterComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeRouterComponent();

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		OwnerPawn->ReceiveRestartedDelegate.AddDynamic(this, &UOverdriveAbilityRouterComponent::OnPawnRestarted);
		OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UOverdriveAbilityRouterComponent::OnPawnControllerChanged);

		// 컴포넌트가 Restart 이후에 붙은 경우(런타임 AddComponent 등) 보정.
		BindInputActions();
	}
}

void UOverdriveAbilityRouterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyAbilitySystemFinder();

	UnbindInputActions();

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		OwnerPawn->ReceiveRestartedDelegate.RemoveDynamic(this, &UOverdriveAbilityRouterComponent::OnPawnRestarted);
		OwnerPawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UOverdriveAbilityRouterComponent::OnPawnControllerChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void UOverdriveAbilityRouterComponent::OnPawnRestarted(APawn* Pawn)
{
	// PawnClientRestart 직후 호출되므로 이 시점에 InputComponent가 존재한다.
	BindInputActions();
}

void UOverdriveAbilityRouterComponent::OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (NewController == nullptr)
	{
		UnbindInputActions();
		return;
	}

	// 빙의는 됐지만 아직 Restart 전일 수 있다. 그 경우 BindInputActions가 조용히 빠져나간다.
	BindInputActions();
}

void UOverdriveAbilityRouterComponent::BindInputActions()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}

	UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(OwnerPawn->InputComponent);
	if (InputComponent == nullptr)
	{
		return; // 아직 생성 전이거나 로컬 컨트롤 폰이 아님(서버 AI/시뮬레이션 프록시).
	}

	if (BoundInputComponent.Get() == InputComponent)
	{
		return; // 이미 같은 컴포넌트에 바인딩됨.
	}

	UnbindInputActions(); // 다른 InputComponent에 남은 바인딩 정리

	for (const FOverdriveAbilityRouterInputAction& Entry : InputActions)
	{
		if (Entry.InputAction == nullptr || !Entry.InputTypeTag.IsValid())
		{
			continue;
		}

		BoundBindingHandles.Add(InputComponent->BindAction(Entry.InputAction, ETriggerEvent::Triggered, this,
			&UOverdriveAbilityRouterComponent::HandleAbilityInputAction, true, Entry.InputTypeTag).GetHandle());
		BoundBindingHandles.Add(InputComponent->BindAction(Entry.InputAction, ETriggerEvent::Completed, this,
			&UOverdriveAbilityRouterComponent::HandleAbilityInputAction, false, Entry.InputTypeTag).GetHandle());
		BoundBindingHandles.Add(InputComponent->BindAction(Entry.InputAction, ETriggerEvent::Canceled, this,
			&UOverdriveAbilityRouterComponent::HandleAbilityInputAction, false, Entry.InputTypeTag).GetHandle());
	}

	BoundInputComponent = InputComponent;
}

void UOverdriveAbilityRouterComponent::UnbindInputActions()
{
	// 빙의 해제 시 엔진이 InputComponent를 먼저 파괴하므로 Get()이 null일 수 있다(정상).
	if (UEnhancedInputComponent* InputComponent = BoundInputComponent.Get())
	{
		for (const uint32 Handle : BoundBindingHandles)
		{
			InputComponent->RemoveBindingByHandle(Handle);
		}
	}

	BoundBindingHandles.Reset();
	BoundInputComponent.Reset();

	ResetPendingInputState();
}

void UOverdriveAbilityRouterComponent::ResetPendingInputState()
{
	// 키를 누른 채 빙의가 풀리면 Release가 유실된다. 남은 Press 상태를 비우지 않으면
	// 재빙의 후 같은 키의 Press가 중복으로 판정돼 무시된다.
	PressedInputTags.Reset();
	BufferedInput.Reset();
}

void UOverdriveAbilityRouterComponent::HandleAbilityInputAction(bool bPressed, FGameplayTag InputTypeTag)
{
	// BindAction 페이로드는 값 타입으로 추론되므로 const FGameplayTag&를 받는
	// AbilityInputActionEvent를 직접 바인딩할 수 없다. 값 전달 어댑터가 필요하다.
	AbilityInputActionEvent(bPressed, InputTypeTag);
}

void UOverdriveAbilityRouterComponent::OnCurrentAbilityEnded(UGameplayAbility* EndedAbility)
{
	if (EndedAbility != CurrentAbilityInstance)
	{
		return;
	}

	CurrentNode.Reset();
	StateTagContainer.Reset();
}

void UOverdriveAbilityRouterComponent::InitializeRouterComponent()
{
	InitializeAbilitySystem();


}

void UOverdriveAbilityRouterComponent::SetAbilitySystemFinderClass(TSubclassOf<UOverdriveAbilitySystemFinder> InFinderClass)
{
	if (!ensure(InFinderClass))
	{
		return;
	}

	AbilitySystemFinderClass = InFinderClass;
}

void UOverdriveAbilityRouterComponent::InitializeAbilitySystem()
{
	if (!ensure(AbilitySystemFinderClass))
	{
		return;
	}

	AbilitySystemFinder = NewObject<UOverdriveAbilitySystemFinder>(this, AbilitySystemFinderClass);
	AbilitySystemFinder->WeakOwnerComponent = this;
	AbilitySystemFinder->RetryPeriod = AbilitySystemFindPeriod;
	AbilitySystemFinder->MaxAttemptCount = AbilitySystemFindMaxCount;
	AbilitySystemFinder->OnFound.BindUObject(this, &UOverdriveAbilityRouterComponent::HandleAbilitySystemFound);
	AbilitySystemFinder->OnFailed.BindUObject(this, &UOverdriveAbilityRouterComponent::HandleAbilitySystemFindFailed);

	AbilitySystemFinder->StartFind();
}

void UOverdriveAbilityRouterComponent::HandleAbilitySystemFound(UAbilitySystemComponent* FoundAbilitySystem)
{
	DestroyAbilitySystemFinder();

	WeakAbilitySystem = FoundAbilitySystem;
}

void UOverdriveAbilityRouterComponent::HandleAbilitySystemFindFailed()
{
	DestroyAbilitySystemFinder();

	ensureMsgf(false, TEXT("%hs::Can't Find AbilitySystem."), __FUNCTION__);
}

void UOverdriveAbilityRouterComponent::DestroyAbilitySystemFinder()
{
	if (!IsValid(AbilitySystemFinder))
	{
		return;
	}

	AbilitySystemFinder->StopFind();
	AbilitySystemFinder->MarkAsGarbage();
	AbilitySystemFinder = nullptr;
}

void UOverdriveAbilityRouterComponent::AbilityInputActionEvent(bool bPressed, const FGameplayTag& InputTypeTag)
{
	// 물리 입력 상태 추적 + 상태와 모순되는 중복 입력 무시(그래프/차단과 무관하게 일관 유지).
	// Triggered는 누르고 있는 동안 매 프레임 발생하므로 여기서 첫 프레임만 통과시킨다.
	if (bPressed)
	{
		if (PressedInputTags.HasTagExact(InputTypeTag))
		{
			return; // 이미 Press 상태 → 중복 Press 무시
		}
		PressedInputTags.AddTag(InputTypeTag);
	}
	else
	{
		if (!PressedInputTags.HasTagExact(InputTypeTag))
		{
			return; // 이미 Release 상태 → 중복 Release 무시
		}
		PressedInputTags.RemoveTag(InputTypeTag);
	}

	if (bPressed)
	{
		BufferedInput.Reset();
	}

	// 매칭 실패한 Press만 버퍼링(미루기). 차단 중인 입력도 버퍼에 올려 언블록 시 재시도한다.
	// Release 미매칭은 버리고 reaction delegate 대기로 처리됨.
	if (!ProcessInput(bPressed, InputTypeTag) && bPressed)
	{
		BufferedInput.InputTag = InputTypeTag;
		BufferedInput.Timestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	}
}

bool UOverdriveAbilityRouterComponent::ProcessInput(bool bPressed, const FGameplayTag& InputTypeTag)
{
	if (!ensure(AbilityGraph))
	{
		return false;
	}

	if (!WeakAbilitySystem.IsValid())
	{
		return false;
	}

	if (InputBlockTagContainer.HasMatchingGameplayTag(InputTypeTag))
	{
		return false; // 해당 입력 차단 중 → 미매칭(버퍼에 올라가 언블록 시 재시도됨)
	}

	if (FOverdriveAbilityInputReactionDelegate* ReactionDelegate = InputReactionDelegates.Find(FOverdriveAbilityInputKey{ bPressed, InputTypeTag }))
	{
		if (ReactionDelegate->IsBound())
		{
			ReactionDelegate->Broadcast(bPressed, InputTypeTag);
			return true; // 바인딩된 반응이 입력을 소비 → 그래프 순회/발동 생략, 버퍼 불필요
		}
	}

	const UOverdriveAbilityRouterNode* LocalRouterNode = CurrentNode.IsValid() ? CurrentNode.Get() : AbilityGraph->GetRootRouterNode();
	if (LocalRouterNode != nullptr && LocalRouterNode->GetAbilityRouterGraph() != AbilityGraph)
	{
		LocalRouterNode = AbilityGraph->GetRootRouterNode();
	}

	// ASC 소유 태그(GE/어빌리티가 붙인 상태)도 엣지 조건에 참여시킨다. 로컬·전역 두 순회가 공유하도록 여기서 한 번만 만든다.
	// 인자를 받는 GetOwnedGameplayTags 오버로드는 컨테이너를 Reset하므로 인자 없는 쪽을 써야 한다.
	FGameplayTagContainer EvaluationTags = StateTagContainer;
	EvaluationTags.AppendTags(WeakAbilitySystem->GetOwnedGameplayTags());

	const UOverdriveAbilityRouterNode* LocalNextNode = FindNodeToActivate(LocalRouterNode, bPressed, InputTypeTag, EvaluationTags);

	// 로컬 순회 실패 → 어느 상태에서든 열려 있는 전역 규칙(대쉬·점프 등)에 두 번째 기회를 준다.
	if (LocalNextNode == nullptr)
	{
		LocalNextNode = FindNodeToActivate(AbilityGraph->GetGlobalSecondChanceRouterNode(), bPressed, InputTypeTag, EvaluationTags);
	}

	if (LocalNextNode == nullptr)
	{
		return false;
	}

	FGameplayAbilitySpec* FoundSpec = WeakAbilitySystem->FindAbilitySpecFromClass(LocalNextNode->GetAbilityToActivate());

	if (FoundSpec == nullptr || !WeakAbilitySystem->TryActivateAbility(FoundSpec->Handle))
	{
		return false;
	}

	TArray<UGameplayAbility*> AbilityInstances = FoundSpec->GetAbilityInstances();
	if (AbilityInstances.IsEmpty() || !AbilityInstances.Last()->IsActive())
	{
		return false;
	}

	// 첫 발동 시 CurrentAbilityInstance가 무효일 수 있어 가드(null TWeakObjectPtr 역참조 방지).
	if (CurrentAbilityInstance.IsValid())
	{
		CurrentAbilityInstance->OnGameplayAbilityEnded.Remove(OnCurrentAbilityEndedHandle);
	}
	StateTagContainer.Reset();

	CurrentNode = LocalNextNode;
	CurrentAbilityInstance = AbilityInstances.Last();

	OnCurrentAbilityEndedHandle = CurrentAbilityInstance->OnGameplayAbilityEnded.AddUObject(this, &UOverdriveAbilityRouterComponent::OnCurrentAbilityEnded);
	return true;
}

const UOverdriveAbilityRouterNode* UOverdriveAbilityRouterComponent::FindNodeToActivate(const UOverdriveAbilityRouterNode* StartNode, bool bPressed, const FGameplayTag& InputTypeTag, const FGameplayTagContainer& InStateTags) const
{
	// 시작 노드가 없을 수 있다: Root 미설정 그래프, GlobalSecondChance가 없는 구버전 에셋 등.
	if (StartNode == nullptr)
	{
		return nullptr;
	}

	TArray<const UOverdriveAbilityRouterNode*> ChildrenNodes;
	StartNode->GetChildrenNodes(ChildrenNodes);

	// Proxy가 서로를 가리키면(A→B→A) 체인 추적이 무한 루프가 된다. 노드 수를 홉 상한으로 둔다.
	const int32 MaxHops = AbilityGraph ? AbilityGraph->AllRouterNodes.Num() : 0;

	for (const UOverdriveAbilityRouterNode* ChildNode : ChildrenNodes)
	{
		// 엣지 없이 핀만 이어진 노드는 ChildrenNodes에는 있지만 EdgeMap에는 없다 → GetRouterEdge가 null.
		const UOverdriveAbilityRouterEdge* RouterEdge = StartNode->GetRouterEdge(ChildNode);
		if (RouterEdge == nullptr)
		{
			continue;
		}

		if (!RouterEdge->CanEnterEndNode(this, bPressed, InputTypeTag, InStateTags))
		{
			continue;
		}

		// Proxy 체인 해소: 자기 자신을 반환할 때까지 타깃을 따라간다.
		const UOverdriveAbilityRouterNode* CurrentHop = ChildNode;
		const UOverdriveAbilityRouterNode* NextHop = ChildNode->GetRouterNodeToActivate(this, bPressed, InputTypeTag, InStateTags);

		int32 HopCount = 0;

		while (NextHop != nullptr && NextHop != CurrentHop)
		{
			if (!ensureMsgf(HopCount++ < MaxHops, TEXT("AbilityRouter: Proxy 노드 체인이 순환합니다. 그래프를 확인하세요.")))
			{
				NextHop = nullptr;
				break;
			}

			CurrentHop = NextHop;
			NextHop = NextHop->GetRouterNodeToActivate(this, bPressed, InputTypeTag, InStateTags);
		}

		if (NextHop != nullptr)
		{
			return NextHop;
		}
	}

	return nullptr;
}

void UOverdriveAbilityRouterComponent::FlushInputBuffer()
{
	if (!BufferedInput.IsSet() || !GetWorld())
	{
		return;
	}

	if (GetWorld()->GetTimeSeconds() - BufferedInput.Timestamp > InputBufferWindow)
	{
		BufferedInput.Reset(); // 만료 → 폐기
		return;
	}

	const FOverdriveAbilityBufferedInput Pending = BufferedInput;
	BufferedInput.Reset(); // 발동이 동기적으로 재-flush를 부를 수 있어 먼저 비움(재진입 안전)

	if (!ProcessInput(true, Pending.InputTag) && !BufferedInput.IsSet())
	{
		BufferedInput = Pending; // 아직 창 안 열림 → 만료까지 보존(타임스탬프 유지)
	}
}

void UOverdriveAbilityRouterComponent::BlockInputTag(const FGameplayTag& InputTypeTag)
{
	InputBlockTagContainer.UpdateTagCount(InputTypeTag, 1, EGameplayTagReplicationState::None);
}

void UOverdriveAbilityRouterComponent::UnblockInputTag(const FGameplayTag& InputTypeTag)
{
	// CountDelta -1. 내부 GameplayTagCountMap은 0 이하로 내려가지 않도록 관리됨.
	InputBlockTagContainer.UpdateTagCount(InputTypeTag, -1, EGameplayTagReplicationState::None);

	// 차단으로 미뤄둔(아직 만료 전) 입력을 재시도.
	FlushInputBuffer();
}

bool UOverdriveAbilityRouterComponent::IsInputTypeBlocked(const FGameplayTag& InputTypeTag) const
{
	return InputBlockTagContainer.HasMatchingGameplayTag(InputTypeTag);
}

void UOverdriveAbilityRouterComponent::AddStateTag(const FGameplayTag& StateTag)
{
	// 이미 보유 중이면 평가 입력이 그대로이므로 재평가할 이유가 없다.
	if (!StateTag.IsValid() || StateTagContainer.HasTagExact(StateTag))
	{
		return;
	}

	StateTagContainer.AddTag(StateTag);

	// 콤보 창이 열렸을 수 있으니 미뤘던 입력을 재평가.
	FlushInputBuffer();
}

void UOverdriveAbilityRouterComponent::RemoveStateTag(const FGameplayTag& StateTag)
{
	if (!StateTagContainer.HasTagExact(StateTag))
	{
		return;
	}

	StateTagContainer.RemoveTag(StateTag);

	// ConditionQuery가 태그 부재를 요구할 수도 있어 제거 시에도 재평가.
	FlushInputBuffer();
}

bool UOverdriveAbilityRouterComponent::HasStateTag(const FGameplayTag& StateTag) const
{
	return StateTagContainer.HasTag(StateTag);
}

void UOverdriveAbilityRouterComponent::UpdateStateTags(const FGameplayTagContainer& TagsToRemove, const FGameplayTagContainer& TagsToAdd)
{
	bool bChanged = false;

	for (const FGameplayTag& Tag : TagsToRemove)
	{
		if (!StateTagContainer.HasTagExact(Tag))
		{
			continue;
		}

		StateTagContainer.RemoveTag(Tag);
		bChanged = true;
	}

	for (const FGameplayTag& Tag : TagsToAdd)
	{
		if (!Tag.IsValid() || StateTagContainer.HasTagExact(Tag))
		{
			continue;
		}

		StateTagContainer.AddTag(Tag);
		bChanged = true;
	}

	if (bChanged)
	{
		FlushInputBuffer();
	}
}

void UOverdriveAbilityRouterComponent::SetStateTags(const FGameplayTagContainer& NewStateTags)
{
	if (StateTagContainer == NewStateTags)
	{
		return;
	}

	StateTagContainer = NewStateTags;

	FlushInputBuffer();
}

FOverdriveAbilityInputReactionDelegate& UOverdriveAbilityRouterComponent::GetInputReactionDelegate(bool bPressed, const FGameplayTag& InputTypeTag)
{
	return InputReactionDelegates.FindOrAdd(FOverdriveAbilityInputKey{ bPressed, InputTypeTag });
}

bool UOverdriveAbilityRouterComponent::IsInputPressed(const FGameplayTag& InputTypeTag) const
{
	return PressedInputTags.HasTagExact(InputTypeTag);
}

UAbilitySystemComponent* UOverdriveAbilityRouterComponent::GetAbilitySystem() const
{
	return WeakAbilitySystem.Get();
}


