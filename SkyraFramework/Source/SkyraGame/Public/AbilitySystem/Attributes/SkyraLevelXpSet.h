// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SkyraAttributeSet.h"
#include "SkyraLevelXpSet.generated.h"


/**
 *
 */
UCLASS()
class SKYRAGAME_API USkyraLevelXpSet : public USkyraAttributeSet
{
	GENERATED_BODY()
	
public:

	USkyraLevelXpSet();

	// 总经验
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TotalXP)
	FGameplayAttributeData TotalXP;

	ATTRIBUTE_ACCESSORS(USkyraLevelXpSet, TotalXP);

	// 当前等级
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Level)
	FGameplayAttributeData Level;

	ATTRIBUTE_ACCESSORS(USkyraLevelXpSet, Level);

protected:

	UFUNCTION()
	void OnRep_TotalXP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

public:

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
};
