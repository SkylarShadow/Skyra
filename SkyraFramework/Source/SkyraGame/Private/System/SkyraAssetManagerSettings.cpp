// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAssetManagerSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAssetManagerSettings)

USkyraAssetManagerSettings::USkyraAssetManagerSettings()
{
	SkyraGameDataPath = FSoftObjectPath(TEXT("/SkyraFramework/DefaultGameData.DefaultGameData"));
	
    DefaultPawnData = FSoftObjectPath(TEXT("/SkyraFramework/DefaultPawnData_EmptyPawn.DefaultPawnData_EmptyPawn"));
}
