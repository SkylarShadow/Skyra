// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"

#include "SkyraTeamCreationComponent.generated.h"

class USkyraExperienceDefinition;
class ASkyraTeamPublicInfo;
class ASkyraTeamPrivateInfo;
class ASkyraPlayerState;
class AGameModeBase;
class APlayerController;
class USkyraTeamDisplayAsset;

UCLASS(Blueprintable)
class SKYRAGAME_API USkyraTeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	USkyraTeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	void OnExperienceLoaded(const USkyraExperienceDefinition* Experience);

protected:
	// List of teams to create (id to display asset mapping, the display asset can be left unset if desired)
	// 默认的队伍
	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TMap<uint8, TObjectPtr<USkyraTeamDisplayAsset>> DefaultTeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<ASkyraTeamPublicInfo> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<ASkyraTeamPrivateInfo> PrivateTeamInfoClass;

#if WITH_SERVER_CODE
protected:
	virtual void ServerCreateTeams();
	virtual void ServerAssignPlayersToTeams();

	/** Sets the team ID for the given player state. Spectator-only player states will be stripped of any team association. */
	virtual void ServerChooseTeamForPlayer(ASkyraPlayerState* PS);
	
	void ServerCreateTeam(int32 TeamId, USkyraTeamDisplayAsset* DisplayAsset);
	
private:
	void OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer);
	

	/** returns the Team ID with the fewest active players, or INDEX_NONE if there are no valid teams */
	int32 GetLeastPopulatedTeamID() const;
#endif
};
