// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAimSensitivityData.h"

#include "Settings/SkyraSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAimSensitivityData)

USkyraAimSensitivityData::USkyraAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ ESkyraGamepadSensitivity::Slow,			0.5f },
		{ ESkyraGamepadSensitivity::SlowPlus,		0.75f },
		{ ESkyraGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ ESkyraGamepadSensitivity::Normal,		1.0f },
		{ ESkyraGamepadSensitivity::NormalPlus,	1.1f },
		{ ESkyraGamepadSensitivity::NormalPlusPlus,1.25f },
		{ ESkyraGamepadSensitivity::Fast,			1.5f },
		{ ESkyraGamepadSensitivity::FastPlus,		1.75f },
		{ ESkyraGamepadSensitivity::FastPlusPlus,	2.0f },
		{ ESkyraGamepadSensitivity::Insane,		2.5f },
	};
}

const float USkyraAimSensitivityData::SensitivtyEnumToFloat(const ESkyraGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

