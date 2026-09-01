// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameplayEffectContext.h"

#include "AbilitySystem/SkyraAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameplayEffectContext)

class FArchive;

FSkyraGameplayEffectContext* FSkyraGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FSkyraGameplayEffectContext::StaticStruct()))
	{
		return (FSkyraGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FSkyraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);
	ContextTags.NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FSkyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(SkyraGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FSkyraGameplayEffectContext::SetAbilitySource(const ISkyraAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const ISkyraAbilitySourceInterface* FSkyraGameplayEffectContext::GetAbilitySource() const
{
	return Cast<ISkyraAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FSkyraGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

void FSkyraGameplayEffectContext::AddContextTag(const FGameplayTag& InTag)
{
	if (InTag.IsValid())
	{
		ContextTags.AddTag(InTag);
	}
}

void FSkyraGameplayEffectContext::AddContextTags(const FGameplayTagContainer& InTags)
{
	ContextTags.AppendTags(InTags);
}

bool FSkyraGameplayEffectContext::HasContextTag(const FGameplayTag& InTag) const
{
	return InTag.IsValid() && ContextTags.HasTag(InTag);
}

