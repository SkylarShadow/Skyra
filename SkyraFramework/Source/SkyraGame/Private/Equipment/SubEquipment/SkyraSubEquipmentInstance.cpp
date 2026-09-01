// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/SubEquipment/SkyraSubEquipmentInstance.h"

#include "AbilitySystem/SkyraAbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Equipment/SkyraEquipmentDefinition.h"
#include "Equipment/SkyraEquipmentInstance.h"
#include "Equipment/SubEquipment/SkyraSubEquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraSubEquipmentInstance)

USkyraSubEquipmentInstance::USkyraSubEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* USkyraSubEquipmentInstance::GetWorld() const
{
	return OwningEquipment ? OwningEquipment->GetWorld() : nullptr;
}

void USkyraSubEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwningEquipment);
	DOREPLIFETIME(ThisClass, SourceItem);
	DOREPLIFETIME(ThisClass, SubEquipmentDefinition);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

#if UE_WITH_IRIS
void USkyraSubEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

void USkyraSubEquipmentInstance::InitializeSubEquipment(
	USkyraEquipmentInstance* InOwningEquipment,
	USkyraInventoryItemInstance* InSourceItem,
	TSubclassOf<USkyraSubEquipmentDefinition> InSubEquipmentDefinition)
{
	OwningEquipment = InOwningEquipment;
	SourceItem = InSourceItem;
	SubEquipmentDefinition = InSubEquipmentDefinition;
}

void USkyraSubEquipmentInstance::RemoveFromAbilitySystem(USkyraAbilitySystemComponent* SkyraASC)
{
	if (SkyraASC != nullptr)
	{
		GrantedHandles.TakeFromAbilitySystem(SkyraASC);
	}

	DestroySubEquipmentActors();
	OwningEquipment = nullptr;
	SourceItem = nullptr;
	SubEquipmentDefinition = nullptr;
}

const USkyraSubEquipmentDefinition* USkyraSubEquipmentInstance::GetSubEquipmentDefinition() const
{
	return SubEquipmentDefinition ? GetDefault<USkyraSubEquipmentDefinition>(SubEquipmentDefinition) : nullptr;
}

FGameplayTag USkyraSubEquipmentInstance::GetSubEquipmentTag() const
{
	if (const USkyraSubEquipmentDefinition* Definition = GetSubEquipmentDefinition())
	{
		return Definition->SubEquipmentTag;
	}

	return FGameplayTag();
}

FGameplayTag USkyraSubEquipmentInstance::GetSlotTag() const
{
	if (const USkyraSubEquipmentDefinition* Definition = GetSubEquipmentDefinition())
	{
		return Definition->SlotTag;
	}

	return FGameplayTag();
}

APawn* USkyraSubEquipmentInstance::GetPawn() const
{
	return OwningEquipment ? OwningEquipment->GetPawn() : nullptr;
}

void USkyraSubEquipmentInstance::SpawnSubEquipmentActors(const TArray<FSkyraEquipmentActorToSpawn>& ActorsToSpawn)
{
	APawn* OwningPawn = GetPawn();
	UWorld* World = GetWorld();
	if ((OwningPawn == nullptr) || (World == nullptr))
	{
		return;
	}

	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
	if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Character->GetMesh();
	}

	for (const FSkyraEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
	{
		if (SpawnInfo.ActorToSpawn == nullptr)
		{
			continue;
		}

		AActor* NewActor = World->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
		if (NewActor == nullptr)
		{
			continue;
		}

		NewActor->FinishSpawning(FTransform::Identity, true);
		NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
		NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
		SpawnedActors.Add(NewActor);
	}
}

void USkyraSubEquipmentInstance::DestroySubEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor != nullptr)
		{
			Actor->Destroy();
		}
	}

	SpawnedActors.Reset();
}

void USkyraSubEquipmentInstance::OnAttached()
{
	K2_OnAttached();
}

void USkyraSubEquipmentInstance::OnDetached()
{
	K2_OnDetached();
}

void USkyraSubEquipmentInstance::OnRep_OwningEquipment()
{
}

void USkyraSubEquipmentInstance::OnRep_SourceItem()
{
}
