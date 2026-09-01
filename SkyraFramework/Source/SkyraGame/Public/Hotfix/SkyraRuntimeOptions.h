// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/RuntimeOptionsBase.h"

#include "SkyraRuntimeOptions.generated.h"

class UObject;
struct FFrame;

/**
 * USkyraRuntimeOptions: Supports checking at runtime whether features are enabled/disabled, changing
 * configuration parameters, console cheats, startup commands.
 *
 * Add a new Property that *defaults* (either naturally or in the constructor) to the desired
 * normal state. (e.g. bDisableSomething). If you ever need to suddenly disable that thing in the
 * live game, you'll be able to.
 *
 * For testing you can run with -ro.bDisableSomething=true to override the defaults. This is only
 * available in non-shipping builds.
 *
 * Variables are registered with the console under the 'ro' namespace. E.g. ro.bDisableSomething
 */
UCLASS(config = RuntimeOptions, BlueprintType)
class SKYRAGAME_API USkyraRuntimeOptions : public URuntimeOptionsBase
{
	GENERATED_BODY()

public:
	static const USkyraRuntimeOptions& Get();

	USkyraRuntimeOptions();

	UFUNCTION(BlueprintPure, Category = Options)
	static USkyraRuntimeOptions* GetRuntimeOptions();
};
