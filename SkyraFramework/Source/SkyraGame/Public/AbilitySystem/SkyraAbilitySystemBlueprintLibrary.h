// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SkyraAbilitySystemBlueprintLibrary.generated.h"

class AActor;
class USkyraAbilitySystemComponent;

/**
 * Blueprint helpers for common Skyra ability-system operations.
 */
UCLASS()
class SKYRAGAME_API USkyraAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static bool ApplySetByCallerDamageEffectToActor(AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static FGameplayEffectSpecHandle MakeSetByCallerDamageEffectSpec(USkyraAbilitySystemComponent* SourceASC, float DamageAmount, float Level = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static FGameplayEffectSpecHandle MakeSetByCallerHealEffectSpec(USkyraAbilitySystemComponent* SourceASC, float HealAmount, float Level = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static bool ApplyGameplayEffectSpecToSelf(USkyraAbilitySystemComponent* ASC, const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static bool ApplyGameplayEffectSpecToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem")
	static bool ApplyGameplayEffectSpecToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerDamageEffectToSelf(USkyraAbilitySystemComponent* ASC, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerDamageEffectToSelf(USkyraAbilitySystemComponent* ASC, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerHealEffectToSelf(USkyraAbilitySystemComponent* ASC, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerHealEffectToSelf(USkyraAbilitySystemComponent* ASC, float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerDamageEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerDamageEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerDamageEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float DamageAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerDamageEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerHealEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerHealEffectToTargetASC(USkyraAbilitySystemComponent* SourceASC, USkyraAbilitySystemComponent* TargetASC, float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Skyra|AbilitySystem", meta = (AdvancedDisplay = "OptionalSpecHandle", AutoCreateRefTerm = "OptionalSpecHandle"))
	static bool ApplySetByCallerHealEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float HealAmount, const FGameplayEffectSpecHandle& OptionalSpecHandle);

	static bool ApplySetByCallerHealEffectToTargetActor(USkyraAbilitySystemComponent* SourceASC, AActor* TargetActor, float HealAmount);
};
