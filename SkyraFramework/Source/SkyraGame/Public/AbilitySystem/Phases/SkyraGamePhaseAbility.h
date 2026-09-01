// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/SkyraGameplayAbility.h"
#include "SkyraGamePhaseAbility.generated.h"

/**
 * USkyraGamePhaseAbility(GPA)
 *
 * The base gameplay ability for any ability that is used to change the active game phase.
 * 
 * 可以使用 USkyraGamePhaseSubsytem从蓝图切换phase，
 * 这会结束前一个phase的GA（如果它正在运行）并激活新GA。
 */
UCLASS(Abstract, HideCategories = Input)
class USkyraGamePhaseAbility : public USkyraGameplayAbility
{
	GENERATED_BODY()

public:

	USkyraGamePhaseAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const FGameplayTag& GetGamePhaseTag() const { return GamePhaseTag; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	// Defines the game phase that this game phase ability is part of.  So for example,
	// if your game phase is GamePhase.RoundStart, then it will cancel all sibling phases.
	// So if you had a phase such as GamePhase.WaitingToStart that was active, starting
	// the ability part of RoundStart would end WaitingToStart.  However to get nested behaviors
	// you can also nest the phases.  So for example, GamePhase.Playing.NormalPlay, is a sub-phase
	// of the parent GamePhase.Playing, so changing the sub-phase to GamePhase.Playing.SuddenDeath,
	// would stop any ability tied to GamePhase.Playing.*, but wouldn't end any ability 
	// tied to the GamePhase.Playing phase.
	/*
	 * 定义此GPA所属的游戏阶段。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skyra|Game Phase")
	FGameplayTag GamePhaseTag;
};
