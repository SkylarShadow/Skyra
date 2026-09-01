// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayTagsManager.h"
#include "Modules/ModuleManager.h"


/**
 * FSkyraGameModule
 */
class FSkyraGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		UGameplayTagsManager::Get().AddTagIniSearchPath(FPaths::ProjectPluginsDir() / TEXT("SkyraFramework/Config/Tags"));
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FSkyraGameModule, SkyraGame);
