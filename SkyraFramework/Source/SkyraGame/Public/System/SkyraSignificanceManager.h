// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SignificanceManager.h"

#include "SkyraSignificanceManager.generated.h"

class UObject;


//给一批对象计算“重要性”。常见标准是离玩家距离、是否在屏幕内、是否关键战斗对象等。可以根据重要性做性能优化
UCLASS()
class SKYRAGAME_API USkyraSignificanceManager : public USignificanceManager
{
	GENERATED_BODY()

};
