// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SkyraInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class USkyraInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class SKYRAGAME_API USkyraInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(USkyraInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * USkyraInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class USkyraInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	USkyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<USkyraInventoryItemFragment>> Fragments;

public:
	const USkyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<USkyraInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class USkyraInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const USkyraInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, TSubclassOf<USkyraInventoryItemFragment> FragmentClass);
};
