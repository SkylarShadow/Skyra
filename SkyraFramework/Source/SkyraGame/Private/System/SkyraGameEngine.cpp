// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameEngine)

class IEngineLoop;


USkyraGameEngine::USkyraGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USkyraGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

