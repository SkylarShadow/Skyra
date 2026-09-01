# Core System Utilities

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)
- [.gitignore](.gitignore)

</details>



The Core System Utilities provide the foundational infrastructure for the SkyraFramework runtime. These systems handle asset management, global data access, logging, gameplay tag organization, and engine-level overrides required for a modular, data-driven architecture.

## Asset Management and Startup Pipeline

The `USkyraAssetManager` is the central authority for loading game assets and executing initialization logic. It extends the standard `UAssetManager` to support asynchronous "Startup Jobs" and provides a global access point for singleton-like data assets.

### Startup Jobs
The manager utilizes `FSkyraAssetManagerStartupJob` to sequence initialization tasks during the engine startup phase. This ensures that critical systems (like Gameplay Tags or global data) are ready before the game simulation begins.

| Function | Purpose |
| :--- | :--- |
| `StartInitialLoading()` | Begins the asynchronous execution of registered startup jobs. [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:142-167]() |
| `DoAllStartupJobs()` | Iterates through the job queue, executing each `TFunction` and tracking progress. [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:169-216]() |
| `InitializeGameplayCueManager()` | Configures the `USkyraGameplayCueManager` to ensure GAS visual effects are correctly routed. [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:455-465]() |

### Asset Access
The manager provides helper templates to load and retrieve specific asset types, such as `USkyraPawnData` or `USkyraExperienceDefinition`.

**Data Flow: Asset Manager Initialization**
```mermaid
graph TD
    subgraph "Engine Startup"
        ["FEngineLoop::Init"] --> ["USkyraAssetManager::StartInitialLoading"]
    end

    subgraph "Job Execution"
        ["USkyraAssetManager::StartInitialLoading"] --> Job1["Initialize Ability System"]
        ["USkyraAssetManager::StartInitialLoading"] --> Job2["Initialize Gameplay Tags"]
        ["USkyraAssetManager::StartInitialLoading"] --> Job3["Load Global Game Data"]
        Job1 & Job2 & Job3 --> Finalize["Finish Startup Jobs"]
    end

    subgraph "Global Access"
        Finalize --> Get["USkyraAssetManager::Get()"]
    end
```
**Sources:** [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:29-65](), [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:142-216]()

---

## Global Game Data (`USkyraGameData`)

`USkyraGameData` is a `UPrimaryDataAsset` that acts as a global repository for Gameplay Effects and other framework-wide constants. This allows designers to swap core logic (like how damage or healing is applied) without modifying C++ code.

### Key References
The class holds hard references to specific `UGameplayEffect` classes used by the framework's internal calculations:
*   **Damage/Heal Effects:** Used by `SkyraDamageExecution` and `SkyraHealExecution`. [Source/SkyraGame/Classes/System/SkyraGameData.h:23-31]()
*   **Access:** Retrieved via `USkyraAssetManager::GetGameData()`. [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:115-123]()

**Sources:** [Source/SkyraGame/Classes/System/SkyraGameData.h:16-33](), [Source/SkyraGame/Private/System/SkyraAssetManager.cpp:115-123]()

---

## Engine and Instance Overrides

SkyraFramework overrides the core Engine and GameInstance classes to provide hooks for global subsystems and better PIE (Play In Editor) support.

### USkyraGameEngine
The `USkyraGameEngine` overrides `Init()` to initialize the `USkyraAssetManager` early in the lifecycle.
*   **File:** `Source/SkyraGame/Private/System/SkyraGameEngine.cpp`
*   **Function:** `Init(IEngineLoop* InEngineLoop)` calls `USkyraAssetManager::Get().StartInitialLoading()`. [Source/SkyraGame/Private/System/SkyraGameEngine.cpp:17-24]()

### USkyraGameInstance
The `USkyraGameInstance` serves as the high-level manager for the game session. It integrates with the `CommonUser` plugin to handle local player initialization.
*   **Initialization:** Overrides `Init()` to call `Super::Init()` and then `OnStart()`. [Source/SkyraGame/Private/System/SkyraGameInstance.cpp:24-30]()
*   **Player Joining:** Uses `GetOnPreClientTravelEvent()` to manage cleanup before moving to new levels. [Source/SkyraGame/Private/System/SkyraGameInstance.cpp:44-48]()

**Sources:** [Source/SkyraGame/Private/System/SkyraGameEngine.cpp:11-25](), [Source/SkyraGame/Private/System/SkyraGameInstance.cpp:18-54]()

---

## Gameplay Tags and Logging

