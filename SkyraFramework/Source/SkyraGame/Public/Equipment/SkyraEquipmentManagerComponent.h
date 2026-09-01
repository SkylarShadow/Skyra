// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/SkyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "SkyraEquipmentManagerComponent.generated.h"

class UActorComponent;
class USkyraAbilitySystemComponent;
class USkyraEquipmentDefinition;
class USkyraEquipmentInstance;
class USkyraEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FSkyraEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
/**
 * 单个装备条目
 *
 * 这是“已经装备”的运行时数据
 * 使用 FastArraySerializerItem 支持高效网络同步
 */
USTRUCT(BlueprintType)
struct FSkyraAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FSkyraAppliedEquipmentEntry()
	{}
	
	// 调试字符串
	FString GetDebugString() const;

private:
	friend FSkyraEquipmentList;
	friend USkyraEquipmentManagerComponent;

	// The equipment class that got equipped
	/**
	 * 装备定义类（DataAsset）
	 */
	UPROPERTY()
	TSubclassOf<USkyraEquipmentDefinition> EquipmentDefinition;
	
	/**
	 * 装备实例（Runtime）
	 *
	 * 每次装备都会创建一个 Instance
	 */
	UPROPERTY()
	TObjectPtr<USkyraEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	/**
	 * 装备给予的 Ability / Effect 句柄
	 *
	 * 只在服务器存在
	 * 卸载装备时用于回收能力
	 */
	UPROPERTY(NotReplicated)
	FSkyraAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
/**
 * 装备列表
 * 使用 FastArraySerializer 实现高效增量同步
 */
USTRUCT(BlueprintType)
struct FSkyraEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FSkyraEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FSkyraEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	/**
	 * 条目删除前
	 *
	 * 客户端收到“装备被移除”时调用
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	/**
	 * 条目新增后
	 *
	 * 客户端收到“新装备”时调用
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	/**
	 * 条目修改后
	 */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract
	
	/**
	 * FastArray 网络序列化
	 * Unreal 的高性能数组同步方案
	 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSkyraAppliedEquipmentEntry, FSkyraEquipmentList>(Entries, DeltaParms, *this);
	}
	
	/**
	 * 添加装备
	 * 1. 创建 EquipmentInstance
	 * 2. 授予能力
	 * 3. Spawn 装备 Actor
	 */
	USkyraEquipmentInstance* AddEntry(TSubclassOf<USkyraEquipmentDefinition> EquipmentDefinition);
	/**
	 * 移除装备
	 * 1. 回收能力
	 * 2. Destroy Actor
	 * 3. 删除数组项
	 */
	void RemoveEntry(USkyraEquipmentInstance* Instance);

private:
	USkyraAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend USkyraEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	/**
	 * 当前装备列表
	 * 会复制到客户端
	 */
	UPROPERTY()
	TArray<FSkyraAppliedEquipmentEntry> Entries;
	
	/**
	 * 所属组件
	 * 不复制
	 */
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 *  Struct 使用 NetDeltaSerialize
 */
template<>
struct TStructOpsTypeTraits<FSkyraEquipmentList> : public TStructOpsTypeTraitsBase2<FSkyraEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};




/**
* Manages equipment applied to a pawn
* Manages items equipped on a specific Pawn.
* Only one weapon can be equipped on a character at a time.
* Holds bookkeeping of equipped instances. (array of AppliedEquipmentEntry)
* Owned by a Pawn.
 */
UCLASS(BlueprintType, Const)
class SKYRAGAME_API USkyraEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	USkyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/**
	 * 装备物品
	 * 服务器调用
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	USkyraEquipmentInstance* EquipItem(TSubclassOf<USkyraEquipmentDefinition> EquipmentDefinition);
	
	/**
	 * 卸载物品
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(USkyraEquipmentInstance* ItemInstance);

	//~UObject interface
	/**
	 * 手动同步子对象
	 * 用于同步 EquipmentInstance
	 */
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkyraEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<USkyraEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<USkyraEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<USkyraEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FSkyraEquipmentList EquipmentList;
};
