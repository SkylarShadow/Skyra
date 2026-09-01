// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/SubEquipment/SkyraSubEquipmentManagerComponent.h"

#include "AbilitySystem/SkyraAbilitySet.h"
#include "AbilitySystem/SkyraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Equipment/SkyraEquipmentInstance.h"
#include "Equipment/SubEquipment/SkyraSubEquipmentDefinition.h"
#include "Equipment/SubEquipment/SkyraSubEquipmentInstance.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_SubEquipment.h"
#include "Inventory/SkyraInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraSubEquipmentManagerComponent)

FString FSkyraAppliedSubEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(OwningEquipment));
}

void FSkyraSubEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		if (USkyraSubEquipmentInstance* Instance = Entries[Index].Instance)
		{
			Instance->OnDetached();
		}
	}
}

void FSkyraSubEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		if (USkyraSubEquipmentInstance* Instance = Entries[Index].Instance)
		{
			Instance->OnAttached();
		}
	}
}

void FSkyraSubEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

USkyraAbilitySystemComponent* FSkyraSubEquipmentList::GetAbilitySystemComponent() const
{
	AActor* OwningActor = OwnerComponent ? OwnerComponent->GetOwner() : nullptr;
	return OwningActor ? Cast<USkyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor)) : nullptr;
}

USkyraSubEquipmentInstance* FSkyraSubEquipmentList::AddEntry(
	USkyraEquipmentInstance* OwningEquipment,
	USkyraInventoryItemInstance* OwnerItemInstance,
	USkyraInventoryItemInstance* SubItemInstance,
	TSubclassOf<USkyraSubEquipmentDefinition> SubEquipmentDefinition,
	TSubclassOf<USkyraSubEquipmentInstance> InstanceType)
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	if ((OwningEquipment == nullptr) || (OwnerItemInstance == nullptr) || (SubItemInstance == nullptr) || (SubEquipmentDefinition == nullptr))
	{
		return nullptr;
	}

	if (InstanceType == nullptr)
	{
		InstanceType = USkyraSubEquipmentInstance::StaticClass();
	}

	FSkyraAppliedSubEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.OwningEquipment = OwningEquipment;
	NewEntry.OwnerItemInstance = OwnerItemInstance;
	NewEntry.SubItemInstance = SubItemInstance;
	NewEntry.SubEquipmentDefinition = SubEquipmentDefinition;
	NewEntry.Instance = NewObject<USkyraSubEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	MarkItemDirty(NewEntry);

	return NewEntry.Instance;
}

void FSkyraSubEquipmentList::RemoveEntry(USkyraSubEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FSkyraAppliedSubEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
			return;
		}
	}
}

USkyraSubEquipmentManagerComponent::USkyraSubEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SubEquipmentList(this)
{
	SetIsReplicatedByDefault(true);
}

void USkyraSubEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SubEquipmentList);
}

bool USkyraSubEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FSkyraAppliedSubEquipmentEntry& Entry : SubEquipmentList.Entries)
	{
		if (IsValid(Entry.Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void USkyraSubEquipmentManagerComponent::UninitializeComponent()
{
	TArray<USkyraSubEquipmentInstance*> Instances;
	for (const FSkyraAppliedSubEquipmentEntry& Entry : SubEquipmentList.Entries)
	{
		Instances.Add(Entry.Instance);
	}

	for (USkyraSubEquipmentInstance* Instance : Instances)
	{
		if (Instance != nullptr)
		{
			DeactivateSubEquipmentItem(Instance->GetSourceItem());
		}
	}

	Super::UninitializeComponent();
}

void USkyraSubEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FSkyraAppliedSubEquipmentEntry& Entry : SubEquipmentList.Entries)
		{
			if (IsValid(Entry.Instance))
			{
				AddReplicatedSubObject(Entry.Instance);
			}
		}
	}
}

