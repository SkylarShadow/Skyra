// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraInventoryItemInstance.h"

#include "Inventory/SkyraInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraInventoryItemInstance)

class FLifetimeProperty;

USkyraInventoryItemInstance::USkyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USkyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

#if UE_WITH_IRIS
void USkyraInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void USkyraInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void USkyraInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

void USkyraInventoryItemInstance::SetStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	const int32 CurrentStackCount = StatTags.GetStackCount(Tag);
	const int32 NewStackCount = FMath::Max(0, StackCount);
	if (NewStackCount > CurrentStackCount)
	{
		StatTags.AddStack(Tag, NewStackCount - CurrentStackCount);
	}
	else if (NewStackCount < CurrentStackCount)
	{
		StatTags.RemoveStack(Tag, CurrentStackCount - NewStackCount);
	}
}

int32 USkyraInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool USkyraInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void USkyraInventoryItemInstance::SetItemDef(TSubclassOf<USkyraInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

const USkyraInventoryItemFragment* USkyraInventoryItemInstance::FindFragmentByClass(TSubclassOf<USkyraInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<USkyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}


