// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyraLevelXpSet.h"

#include "Net/UnrealNetwork.h"


USkyraLevelXpSet::USkyraLevelXpSet()
:TotalXP(0)
,Level(1)
{
}

void USkyraLevelXpSet::OnRep_TotalXP(
	const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(
		USkyraLevelXpSet,
		TotalXP,
		OldValue);
}

void USkyraLevelXpSet::OnRep_Level(
	const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(
		USkyraLevelXpSet,
		Level,
		OldValue);
}

void USkyraLevelXpSet::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(
		USkyraLevelXpSet,
		TotalXP,
		COND_None,
		REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(
		USkyraLevelXpSet,
		Level,
		COND_None,
		REPNOTIFY_Always);
}