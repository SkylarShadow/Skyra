// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/SkyraGameplayAbility.h"

#include "SkyraGameplayAbility_FromSubEquipment.generated.h"

class USkyraEquipmentInstance;
class USkyraInventoryItemInstance;
class USkyraSubEquipmentInstance;

/**
 * 由子装备授予的 GameplayAbility。
 *
 * AbilitySpec.SourceObject 应指向 SubEquipmentInstance，GA 可由此读取所属装备和来源物品。
 */
UCLASS()
class SKYRAGAME_API USkyraGameplayAbility_FromSubEquipment : public USkyraGameplayAbility
{
	GENERATED_BODY()

public:
	USkyraGameplayAbility_FromSubEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Skyra|Ability")
	USkyraSubEquipmentInstance* GetAssociatedSubEquipment() const;

	UFUNCTION(BlueprintCallable, Category="Skyra|Ability")
	USkyraEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category="Skyra|Ability")
	USkyraInventoryItemInstance* GetAssociatedItem() const;

	virtual void InitializeGameplayCueParameters(FGameplayCueParameters& Params) const override;
	virtual void InitializeGameplayEventData(FGameplayEventData& EventData) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
