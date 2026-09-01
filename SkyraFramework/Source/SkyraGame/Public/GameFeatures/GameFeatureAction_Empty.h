// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_Empty.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI,meta = (DisplayName = "Empty Action"))
class UGameFeatureAction_Empty : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
	
private:
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
};
