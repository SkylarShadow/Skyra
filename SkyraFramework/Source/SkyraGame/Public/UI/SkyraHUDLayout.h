// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SkyraActivatableWidget.h"

#include "SkyraHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;


/**
 * USkyraHUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Skyra HUD Layout", Category = "Skyra|HUD"))
class USkyraHUDLayout : public USkyraActivatableWidget
{
	GENERATED_BODY()

public:

	USkyraHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
