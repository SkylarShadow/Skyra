// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "SkyraSubEquipmentManagerComponent.generated.h"

class UActorChannel;
class UInventoryFragment_SubEquipment;
class USkyraAbilitySystemComponent;
class USkyraEquipmentInstance;
class USkyraInventoryItemInstance;
class USkyraSubEquipmentDefinition;
class USkyraSubEquipmentInstance;
class FLifetimeProperty;
struct FReplicationFlags;

struct FSkyraSubEquipmentAttachData
{
	const UInventoryFragment_SubEquipment* Fragment = nullptr;
	TSubclassOf<USkyraSubEquipmentDefinition> DefinitionClass;
	const USkyraSubEquipmentDefinition* Definition = nullptr;
	TSubclassOf<USkyraSubEquipmentInstance> InstanceType;
};

/** 库存层的子装备归属关系 */
USTRUCT(BlueprintType)
struct FSkyraSubEquipmentItemBinding
{
	GENERATED_BODY()

public:
	// 拥有子装备的父物品，例如武器 InventoryItemInstance。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Equipment|SubEquipment")
	TObjectPtr<USkyraInventoryItemInstance> OwnerItemInstance;

	// 子装备物品，例如枪管、模块、植入体等 InventoryItemInstance。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Equipment|SubEquipment")
	TObjectPtr<USkyraInventoryItemInstance> SubItemInstance;
};

/** 当前已激活到 EquipmentInstance 上的运行时子装备。 */
USTRUCT(BlueprintType)
struct FSkyraAppliedSubEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FSkyraAppliedSubEquipmentEntry() {}

	FString GetDebugString() const;

private:
	friend struct FSkyraSubEquipmentList;
	friend class USkyraSubEquipmentManagerComponent;

	// 当前子装备挂载到的运行时父装备。
	UPROPERTY()
	TObjectPtr<USkyraEquipmentInstance> OwningEquipment;

	// 父装备对应的库存物品。
	UPROPERTY()
	TObjectPtr<USkyraInventoryItemInstance> OwnerItemInstance;

	// 子装备对应的库存物品。
	UPROPERTY()
	TObjectPtr<USkyraInventoryItemInstance> SubItemInstance;

	// 子装备静态行为定义。
	UPROPERTY()
	TSubclassOf<USkyraSubEquipmentDefinition> SubEquipmentDefinition;

	// 子装备运行时实例。子装备 GA 的 SourceObject 指向它。
	UPROPERTY()
	TObjectPtr<USkyraSubEquipmentInstance> Instance;
};

/** 当前 Pawn 上已激活子装备的复制列表。 */
USTRUCT(BlueprintType)
struct FSkyraSubEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FSkyraSubEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FSkyraSubEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSkyraAppliedSubEquipmentEntry, FSkyraSubEquipmentList>(Entries, DeltaParms, *this);
	}

	USkyraSubEquipmentInstance* AddEntry(
		USkyraEquipmentInstance* OwningEquipment,
		USkyraInventoryItemInstance* OwnerItemInstance,
		USkyraInventoryItemInstance* SubItemInstance,
		TSubclassOf<USkyraSubEquipmentDefinition> SubEquipmentDefinition,
		TSubclassOf<USkyraSubEquipmentInstance> InstanceType);

	void RemoveEntry(USkyraSubEquipmentInstance* Instance);

private:
	USkyraAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend class USkyraSubEquipmentManagerComponent;

private:
	UPROPERTY()
	TArray<FSkyraAppliedSubEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FSkyraSubEquipmentList> : public TStructOpsTypeTraitsBase2<FSkyraSubEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
	FSkyraSubEquipmentChangedDelegate,
	USkyraEquipmentInstance*, OwningEquipment,
	USkyraInventoryItemInstance*, OwnerItemInstance,
	USkyraInventoryItemInstance*, SubItemInstance,
	USkyraSubEquipmentInstance*, SubEquipmentInstance,
	bool, bAdded);

