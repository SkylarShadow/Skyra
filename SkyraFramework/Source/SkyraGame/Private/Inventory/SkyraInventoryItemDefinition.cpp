// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// USkyraInventoryItemDefinition

USkyraInventoryItemDefinition::USkyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const USkyraInventoryItemFragment* USkyraInventoryItemDefinition::FindFragmentByClass(TSubclassOf<USkyraInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (USkyraInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// USkyraInventoryItemDefinition

const USkyraInventoryItemFragment* USkyraInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<USkyraInventoryItemDefinition> ItemDef, TSubclassOf<USkyraInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<USkyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

