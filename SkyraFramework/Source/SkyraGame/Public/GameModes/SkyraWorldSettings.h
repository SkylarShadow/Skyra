// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "SkyraWorldSettings.generated.h"

class USkyraExperienceDefinition;

/**
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class SKYRAGAME_API ASkyraWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	ASkyraWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	//在“Map_Check”内部被调用，以便此角色能够检查自身是否存在任何潜在错误，并将这些错误信息通过“地图检查对话框”进行记录。
	virtual void CheckForErrors() override;
#endif

public:
	// Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	// 当服务器打开此地图时，如果用户界面体验没有覆盖，则返回要使用的默认体验
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	// 当服务器打开此地图时，如果用户界面体验没有覆盖，则使用默认体验
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<USkyraExperienceDefinition> DefaultGameplayExperience;

public:

#if WITH_EDITORONLY_DATA
	// Is this level part of a front-end or other standalone experience?
	// When set, the net mode will be forced to Standalone when you hit Play in the editor
	// 这关卡是否是前端或其他独立体验的一部分？当设置时，当你在编辑器中点击Play时，网络模式将被强制为Standalone
	UPROPERTY(EditDefaultsOnly, Category=PIE)
	bool ForceStandaloneNetMode = false;
#endif
};
