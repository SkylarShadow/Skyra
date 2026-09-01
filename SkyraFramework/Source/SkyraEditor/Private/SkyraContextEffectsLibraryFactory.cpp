// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraContextEffectsLibraryFactory.h"

#include "Feedback/ContextEffects/SkyraContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

USkyraContextEffectsLibraryFactory::USkyraContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = USkyraContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* USkyraContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	USkyraContextEffectsLibrary* SkyraContextEffectsLibrary = NewObject<USkyraContextEffectsLibrary>(InParent, Name, Flags);

	return SkyraContextEffectsLibrary;
}
