// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameSession)


ASkyraGameSession::ASkyraGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ASkyraGameSession::ProcessAutoLogin()
{
	// This is actually handled in SkyraGameMode::TryDedicatedServerLogin
	return true;
}

void ASkyraGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ASkyraGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