bool USkyraSubEquipmentManagerComponent::BindSubEquipmentItem(USkyraInventoryItemInstance* OwnerItemInstance, USkyraInventoryItemInstance* SubItemInstance)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (OwnerItemInstance == nullptr) || (SubItemInstance == nullptr))
	{
		return false;
	}

	FSkyraSubEquipmentAttachData AttachData;
	if (!BuildSubEquipmentAttachData(SubItemInstance, AttachData))
	{
		return false;
	}

	const int32 ExistingIndex = FindItemBindingIndexBySubItem(SubItemInstance);
	if (ExistingIndex != INDEX_NONE)
	{
		return ItemBindings[ExistingIndex].OwnerItemInstance == OwnerItemInstance;
	}

	FSkyraSubEquipmentItemBinding& Binding = ItemBindings.AddDefaulted_GetRef();
	Binding.OwnerItemInstance = OwnerItemInstance;
	Binding.SubItemInstance = SubItemInstance;
	return true;
}

bool USkyraSubEquipmentManagerComponent::UnbindSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (SubItemInstance == nullptr))
	{
		return false;
	}

	DeactivateSubEquipmentItem(SubItemInstance);

	const int32 BindingIndex = FindItemBindingIndexBySubItem(SubItemInstance);
	if (BindingIndex == INDEX_NONE)
	{
		return false;
	}

	ItemBindings.RemoveAt(BindingIndex);
	return true;
}

USkyraSubEquipmentInstance* USkyraSubEquipmentManagerComponent::AttachSubEquipmentItemToEquipment(
	USkyraEquipmentInstance* OwningEquipment,
	USkyraInventoryItemInstance* OwnerItemInstance,
	USkyraInventoryItemInstance* SubItemInstance)
{
	if (!CanAttachSubEquipmentItem(OwningEquipment, OwnerItemInstance, SubItemInstance))
	{
		return nullptr;
	}

	if (!BindSubEquipmentItem(OwnerItemInstance, SubItemInstance))
	{
		return nullptr;
	}

	if (USkyraSubEquipmentInstance* ExistingInstance = FindSubEquipmentInstanceByItem(SubItemInstance))
	{
		return ExistingInstance->GetOwningEquipment() == OwningEquipment ? ExistingInstance : nullptr;
	}

	FSkyraSubEquipmentAttachData AttachData;
	if (!BuildSubEquipmentAttachData(SubItemInstance, AttachData))
	{
		return nullptr;
	}

	if (AttachData.Definition->SubEquipmentTag.IsValid()
		&& (FindRuntimeEntryIndexByEquipmentAndTag(OwningEquipment, AttachData.Definition->SubEquipmentTag) != INDEX_NONE))
	{
		return nullptr;
	}

	USkyraSubEquipmentInstance* Instance = SubEquipmentList.AddEntry(
		OwningEquipment,
		OwnerItemInstance,
		SubItemInstance,
		AttachData.DefinitionClass,
		AttachData.InstanceType);
	if (Instance == nullptr)
	{
		return nullptr;
	}

	InitializeSubEquipmentInstance(Instance, OwningEquipment, SubItemInstance, AttachData.DefinitionClass);
	AddReplicatedSubEquipmentInstance(Instance);
	GrantSubEquipmentAbilities(Instance);
	Instance->SpawnSubEquipmentActors(AttachData.Definition->ActorsToSpawn);
	Instance->OnAttached();
	OnSubEquipmentChanged.Broadcast(OwningEquipment, OwnerItemInstance, SubItemInstance, Instance, true);

	return Instance;
}

bool USkyraSubEquipmentManagerComponent::DeactivateSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (SubItemInstance == nullptr))
	{
		return false;
	}

	const int32 EntryIndex = FindRuntimeEntryIndexBySubItem(SubItemInstance);
	if (EntryIndex == INDEX_NONE)
	{
		return false;
	}

	FSkyraAppliedSubEquipmentEntry Entry = SubEquipmentList.Entries[EntryIndex];
	USkyraSubEquipmentInstance* Instance = Entry.Instance;
	if (Instance != nullptr)
	{
		RemoveSubEquipmentAbilities(Instance);
		RemoveReplicatedSubEquipmentInstance(Instance);
		Instance->OnDetached();
		OnSubEquipmentChanged.Broadcast(Entry.OwningEquipment, Entry.OwnerItemInstance, Entry.SubItemInstance, Instance, false);
		Instance->RemoveFromAbilitySystem(nullptr);
	}

	SubEquipmentList.RemoveEntry(Instance);
	return true;
}

