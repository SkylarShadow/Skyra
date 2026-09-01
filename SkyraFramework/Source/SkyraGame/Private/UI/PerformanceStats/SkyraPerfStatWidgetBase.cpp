// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraPerfStatWidgetBase.h"

#include "Engine/GameInstance.h"
#include "Performance/SkyraPerformanceStatSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraPerfStatWidgetBase)

//////////////////////////////////////////////////////////////////////
// USkyraPerfStatWidgetBase

USkyraPerfStatWidgetBase::USkyraPerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

double USkyraPerfStatWidgetBase::FetchStatValue()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<USkyraPerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}

