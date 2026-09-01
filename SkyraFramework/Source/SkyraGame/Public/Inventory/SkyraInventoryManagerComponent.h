// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "SkyraInventoryManagerComponent.generated.h"

class USkyraInventoryItemDefinition;
class USkyraInventoryItemInstance;
class USkyraInventoryManagerComponent;
class UObject;
struct FFrame;
struct FSkyraInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FSkyraInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<USkyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
//背包中的一个格子/条目
USTRUCT(BlueprintType)
struct FSkyraInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FSkyraInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FSkyraInventoryList;
	friend USkyraInventoryManagerComponent;
	
	//运行时物品实例。
	UPROPERTY()
	TObjectPtr<USkyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FSkyraInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FSkyraInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FSkyraInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<USkyraInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSkyraInventoryEntry, FSkyraInventoryList>(Entries, DeltaParms, *this);
	}

	USkyraInventoryItemInstance* AddEntry(TSubclassOf<USkyraInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(USkyraInventoryItemInstance* Instance);

	void RemoveEntry(USkyraInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FSkyraInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend USkyraInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FSkyraInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FSkyraInventoryList> : public TStructOpsTypeTraitsBase2<FSkyraInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class SKYRAGAME_API USkyraInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 StackCount = 1);
	
	//添加物品
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	USkyraInventoryItemInstance* AddItemDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(USkyraInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(USkyraInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<USkyraInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	USkyraInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FSkyraInventoryList InventoryList;
};