bool USkyraSubEquipmentManagerComponent::DetachSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance)
{
	return UnbindSubEquipmentItem(SubItemInstance);
}

bool USkyraSubEquipmentManagerComponent::DetachSubEquipmentFromEquipment(USkyraEquipmentInstance* OwningEquipment, FGameplayTag SubEquipmentTag)
{
	if ((OwningEquipment == nullptr) || !SubEquipmentTag.IsValid())
	{
		return false;
	}

	const int32 EntryIndex = FindRuntimeEntryIndexByEquipmentAndTag(OwningEquipment, SubEquipmentTag);
	return (EntryIndex != INDEX_NONE)
		? DetachSubEquipmentItem(SubEquipmentList.Entries[EntryIndex].SubItemInstance)
		: false;
}

void USkyraSubEquipmentManagerComponent::ActivateBoundSubEquipmentForEquipment(
	USkyraEquipmentInstance* OwningEquipment,
	USkyraInventoryItemInstance* OwnerItemInstance)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (OwningEquipment == nullptr) || (OwnerItemInstance == nullptr))
	{
		return;
	}

	for (const FSkyraSubEquipmentItemBinding& Binding : ItemBindings)
	{
		if ((Binding.OwnerItemInstance == OwnerItemInstance) && (Binding.SubItemInstance != nullptr))
		{
			AttachSubEquipmentItemToEquipment(OwningEquipment, OwnerItemInstance, Binding.SubItemInstance);
		}
	}
}

void USkyraSubEquipmentManagerComponent::DeactivateSubEquipmentForEquipment(USkyraEquipmentInstance* OwningEquipment)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (OwningEquipment == nullptr))
	{
		return;
	}

	for (int32 Index = SubEquipmentList.Entries.Num() - 1; Index >= 0; --Index)
	{
		if (SubEquipmentList.Entries[Index].OwningEquipment == OwningEquipment)
		{
			DeactivateSubEquipmentItem(SubEquipmentList.Entries[Index].SubItemInstance);
		}
	}
}

void USkyraSubEquipmentManagerComponent::DetachAllSubEquipmentFromOwnerItem(USkyraInventoryItemInstance* OwnerItemInstance)
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (OwnerItemInstance == nullptr))
	{
		return;
	}

	for (int32 Index = ItemBindings.Num() - 1; Index >= 0; --Index)
	{
		if (ItemBindings[Index].OwnerItemInstance == OwnerItemInstance)
		{
			UnbindSubEquipmentItem(ItemBindings[Index].SubItemInstance);
		}
	}
}

TArray<USkyraInventoryItemInstance*> USkyraSubEquipmentManagerComponent::GetSubEquipmentItemsForOwnerItem(USkyraInventoryItemInstance* OwnerItemInstance) const
{
	TArray<USkyraInventoryItemInstance*> Result;
	for (const FSkyraSubEquipmentItemBinding& Binding : ItemBindings)
	{
		if ((Binding.OwnerItemInstance == OwnerItemInstance) && (Binding.SubItemInstance != nullptr))
		{
			Result.Add(Binding.SubItemInstance);
		}
	}

	return Result;
}

USkyraInventoryItemInstance* USkyraSubEquipmentManagerComponent::GetOwnerItemForSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance) const
{
	const int32 BindingIndex = FindItemBindingIndexBySubItem(SubItemInstance);
	return (BindingIndex != INDEX_NONE) ? ItemBindings[BindingIndex].OwnerItemInstance : nullptr;
}

