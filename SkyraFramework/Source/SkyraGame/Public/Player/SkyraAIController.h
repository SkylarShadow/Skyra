// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularAIController.h"
#include "Teams/SkyraTeamAgentInterface.h"

#include "SkyraAIController.generated.h"

namespace ETeamAttitude { enum Type : int; }
struct FGenericTeamId;

class APlayerState;
class UAIPerceptionComponent;
class UObject;
struct FFrame;

/**
 * ASkyraAIController
 *
 *	SkyraAIController是用于游戏中ai单位的aicontroller,和PlayerBotController不同，他负责和玩家行为差异很大的character
 *	例如RPG中的小怪，boss,带生命或带能力的障碍物，防御塔、水晶等.....
 */
UCLASS(Blueprintable)
class SKYRAGAME_API ASkyraAIController : public AModularAIController, public ISkyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	ASkyraAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ISkyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnSkyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~End of ISkyraTeamAgentInterface interface

	// Attempts to restart this controller (e.g., to respawn it)
	//TODO: 待修正，不适合与直接重生使用
	void ServerRestartController();

	//Update Team Attitude for the AI
	UFUNCTION(BlueprintCallable, Category = "Skyra AI Player Controller")
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);

	virtual void OnUnPossess() override;


private:
	/*UFUNCTION()
	void OnPawnChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
	*/



private:
	UPROPERTY()
	FOnSkyraTeamIndexChangedDelegate OnTeamChangedDelegate;
	
};
