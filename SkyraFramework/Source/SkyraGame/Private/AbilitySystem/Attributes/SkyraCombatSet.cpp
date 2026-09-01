// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraCombatSet.h"

#include "AbilitySystem/Attributes/SkyraAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraCombatSet)

class FLifetimeProperty;


USkyraCombatSet::USkyraCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void USkyraCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USkyraCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USkyraCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void USkyraCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USkyraCombatSet, BaseDamage, OldValue);
}

void USkyraCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USkyraCombatSet, BaseHeal, OldValue);
}





