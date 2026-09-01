// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "SkyraInventoryItemDefinition.h"
#include "SkyraInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Skyra_Inventory_Message_StackChanged, "Skyra.Inventory.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Skyra_Inventory_Item_Count, "Skyra.Inventory.Item.Count");
//////////////////////////////////////////////////////////////////////
// FSkyraInventoryEntry

FString FSkyraInventoryEntry::GetDebugString() const
{
	TSubclassOf<USkyraInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FSkyraInventoryList

void FSkyraInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FSkyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FSkyraInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FSkyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FSkyraInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FSkyraInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FSkyraInventoryList::BroadcastChangeMessage(FSkyraInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FSkyraInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Skyra_Inventory_Message_StackChanged, Message);
}

USkyraInventoryItemInstance* FSkyraInventoryList::AddEntry(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	USkyraInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	//设置 ItemDef
	FSkyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<USkyraInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (USkyraInventoryItemFragment* Fragment : GetDefault<USkyraInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;
	// 26/07/24给inventory新增显示数量
	NewEntry.Instance->AddStatTagStack(TAG_Skyra_Inventory_Item_Count, StackCount);
	//const USkyraInventoryItemDefinition* ItemCDO = GetDefault<USkyraInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);
	BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
	return Result;
}

void FSkyraInventoryList::AddEntry(USkyraInventoryItemInstance* Instance)
{
	unimplemented();
}

void FSkyraInventoryList::RemoveEntry(USkyraInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FSkyraInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<USkyraInventoryItemInstance*> FSkyraInventoryList::GetAllItems() const
{
	TArray<USkyraInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FSkyraInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// USkyraInventoryManagerComponent

USkyraInventoryManagerComponent::USkyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void USkyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool USkyraInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

USkyraInventoryItemInstance* USkyraInventoryManagerComponent::AddItemDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	USkyraInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void USkyraInventoryManagerComponent::AddItemInstance(USkyraInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void USkyraInventoryManagerComponent::RemoveItemInstance(USkyraInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<USkyraInventoryItemInstance*> USkyraInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

USkyraInventoryItemInstance* USkyraInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef) const
{
	for (const FSkyraInventoryEntry& Entry : InventoryList.Entries)
	{
		USkyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 USkyraInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FSkyraInventoryEntry& Entry : InventoryList.Entries)
	{
		USkyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool USkyraInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (USkyraInventoryItemInstance* Instance = USkyraInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void USkyraInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing USkyraInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FSkyraInventoryEntry& Entry : InventoryList.Entries)
		{
			USkyraInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool USkyraInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FSkyraInventoryEntry& Entry : InventoryList.Entries)
	{
		USkyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class USkyraInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(USkyraInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class USkyraInventoryFilter_HasTag : public USkyraInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(USkyraInventoryItemInstance* Instance) const { return true; }
// };


