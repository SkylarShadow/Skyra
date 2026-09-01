// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "SkyraPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

USkyraUICameraManagerComponent* USkyraUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (ASkyraPlayerCameraManager* PCCamera = Cast<ASkyraPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

USkyraUICameraManagerComponent::USkyraUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void USkyraUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void USkyraUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ASkyraPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool USkyraUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void USkyraUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void USkyraUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
