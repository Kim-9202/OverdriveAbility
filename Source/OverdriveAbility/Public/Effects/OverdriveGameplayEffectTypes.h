// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "OverdriveGameplayEffectTypes.generated.h"

/**
 * 
 */
USTRUCT()
struct OVERDRIVEABILITY_API FOverdriveEffectContextFragment
{
	GENERATED_BODY()
public:
	FOverdriveEffectContextFragment()
	{}

	~FOverdriveEffectContextFragment()
	{}
};

USTRUCT()
struct OVERDRIVEABILITY_API FOverdriveGameplayEffectContext :public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	FOverdriveGameplayEffectContext();
	~FOverdriveGameplayEffectContext();

	//~ FGameplayEffectContext polymorphic interface begin
	// 커스텀 컨텍스트는 이 셋을 반드시 오버라이드해야 함. 미오버라이드 시
	// Duplicate가 베이스 타입으로 슬라이싱 복사돼 FragmentMap이 유실되고,
	// 이후 정책의 StaticCast<FOverdriveGameplayEffectContext*>가 UB가 됨.
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	virtual FGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	//~ FGameplayEffectContext polymorphic interface end

private:
	UPROPERTY()
	TMap<const UScriptStruct*, TInstancedStruct<FOverdriveEffectContextFragment>> FragmentMap;

public:
	TInstancedStruct<FOverdriveEffectContextFragment> GetFragment(const UScriptStruct* ScriptStruct) const;
	FOverdriveEffectContextFragment* GetFragmentMemory(const UScriptStruct* ScriptStruct);
	const FOverdriveEffectContextFragment* GetFragmentMemory(const UScriptStruct* ScriptStruct) const;

	void AddFragment(const TInstancedStruct<FOverdriveEffectContextFragment>& InFragment);
	void AddFragment(TInstancedStruct<FOverdriveEffectContextFragment>&& InFragment);

	void AddFragments(const TArray<TInstancedStruct<FOverdriveEffectContextFragment>>& InFragments);
};

// 폴리모픽 핸들(FGameplayEffectContextHandle)이 GetScriptStruct()로 타입을 식별해
// 직렬화하려면 파생 struct도 베이스와 동일한 StructOps 특성이 필요하다.
template<>
struct TStructOpsTypeTraits<FOverdriveGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FOverdriveGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// TSharedPtr<FHitResult> Data / FragmentMap 복사 보장
	};
};

