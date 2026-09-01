// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraPawnData)

USkyraPawnData::USkyraPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}

