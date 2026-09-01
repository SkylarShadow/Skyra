// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Equipment/SkyraEquipmentDefinition.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "SkyraSubEquipmentDefinition.generated.h"

class USkyraAbilitySet;
class USkyraSubEquipmentInstance;

/**
 * 子装备的静态行为定义。
 *
 * InventoryItemDefinition 描述“这是一个物品”，本 Definition 描述“这个物品作为子装备时如何工作”。
 * 品质、耐久、随机词条等可变状态应保存在对应的 InventoryItemInstance 中。
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class SKYRAGAME_API USkyraSubEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	USkyraSubEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 子装备运行时实例类型。玩法可以继承 USkyraSubEquipmentInstance 保存自己的运行时上下文。
	UPROPERTY(EditDefaultsOnly, Category="SubEquipment")
	TSubclassOf<USkyraSubEquipmentInstance> InstanceType;

	// 子装备身份标签，用于去重、查找、插槽规则和玩法筛选。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SubEquipment")
	FGameplayTag SubEquipmentTag;

	// 子装备插槽标签，例如 Weapon.Module.Barrel。为空表示不参与插槽限制。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SubEquipment")
	FGameplayTag SlotTag;

	// 可选兼容标签。通用层只保存配置，具体如何解释由 Manager 或派生类决定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SubEquipment")
	FGameplayTagContainer CompatibleParentTags;

	// 子装备激活到装备上时授予的能力集合。AbilitySpec.SourceObject 会指向 SubEquipmentInstance。
	UPROPERTY(EditDefaultsOnly, Category="SubEquipment")
	TArray<TObjectPtr<const USkyraAbilitySet>> AbilitySetsToGrant;

	// 子装备激活到装备上时生成的附属 Actor。默认挂到 Pawn 根组件或角色 Mesh。
	UPROPERTY(EditDefaultsOnly, Category="SubEquipment")
	TArray<FSkyraEquipmentActorToSpawn> ActorsToSpawn;
};
