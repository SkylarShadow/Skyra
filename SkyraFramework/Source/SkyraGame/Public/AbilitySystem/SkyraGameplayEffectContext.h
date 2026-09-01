// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"

#include "SkyraGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class ISkyraAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct SKYRAGAME_API FSkyraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FSkyraGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FSkyraGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FSkyraGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static  FSkyraGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilitySource(const ISkyraAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const ISkyraAbilitySourceInterface* GetAbilitySource() const;
	
	//执行HitResults和其他无法进行内存复制的成员的深拷贝
	virtual FGameplayEffectContext* Duplicate() const override
	{
		FSkyraGameplayEffectContext* NewContext = new FSkyraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSkyraGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;
	
	
	//// ContextTag,
	/** Adds a semantic context tag that can later be read by GameplayCues or other post-hit systems */
	void AddContextTag(const FGameplayTag& InTag);

	/** Appends a set of semantic context tags to this effect context */
	void AddContextTags(const FGameplayTagContainer& InTags);

	/** Returns whether the effect context contains the given semantic tag */
	bool HasContextTag(const FGameplayTag& InTag) const;

	/** Returns all semantic context tags attached to this effect context */
	const FGameplayTagContainer& GetContextTags() const { return ContextTags; }
	//// ~ContextTag,
public:
	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID = -1;

	/** Semantic tags describing this specific hit/effect instance, such as critical hits */
	UPROPERTY()
	FGameplayTagContainer ContextTags;

protected:
	/** Ability Source object (should implement ISkyraAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FSkyraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FSkyraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

UCLASS()
class SKYRAGAME_API USkyraGameplayEffectContextBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category="Skyra|GameplayEffectContext")
	static FGameplayTagContainer GetContextTags(FGameplayEffectContextHandle Handle)
	{
		if (auto* Context = FSkyraGameplayEffectContext::ExtractEffectContext(Handle))
		{
			return Context->GetContextTags();
		}

		return FGameplayTagContainer();
	}
	
	UFUNCTION(BlueprintPure, Category="Skyra|GameplayEffectContext")
	static bool HasAnyContextTags(FGameplayEffectContextHandle Handle,const FGameplayTagContainer& Tags)
	{
		if (const auto* Context = FSkyraGameplayEffectContext::ExtractEffectContext(Handle))
		{
			return Context->GetContextTags().HasAny(Tags);
		}

		return false;
	}

	UFUNCTION(BlueprintPure, Category="Skyra|GameplayEffectContext")
	static bool HasAllContextTags(FGameplayEffectContextHandle Handle,const FGameplayTagContainer& Tags)
	{
		if (const auto* Context = FSkyraGameplayEffectContext::ExtractEffectContext(Handle))
		{
			return Context->GetContextTags().HasAll(Tags);
		}

		return false;
	}
};