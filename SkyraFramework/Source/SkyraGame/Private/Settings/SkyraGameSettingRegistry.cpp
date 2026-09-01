// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "SkyraSettingsLocal.h"
#include "SkyraSettingsShared.h"
#include "Player/SkyraLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogSkyraGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Skyra"

//--------------------------------------
// USkyraGameSettingRegistry
//--------------------------------------

USkyraGameSettingRegistry::USkyraGameSettingRegistry()
{
}

USkyraGameSettingRegistry* USkyraGameSettingRegistry::Get(USkyraLocalPlayer* InLocalPlayer)
{
	USkyraGameSettingRegistry* Registry = FindObject<USkyraGameSettingRegistry>(InLocalPlayer, TEXT("SkyraGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<USkyraGameSettingRegistry>(InLocalPlayer, TEXT("SkyraGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool USkyraGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (USkyraLocalPlayer* LocalPlayer = Cast<USkyraLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void USkyraGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	USkyraLocalPlayer* SkyraLocalPlayer = Cast<USkyraLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(SkyraLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, SkyraLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(SkyraLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(SkyraLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(SkyraLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(SkyraLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void USkyraGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (USkyraLocalPlayer* LocalPlayer = Cast<USkyraLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

