// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkyraCameraMode.h"
#include "SkyraCameraMode_TopDownFollow.generated.h"

/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class SKYRAGAME_API USkyraCameraMode_TopDownFollow : public USkyraCameraMode
{
	GENERATED_BODY()
	
public:
	USkyraCameraMode_TopDownFollow();

protected:
	virtual void UpdateView(float DeltaTime) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="TopDown")
	float CameraDistance = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="TopDown")
	float CameraPitch = -60.f;

	UPROPERTY(EditDefaultsOnly, Category="TopDown")
	float FollowInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="TopDown")
	bool bFollowPawnYaw = false;

private:
	FVector SmoothedLocation;
};
