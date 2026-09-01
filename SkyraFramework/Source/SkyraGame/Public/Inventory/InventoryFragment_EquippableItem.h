// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/SkyraInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class USkyraEquipmentDefinition;
class UObject;

UCLASS()
class SKYRAGAME_API UInventoryFragment_EquippableItem : public USkyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Skyra)
	TSubclassOf<USkyraEquipmentDefinition> EquipmentDefinition;
};
