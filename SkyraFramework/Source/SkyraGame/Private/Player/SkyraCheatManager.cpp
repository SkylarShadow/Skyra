// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraCheatManager.h"
#include "GameFramework/Pawn.h"
#include "SkyraPlayerController.h"
#include "SkyraDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/SkyraAssetManager.h"
#include "System/SkyraGameData.h"
#include "SkyraGameplayTags.h"
#include "AbilitySystem/SkyraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/SkyraHealthComponent.h"
#include "Character/SkyraPawnExtensionComponent.h"
#include "System/SkyraSystemStatics.h"
#include "Development/SkyraDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraCheatManager)

DEFINE_LOG_CATEGORY(LogSkyraCheat);

namespace SkyraCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("SkyraCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("SkyraCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


USkyraCheatManager::USkyraCheatManager()
{
	DebugCameraControllerClass = ASkyraDebugCameraController::StaticClass();
}

void USkyraCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FSkyraCheatToRun& CheatRow : GetDefault<USkyraDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (SkyraCheat::bStartInGodMode)
	{
		God();	
	}
}

void USkyraCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogSkyraCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void USkyraCheatManager::Cheat(const FString& Msg)
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		SkyraPC->ServerCheat(Msg.Left(128));
	}
}

void USkyraCheatManager::CheatAll(const FString& Msg)
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		SkyraPC->ServerCheatAll(Msg.Left(128));
	}
}

void USkyraCheatManager::PlayNextGame()
{
	USkyraSystemStatics::PlayNextGame(this);
}

void USkyraCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void USkyraCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == SkyraCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool USkyraCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void USkyraCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(SkyraCheat::NAME_Fixed);
	}
}

void USkyraCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool USkyraCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == SkyraCheat::NAME_Fixed);
	}

	return false;
}

void USkyraCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void USkyraCheatManager::CycleDebugCameras()
{
	if (!SkyraCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void USkyraCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void USkyraCheatManager::CancelActivatedAbilities()
{
	if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		SkyraASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void USkyraCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = SkyraGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
		{
			SkyraASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogSkyraCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void USkyraCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = SkyraGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
		{
			SkyraASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogSkyraCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void USkyraCheatManager::DamageSelf(float DamageAmount)
{
	if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(SkyraASC, DamageAmount);
	}
}

void USkyraCheatManager::DamageTarget(float DamageAmount)
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		if (SkyraPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			SkyraPC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(SkyraPC, TargetHitResult);

		if (USkyraAbilitySystemComponent* SkyraTargetASC = Cast<USkyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(SkyraTargetASC, DamageAmount);
		}
	}
}

void USkyraCheatManager::ApplySetByCallerDamage(USkyraAbilitySystemComponent* SkyraASC, float DamageAmount)
{
	check(SkyraASC);

	TSubclassOf<UGameplayEffect> DamageGE = USkyraAssetManager::GetSubclass(USkyraGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = SkyraASC->MakeOutgoingSpec(DamageGE, 1.0f, SkyraASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SkyraGameplayTags::SetByCaller_Damage, DamageAmount);
		SkyraASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void USkyraCheatManager::HealSelf(float HealAmount)
{
	if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(SkyraASC, HealAmount);
	}
}

void USkyraCheatManager::HealTarget(float HealAmount)
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(SkyraPC, TargetHitResult);

		if (USkyraAbilitySystemComponent* SkyraTargetASC = Cast<USkyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(SkyraTargetASC, HealAmount);
		}
	}
}

void USkyraCheatManager::ApplySetByCallerHeal(USkyraAbilitySystemComponent* SkyraASC, float HealAmount)
{
	check(SkyraASC);

	TSubclassOf<UGameplayEffect> HealGE = USkyraAssetManager::GetSubclass(USkyraGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = SkyraASC->MakeOutgoingSpec(HealGE, 1.0f, SkyraASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SkyraGameplayTags::SetByCaller_Heal, HealAmount);
		SkyraASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

USkyraAbilitySystemComponent* USkyraCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		return SkyraPC->GetSkyraAbilitySystemComponent();
	}
	return nullptr;
}

void USkyraCheatManager::DamageSelfDestruct()
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
 		if (const USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(SkyraPC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(SkyraGameplayTags::InitState_GameplayReady))
			{
				if (USkyraHealthComponent* HealthComponent = USkyraHealthComponent::FindHealthComponent(SkyraPC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void USkyraCheatManager::God()
{
	if (ASkyraPlayerController* SkyraPC = Cast<ASkyraPlayerController>(GetOuterAPlayerController()))
	{
		if (SkyraPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			SkyraPC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (USkyraAbilitySystemComponent* SkyraASC = SkyraPC->GetSkyraAbilitySystemComponent())
		{
			const FGameplayTag Tag = SkyraGameplayTags::Cheat_GodMode;
			const bool bHasTag = SkyraASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				SkyraASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				SkyraASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void USkyraCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (USkyraAbilitySystemComponent* SkyraASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = SkyraGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = SkyraASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				SkyraASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				SkyraASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

