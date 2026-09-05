#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "UObject/Object.h"
#include "InterchangeType.generated.h"

USTRUCT(BlueprintType)
struct FInterchangeTexture
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Path;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TextureGroup> LODGroup = TEXTUREGROUP_World;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextureSourceColorSettings SourceColorSettings;

};
