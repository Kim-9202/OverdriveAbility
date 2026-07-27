// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/OverdriveGameplayEffectTypes.h"

FOverdriveGameplayEffectContext::FOverdriveGameplayEffectContext()
{
}

FOverdriveGameplayEffectContext::~FOverdriveGameplayEffectContext()
{
}

FGameplayEffectContext* FOverdriveGameplayEffectContext::Duplicate() const
{
	// 자기 타입으로 깊은 복사(컴파일러 생성 복사 연산자가 FragmentMap 포함).
	FOverdriveGameplayEffectContext* NewContext = new FOverdriveGameplayEffectContext(*this);
	if (GetHitResult())
	{
		// 베이스 관례대로 HitResult는 별도 깊은 복사.
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}

bool FOverdriveGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// GameplayCue 파라미터가 EffectContext를 클라로 리플리케이트하므로
	// 큐/클라 UI가 프래그먼트를 읽으려면 FragmentMap도 직렬화해야 한다.
	// 키(UScriptStruct*)는 TInstancedStruct가 자기 타입을 직렬화하므로
	// 로드 시 GetScriptStruct()로 재구성한다(중복 전송 불필요).
	int32 Num = FragmentMap.Num();
	Ar << Num;

	if (Ar.IsLoading())
	{
		FragmentMap.Empty(Num);
		for (int32 i = 0; i < Num; ++i)
		{
			TInstancedStruct<FOverdriveEffectContextFragment> Fragment;
			Fragment.NetSerialize(Ar, Map, bOutSuccess);
			if (Fragment.IsValid())
			{
				FragmentMap.Emplace(Fragment.GetScriptStruct(), MoveTemp(Fragment));
			}
		}
	}
	else
	{
		for (TPair<const UScriptStruct*, TInstancedStruct<FOverdriveEffectContextFragment>>& Pair : FragmentMap)
		{
			Pair.Value.NetSerialize(Ar, Map, bOutSuccess);
		}
	}

	bOutSuccess = true;
	return true;
}

TInstancedStruct<FOverdriveEffectContextFragment> FOverdriveGameplayEffectContext::GetFragment(const UScriptStruct* ScriptStruct) const
{
	if (ScriptStruct == nullptr)
	{
		return TInstancedStruct<FOverdriveEffectContextFragment>();
	}

	if (FragmentMap.Contains(ScriptStruct))
	{
		return FragmentMap[ScriptStruct];
	}

    return TInstancedStruct<FOverdriveEffectContextFragment>();
}

FOverdriveEffectContextFragment* FOverdriveGameplayEffectContext::GetFragmentMemory(const UScriptStruct* ScriptStruct)
{
	if (ScriptStruct == nullptr)
	{
		return nullptr;
	}

	if (FragmentMap.Contains(ScriptStruct))
	{
		return FragmentMap[ScriptStruct].GetMutablePtr();
	}

	return nullptr;
}

const FOverdriveEffectContextFragment* FOverdriveGameplayEffectContext::GetFragmentMemory(const UScriptStruct* ScriptStruct) const
{
	if (ScriptStruct == nullptr)
	{
		return nullptr;
	}

	if (FragmentMap.Contains(ScriptStruct))
	{
		return FragmentMap[ScriptStruct].GetPtr();
	}

	return nullptr;
}

void FOverdriveGameplayEffectContext::AddFragment(const TInstancedStruct<FOverdriveEffectContextFragment>& InFragment)
{
	if (InFragment.IsValid())
	{
		FragmentMap.Emplace(InFragment.GetScriptStruct(), InFragment);
	}
}

void FOverdriveGameplayEffectContext::AddFragment(TInstancedStruct<FOverdriveEffectContextFragment>&& InFragment)
{
	if (InFragment.IsValid())
	{
		FragmentMap.Emplace(InFragment.GetScriptStruct(), MoveTemp(InFragment));
	}
}

void FOverdriveGameplayEffectContext::AddFragments(const TArray<TInstancedStruct<FOverdriveEffectContextFragment>>& InFragments)
{
	FragmentMap.Reserve(FragmentMap.Num() + InFragments.Num());

	for (auto& InFragment : InFragments)
	{
		AddFragment(InFragment);
	}
}


