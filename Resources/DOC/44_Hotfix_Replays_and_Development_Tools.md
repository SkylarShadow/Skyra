# Hotfix, Replays, and Development Tools

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



This page documents the infrastructure for live updates (Hotfixing), gameplay recording (Replays), and the suite of developer tools used to emulate platforms and automate testing within the SkyraFramework.

## Hotfix and Runtime Configuration

SkyraFramework utilizes a multi-layered approach to runtime configuration and hotfixing, allowing developers to patch text, adjust gameplay constants, and override engine-level settings without requiring a full client update.

### SkyraHotfixManager
The `USkyraHotfixManager` is the central authority for applying patches at runtime. It extends the base engine hotfix functionality to provide framework-specific hooks for data validation and application.

### SkyraRuntimeOptions
`USkyraRuntimeOptions` serves as a data-driven container for global gameplay constants that can be adjusted via hotfix. It provides a centralized location for "tunables" that designers might need to change frequently.

### SkyraTextHotfixConfig
The `USkyraTextHotfixConfig` class handles the redirection of localized text. This allows for fixing typos or updating UI strings dynamically by mapping existing text keys to new values provided in a hotfix data asset.

| Class | Responsibility |
| :--- | :--- |
| `USkyraHotfixManager` | Orchestrates the loading and application of hotfix patches. |
| `USkyraRuntimeOptions` | Stores runtime-adjustable gameplay parameters and flags. |
| `USkyraTextHotfixConfig` | Manages dynamic overrides for localized text strings. |

**Sources:**
- [Source/SkyraGame/Private/System/SkyraHotfixManager.cpp:1-20]()
- [Source/SkyraGame/Private/System/SkyraRuntimeOptions.cpp:1-20]()

---

## Replay Subsystem

SkyraFramework integrates with the Unreal Replay system through a specialized subsystem and asynchronous actions to manage the recording and querying of gameplay sessions.

### SkyraReplaySubsystem
The `USkyraReplaySubsystem` manages the lifecycle of replay recordings. It handles the initialization of the streamer and provides an interface for the game to start and stop recording based on game state or developer commands.

### AsyncAction_QueryReplays
To prevent blocking the main thread when searching for available replays (especially when using network-based streamers), the framework provides `UAsyncAction_QueryReplays`. This action wraps the internal replay metadata queries into a Blueprint-friendly latent node.

**Replay Query Flow:**
1. **Call:** Blueprint calls `QueryReplays`.
2. **Execution:** The action interfaces with the `USkyraReplaySubsystem` to fetch metadata.
3. **Completion:** On success or failure, the appropriate execution pin is fired with the list of replay information.

**Sources:**
- [Source/SkyraGame/Private/Replays/SkyraReplaySubsystem.cpp:1-30]()
- [Source/SkyraGame/Private/Replays/AsyncAction_QueryReplays.cpp:1-30]()

---

## Development and Emulation Tools

SkyraFramework includes robust tools for simulating different hardware environments and streamlining the development process within the editor.

### SkyraDeveloperSettings
`USkyraDeveloperSettings` provides a persistent configuration interface in the Project Settings for developer-specific overrides. A key feature is the **Experience Override**, which allows developers to force the game to load a specific `USkyraExperienceDefinition` regardless of the default map settings.

*   **Experience Override Notification:** When PIE (Play In Editor) starts, if an override is active, a toast notification is displayed to the developer [Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:47-59]().

### SkyraPlatformEmulationSettings
The `USkyraPlatformEmulationSettings` class allows developers to "pretend" the editor is running on a different platform (e.g., mobile or console). This is critical for testing UI visibility and performance scaling.

*   **Platform Traits:** Developers can manually enable or suppress specific Gameplay Tags representing platform traits [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:60-79]().
*   **Device Profile Simulation:** The system can emulate specific device profiles to test scalability settings [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:127-159]().
*   **Visibility Integration:** It updates the `UCommonUIVisibilitySubsystem` to reflect the emulated traits [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:95-95]().

### Platform Emulation Data Flow
The following diagram illustrates how emulation settings propagate from the developer's configuration to the UI and Engine systems.

**Diagram: Platform Emulation Flow**
```mermaid
graph TD
    subgraph "Natural Language Space"
        "Dev Wants to Test Mobile UI" --> "Change Pretend Platform"
        "Change Pretend Platform" --> "Verify UI Hides/Shows"
    end

    subgraph "Code Entity Space"
        "USkyraPlatformEmulationSettings" -- "ApplySettings()" --> "UCommonUIVisibilitySubsystem"
        "USkyraPlatformEmulationSettings" -- "ChangeActivePretendPlatform()" --> "UPlatformSettingsManager"
        "UPlatformSettingsManager" -- "SetEditorSimulatedPlatform()" --> "UnrealEngineCore"
        "USkyraPlatformEmulationSettings" -- "PickReasonableBaseDeviceProfile()" --> "UDeviceProfileManager"
    end

    "USkyraPlatformEmulationSettings" -- "PostEditChangeProperty()" --> "ApplySettings()"
```
**Sources:**
- [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:93-112]()
- [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-193]()

---

## Testing and Automation

### SkyraTestControllerBootTest
`USkyraTestControllerBootTest` is a specialized controller used for automated "boot tests." It verifies that the game can successfully initialize, load an experience, and reach a playable state without crashing or encountering critical errors.

### SkyraBotCheats
`USkyraBotCheats` provides a cheat manager extension to facilitate testing with AI entities. It interfaces with the `USkyraBotCreationComponent` to dynamically add or remove bots during a session.

*   **Registration:** It automatically registers itself with the `UCheatManager` upon creation [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:13-25]().
*   **Bot Management:** It provides `AddPlayerBot` and `RemovePlayerBot` commands which locate the `USkyraBotCreationComponent` on the `AGameStateBase` [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:27-58]().

**Diagram: Cheat Manager Extension Logic**
```mermaid
graph LR
    subgraph "Natural Language Space"
        "Developer enters 'AddPlayerBot' in Console" --> "Cheat System finds Bot Component"
        "Cheat System finds Bot Component" --> "Bot is Spawned"
    end

    subgraph "Code Entity Space"
        "USkyraBotCheats" -- "GetBotComponent()" --> "AGameStateBase"
        "AGameStateBase" -- "FindComponentByClass()" --> "USkyraBotCreationComponent"
        "USkyraBotCheats" -- "Cheat_AddBot()" --> "USkyraBotCreationComponent"
    end
```

**Sources:**
- [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:13-58]()

---

## Physics and Materials

### PhysicalMaterialWithTags
SkyraFramework extends the standard physical material system with `UPhysicalMaterialWithTags`. This allows developers to associate Gameplay Tags directly with physical materials.

These tags are primarily used by the **Context Effects System** (see Section 10.1) to determine which visual or audio effects to play when a collision or footstep occurs on a specific surface (e.g., `Surface.Type.Grass` or `Surface.Type.Metal`).

**Sources:**
- [Source/SkyraGame/Private/Physics/PhysicalMaterialWithTags.cpp:1-15]()