TArray<USkyraSubEquipmentInstance*> USkyraSubEquipmentManagerComponent::GetSubEquipmentInstancesForEquipment(USkyraEquipmentInstance* OwningEquipment) const
{
	TArray<USkyraSubEquipmentInstance*> Result;
	for (const FSkyraAppliedSubEquipmentEntry& Entry : SubEquipmentList.Entries)
	{
		if ((Entry.OwningEquipment == OwningEquipment) && (Entry.Instance != nullptr))
		{
			Result.Add(Entry.Instance);
		}
	}

	return Result;
}

USkyraSubEquipmentInstance* USkyraSubEquipmentManagerComponent::FindSubEquipmentInstanceByItem(USkyraInventoryItemInstance* SubItemInstance) const
{
	const int32 EntryIndex = FindRuntimeEntryIndexBySubItem(SubItemInstance);
	return (EntryIndex != INDEX_NONE) ? SubEquipmentList.Entries[EntryIndex].Instance : nullptr;
}

USkyraSubEquipmentInstance* USkyraSubEquipmentManagerComponent::FindSubEquipmentInstance(USkyraEquipmentInstance* OwningEquipment, FGameplayTag SubEquipmentTag) const
{
	const int32 EntryIndex = FindRuntimeEntryIndexByEquipmentAndTag(OwningEquipment, SubEquipmentTag);
	return (EntryIndex != INDEX_NONE) ? SubEquipmentList.Entries[EntryIndex].Instance : nullptr;
}

bool USkyraSubEquipmentManagerComponent::CanAttachSubEquipmentItem_Implementation(
	USkyraEquipmentInstance* OwningEquipment,
	USkyraInventoryItemInstance* OwnerItemInstance,
	USkyraInventoryItemInstance* SubItemInstance) const
{
	if ((GetOwner() == nullptr) || !GetOwner()->HasAuthority() || (OwningEquipment == nullptr) || (OwnerItemInstance == nullptr) || (SubItemInstance == nullptr))
	{
		return false;
	}

	const int32 BindingIndex = FindItemBindingIndexBySubItem(SubItemInstance);
	if ((BindingIndex != INDEX_NONE) && (ItemBindings[BindingIndex].OwnerItemInstance != OwnerItemInstance))
	{
		return false;
	}

	FSkyraSubEquipmentAttachData AttachData;
	return BuildSubEquipmentAttachData(SubItemInstance, AttachData);
}

bool USkyraSubEquipmentManagerComponent::BuildSubEquipmentAttachData(
	const USkyraInventoryItemInstance* SubItemInstance,
	FSkyraSubEquipmentAttachData& OutAttachData) const
{
	OutAttachData = FSkyraSubEquipmentAttachData();

	if (SubItemInstance == nullptr)
	{
		return false;
	}

	OutAttachData.Fragment = SubItemInstance->FindFragmentByClass<UInventoryFragment_SubEquipment>();
	if (OutAttachData.Fragment == nullptr)
	{
		return false;
	}

	OutAttachData.DefinitionClass = OutAttachData.Fragment->SubEquipmentDefinition;
	OutAttachData.Definition = OutAttachData.DefinitionClass ? GetDefault<USkyraSubEquipmentDefinition>(OutAttachData.DefinitionClass) : nullptr;
	if (OutAttachData.Definition == nullptr)
	{
		return false;
	}

	OutAttachData.InstanceType = OutAttachData.Definition->InstanceType;
	if (OutAttachData.InstanceType == nullptr)
	{
		OutAttachData.InstanceType = USkyraSubEquipmentInstance::StaticClass();
	}

	return true;
}

void USkyraSubEquipmentManagerComponent::InitializeSubEquipmentInstance(
	USkyraSubEquipmentInstance* Instance,
	USkyraEquipmentInstance* OwningEquipment,
	USkyraInventoryItemInstance* SubItemInstance,
	TSubclassOf<USkyraSubEquipmentDefinition> DefinitionClass)
{
	if (Instance != nullptr)
	{
		Instance->InitializeSubEquipment(OwningEquipment, SubItemInstance, DefinitionClass);
	}
}

