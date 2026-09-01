// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Performance/SkyraPerformanceStatTypes.h"

#include "SkyraPerfStatContainerBase.generated.h"

class UObject;
struct FFrame;

/**
 * USkyraPerfStatsContainerBase
 *
 * Panel that contains a set of USkyraPerfStatWidgetBase widgets and manages
 * their visibility based on user settings.
 */
 UCLASS(Abstract)
class USkyraPerfStatContainerBase : public UCommonUserWidget
{
public:
	USkyraPerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	// Are we showing text or graph stats?
	UPROPERTY(EditAnywhere, Category=Display)
	ESkyraStatDisplayMode StatDisplayModeFilter = ESkyraStatDisplayMode::TextAndGraph;
};
