// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraDebugCameraController.h"
#include "SkyraCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraDebugCameraController)


ASkyraDebugCameraController::ASkyraDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as SkyraPlayerController to allow toggling the debug camera through cheats.
	CheatClass = USkyraCheatManager::StaticClass();
}

void ASkyraDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors SkyraPlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

