# OverdriveAbility

Unreal Engine 5.8 플러그인. GameplayAbilitySystem 확장 — **그래프 기반 입력 라우팅**, 정책으로 분리한 쿨다운·코스트, EffectContext 프래그먼트.

## 개요

콤보를 if 문으로 짜면 금방 손을 못 대게 된다. 이 플러그인의 중심은 **AbilityRouter** — 입력과 어빌리티의 연결을 그래프 에셋으로 편집하는 시스템이다.

노드가 어빌리티고, 엣지가 "어떤 입력에 어떤 상태에서 넘어갈 수 있는가"다. 현재 발동 중인 어빌리티가 곧 현재 노드이므로, 콤보는 그래프를 따라 걷는 일이 된다.

## 모듈 구성

| 모듈 | 타입 | 역할 |
|---|---|---|
| `OverdriveAbility` | Runtime | 라우터 그래프·컴포넌트, 어빌리티 베이스, 정책, 프래그먼트 |
| `OverdriveAbilityUI` | Runtime | MVVM 뷰모델 |
| `OverdriveAbilityEditor` | Editor | 라우터 그래프 에디터, 에디터 모드 |
| `OverdriveAbilityNodes` | UncookedOnly | EffectContext 프래그먼트 K2Node |

## 주요 기능

### AbilityRouter

전용 그래프 에디터를 가진 애셋이다. 노드 종류는 넷이다.

| 노드 | 역할 |
|---|---|
| `Root` | 기본 진입점. 아무 어빌리티도 실행 중이 아닐 때의 시작점 |
| `GlobalSecondChance` | 전역 폴백 진입점. 로컬 순회가 실패하면 여기서 한 번 더 찾는다 |
| `Simple` | 어빌리티 하나를 지정하는 일반 노드 |
| `Proxy` | 다른 노드로 넘기는 참조 노드. 그래프가 트리 밖으로 합류할 때 사용 |

`GlobalSecondChance`는 Root의 자식이 아니라 **형제**다. 어느 상태에서든 열려 있어야 하는 대쉬·점프 같은 어빌리티를 여기 매단다.

입력이 들어오면 라우터는 이 순서로 처리한다.

```
입력 차단 확인 → 입력 반응 델리게이트 → 그래프 순회
                                          ├─ CurrentNode의 자식에서 탐색
                                          └─ 실패 시 GlobalSecondChance에서 재탐색
```

### 입력 버퍼

매칭에 실패한 Press 입력은 단일 슬롯에 잠시 보관된다(기본 0.2초). 상태 태그가 바뀌어 콤보 창이 열리면 보관된 입력이 재평가되어 발동한다. 창이 열리기 직전에 누른 입력이 씹히지 않게 하기 위한 장치다.

### UOverdriveAbilityRouterComponent

EnhancedInput의 `UInputAction`을 라우터 InputType 태그에 매핑한다. 폰 재시작·빙의 변경을 감지해 바인딩을 다시 건다.

```cpp
// 콤보 창 열기 (보통 애님 노티파이에서)
RouterComponent->AddStateTag(ComboWindowTag);

// 특정 입력 일시 차단
RouterComponent->BlockInputTag(DodgeInputTag);
```

- `AddStateTag` / `RemoveStateTag` — 엣지 조건이 참조하는 상태. 변경 시 버퍼 입력 재평가
- `BlockInputTag` / `UnblockInputTag` — 카운트 기반 입력 차단
- `GetInputReactionDelegate(bPressed, InputType)` — 바인딩하면 해당 입력은 그래프 순회 대신 델리게이트만 실행한다. 차지·홀드처럼 어빌리티가 입력을 직접 소비할 때 사용

### UOverdriveGameplayAbility

쿨다운과 코스트를 **Instanced 정책 오브젝트**로 분리했다. 어빌리티마다 GE를 새로 만들지 않고 정책만 바꿔 끼운다.

| 정책 | 동작 |
|---|---|
| `CooldownPolicy_Default` | 기본 쿨다운 |
| `CooldownPolicy_Stack` | 스택형 쿨다운(충전식) |
| `CostPolicy_Default` | 기본 코스트 |
| `CostPolicy_CheckImmune` | 면역 상태면 코스트 면제 |

`Fragments` 배열로 어빌리티에 재사용 가능한 동작 조각을 붙일 수 있다.

Commit 계열은 `FGameplayEventData`를 받는 오버로드가 함께 제공된다(`CommitAbilityWithEvent` 등). 이벤트 페이로드에 따라 코스트·쿨다운이 달라지는 어빌리티를 위한 것이다.

### EffectContext 프래그먼트

`FGameplayEffectContext`에 임의의 구조체를 태그로 붙이고 꺼내는 확장. 전용 K2Node 두 개가 제공된다.

- `Add Context Fragment` — 컨텍스트에 프래그먼트 추가
- `Get Effect Context Fragment` — 타입으로 꺼내기 (와일드카드 핀)

기본 제공으로 `OverdriveEffectContextFragment_Cooldown`이 있다.

### 태스크 / 비동기 액션

`WaitInputReaction` — 라우터의 입력 반응 델리게이트를 어빌리티에서 대기한다. AbilityTask와 AsyncAction 두 형태로 제공된다.

### MVVM 뷰모델

| 뷰모델 | 대상 |
|---|---|
| `OAVM_SingleAttribute` | 어트리뷰트 1개 |
| `OAVM_PairedAttribute` | 현재/최대 쌍 (체력바 등) |
| `OAVM_FilteredSingleAttribute` / `OAVM_FilteredPairedAttribute` | 태그로 필터링한 어트리뷰트 |
| `OAVM_ActiveEffect` / `OAVM_ActiveEffectList` | 활성 GameplayEffect 목록(버프 아이콘 등) |

## 요구 사항

- Unreal Engine **5.8**
- 엔진 플러그인: `GameplayAbilities`, `EnhancedInput`, `ModelViewViewModel`

다른 Overdrive 플러그인에는 의존하지 않는다.

## 설치

```bash
cd YourProject/Plugins
git clone https://github.com/Kim-9202/OverdriveAbility.git
```

`.uproject`의 `Plugins` 배열에 추가한 뒤 프로젝트 파일을 재생성하고 빌드한다.

## 상태

개인 개발 중인 플러그인이다. API는 예고 없이 바뀔 수 있다.
