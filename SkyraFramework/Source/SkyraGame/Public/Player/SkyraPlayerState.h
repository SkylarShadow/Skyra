// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "System/GameplayTagStack.h"
#include "Teams/SkyraTeamAgentInterface.h"

#include "SkyraPlayerState.generated.h"

struct FSkyraVerbMessage;

class AController;
class ASkyraPlayerController;
class APlayerState;
class FName;
class UAbilitySystemComponent;
class USkyraAbilitySystemComponent;
class USkyraExperienceDefinition;
class USkyraPawnData;
class UObject;
struct FFrame;
struct FGameplayTag;

/** Defines the types of client connected */
UENUM()
enum class ESkyraPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * ASkyraPlayerState
 *
 *	Base player state class used by this project.
 *	在这个架构中AbilitySystem,Attribute,PawnData,Team都放在PS中
 *	如果需要添加AttributeSet ，个人觉得继承此PlayerState去使用会更好（Character同理）
 */
UCLASS(Config = Game)
class SKYRAGAME_API ASkyraPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public ISkyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	ASkyraPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Skyra|PlayerState")
	ASkyraPlayerController* GetSkyraPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Skyra|PlayerState")
	USkyraAbilitySystemComponent* GetSkyraAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const USkyraPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	//~ISkyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnSkyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ISkyraTeamAgentInterface interface

	static const FName NAME_SkyraAbilityReady;

	void SetPlayerConnectionType(ESkyraPlayerConnectionType NewType);
	ESkyraPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	/** Returns the Squad ID of the squad the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	void SetSquadID(int32 NewSquadID);

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Skyra|PlayerState")
	void ClientBroadcastMessage(const FSkyraVerbMessage Message);

	// Gets the replicated view rotation of this player, used for spectating
	FRotator GetReplicatedViewRotation() const;

	// Sets the replicated view rotation, only valid on the server
	void SetReplicatedViewRotation(const FRotator& NewRotation);

private:
	void OnExperienceLoaded(const USkyraExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:
	// 这里的PawnData 是 “根” PawnData, PawnExtComp的PawnData是取这里的引用
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const USkyraPawnData> PawnData;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Skyra|PlayerState")
	TObjectPtr<USkyraAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class USkyraHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class USkyraCombatSet> CombatSet;
	
	/*UPROPERTY()
	TObjectPtr<const class USkyraLevelXpSet> LevelXPSet;*/

	UPROPERTY(Replicated)
	ESkyraPlayerConnectionType MyPlayerConnectionType;

	UPROPERTY()
	FOnSkyraTeamIndexChangedDelegate OnTeamChangedDelegate;
	
	// TODO: 添加一个组件之类的，对这个变量操作进行对队伍的关系管理（敌对友好中立）
	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
	int32 MySquadID;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnRep_MySquadID();
};
