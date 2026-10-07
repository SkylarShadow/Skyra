// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyraBlueprintFunctionLibrary.h"
#include "AbilitySystem/Abilities/SkyraGameplayAbility.h"
//#include "Character/SkyraCharacter.h"

bool USkyraBlueprintFunctionLibrary::IsShippingBuild()
{
#if UE_BUILD_SHIPPING
	return true;
#else
	return false;
#endif
}

bool USkyraBlueprintFunctionLibrary::IsDevelopmentBuild()
{
#if UE_BUILD_DEVELOPMENT
	return true;
#else
	return false;
#endif
}

bool USkyraBlueprintFunctionLibrary::IsDebugBuild()
{
#if UE_BUILD_DEBUG
	return true;
#else
	return false;
#endif
}

bool USkyraBlueprintFunctionLibrary::IsEditor()
{
#if WITH_EDITOR
	return GIsEditor;
#else
	return false;
#endif
}
