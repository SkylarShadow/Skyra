// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SkyraVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FSkyraVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class SKYRAGAME_API USkyraVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Skyra")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Skyra")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Skyra")
	static FGameplayCueParameters VerbMessageToCueParameters(const FSkyraVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Skyra")
	static FSkyraVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
