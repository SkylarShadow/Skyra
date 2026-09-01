// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "Inventory/SkyraInventoryItemInstance.h"

#include "SkyraQuickBarComponent.generated.h"

class AActor;
class USkyraEquipmentInstance;
class USkyraEquipmentManagerComponent;
class UObject;
struct FFrame;

/*
 *快捷栏 / 武器栏 / 当前装备管理器
 * 负责内容
 *	保存当前拥有的快捷栏物品
	切换当前激活槽位
	装备/卸下武器
	同步到客户端
	广播 UI 消息
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class USkyraQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	USkyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/**
	 * 向前循环切换槽位（自动跳过空槽）
	 */
	UFUNCTION(BlueprintCallable, Category="Skyra")
	void CycleActiveSlotForward();

	/**
	 * 向后循环切换槽位（自动跳过空槽）
	 */
	UFUNCTION(BlueprintCallable, Category="Skyra")
	void CycleActiveSlotBackward();
	
	/**
	 * 服务器RPC：设置当前激活槽位
	 *
	 * 注意：
	 * - 只能由客户端请求
	 * - 真正逻辑在 Server 端执行
	 * - 用于防止客户端作弊直接改装备
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Skyra")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<USkyraInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}
	
	/**
	 * 获取当前选中槽位 index
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }
	
	/**
	 * 获取当前装备的物品
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	USkyraInventoryItemInstance* GetActiveSlotItem() const;
	
	/**
	 * 找到第一个空槽位
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;
	
	/**
	 * 向槽位添加物品
	 * AuthorityOnly：必须服务器执行
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, USkyraInventoryItemInstance* Item);
	
	/**
	 * 从槽位移除物品
	 * AuthorityOnly：必须服务器执行
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	USkyraInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	/**
	 * 卸下当前装备
	 * - 只负责 EquipmentManager 层
	 * - 不直接处理 Inventory
	 */
	void UnequipItemInSlot();
	/**
	* 装备当前槽位物品
	* **/
	void EquipItemInSlot();

	USkyraEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	/**
	 * 默认槽位数量
	 */
	UPROPERTY()
	int32 NumSlots = 3;
	
	/**
	 * RepNotify：槽位变化
	 */
	UFUNCTION()
	void OnRep_Slots();
	
	/**
	 * RepNotify：当前槽位变化
	 */
	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	//拥有装备列表
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<USkyraInventoryItemInstance>> Slots;
	
	//当前装备的索引
	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;
	
	//当前装备
	UPROPERTY()
	TObjectPtr<USkyraEquipmentInstance> EquippedItem;
};


USTRUCT(BlueprintType)
struct FSkyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<USkyraInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FSkyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
