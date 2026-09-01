// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

SKYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSkyra, Log, All);
SKYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSkyraExperience, Log, All);
SKYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSkyraAbilitySystem, Log, All);
SKYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSkyraTeams, Log, All);

SKYRAGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
