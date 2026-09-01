// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameplayAbility_FromEquipment.h"
#include "SkyraEquipmentInstance.h"
#include "Inventory/SkyraInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameplayAbility_FromEquipment)

USkyraGameplayAbility_FromEquipment::USkyraGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USkyraEquipmentInstance* USkyraGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<USkyraEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

USkyraInventoryItemInstance* USkyraGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (USkyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<USkyraInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}

void USkyraGameplayAbility_FromEquipment::InitializeGameplayCueParameters(FGameplayCueParameters& Params) const
{
	Super::InitializeGameplayCueParameters(Params);

	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		// 发起者
		Params.Instigator = ActorInfo->AvatarActor.Get();
		
		//Params.EffectCauser = ActorInfo->AvatarActor.Get();
	}

	// SourceObject 保存 EquipmentInstance
	if (USkyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		Params.SourceObject = Equipment;
	}
}

void USkyraGameplayAbility_FromEquipment::InitializeGameplayEventData(FGameplayEventData& EventData) const
{
	Super::InitializeGameplayEventData(EventData);

	if (USkyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		EventData.OptionalObject = Equipment;
		EventData.OptionalObject2 = GetAssociatedItem();
	}
}


#if WITH_EDITOR
EDataValidationResult USkyraGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("Skyra", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
