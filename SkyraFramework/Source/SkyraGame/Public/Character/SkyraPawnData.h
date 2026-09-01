// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "SkyraPawnData.generated.h"

class APawn;
class USkyraAbilitySet;
class USkyraAbilityTagRelationshipMapping;
class USkyraCameraMode;
class USkyraInputConfig;
class UObject;


/**
 * USkyraPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Skyra Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class SKYRAGAME_API USkyraPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	USkyraPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ASkyraPawn or ASkyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Abilities")
	TArray<TObjectPtr<USkyraAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Abilities")
	TObjectPtr<USkyraAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Input")
	TObjectPtr<USkyraInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Camera")
	TSubclassOf<USkyraCameraMode> DefaultCameraMode;
};
