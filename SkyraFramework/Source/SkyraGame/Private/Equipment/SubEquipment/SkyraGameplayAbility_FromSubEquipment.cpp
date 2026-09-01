// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/SubEquipment/SkyraGameplayAbility_FromSubEquipment.h"

#include "Equipment/SkyraEquipmentInstance.h"
#include "Equipment/SubEquipment/SkyraSubEquipmentInstance.h"
#include "Inventory/SkyraInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameplayAbility_FromSubEquipment)

USkyraGameplayAbility_FromSubEquipment::USkyraGameplayAbility_FromSubEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USkyraSubEquipmentInstance* USkyraGameplayAbility_FromSubEquipment::GetAssociatedSubEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<USkyraSubEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

USkyraEquipmentInstance* USkyraGameplayAbility_FromSubEquipment::GetAssociatedEquipment() const
{
	if (USkyraSubEquipmentInstance* SubEquipmentInstance = GetAssociatedSubEquipment())
	{
		return SubEquipmentInstance->GetOwningEquipment();
	}

	return nullptr;
}

USkyraInventoryItemInstance* USkyraGameplayAbility_FromSubEquipment::GetAssociatedItem() const
{
	if (USkyraSubEquipmentInstance* SubEquipmentInstance = GetAssociatedSubEquipment())
	{
		if (USkyraInventoryItemInstance* SourceItem = SubEquipmentInstance->GetSourceItem())
		{
			return SourceItem;
		}
	}

	return nullptr;
}

void USkyraGameplayAbility_FromSubEquipment::InitializeGameplayCueParameters(FGameplayCueParameters& Params) const
{
	Super::InitializeGameplayCueParameters(Params);

	if (USkyraSubEquipmentInstance* SubEquipmentInstance = GetAssociatedSubEquipment())
	{
		Params.SourceObject = SubEquipmentInstance;
	}
}

void USkyraGameplayAbility_FromSubEquipment::InitializeGameplayEventData(FGameplayEventData& EventData) const
{
	Super::InitializeGameplayEventData(EventData);

	if (USkyraSubEquipmentInstance* SubEquipmentInstance = GetAssociatedSubEquipment())
	{
		EventData.OptionalObject = SubEquipmentInstance;
		EventData.OptionalObject2 = SubEquipmentInstance->GetSourceItem();
	}
}

#if WITH_EDITOR
EDataValidationResult USkyraGameplayAbility_FromSubEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("Skyra", "SubEquipmentAbilityMustBeInstanced", "Sub equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
