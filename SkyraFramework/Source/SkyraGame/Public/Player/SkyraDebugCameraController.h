// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DebugCameraController.h"

#include "SkyraDebugCameraController.generated.h"

class UObject;


/**
 * ASkyraDebugCameraController
 *
 *	Used for controlling the debug camera when it is enabled via the cheat manager.
 */
UCLASS()
class ASkyraDebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:

	ASkyraDebugCameraController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void AddCheats(bool bForce) override;
};
