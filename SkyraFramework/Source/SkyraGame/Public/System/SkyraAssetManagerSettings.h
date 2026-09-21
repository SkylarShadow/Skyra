// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"

#include "SkyraAssetManagerSettings.generated.h"

class USkyraGameData;
class USkyraPawnData;

/**
 * Project settings for Skyra's asset manager defaults.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Skyra Asset Manager"))
class USkyraAssetManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USkyraAssetManagerSettings();

	// Global game data asset to use.
	UPROPERTY(config, EditAnywhere, Category=GameData)
	TSoftObjectPtr<USkyraGameData> SkyraGameDataPath;

	// Pawn data used when spawning player pawns if there isn't one set on the player state.
	UPROPERTY(config, EditAnywhere, Category=GameData)
	TSoftObjectPtr<USkyraPawnData> DefaultPawnData;
};
