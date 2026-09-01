# Player Controller, State, and Spawning

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



This page details the core player infrastructure within SkyraFramework. It covers the specialized player controller for both humans and bots, the persistent player state, and the modular systems responsible for spawning and managing player lifecycle within an experience.

## ASkyraPlayerController

`ASkyraPlayerController` serves as the primary interface between the human player and the game world. It extends the base `APlayerController` to integrate with Skyra's team system, camera management, and gameplay systems.

### Key Responsibilities
*   **Team Integration**: Implements `ISkyraTeamAgentInterface` to allow the player to belong to a specific team and handle team-based queries.
*   **Camera Assistance**: Integrates with the camera system to provide smooth transitions and assist logic via `ASkyraPlayerCameraManager`.
*   **Input Routing**: Forwards input to the `USkyraHeroComponent` attached to the controlled pawn.
*   **Cheat Management**: Provides access to `USkyraCheatManager` for development and debugging commands.

### Replay and Recording
The controller includes hooks for the `USkyraReplaySubsystem`, enabling the recording and playback of gameplay sessions for review or debugging.

## ASkyraPlayerState

`ASkyraPlayerState` manages persistent data for a player that remains across pawn deaths and respawns. It is the primary owner of the `USkyraAbilitySystemComponent` (ASC) for player-controlled entities.

### Data Flow: Player State and ASC
The Player State initializes the ASC and attribute sets (such as `USkyraHealthSet` and `USkyraCombatSet`). When a pawn is possessed, the ASC is updated to point to the new pawn as its physical avatar, while the Player State remains the owner.

| Component/Class | Responsibility |
| :--- | :--- |
| `USkyraAbilitySystemComponent` | Handles gameplay tags, abilities, and effects. |
| `USkyraHealthSet` | Tracks Health, Max Health, and Shield attributes. |
| `USkyraCombatSet` | Tracks Damage and Healing attributes. |

## Player Spawning and Bot Management

Skyra utilizes a modular approach to spawning through the `USkyraPlayerSpawningManagerComponent` (found on the Game State) and specialized `ASkyraPlayerStart` actors.

### Bot Spawning Logic
The `USkyraBotCreationComponent` automates the creation of AI-controlled players during experience loading.

#### Implementation Flow: Bot Creation
1.  **Experience Load**: The component listens for `OnExperienceLoaded` via the `USkyraExperienceManagerComponent`.
2.  **Count Determination**: It calculates the number of bots based on `NumBotsToCreate`, developer settings overrides, or URL options (`?NumBots=X`).
3.  **Controller Spawning**: Spawns an `AAIController` (or `ASkyraPlayerBotController`).
4.  **Initialization**: Calls `GenericPlayerInitialization` on the Game Mode to set up team and player state.
5.  **Pawn Restart**: Triggers `RestartPlayer` to spawn the actual pawn.

### System Architecture: Spawning and Bots
The following diagram illustrates the relationship between the Game Mode, the Bot Creation Component, and the resulting AI entities.

Title: Spawning and Bot Architecture
```mermaid
graph TD
    subgraph "GameMode_Space" ["GameMode Space"]
        "ASkyraGameMode" -- "Owns" --> "USkyraBotCreationComponent"
        "ASkyraGameMode" -- "Spawns" --> "ASkyraPlayerBotController"
    end

    subgraph "Logic_Flow" ["Logic Flow"]
        "USkyraExperienceManagerComponent" -- "OnExperienceLoaded" --> "USkyraBotCreationComponent"
        "USkyraBotCreationComponent" -- "SpawnOneBot()" --> "ASkyraPlayerBotController"
        "ASkyraPlayerBotController" -- "Possesses" --> "APawn"
    end

    subgraph "Entity_Initialization" ["Entity Initialization"]
        "ASkyraGameMode" -- "GenericPlayerInitialization()" --> "ASkyraPlayerBotController"
        "ASkyraPlayerBotController" -- "Finds" --> "USkyraPawnExtensionComponent"
    end
```
Sources: [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:21-40](), [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:98-129]()

## Development and Debugging

### SkyraCheatManager and Bot Cheats
The `USkyraBotCheats` class extends the cheat manager to provide runtime commands for bot management. These are registered automatically when the cheat manager is created.

*   **AddPlayerBot**: Triggers `Cheat_AddBot` on the creation component.
*   **RemovePlayerBot**: Triggers `Cheat_RemoveBot` to remove a random bot from the world.

### Debug Camera
`ASkyraDebugCameraController` provides enhanced debugging views, allowing developers to detach from the player pawn and inspect the world while maintaining access to Skyra-specific debug information.

### Code Entity Mapping: Debug and Cheats
This diagram bridges the cheat commands to the internal component logic.

Title: Cheat Manager to Bot Logic Mapping
```mermaid
graph LR
    subgraph "Console_Commands" ["Console Commands"]
        "AddPlayerBot" --> "USkyraBotCheats::AddPlayerBot()"
        "RemovePlayerBot" --> "USkyraBotCheats::RemovePlayerBot()"
    end

    subgraph "Code_Execution" ["Code Execution"]
        "USkyraBotCheats::AddPlayerBot()" -- "GetBotComponent()" --> "USkyraBotCreationComponent"
        "USkyraBotCreationComponent" -- "SpawnOneBot()" --> "ASkyraPlayerBotController"
        "USkyraBotCreationComponent" -- "RemoveOneBot()" --> "AAIController::Destroy()"
    end
```
Sources: [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:27-58](), [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:131-163]()

## Summary of Spawning Classes

| Class | Role |
| :--- | :--- |
| `ASkyraPlayerStart` | Extends `APlayerStart` with team-specific tags and requirements. |
| `SkyraPlayerSpawningManagerComponent` | Handles the logic of finding the best `ASkyraPlayerStart` for a player. |
| `USkyraBotCreationComponent` | Server-side component that manages the population of AI bots. |
| `ASkyraPlayerBotController` | The AI controller used for bots, designed to mimic human player state setup. |

Sources:
* [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:1-183]()
* [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:1-60]()
* [.gitattributes:1-4]()