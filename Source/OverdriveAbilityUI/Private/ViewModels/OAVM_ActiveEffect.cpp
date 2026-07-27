// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/OAVM_ActiveEffect.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

void UOAVM_ActiveEffect::InitializeFromHandle(FActiveGameplayEffectHandle InHandle)
{
	UnbindEvents();

	Handle = InHandle;
	AbilitySystem = InHandle.GetOwningAbilitySystemComponent();

	BindEvents();
	RefreshFromEffect();
}

float UOAVM_ActiveEffect::GetTimeRemaining() const
{
	if (bInfinite || Duration <= 0.f)
	{
		return -1.f;
	}

	if (!AbilitySystem.IsValid())
	{
		return 0.f;
	}

	const float WorldTime = AbilitySystem->GetActiveGameplayEffects().GetWorldTime();
	return FMath::Max(0.f, Duration - (WorldTime - StartWorldTime));
}

float UOAVM_ActiveEffect::GetProgress() const
{
	if (bInfinite)
	{
		return 1.f;
	}

	if (Duration <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(GetTimeRemaining() / Duration, 0.f, 1.f);
}

void UOAVM_ActiveEffect::BeginDestroy()
{
	UnbindEvents();

	Super::BeginDestroy();
}

void UOAVM_ActiveEffect::BindEvents()
{
	if (!AbilitySystem.IsValid() || !Handle.IsValid())
	{
		return;
	}

	// 유효하지 않은 핸들이면 nullptr을 반환할 수 있다.
	FActiveGameplayEffectEvents* Events = AbilitySystem->GetActiveEffectEventSet(Handle);
	if (!Events)
	{
		return;
	}

	StackChangedHandle = Events->OnStackChanged.AddUObject(this, &UOAVM_ActiveEffect::OnStackChanged);
	TimeChangedHandle = Events->OnTimeChanged.AddUObject(this, &UOAVM_ActiveEffect::OnTimeChanged);
	InhibitionChangedHandle = Events->OnInhibitionChanged.AddUObject(this, &UOAVM_ActiveEffect::OnInhibitionChanged);
}

void UOAVM_ActiveEffect::UnbindEvents()
{
	if (AbilitySystem.IsValid() && Handle.IsValid())
	{
		if (FActiveGameplayEffectEvents* Events = AbilitySystem->GetActiveEffectEventSet(Handle))
		{
			if (StackChangedHandle.IsValid())
			{
				Events->OnStackChanged.Remove(StackChangedHandle);
			}
			if (TimeChangedHandle.IsValid())
			{
				Events->OnTimeChanged.Remove(TimeChangedHandle);
			}
			if (InhibitionChangedHandle.IsValid())
			{
				Events->OnInhibitionChanged.Remove(InhibitionChangedHandle);
			}
		}
	}

	StackChangedHandle.Reset();
	TimeChangedHandle.Reset();
	InhibitionChangedHandle.Reset();
}

void UOAVM_ActiveEffect::RefreshFromEffect()
{
	if (!AbilitySystem.IsValid())
	{
		return;
	}

	const FActiveGameplayEffect* ActiveEffect = AbilitySystem->GetActiveGameplayEffect(Handle);
	if (!ActiveEffect)
	{
		return;
	}

	const float NewDuration = ActiveEffect->GetDuration();

	UE_MVVM_SET_PROPERTY_VALUE(StackCount, AbilitySystem->GetCurrentStackCount(Handle));
	UE_MVVM_SET_PROPERTY_VALUE(StartWorldTime, ActiveEffect->StartWorldTime);
	UE_MVVM_SET_PROPERTY_VALUE(Duration, NewDuration);
	UE_MVVM_SET_PROPERTY_VALUE(bInfinite, NewDuration == FGameplayEffectConstants::INFINITE_DURATION);
	UE_MVVM_SET_PROPERTY_VALUE(bInhibited, ActiveEffect->bIsInhibited);

	FGameplayTagContainer NewAssetTags;
	ActiveEffect->Spec.GetAllAssetTags(NewAssetTags);
	UE_MVVM_SET_PROPERTY_VALUE(AssetTags, NewAssetTags);
}

void UOAVM_ActiveEffect::OnStackChanged(FActiveGameplayEffectHandle InHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(StackCount, NewStackCount);
}

void UOAVM_ActiveEffect::OnTimeChanged(FActiveGameplayEffectHandle InHandle, float NewStartTime, float NewDuration)
{
	UE_MVVM_SET_PROPERTY_VALUE(StartWorldTime, NewStartTime);
	UE_MVVM_SET_PROPERTY_VALUE(Duration, NewDuration);
	UE_MVVM_SET_PROPERTY_VALUE(bInfinite, NewDuration == FGameplayEffectConstants::INFINITE_DURATION);
}

void UOAVM_ActiveEffect::OnInhibitionChanged(FActiveGameplayEffectHandle InHandle, bool bInInhibited)
{
	UE_MVVM_SET_PROPERTY_VALUE(bInhibited, bInInhibited);
}
