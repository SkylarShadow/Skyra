# Frontend and Settings UI

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



The Frontend and Settings UI systems in SkyraFramework provide a data-driven approach to managing the main menu flow, lobby backgrounds, and a hierarchical settings registry. This system leverages Unreal Engine's `CommonUI` to create a cross-platform compatible interface that handles input mapping, scalability settings, and user preferences.

## Frontend Flow and State Management

The entry point for the game's frontend is managed by the `SkyraFrontendStateComponent`. This component handles the transition between the initial loading states and the main menu.

### SkyraFrontendStateComponent
The `USkyraFrontendStateComponent` is responsible for managing the flow of the frontend experience. It coordinates with the `USkyraExperienceManagerComponent` to ensure the "Frontend" experience is fully loaded before displaying the UI.

*   **Flow Control**: It monitors the experience loading status via `USkyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded` [[Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:63-68]]().
*   **UI Integration**: Once the experience is ready, it utilizes the `USkyraUIManagerSubsystem` to push the main menu layout [[Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:70-75]]().

### Lobby and Backgrounds
The visual backdrop of the frontend is managed by the `ASkyraLobbyBackground`. This actor allows for level-based or actor-based backgrounds that can be swapped depending on the current frontend state or active event.

### Performance Actions
During the frontend phase, specific `GameFeatureActions` are used to optimize the engine for menu rendering.
*   **ApplyFrontendPerfSettingsAction**: This action, defined in `UGameFeatureAction_ApplyFrontendPerfSettings`, is used to toggle specific performance modes (like capped frame rates or reduced global illumination) when the player is in the menus to save power and reduce heat [[Source/SkyraGame/Private/GameFeatures/GameFeatureAction_ApplyFrontendPerfSettings.cpp:15-25]]().

### Frontend Flow Diagram
This diagram shows the relationship between the state component, the experience system, and the UI subsystem.

```mermaid
graph TD
    subgraph "Frontend_State_Flow"
        "USkyraFrontendStateComponent" -- "1. Monitors" --> "USkyraExperienceManagerComponent"
        "USkyraExperienceManagerComponent" -- "2. OnExperienceLoaded" --> "USkyraFrontendStateComponent"
        "USkyraFrontendStateComponent" -- "3. PushLayout" --> "USkyraUIManagerSubsystem"
        "USkyraUIManagerSubsystem" -- "4. Spawns" --> "SkyraHUDLayout"
    end

    subgraph "Code_Entities"
        "USkyraFrontendStateComponent"["USkyraFrontendStateComponent [Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp]"]
        "USkyraExperienceManagerComponent"["USkyraExperienceManagerComponent"]
        "USkyraUIManagerSubsystem"["USkyraUIManagerSubsystem"]
    end
```
**Sources:**
*   `USkyraFrontendStateComponent`: [Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:1-80]()
*   `UGameFeatureAction_ApplyFrontendPerfSettings`: [Source/SkyraGame/Private/GameFeatures/GameFeatureAction_ApplyFrontendPerfSettings.cpp:1-40]()

---

## Settings System Architecture

SkyraFramework uses a decentralized registry system to manage game settings. This allows different modules (Audio, Video, Input) to register their own settings without bloating a single global class.

### Settings Registry Hierarchy
The core of the system is the `USkyraGameSettingRegistry`. It acts as a container for `UGameSetting` objects, organized into categories.

*   **USkyraGameSettingRegistry**: The base class for the settings database. It provides methods to initialize settings and find them by name or tag [[Source/SkyraGame/Private/Settings/SkyraGameSettingRegistry.cpp:15-30]]().
*   **Per-Category Registries**: The framework implements specialized registration logic for different domains:
    *   **Video/Graphics**: Managed via scalability settings in `USkyraSettingsLocal`.
    *   **Audio**: Interfaces with `USkyraAudioSettings`.
    *   **Input**: Handles mouse sensitivity and gamepad remapping.

### Settings Data Storage
Settings are split into two categories based on how they are persisted:

