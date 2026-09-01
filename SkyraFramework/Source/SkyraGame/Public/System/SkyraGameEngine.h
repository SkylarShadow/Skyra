// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameEngine.h"

#include "SkyraGameEngine.generated.h"

class IEngineLoop;
class UObject;


UCLASS()
class USkyraGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	USkyraGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
