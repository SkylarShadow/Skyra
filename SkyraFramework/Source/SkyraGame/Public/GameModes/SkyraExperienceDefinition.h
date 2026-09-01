// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "SkyraExperienceDefinition.generated.h"

class UGameFeatureAction;
class USkyraPawnData;
class USkyraExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class USkyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USkyraExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	// 验证数据是否合法
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// List of Game Feature Plugins this experience wants to have active
	// 这个experience想要激活的Game Feature插件列表
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** The default pawn class to spawn for players */
	// 用于生成玩家的默认pawn类
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const USkyraPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	// 这个experience在加载/激活/停用/卸载时要执行的操作列表
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of additional action sets to compose into this experience
	// 用于构成此experience的额外操作集列表
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<USkyraExperienceActionSet>> ActionSets;
};
