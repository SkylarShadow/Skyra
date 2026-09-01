// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraPerfStatContainerBase.h"

#include "Blueprint/WidgetTree.h"
#include "SkyraPerfStatWidgetBase.h"
#include "Settings/SkyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraPerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// USkyraPerfStatsContainerBase

USkyraPerfStatContainerBase::USkyraPerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USkyraPerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	USkyraSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void USkyraPerfStatContainerBase::NativeDestruct()
{
	USkyraSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void USkyraPerfStatContainerBase::UpdateVisibilityOfChildren()
{
	USkyraSettingsLocal* UserSettings = USkyraSettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == ESkyraStatDisplayMode::TextOnly) || (StatDisplayModeFilter == ESkyraStatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == ESkyraStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == ESkyraStatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (USkyraPerfStatWidgetBase* TypedWidget = Cast<USkyraPerfStatWidgetBase>(Widget))
		{
			const ESkyraStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case ESkyraStatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case ESkyraStatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case ESkyraStatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case ESkyraStatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}

