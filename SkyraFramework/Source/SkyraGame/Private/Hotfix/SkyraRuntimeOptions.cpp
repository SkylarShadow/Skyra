// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraRuntimeOptions.h"

#include "UObject/Class.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraRuntimeOptions)

USkyraRuntimeOptions::USkyraRuntimeOptions()
{
	OptionCommandPrefix = TEXT("ro");
}

USkyraRuntimeOptions* USkyraRuntimeOptions::GetRuntimeOptions()
{
	return GetMutableDefault<USkyraRuntimeOptions>();
}

const USkyraRuntimeOptions& USkyraRuntimeOptions::Get()
{
	const USkyraRuntimeOptions& RuntimeOptions = *GetDefault<USkyraRuntimeOptions>();
	return RuntimeOptions;
}
