// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkyraPawnControlComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "Input/SkyraMappableConfigPair.h"
#include "SkyraLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "Player/SkyraPlayerController.h"
#include "Player/SkyraPlayerState.h"
#include "Player/SkyraLocalPlayer.h"
#include "Character/SkyraPawnExtensionComponent.h"
#include "Character/SkyraPawnData.h"
#include "AbilitySystem/SkyraAbilitySystemComponent.h"
#include "Input/SkyraInputConfig.h"
#include "Input/SkyraInputComponent.h"
#include "Camera/SkyraCameraComponent.h"
#include "SkyraGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/SkyraCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SkyraPawnControlComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR



const FName USkyraPawnControlComponent::NAME_BindInputsNow("BindInputsNow");
const FName USkyraPawnControlComponent::NAME_ActorFeatureName("PawnControl");

USkyraPawnControlComponent::USkyraPawnControlComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void USkyraPawnControlComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogSkyra, Error, TEXT("[USkyraPawnControlComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("SkyraPawnControlComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName PawnControlMessageLogName = TEXT("SkyraPawnControlComponent");
			
			FMessageLog(PawnControlMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(PawnControlMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool USkyraPawnControlComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == SkyraGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == SkyraGameplayTags::InitState_Spawned && DesiredState == SkyraGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ASkyraPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ASkyraPlayerController* SkyraPC = GetController<ASkyraPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !SkyraPC || !SkyraPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == SkyraGameplayTags::InitState_DataAvailable && DesiredState == SkyraGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ASkyraPlayerState* SkyraPS = GetPlayerState<ASkyraPlayerState>();

		return SkyraPS && Manager->HasFeatureReachedInitState(Pawn, USkyraPawnExtensionComponent::NAME_ActorFeatureName, SkyraGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == SkyraGameplayTags::InitState_DataInitialized && DesiredState == SkyraGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void USkyraPawnControlComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == SkyraGameplayTags::InitState_DataAvailable && DesiredState == SkyraGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ASkyraPlayerState* SkyraPS = GetPlayerState<ASkyraPlayerState>();
		if (!ensure(Pawn && SkyraPS))
		{
			return;
		}

		const USkyraPawnData* PawnData = nullptr;

		if (USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<USkyraPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(SkyraPS->GetSkyraAbilitySystemComponent(), SkyraPS);
		}

		if (ASkyraPlayerController* SkyraPC = GetController<ASkyraPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		if (PawnData)
		{
			if (USkyraCameraComponent* CameraComponent = USkyraCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void USkyraPawnControlComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == USkyraPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == SkyraGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void USkyraPawnControlComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { SkyraGameplayTags::InitState_Spawned, SkyraGameplayTags::InitState_DataAvailable, SkyraGameplayTags::InitState_DataInitialized, SkyraGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void USkyraPawnControlComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(USkyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(SkyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void USkyraPawnControlComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void USkyraPawnControlComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const USkyraLocalPlayer* LP = Cast<USkyraLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const USkyraPawnData* PawnData = PawnExtComp->GetPawnData<USkyraPawnData>())
		{
			if (const USkyraInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Skyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the USkyraInputComponent or modify this component accordingly.
				USkyraInputComponent* SkyraIC = Cast<USkyraInputComponent>(PlayerInputComponent);
				if (ensureMsgf(SkyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to USkyraInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					SkyraIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					SkyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					// 这里可用于添加基础input例如移动之类
					
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void USkyraPawnControlComponent::AddAdditionalInputConfig(const USkyraInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		USkyraInputComponent* SkyraIC = Pawn->FindComponentByClass<USkyraInputComponent>();
		if (ensureMsgf(SkyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to USkyraInputComponent or a subclass of it.")))
		{
			SkyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void USkyraPawnControlComponent::RemoveAdditionalInputConfig(const USkyraInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool USkyraPawnControlComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void USkyraPawnControlComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (USkyraAbilitySystemComponent* SkyraASC = PawnExtComp->GetSkyraAbilitySystemComponent())
			{
				SkyraASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void USkyraPawnControlComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (USkyraAbilitySystemComponent* SkyraASC = PawnExtComp->GetSkyraAbilitySystemComponent())
		{
			SkyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}


TSubclassOf<USkyraCameraMode> USkyraPawnControlComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (USkyraPawnExtensionComponent* PawnExtComp = USkyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const USkyraPawnData* PawnData = PawnExtComp->GetPawnData<USkyraPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void USkyraPawnControlComponent::SetAbilityCameraMode(TSubclassOf<USkyraCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void USkyraPawnControlComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

