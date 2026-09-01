// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAnimInstance.h"
#include "AbilitySystemGlobals.h"
//#include "Character/SkyraCharacter.h"
//#include "Character/SkyraCharacterMovementComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAnimInstance)


USkyraAnimInstance::USkyraAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USkyraAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult USkyraAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void USkyraAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void USkyraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// const ASkyraCharacter* Character = Cast<ASkyraCharacter>(GetOwningActor());
	// if (!Character)
	// {
	// 	return;
	// }

	// USkyraCharacterMovementComponent* CharMoveComp = CastChecked<USkyraCharacterMovementComponent>(Character->GetCharacterMovement());
	// const FSkyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	// GroundDistance = GroundInfo.GroundDistance;
}

