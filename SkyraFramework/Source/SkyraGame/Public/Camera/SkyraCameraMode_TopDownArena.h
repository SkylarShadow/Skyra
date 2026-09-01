// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkyraCameraMode.h"
#include "Curves/CurveFloat.h"
#include "SkyraCameraMode_TopDownArena.generated.h"

/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class SKYRAGAME_API USkyraCameraMode_TopDownArena : public USkyraCameraMode
{
	GENERATED_BODY()
	
public:	
	
	USkyraCameraMode_TopDownArena();
	
protected:

	//~ULyraCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~End of ULyraCameraMode interface
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	float ArenaWidth;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	float ArenaHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FRotator DefaultPivotRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FRuntimeFloatCurve BoundsSizeToDistance;
};
