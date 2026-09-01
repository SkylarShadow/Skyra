#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InterchangeType.generated.h"

USTRUCT(BlueprintType)
struct FInterchangeTexture
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Path;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TextureGroup> LODGroup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextureSourceColorSettings SourceColorSettings;

};
