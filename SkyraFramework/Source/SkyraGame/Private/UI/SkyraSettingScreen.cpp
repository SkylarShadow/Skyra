// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/SkyraLocalPlayer.h"
#include "Settings/SkyraGameSettingRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraSettingScreen)

class UGameSettingRegistry;

void USkyraSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* USkyraSettingScreen::CreateRegistry()
{
	USkyraGameSettingRegistry* NewRegistry = NewObject<USkyraGameSettingRegistry>();

	if (USkyraLocalPlayer* LocalPlayer = CastChecked<USkyraLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void USkyraSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void USkyraSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void USkyraSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void USkyraSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
