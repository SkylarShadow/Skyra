// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/SkyraInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_SubEquipment.generated.h"

class USkyraSubEquipmentDefinition;

/**
 * 声明一个库存物品可以作为子装备被挂载到装备上。
 *
 * Fragment 只保存数据引用；装载、卸载、能力授予等运行时逻辑由 SubEquipmentManager 处理。
 */
UCLASS()
class SKYRAGAME_API UInventoryFragment_SubEquipment : public USkyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	// 子装备行为定义。为空表示该物品不能被通用 SubEquipmentManager 装载。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment|SubEquipment")
	TSubclassOf<USkyraSubEquipmentDefinition> SubEquipmentDefinition;
};
