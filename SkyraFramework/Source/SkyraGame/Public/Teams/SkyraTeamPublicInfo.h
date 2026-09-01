// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SkyraTeamInfoBase.h"

#include "SkyraTeamPublicInfo.generated.h"

class USkyraTeamCreationComponent;
class USkyraTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class ASkyraTeamPublicInfo : public ASkyraTeamInfoBase
{
	GENERATED_BODY()

	friend USkyraTeamCreationComponent;

public:
	ASkyraTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkyraTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<USkyraTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<USkyraTeamDisplayAsset> TeamDisplayAsset;
};
