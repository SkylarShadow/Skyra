// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraTeamCheats.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Teams/SkyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraTeamCheats)

//////////////////////////////////////////////////////////////////////
// USkyraTeamCheats

void USkyraTeamCheats::CycleTeam()
{
	if (USkyraTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<USkyraTeamSubsystem>(GetWorld()))
	{
		APlayerController* PC = GetPlayerController();

		const int32 OldTeamId = TeamSubsystem->FindTeamFromObject(PC);
		const TArray<int32> TeamIds = TeamSubsystem->GetTeamIDs();
		
		if (TeamIds.Num())
		{
			const int32 IndexOfOldTeam = TeamIds.Find(OldTeamId);
			const int32 IndexToUse = (IndexOfOldTeam + 1) % TeamIds.Num();

			const int32 NewTeamId = TeamIds[IndexToUse];

			TeamSubsystem->ChangeTeamForActor(PC, NewTeamId);
		}

		const int32 ActualNewTeamId = TeamSubsystem->FindTeamFromObject(PC);

		UE_LOG(LogConsoleResponse, Log, TEXT("Changed to team %d (from team %d)"), ActualNewTeamId, OldTeamId);
	}
}

void USkyraTeamCheats::SetTeam(int32 TeamID)
{
	if (USkyraTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<USkyraTeamSubsystem>(GetWorld()))
	{
		if (TeamSubsystem->DoesTeamExist(TeamID))
		{
			APlayerController* PC = GetPlayerController();

			TeamSubsystem->ChangeTeamForActor(PC, TeamID);
		}
	}
}

void USkyraTeamCheats::ListTeams()
{
	if (USkyraTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<USkyraTeamSubsystem>(GetWorld()))
	{
		const TArray<int32> TeamIDs = TeamSubsystem->GetTeamIDs();

		for (const int32 TeamID : TeamIDs)
		{
			UE_LOG(LogConsoleResponse, Log, TEXT("Team ID %d"), TeamID);
		}
	}
}

