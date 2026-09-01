// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "SkyraSettingValueDiscrete_PerfStat.generated.h"

enum class ESkyraDisplayablePerformanceStat : uint8;
enum class ESkyraStatDisplayMode : uint8;

class UObject;

UCLASS()
class USkyraSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	USkyraSettingValueDiscrete_PerfStat();

	void SetStat(ESkyraDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, ESkyraStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<ESkyraStatDisplayMode> DisplayModes;

	ESkyraDisplayablePerformanceStat StatToDisplay;
	ESkyraStatDisplayMode InitialMode;
};
