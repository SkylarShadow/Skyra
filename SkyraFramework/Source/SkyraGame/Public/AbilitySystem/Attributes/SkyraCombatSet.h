// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "SkyraAttributeSet.h"

#include "SkyraCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * USkyraCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class SKYRAGAME_API USkyraCombatSet : public USkyraAttributeSet
{
	GENERATED_BODY()

public:

	USkyraCombatSet();

	ATTRIBUTE_ACCESSORS(USkyraCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(USkyraCombatSet, BaseHeal);
protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Skyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Skyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
	
	
};
