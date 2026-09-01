// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_SkyraContextEffectsLibrary.h"

#include "Feedback/ContextEffects/SkyraContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_SkyraContextEffectsLibrary::GetSupportedClass() const
{
	return USkyraContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