void USkyraSubEquipmentManagerComponent::GrantSubEquipmentAbilities(USkyraSubEquipmentInstance* Instance)
{
	if (Instance == nullptr)
	{
		return;
	}

	USkyraAbilitySystemComponent* ASC = GetAbilitySystemComponentForEquipment(Instance->GetOwningEquipment());
	const USkyraSubEquipmentDefinition* Definition = Instance->GetSubEquipmentDefinition();
	if ((ASC == nullptr) || (Definition == nullptr))
	{
		return;
	}

	for (const TObjectPtr<const USkyraAbilitySet>& AbilitySet : Definition->AbilitySetsToGrant)
	{
		if (AbilitySet != nullptr)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &Instance->GetMutableGrantedHandles(), Instance);
		}
	}
}

void USkyraSubEquipmentManagerComponent::RemoveSubEquipmentAbilities(USkyraSubEquipmentInstance* Instance)
{
	if (Instance == nullptr)
	{
		return;
	}

	if (USkyraAbilitySystemComponent* ASC = GetAbilitySystemComponentForEquipment(Instance->GetOwningEquipment()))
	{
		Instance->GetMutableGrantedHandles().TakeFromAbilitySystem(ASC);
	}
}

int32 USkyraSubEquipmentManagerComponent::FindItemBindingIndexBySubItem(USkyraInventoryItemInstance* SubItemInstance) const
{
	for (int32 Index = 0; Index < ItemBindings.Num(); ++Index)
	{
		if (ItemBindings[Index].SubItemInstance == SubItemInstance)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USkyraSubEquipmentManagerComponent::FindRuntimeEntryIndexBySubItem(USkyraInventoryItemInstance* SubItemInstance) const
{
	for (int32 Index = 0; Index < SubEquipmentList.Entries.Num(); ++Index)
	{
		if (SubEquipmentList.Entries[Index].SubItemInstance == SubItemInstance)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USkyraSubEquipmentManagerComponent::FindRuntimeEntryIndexByInstance(USkyraSubEquipmentInstance* Instance) const
{
	for (int32 Index = 0; Index < SubEquipmentList.Entries.Num(); ++Index)
	{
		if (SubEquipmentList.Entries[Index].Instance == Instance)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USkyraSubEquipmentManagerComponent::FindRuntimeEntryIndexByEquipmentAndTag(
	USkyraEquipmentInstance* OwningEquipment,
	FGameplayTag SubEquipmentTag) const
{
	if ((OwningEquipment == nullptr) || !SubEquipmentTag.IsValid())
	{
		return INDEX_NONE;
	}

	for (int32 Index = 0; Index < SubEquipmentList.Entries.Num(); ++Index)
	{
		USkyraSubEquipmentInstance* Instance = SubEquipmentList.Entries[Index].Instance;
		if ((SubEquipmentList.Entries[Index].OwningEquipment == OwningEquipment)
			&& (Instance != nullptr)
			&& Instance->GetSubEquipmentTag().MatchesTagExact(SubEquipmentTag))
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

USkyraAbilitySystemComponent* USkyraSubEquipmentManagerComponent::GetAbilitySystemComponentForEquipment(USkyraEquipmentInstance* OwningEquipment) const
{
	APawn* Pawn = OwningEquipment ? OwningEquipment->GetPawn() : nullptr;
	return Pawn ? Cast<USkyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn)) : nullptr;
}

void USkyraSubEquipmentManagerComponent::AddReplicatedSubEquipmentInstance(USkyraSubEquipmentInstance* Instance)
{
	if (IsValid(Instance) && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}
}

void USkyraSubEquipmentManagerComponent::RemoveReplicatedSubEquipmentInstance(USkyraSubEquipmentInstance* Instance)
{
	if (IsValid(Instance) && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(Instance);
	}
}
