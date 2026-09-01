// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/SkyraAbilitySet.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "SkyraSubEquipmentInstance.generated.h"

class AActor;
class APawn;
class USkyraAbilitySystemComponent;
class USkyraEquipmentInstance;
class USkyraInventoryItemInstance;
class USkyraSubEquipmentDefinition;
struct FSkyraEquipmentActorToSpawn;

/**
 * 子装备运行时实例。
 *
 * 它是子装备能力的 SourceObject，负责保存装备期间的上下文和授予句柄；
 * 权威状态仍然保存在 SourceItem 对应的 InventoryItemInstance 中。
 */
UCLASS(BlueprintType, Blueprintable)
class SKYRAGAME_API USkyraSubEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	USkyraSubEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override;
	//~End of UObject interface

	virtual void InitializeSubEquipment(
		USkyraEquipmentInstance* InOwningEquipment,
		USkyraInventoryItemInstance* InSourceItem,
		TSubclassOf<USkyraSubEquipmentDefinition> InSubEquipmentDefinition);

	virtual void RemoveFromAbilitySystem(USkyraAbilitySystemComponent* SkyraASC);

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	USkyraEquipmentInstance* GetOwningEquipment() const { return OwningEquipment; }

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	USkyraInventoryItemInstance* GetSourceItem() const { return SourceItem; }

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	TSubclassOf<USkyraSubEquipmentDefinition> GetSubEquipmentDefinitionClass() const { return SubEquipmentDefinition; }

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	const USkyraSubEquipmentDefinition* GetSubEquipmentDefinition() const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	FGameplayTag GetSubEquipmentTag() const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	FGameplayTag GetSlotTag() const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category="Equipment|SubEquipment")
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnSubEquipmentActors(const TArray<FSkyraEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroySubEquipmentActors();

	virtual void OnAttached();
	virtual void OnDetached();

	FSkyraAbilitySet_GrantedHandles& GetMutableGrantedHandles() { return GrantedHandles; }
	const FSkyraAbilitySet_GrantedHandles& GetGrantedHandles() const { return GrantedHandles; }

protected:
#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif

	UFUNCTION(BlueprintImplementableEvent, Category="Equipment|SubEquipment", meta=(DisplayName="OnAttached"))
	void K2_OnAttached();

	UFUNCTION(BlueprintImplementableEvent, Category="Equipment|SubEquipment", meta=(DisplayName="OnDetached"))
	void K2_OnDetached();

private:
	UFUNCTION()
	void OnRep_OwningEquipment();

	UFUNCTION()
	void OnRep_SourceItem();

protected:
	// 当前挂载到的父装备实例。运行时上下文，不是存档权威关系。
	UPROPERTY(ReplicatedUsing=OnRep_OwningEquipment, Transient)
	TObjectPtr<USkyraEquipmentInstance> OwningEquipment;

	// 子装备来源物品实例。品质、耐久、词条等可变状态从这里读取。
	UPROPERTY(ReplicatedUsing=OnRep_SourceItem, Transient)
	TObjectPtr<USkyraInventoryItemInstance> SourceItem;

	// 子装备静态行为定义类。
	UPROPERTY(Replicated, Transient)
	TSubclassOf<USkyraSubEquipmentDefinition> SubEquipmentDefinition;

	// 当前子装备授予出去的能力/效果/属性句柄。只在服务器用于精确回收。
	UPROPERTY(Transient)
	FSkyraAbilitySet_GrantedHandles GrantedHandles;

	// 子装备激活时生成的附属 Actor。
	UPROPERTY(Replicated, Transient)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
