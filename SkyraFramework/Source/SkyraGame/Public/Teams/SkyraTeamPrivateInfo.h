// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SkyraTeamInfoBase.h"

#include "SkyraTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class ASkyraTeamPrivateInfo : public ASkyraTeamInfoBase
{
	GENERATED_BODY()

public:
	ASkyraTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
