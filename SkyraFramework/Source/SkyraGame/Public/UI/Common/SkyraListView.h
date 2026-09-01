// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonListView.h"

#include "SkyraListView.generated.h"

class UUserWidget;
class ULocalPlayer;
class USkyraWidgetFactory;

UCLASS(meta = (DisableNativeTick))
class SKYRAGAME_API USkyraListView : public UCommonListView
{
	GENERATED_BODY()

public:
	USkyraListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	//virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
	TArray<TObjectPtr<USkyraWidgetFactory>> FactoryRules;
};
