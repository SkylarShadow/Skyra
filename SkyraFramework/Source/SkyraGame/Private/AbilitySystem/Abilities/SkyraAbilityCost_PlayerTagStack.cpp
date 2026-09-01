// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "SkyraGameplayAbility.h"
#include "Player/SkyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAbilityCost_PlayerTagStack)

USkyraAbilityCost_PlayerTagStack::USkyraAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool USkyraAbilityCost_PlayerTagStack::CheckCost(const USkyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (ASkyraPlayerState* PS = Cast<ASkyraPlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void USkyraAbilityCost_PlayerTagStack::ApplyCost(const USkyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (ASkyraPlayerState* PS = Cast<ASkyraPlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}

