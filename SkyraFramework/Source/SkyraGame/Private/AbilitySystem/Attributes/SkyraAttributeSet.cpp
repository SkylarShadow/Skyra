// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAttributeSet.h"

#include "AbilitySystem/SkyraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAttributeSet)

class UWorld;


USkyraAttributeSet::USkyraAttributeSet()
{
}

UWorld* USkyraAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

USkyraAbilitySystemComponent* USkyraAttributeSet::GetSkyraAbilitySystemComponent() const
{
	return Cast<USkyraAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

