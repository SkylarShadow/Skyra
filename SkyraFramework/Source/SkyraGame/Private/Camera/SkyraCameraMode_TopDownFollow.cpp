// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyraCameraMode_TopDownFollow.h"


USkyraCameraMode_TopDownFollow::USkyraCameraMode_TopDownFollow()
{
	ViewPitchMin = -89.f;
	ViewPitchMax = 0.f;
}

void USkyraCameraMode_TopDownFollow::UpdateView(float DeltaTime)
{
	AActor* Target = GetTargetActor();
	if (!Target)
		return;
	
	// 1. Pivot（角色位置）
	FVector PivotLocation = Target->GetActorLocation();
	
	// 2. Rotation（俯视）
	float Yaw = 0.f;

	if (bFollowPawnYaw)
	{
		Yaw = Target->GetActorRotation().Yaw;
	}

	FRotator CameraRot = FRotator(CameraPitch, Yaw, 0.f);

	//----------------------------------
	// 3. Desired Camera Location
	//----------------------------------
	FVector DesiredLocation = PivotLocation - CameraRot.Vector() * CameraDistance;
	
	// 4. 平滑（防抖）
	if (SmoothedLocation.IsZero())
	{
		SmoothedLocation = DesiredLocation;
	}
	else
	{
		SmoothedLocation = FMath::VInterpTo(
			SmoothedLocation,
			DesiredLocation,
			DeltaTime,
			FollowInterpSpeed
		);
	}


	// 5. 输出 View
	View.Location = SmoothedLocation;
	View.Rotation = CameraRot;
	View.ControlRotation = CameraRot;
	View.FieldOfView = FieldOfView;
}