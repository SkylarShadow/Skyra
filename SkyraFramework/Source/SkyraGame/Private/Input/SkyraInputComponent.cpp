// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/SkyraLocalPlayer.h"
#include "Settings/SkyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraInputComponent)

class USkyraInputConfig;

USkyraInputComponent::USkyraInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void USkyraInputComponent::AddInputMappings(const USkyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void USkyraInputComponent::RemoveInputMappings(const USkyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void USkyraInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