/**
 * 通用子装备管理组件。
 *
 * 负责库存物品归属、运行时实例创建、AbilitySet 授予/回收和复制；
 * EquipmentInstance 只作为父装备上下文，不承担子装备管理逻辑。
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYRAGAME_API USkyraSubEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	USkyraSubEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;

	UPROPERTY(BlueprintAssignable, Category="Equipment|SubEquipment")
	FSkyraSubEquipmentChangedDelegate OnSubEquipmentChanged;

	// 建立库存层归属关系。不会创建运行时实例，适合读档或未装备时安装子装备。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	bool BindSubEquipmentItem(USkyraInventoryItemInstance* OwnerItemInstance, USkyraInventoryItemInstance* SubItemInstance);

	// 移除库存层归属关系。如果该子装备当前已激活，会先卸载运行时实例。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	bool UnbindSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance);

	// 绑定子装备物品并立即激活到指定装备实例上。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	USkyraSubEquipmentInstance* AttachSubEquipmentItemToEquipment(
		USkyraEquipmentInstance* OwningEquipment,
		USkyraInventoryItemInstance* OwnerItemInstance,
		USkyraInventoryItemInstance* SubItemInstance);

	// 只卸载运行时实例，不移除库存层绑定。适合父装备卸下时调用。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	bool DeactivateSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance);

	// 卸载并解绑指定子装备物品。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	bool DetachSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance);

	// 按标签卸载并解绑指定装备上的子装备。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	bool DetachSubEquipmentFromEquipment(USkyraEquipmentInstance* OwningEquipment, FGameplayTag SubEquipmentTag);

	// 激活指定父物品已经绑定的全部子装备。适合父装备装备成功后调用。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	void ActivateBoundSubEquipmentForEquipment(USkyraEquipmentInstance* OwningEquipment, USkyraInventoryItemInstance* OwnerItemInstance);

	// 只卸载指定装备当前激活的全部子装备，不移除库存层绑定。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	void DeactivateSubEquipmentForEquipment(USkyraEquipmentInstance* OwningEquipment);

	// 卸载并解绑指定装备物品拥有的全部子装备。
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment|SubEquipment")
	void DetachAllSubEquipmentFromOwnerItem(USkyraInventoryItemInstance* OwnerItemInstance);

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	TArray<USkyraInventoryItemInstance*> GetSubEquipmentItemsForOwnerItem(USkyraInventoryItemInstance* OwnerItemInstance) const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	USkyraInventoryItemInstance* GetOwnerItemForSubEquipmentItem(USkyraInventoryItemInstance* SubItemInstance) const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	TArray<USkyraSubEquipmentInstance*> GetSubEquipmentInstancesForEquipment(USkyraEquipmentInstance* OwningEquipment) const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	USkyraSubEquipmentInstance* FindSubEquipmentInstanceByItem(USkyraInventoryItemInstance* SubItemInstance) const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	USkyraSubEquipmentInstance* FindSubEquipmentInstance(USkyraEquipmentInstance* OwningEquipment, FGameplayTag SubEquipmentTag) const;

	// 校验是否允许安装 派生 Manager 可以在这里解释 CompatibleParentTags、插槽数量等项目规则。
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Equipment|SubEquipment")
	bool CanAttachSubEquipmentItem(
		USkyraEquipmentInstance* OwningEquipment,
		USkyraInventoryItemInstance* OwnerItemInstance,
		USkyraInventoryItemInstance* SubItemInstance) const;

protected:
	virtual bool CanAttachSubEquipmentItem_Implementation(
		USkyraEquipmentInstance* OwningEquipment,
		USkyraInventoryItemInstance* OwnerItemInstance,
		USkyraInventoryItemInstance* SubItemInstance) const;
	
	// 构建subequipment数据来源或实例类型
	virtual bool BuildSubEquipmentAttachData(const USkyraInventoryItemInstance* SubItemInstance, FSkyraSubEquipmentAttachData& OutAttachData) const;
	//初始化运行时上下文
	virtual void InitializeSubEquipmentInstance(USkyraSubEquipmentInstance* Instance, USkyraEquipmentInstance* OwningEquipment, USkyraInventoryItemInstance* SubItemInstance, TSubclassOf<USkyraSubEquipmentDefinition> DefinitionClass);
	//授予能力
	virtual void GrantSubEquipmentAbilities(USkyraSubEquipmentInstance* Instance);
	// 回收能力
	virtual void RemoveSubEquipmentAbilities(USkyraSubEquipmentInstance* Instance);

	int32 FindItemBindingIndexBySubItem(USkyraInventoryItemInstance* SubItemInstance) const;
	int32 FindRuntimeEntryIndexBySubItem(USkyraInventoryItemInstance* SubItemInstance) const;
	int32 FindRuntimeEntryIndexByInstance(USkyraSubEquipmentInstance* Instance) const;
	int32 FindRuntimeEntryIndexByEquipmentAndTag(USkyraEquipmentInstance* OwningEquipment, FGameplayTag SubEquipmentTag) const;

	USkyraAbilitySystemComponent* GetAbilitySystemComponentForEquipment(USkyraEquipmentInstance* OwningEquipment) const;
	void AddReplicatedSubEquipmentInstance(USkyraSubEquipmentInstance* Instance);
	void RemoveReplicatedSubEquipmentInstance(USkyraSubEquipmentInstance* Instance);

private:
	UPROPERTY(Replicated)
	FSkyraSubEquipmentList SubEquipmentList;

	// 库存层绑定关系。存档系统应保存 OwnerItem/SubItem 的持久 ID，而不是保存运行时 Instance。
	UPROPERTY(Transient)
	TArray<FSkyraSubEquipmentItemBinding> ItemBindings;
};
