// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_Empty.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_Empty)

#define LOCTEXT_NAMESPACE "SkyraGameFeatures"

void UGameFeatureAction_Empty::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
}
