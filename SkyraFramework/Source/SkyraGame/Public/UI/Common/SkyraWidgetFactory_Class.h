// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SkyraWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "SkyraWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

UCLASS()
class SKYRAGAME_API USkyraWidgetFactory_Class : public USkyraWidgetFactory
{
	GENERATED_BODY()

public:
	USkyraWidgetFactory_Class() { }

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
