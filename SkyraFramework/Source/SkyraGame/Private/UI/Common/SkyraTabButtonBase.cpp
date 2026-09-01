// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/SkyraTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraTabButtonBase)

class UObject;
struct FSlateBrush;

void USkyraTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void USkyraTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void USkyraTabButtonBase::SetTabLabelInfo_Implementation(const FSkyraTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

