# Ability Costs, Game Phases, and Global Ability System

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



This page documents the specialized extensions to the Gameplay Ability System (GAS) in SkyraFramework. It covers how abilities consume resources via custom cost classes, how the game flow is managed through a tag-driven phase system, and how the Global Ability System manages effects across all active participants.

## Ability Costs

SkyraFramework extends the standard GAS cost mechanism by providing data-driven cost classes that interact with the inventory and player state systems. These classes inherit from `USkyraAbilityCost` and implement `CheckCost` and `ApplyCost`.

### Inventory and Tag Stack Costs

The framework provides three primary cost implementations:

| Class | Resource Source | Description |
| :--- | :--- | :--- |
| `USkyraAbilityCost_InventoryItem` | `USkyraInventoryManagerComponent` | Checks if a player has a specific number of items defined by a `USkyraInventoryItemDefinition` [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_InventoryItem.cpp:14-26](). |
| `USkyraAbilityCost_ItemTagStack` | `USkyraInventoryItemInstance` | Used by abilities granted from equipment. It checks for Gameplay Tag stacks (e.g., Ammo) on the item instance itself [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:19-29](). |
| `USkyraAbilityCost_PlayerTagStack` | `ASkyraPlayerState` | Checks for Gameplay Tag stacks stored globally on the player's state [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_PlayerTagStack.cpp:16-28](). |

### Cost Flow and Failure Handling
When an ability fails a cost check in `USkyraAbilityCost_ItemTagStack`, it can optionally add a `FailureTag` (defaulting to `Ability.ActivateFail.Cost`) to the `OptionalRelevantTags` container. This allows the UI or other systems to respond specifically to "out of ammo" or "out of resource" states [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:11-35]().

**Sources:**
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_InventoryItem.cpp:9-52]()
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:11-59]()
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_PlayerTagStack.cpp:11-50]()

---

## Game Phase System

The Game Phase system uses Gameplay Abilities to represent high-level game states (e.g., Warmup, Playing, Sudden Death). This allows the game state to benefit from the replication and logic encapsulation inherent in GAS.

### SkyraGamePhaseAbility
A specialized ability class `USkyraGamePhaseAbility` represents a single phase.
- **Server Initiated**: Phases are always started by the server [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:21-22]().
- **Tag Association**: Each phase ability is associated with a `GamePhaseTag` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:54]().
- **Registration**: Upon activation, it registers itself with the `USkyraGamePhaseSubsystem` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:31]().

### SkyraGamePhaseSubsystem
This World Subsystem manages the lifecycle of active phases and handles transitions.

- **Phase Transitions**: When a new phase begins, the subsystem checks if the incoming tag matches existing active phases. If a new phase (e.g., `Game.GameOver`) is pushed, it cancels incompatible active phases (e.g., `Game.Playing`) [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:156-177]().
- **Sub-phases**: The system supports hierarchical phases. Multiple abilities can share a parent tag; starting `Game.Playing.SuddenDeath` does not necessarily end `Game.Playing` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:164-167]().
- **Observers**: Systems can register to be notified when a phase starts or ends via `WhenPhaseStartsOrIsActive` and `WhenPhaseEnds` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:99-120]().

#### Game Phase Code Entity Association
Title: Game Phase Code Entity Association
```mermaid
graph TD
    subgraph "Logic Space"
        "StartPhase()" --> "ActivateAbility()"
        "EndAbility()" --> "OnEndPhase()"
    end

    subgraph "Code Entities"
        "StartPhase()" -- "calls" --> Subsystem["USkyraGamePhaseSubsystem"]
        Subsystem -- "grants" --> PhaseAbility["USkyraGamePhaseAbility"]
        PhaseAbility -- "has" --> Tag["GamePhaseTag (FGameplayTag)"]
        Subsystem -- "manages" --> Map["ActivePhaseMap (TMap)"]
    end

    subgraph "State Management"
        Map -- "tracks" --> Entry["FSkyraGamePhaseEntry"]
        Entry -- "holds" --> Callback["PhaseEndedCallback"]
    end
```
**Sources:**
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:16-47]()
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:50-134]()
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:136-194]()

---

## Global Ability System

The `USkyraGlobalAbilitySystem` provides a mechanism to apply Gameplay Abilities and Gameplay Effects to all actors that possess a `USkyraAbilitySystemComponent`.

### Registration and Application
- **ASC Registration**: When a character or pawn initializes its ASC, it registers with this system via `RegisterASC`. The system immediately applies any currently active "global" abilities or effects to that new ASC [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:126-140]().
- **Mass Application**: Functions like `ApplyAbilityToAll` and `ApplyEffectToAll` iterate through all `RegisteredASCs` to grant the specified class [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:82-104]().
- **Cleanup**: When an ASC is unregistered (e.g., actor destruction) via `UnregisterASC`, the system removes all globally applied handles from that specific ASC [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:142-155]().

#### Global Ability Distribution Flow
Title: Global Ability Distribution Flow
```mermaid
graph LR
    "Manager"["USkyraGlobalAbilitySystem"] -- "ApplyAbilityToAll()" --> "List"["FGlobalAppliedAbilityList"]
    "List" -- "Iterates" --> "RegisteredASCs"["TArray<USkyraAbilitySystemComponent*>"]
    "RegisteredASCs" -- "GiveAbility()" --> "Handles"["TMap<ASC, FGameplayAbilitySpecHandle>"]
    
    "NewActor" -- "RegisterASC()" --> "Manager"
    "Manager" -- "BackfillsAbilities" --> "NewActor"
```
**Sources:**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:9-41]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:82-124]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:126-155]()

---

## Gameplay Cue Manager

The `USkyraGameplayCueManager` optimizes the loading and execution of Gameplay Cues (VFX/SFX).

- **Async Loading**: It supports different load modes, including `LoadUpfront` for editor stability and `PreloadAsCuesAreReferenced` for runtime performance [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:16-28]().
- **Tag-Driven Preloading**: As gameplay tags are loaded by the engine, the manager intercepts tags starting with `GameplayCue` and initiates async loads for the corresponding Notify assets [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:180-202]().
- **Memory Management**: It tracks "Always Loaded" cues versus those loaded on demand, providing debug commands like `Skyra.DumpGameplayCues` to inspect memory usage [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:32-35]().

**Sources:**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:59-93]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:126-178]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:180-202]()

---

## Gameplay Effect Context

`FSkyraGameplayEffectContext` is a custom struct that extends the base GAS context to carry Skyra-specific data through the execution pipeline.

- **Ability Source**: It can store a weak pointer to an object implementing `ISkyraAbilitySourceInterface`, allowing damage calculations to identify the origin (e.g., a specific weapon or gadget) [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:48-57]().
- **Physical Materials**: It provides a helper `GetPhysicalMaterial()` which extracts the physics material from the hit result stored in the context, facilitating surface-specific logic in execution calculations [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:59-66]().
- **Serialization**: Implements `NetSerialize` and integrates with the Iris replication system for efficient networking [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:29-46]().

**Sources:**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:18-27]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:48-66]()