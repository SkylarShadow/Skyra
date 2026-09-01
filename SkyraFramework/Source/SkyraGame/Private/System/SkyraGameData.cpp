// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameData.h"
#include "SkyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameData)

USkyraGameData::USkyraGameData()
{
}

const USkyraGameData& USkyraGameData::USkyraGameData::Get()
{
	return USkyraAssetManager::Get().GetGameData();
}
