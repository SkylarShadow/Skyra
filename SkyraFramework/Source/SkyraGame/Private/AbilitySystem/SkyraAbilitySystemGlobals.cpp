// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAbilitySystemGlobals.h"

#include "SkyraGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAbilitySystemGlobals)

struct FGameplayEffectContext;

USkyraAbilitySystemGlobals::USkyraAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 不生效，必须要在DefaultGame.ini里设置
	// AbilitySystemGlobalsClassName = FSoftClassPath(("/Script/SkyraGame.SkyraAbilitySystemGlobals"));
	// GlobalGameplayCueManagerClass = FSoftClassPath(("/Script/SkyraGame.SkyraGameplayCueManager"));
	// PredictTargetGameplayEffects = false;
	// bUseDebugTargetFromHud = true;
	// ActivateFailIsDeadName = TEXT("Ability.ActivateFail.IsDead");
	// ActivateFailCooldownName = TEXT("Ability.ActivateFail.Cooldown");
	// ActivateFailCostName = TEXT("Ability.ActivateFail.Cost");
	// ActivateFailTagsBlockedName = TEXT("Ability.ActivateFail.TagsBlocked");
	// ActivateFailTagsMissingName = TEXT("Ability.ActivateFail.TagsMissing");
	// ActivateFailNetworkingName = TEXT("Ability.ActivateFail.Networking");
	// GameplayCueNotifyPaths.Add("/SkyraFramework/GameplayCueNotifies");
	// GameplayCueNotifyPaths.Add("/SkyraFramework/GameplayCues");
}	

FGameplayEffectContext* USkyraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FSkyraGameplayEffectContext();
}