| Class | Description |
| :--- | :--- |
| `USkyraSettingsLocal` | Stores machine-specific settings such as resolution, scalability levels, and device-specific input overrides [[Source/SkyraGame/Private/Settings/SkyraSettingsLocal.cpp:10-50]](). |
| `USkyraSettingsShared` | Stores user-specific preferences that should roam across devices, such as subtitles, colorblind modes, and gameplay toggles. |

### Settings UI Hierarchy
The UI for settings is built using a recursive widget structure:
1.  **SkyraSettingScreen**: The top-level activatable widget that contains the navigation bar and the setting details area.
2.  **SkyraSettingList**: A dynamic list that generates rows based on the `USkyraGameSettingRegistry`.
3.  **SkyraSettingDetailView**: Displays descriptions and specific widgets (sliders, combos) for the selected setting.

### Settings Registry Entity Mapping
This diagram maps the logical settings categories to the code entities responsible for registering them.

```mermaid
graph LR
    subgraph "Registry_Logic"
        "USkyraGameSettingRegistry" --> "InitializeVideoSettings()"
        "USkyraGameSettingRegistry" --> "InitializeAudioSettings()"
        "USkyraGameSettingRegistry" --> "InitializeGameplaySettings()"
        "USkyraGameSettingRegistry" --> "InitializeInputSettings()"
    end

    subgraph "Data_Sources"
        "InitializeVideoSettings()" -- "Reads/Writes" --> "USkyraSettingsLocal"
        "InitializeAudioSettings()" -- "Reads/Writes" --> "USkyraSettingsShared"
        "InitializeInputSettings()" -- "Reads/Writes" --> "SkyraInputUserSettings"
    end

    subgraph "Files"
        "USkyraGameSettingRegistry"["USkyraGameSettingRegistry [Source/SkyraGame/Private/Settings/SkyraGameSettingRegistry.cpp]"]
        "USkyraSettingsLocal"["USkyraSettingsLocal [Source/SkyraGame/Private/Settings/SkyraSettingsLocal.cpp]"]
    end
```
**Sources:**
*   `USkyraGameSettingRegistry`: [Source/SkyraGame/Private/Settings/SkyraGameSettingRegistry.cpp:1-100]()
*   `USkyraSettingsLocal`: [Source/SkyraGame/Private/Settings/SkyraSettingsLocal.cpp:1-200]()

---

## Developer and Platform Emulation

To facilitate testing various hardware configurations and frontend flows, the framework includes specialized developer settings.

### SkyraDeveloperSettings
The `USkyraDeveloperSettings` class allows developers to override the default experience when launching the game in PIE (Play In Editor).
*   **Experience Override**: If `ExperienceOverride` is set, a notification toast is displayed when PIE starts to alert the developer that they are not using the default game flow [[Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:50-58]]().

### SkyraPlatformEmulationSettings
The `USkyraPlatformEmulationSettings` system allows developers to "pretend" the game is running on a different platform (e.g., Mobile, Console) while inside the editor.

*   **Trait Overrides**: It can enable or suppress specific `CommonUI` platform traits, which triggers UI visibility changes (e.g., showing touch controls on PC) [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:95-100]]().
*   **Device Profile Emulation**: It can force the engine to use a specific `UDeviceProfile` to test scalability settings for target hardware [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-175]]().
*   **Editor Simulation**: Uses `UPlatformSettingsManager::SetEditorSimulatedPlatform` to change the engine's reported platform [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:110-111]]().

### Emulation Flow
| Function | Purpose |
| :--- | :--- |
| `ApplySettings()` | Called whenever a property changes in the editor to refresh UI visibility and platform state [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:93-103]](). |
| `PickReasonableBaseDeviceProfile()` | Automatically selects the most appropriate device profile for the emulated platform [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-192]](). |
| `OnPlayInEditorStarted()` | Displays notification toasts regarding active overrides (Traits, Platform, or Experience) [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:57-91]](). |

**Sources:**
*   `USkyraDeveloperSettings`: [Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:12-60]()
*   `USkyraPlatformEmulationSettings`: [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:16-193]()