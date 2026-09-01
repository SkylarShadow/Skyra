// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameModes/SkyraGameMode.h"
#include "SkyraLogChannels.h"
#include "Perception/AIPerceptionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraAIController)

class UObject;

ASkyraAIController::ASkyraAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = false;
	bStopAILogicOnUnposses = false;
}

/*void ASkyraAIController::OnPawnChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}*/


void ASkyraAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(LogSkyraTeams, Error, TEXT("You can't set the team ID on a AI controller (%s); it's driven by the associated SkyraCharacter(WithAbilities"), *GetPathNameSafe(this));
}

FGenericTeamId ASkyraAIController::GetGenericTeamId() const
{
	// 注意这里的TeamId从Pawn拿
	if (ISkyraTeamAgentInterface* PawnWithTeamInterface = Cast<ISkyraTeamAgentInterface>(GetPawn()))
	{
		return PawnWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnSkyraTeamIndexChangedDelegate* ASkyraAIController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}


void ASkyraAIController::ServerRestartController()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	ensure((GetPawn() == nullptr) && IsInState(NAME_Inactive));

	if (IsInState(NAME_Inactive) || (IsInState(NAME_Spectating)))
	{
 		ASkyraGameMode* const GameMode = GetWorld()->GetAuthGameMode<ASkyraGameMode>();

		if ((GameMode == nullptr) || !GameMode->ControllerCanRestart(this))
		{
			return;
		}

		// If we're still attached to a Pawn, leave it
		if (GetPawn() != nullptr)
		{
			UnPossess();
		}

		// Re-enable input, similar to code in ClientRestart
		ResetIgnoreInputFlags();

		GameMode->RestartPlayer(this);
	}
}

ETeamAttitude::Type ASkyraAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {

		if (const ISkyraTeamAgentInterface* TeamAgent = Cast<ISkyraTeamAgentInterface>(OtherPawn->GetController()))
		{
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();

			//Checking Other pawn ID to define Attitude
			if (OtherTeamID.GetId() != GetGenericTeamId().GetId())
			{
				return ETeamAttitude::Hostile;
			}
			else
			{
				return ETeamAttitude::Friendly;
			}
		}
	}

	return ETeamAttitude::Neutral;
}

void ASkyraAIController::UpdateTeamAttitude(UAIPerceptionComponent* AIPerception)
{
	if (AIPerception)
	{
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void ASkyraAIController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

