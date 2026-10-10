// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/SkyraAbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/SkyraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "SkyraGameplayTags.h"
#include "System/SkyraAssetManager.h"
#include "System/SkyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAbilitySystemBlueprintLibrary)

namespace SkyraAbilitySystemBlueprintLibrary
{
	static USkyraAbilitySystemComponent* GetSkyraAbilitySystemComponentFromActor(AActor* Actor)
	{
		return Actor ? Cast<USkyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)) : nullptr;
	}

	static FGameplayEffectSpecHandle MakeSetByCallerEffectSpec(USkyraAbilitySystemComponent* SourceASC, const TSoftClassPtr<UGameplayEffect>& GameplayEffectClass, const FGameplayTag& SetByCallerTag, float Magnitude, float Level)
	{
		if (!SourceASC || Magnitude <= 0.0f || Level <= 0.0f || !SetByCallerTag.IsValid())
		{
			return FGameplayEffectSpecHandle();
		}

		const TSubclassOf<UGameplayEffect> GameplayEffect = USkyraAssetManager::GetSubclass(GameplayEffectClass);
		if (!GameplayEffect)
		{
			return FGameplayEffectSpecHandle();
		}

		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GameplayEffect, Level, SourceASC->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);
		}

		return SpecHandle;
	}

	static FGameplayEffectSpecHandle GetOrMakeSetByCallerEffectSpec(USkyraAbilitySystemComponent* SourceASC, FGameplayEffectSpecHandle OptionalSpecHandle, const TSoftClassPtr<UGameplayEffect>& GameplayEffectClass, const FGameplayTag& SetByCallerTag, float Magnitude)
	{
		FGameplayEffectSpecHandle SpecHandle = OptionalSpecHandle.IsValid()
			? OptionalSpecHandle
			: MakeSetByCallerEffectSpec(SourceASC, GameplayEffectClass, SetByCallerTag, Magnitude, 1.0f);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);
		}

		return SpecHandle;
	}
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToActor(AActor* TargetActor, float DamageAmount)
{
	return ApplySetByCallerDamageEffectToSelf(SkyraAbilitySystemBlueprintLibrary::GetSkyraAbilitySystemComponentFromActor(TargetActor), DamageAmount, FGameplayEffectSpecHandle());
}

FGameplayEffectSpecHandle USkyraAbilitySystemBlueprintLibrary::MakeSetByCallerDamageEffectSpec(USkyraAbilitySystemComponent* SourceASC, float DamageAmount, float Level)
{
	return SkyraAbilitySystemBlueprintLibrary::MakeSetByCallerEffectSpec(
		SourceASC,
		USkyraGameData::Get().DamageGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Damage,
		DamageAmount,
		Level);
}

FGameplayEffectSpecHandle USkyraAbilitySystemBlueprintLibrary::MakeSetByCallerHealEffectSpec(USkyraAbilitySystemComponent* SourceASC, float HealAmount, float Level)
{
	return SkyraAbilitySystemBlueprintLibrary::MakeSetByCallerEffectSpec(
		SourceASC,
		USkyraGameData::Get().HealGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Heal,
		HealAmount,
		Level);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplyGameplayEffectSpecToSelf(USkyraAbilitySystemComponent* ASC, const FGameplayEffectSpecHandle& SpecHandle)
{
	if (!ASC || !SpecHandle.IsValid())
	{
		return false;
	}

	return ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()).IsValid();
}

bool USkyraAbilitySystemBlueprintLibrary::ApplyGameplayEffectSpecToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& SpecHandle)
{
	if (!SourceASC || !TargetASC || !SpecHandle.IsValid())
	{
		return false;
	}

	return SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC).IsValid();
}

bool USkyraAbilitySystemBlueprintLibrary::ApplyGameplayEffectSpecToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle)
{
	return ApplyGameplayEffectSpecToTargetASC(SourceASC, SkyraAbilitySystemBlueprintLibrary::GetSkyraAbilitySystemComponentFromActor(TargetActor), SpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToSelf(USkyraAbilitySystemComponent* ASC, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	const FGameplayEffectSpecHandle SpecHandle = SkyraAbilitySystemBlueprintLibrary::GetOrMakeSetByCallerEffectSpec(
		ASC,
		OptionalSpecHandle,
		USkyraGameData::Get().DamageGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Damage,
		DamageAmount);

	return ApplyGameplayEffectSpecToSelf(ASC, SpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToSelf(USkyraAbilitySystemComponent* ASC, float DamageAmount)
{
	return ApplySetByCallerDamageEffectToSelf(ASC, DamageAmount, FGameplayEffectSpecHandle());
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToSelf(USkyraAbilitySystemComponent* ASC, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	const FGameplayEffectSpecHandle SpecHandle = SkyraAbilitySystemBlueprintLibrary::GetOrMakeSetByCallerEffectSpec(
		ASC,
		OptionalSpecHandle,
		USkyraGameData::Get().HealGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Heal,
		HealAmount);

	return ApplyGameplayEffectSpecToSelf(ASC, SpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToSelf(USkyraAbilitySystemComponent* ASC, float HealAmount)
{
	return ApplySetByCallerHealEffectToSelf(ASC, HealAmount, FGameplayEffectSpecHandle());
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	const FGameplayEffectSpecHandle SpecHandle = SkyraAbilitySystemBlueprintLibrary::GetOrMakeSetByCallerEffectSpec(
		SourceASC,
		OptionalSpecHandle,
		USkyraGameData::Get().DamageGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Damage,
		DamageAmount);

	return ApplyGameplayEffectSpecToTargetASC(SourceASC, TargetASC, SpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float DamageAmount)
{
	return ApplySetByCallerDamageEffectToTargetASC(SourceASC, TargetASC, DamageAmount, FGameplayEffectSpecHandle());
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	return ApplySetByCallerDamageEffectToTargetASC(
		SourceASC,
		SkyraAbilitySystemBlueprintLibrary::GetSkyraAbilitySystemComponentFromActor(TargetActor),
		DamageAmount,
		OptionalSpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerDamageEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float DamageAmount)
{
	return ApplySetByCallerDamageEffectToTargetActor(SourceASC, TargetActor, DamageAmount, FGameplayEffectSpecHandle());
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	const FGameplayEffectSpecHandle SpecHandle = SkyraAbilitySystemBlueprintLibrary::GetOrMakeSetByCallerEffectSpec(
		SourceASC,
		OptionalSpecHandle,
		USkyraGameData::Get().HealGameplayEffect_SetByCaller,
		SkyraGameplayTags::SetByCaller_Heal,
		HealAmount);

	return ApplyGameplayEffectSpecToTargetASC(SourceASC, TargetASC, SpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float HealAmount)
{
	return ApplySetByCallerHealEffectToTargetASC(SourceASC, TargetASC, HealAmount, FGameplayEffectSpecHandle());
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle)
{
	return ApplySetByCallerHealEffectToTargetASC(
		SourceASC,
		SkyraAbilitySystemBlueprintLibrary::GetSkyraAbilitySystemComponentFromActor(TargetActor),
		HealAmount,
		OptionalSpecHandle);
}

bool USkyraAbilitySystemBlueprintLibrary::ApplySetByCallerHealEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float HealAmount)
{
	return ApplySetByCallerHealEffectToTargetActor(SourceASC, TargetActor, HealAmount, FGameplayEffectSpecHandle());
}
