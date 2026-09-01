// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/SkyraTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraTeamPublicInfo)

class FLifetimeProperty;

ASkyraTeamPublicInfo::ASkyraTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ASkyraTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void ASkyraTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<USkyraTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void ASkyraTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

