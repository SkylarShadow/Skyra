// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraPerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/SkyraPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraPerformanceSettings)

//////////////////////////////////////////////////////////////////////

USkyraPlatformSpecificRenderingSettings::USkyraPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const USkyraPlatformSpecificRenderingSettings* USkyraPlatformSpecificRenderingSettings::Get()
{
	USkyraPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

USkyraPerformanceSettings::USkyraPerformanceSettings()
{
	PerPlatformSettings.Initialize(USkyraPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FSkyraPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (ESkyraDisplayablePerformanceStat PerfStat : TEnumRange<ESkyraDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

