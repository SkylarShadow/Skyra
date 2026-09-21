// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"

#include "CommonGameSettings.generated.h"

class UGameUIPolicy;

/**
 * Project settings for CommonGame systems.
 */
UCLASS(MinimalAPI, config=Game, defaultconfig, meta=(DisplayName="Common Game"))
class UCommonGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category=UI)
	TSoftClassPtr<UGameUIPolicy> DefaultUIPolicyClass;
};
