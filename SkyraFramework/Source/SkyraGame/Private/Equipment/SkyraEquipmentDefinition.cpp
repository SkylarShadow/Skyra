// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraEquipmentDefinition.h"
#include "SkyraEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraEquipmentDefinition)

USkyraEquipmentDefinition::USkyraEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = USkyraEquipmentInstance::StaticClass();
}

