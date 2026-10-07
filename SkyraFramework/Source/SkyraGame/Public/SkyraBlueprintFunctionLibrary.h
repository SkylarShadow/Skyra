// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyraBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SKYRAGAME_API USkyraBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Skyra|Build")
	static bool IsShippingBuild();

	UFUNCTION(BlueprintPure, Category = "Skyra|Build")
	static bool IsDevelopmentBuild();

	UFUNCTION(BlueprintPure, Category = "Skyra|Build")
	static bool IsDebugBuild();

	UFUNCTION(BlueprintPure, Category = "Skyra|Build")
	static bool IsEditor();

};
