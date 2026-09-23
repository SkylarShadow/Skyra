// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"

#include "SkyraPlayerSpawningManagerComponent.generated.h"

class AController;
class APlayerController;
class APlayerState;
class APlayerStart;
class ASkyraPlayerStart;
class AActor;

/**
 * @class USkyraPlayerSpawningManagerComponent
 */
UCLASS(Blueprintable)
class SKYRAGAME_API USkyraPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	USkyraPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** ~UActorComponent */

	/** Entry points proxied from ASkyraGameMode. Override the protected hooks below for project-specific spawn behavior. */
	AActor* ChoosePlayerStart(AController* Player);
	bool ControllerCanRestart(AController* Player) const;
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);

protected:
	// Utility
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Skyra|Spawning", meta=(BlueprintProtected="true"))
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<ASkyraPlayerStart*>& FoundStartPoints) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Skyra|Spawning")
	AActor* OnChoosePlayerStart(AController* Player, const TArray<ASkyraPlayerStart*>& PlayerStarts);
	virtual AActor* OnChoosePlayerStart_Implementation(AController* Player, const TArray<ASkyraPlayerStart*>& PlayerStarts);

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Skyra|Spawning")
	bool OnControllerCanRestart(AController* Player) const;
	virtual bool OnControllerCanRestart_Implementation(AController* Player) const;

	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) { }

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category="Skyra|Spawning")
	bool ShouldClaimPlayerStart(AController* Player, ASkyraPlayerStart* PlayerStart) const;
	virtual bool ShouldClaimPlayerStart_Implementation(AController* Player, ASkyraPlayerStart* PlayerStart) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category="Skyra|Spawning")
	void OnPlayerStartClaimed(AController* Player, ASkyraPlayerStart* PlayerStart);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ASkyraPlayerStart>> CachedPlayerStarts;

private:
	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};
