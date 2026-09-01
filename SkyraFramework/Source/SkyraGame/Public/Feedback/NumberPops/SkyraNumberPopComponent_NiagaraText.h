// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SkyraNumberPopComponent.h"

#include "SkyraNumberPopComponent_NiagaraText.generated.h"

class USkyraDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class USkyraNumberPopComponent_NiagaraText : public USkyraNumberPopComponent
{
	GENERATED_BODY()

public:

	USkyraNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~USkyraNumberPopComponent interface
	virtual void AddNumberPop(const FSkyraNumberPopRequest& NewRequest) override;
	//~End of USkyraNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<USkyraDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
