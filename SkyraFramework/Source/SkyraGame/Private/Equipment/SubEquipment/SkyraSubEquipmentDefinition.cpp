// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/SubEquipment/SkyraSubEquipmentDefinition.h"

#include "Equipment/SubEquipment/SkyraSubEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraSubEquipmentDefinition)

USkyraSubEquipmentDefinition::USkyraSubEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = USkyraSubEquipmentInstance::StaticClass();
}