### SkyraGameplayTags
Instead of relying on string lookups, SkyraFramework uses a centralized `FSkyraGameplayTags` singleton to store native tags. This provides compile-time safety and IDE auto-completion.

*   **Initialization:** Tags are registered in `AddAllTags` via `UGameplayTagsManager`. [Source/SkyraGame/Private/SkyraGameplayTags.cpp:115-225]()
*   **Categories:** Includes tags for Ability Input (e.g., `InputTag.Ability.Main`), Status (e.g., `Status.Death`), and Character States. [Source/SkyraGame/Classes/SkyraGameplayTags.h:17-100]()

### SkyraLogChannels
Custom log categories are defined to provide granular control over debug output.
*   **LogSkyra:** General framework logs.
*   **LogSkyraExperience:** Logs related to experience loading and transitions.
*   **LogSkyraAbilitySystem:** GAS-specific debugging.
*   **Source:** [Source/SkyraGame/Private/SkyraLogChannels.cpp:7-14]()

**Sources:** [Source/SkyraGame/Classes/SkyraGameplayTags.h:17-110](), [Source/SkyraGame/Private/SkyraGameplayTags.cpp:115-225](), [Source/SkyraGame/Private/SkyraLogChannels.cpp:7-14]()

---

## Gameplay Tag Stacks

The `FGameplayTagStack` and `FGameplayTagStackContainer` provide a lightweight way to track counts of gameplay tags on an object. This is used for inventory quantities, ammo counts, or any numeric value associated with a tag that does not require the full overhead of a GAS Attribute.

### Structure
*   **FGameplayTagStack:** A `FFastArraySerializerItem` containing a `FGameplayTag` and an `int32` count. [Source/SkyraGame/Classes/System/GameplayTagStack.h:18-35]()
*   **FGameplayTagStackContainer:** A `FFastArraySerializer` that manages a list of stacks, supporting replication and efficient delta updates. [Source/SkyraGame/Classes/System/GameplayTagStack.h:41-75]()

**Sources:** [Source/SkyraGame/Classes/System/GameplayTagStack.h:18-75]()

---

## SkyraSignificanceManager

The `USkyraSignificanceManager` provides a way to optimize performance by determining the "significance" of actors based on their distance from the player and whether they are on screen. This is used to throttle tick rates, disable animations, or simplify AI logic for distant entities.

*   **Implementation:** Extends `USignificanceManager`.
*   **Integration:** Often used in conjunction with `SkyraHealthComponent` or `SkyraPawnExtensionComponent` to reduce update frequency for background characters.

**Entity Association Map**
```mermaid
classDiagram
    class USkyraAssetManager {
        +Get()$
        +GetGameData()
        +StartInitialLoading()
    }
    class USkyraGameData {
        +DamageGameplayEffect
        +HealGameplayEffect
    }
    class FSkyraGameplayTags {
        +Get()$
        +Ability_Input_Main
        +Status_Death
    }
    class USkyraGameInstance {
        +OnStart()
        +HandlePrivilegeChanged()
    }
    
    ["USkyraAssetManager"] --> ["USkyraGameData"] : "Loads/Holds"
    ["USkyraAssetManager"] --> ["FSkyraGameplayTags"] : "Initializes"
    ["USkyraGameEngine"] --> ["USkyraAssetManager"] : "Triggers Startup"
    ["USkyraGameInstance"] --> ["USkyraAssetManager"] : "Accesses"
```
**Sources:** [Source/SkyraGame/Classes/System/SkyraAssetManager.h:25-60](), [Source/SkyraGame/Classes/System/SkyraGameData.h:16-33](), [Source/SkyraGame/Classes/SkyraGameplayTags.h:17-110]()

---

## Tagged Actor Utility

The `ASkyraTaggedActor` is a base actor class that implements the `IGameplayTagAssetInterface`. It allows designers to apply static tags to an actor in the level which can be queried by gameplay systems (e.g., for interaction filters or team identification).

*   **GetOwnedGameplayTags:** Returns the `StaticGameplayTags` container. [Source/SkyraGame/Private/AbilitySystem/SkyraTaggedActor.cpp:13-16]()
*   **Editor Safety:** Overrides `CanEditChange` to prevent manual editing of the native `AActor::Tags` property, forcing use of the Gameplay Tag system. [Source/SkyraGame/Private/AbilitySystem/SkyraTaggedActor.cpp:19-29]()

**Sources:** [Source/SkyraGame/Classes/AbilitySystem/SkyraTaggedActor.h:15-30](), [Source/SkyraGame/Private/AbilitySystem/SkyraTaggedActor.cpp:8-30]()