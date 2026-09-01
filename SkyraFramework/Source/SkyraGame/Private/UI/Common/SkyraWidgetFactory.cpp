// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> USkyraWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
