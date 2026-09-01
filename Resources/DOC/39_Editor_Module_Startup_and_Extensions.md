# Editor Module Startup and Extensions

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



The `SkyraEditor` module provides the foundational tooling and editor-specific overrides for the SkyraFramework. This includes custom Slate styling, toolbar extensions for content validation and map navigation, specialized Gameplay Ability System (GAS) editor integration, and a custom Editor Engine to manage Play-In-Editor (PIE) lifecycles and workspace automation.

## Module Lifecycle and Initialization

The `FSkyraEditorModule` class handles the startup and shutdown of editor-specific systems. It ensures that the framework's custom styles are registered and that specialized delegates for the Gameplay Ability System are bound to the editor's UI.

### Startup and Shutdown Flow
1.  **Initialize Style**: The `FGameEditorStyle` is initialized to provide icons and brushes for the custom UI [Source/SkyraEditor/Private/SkyraEditor.cpp:210]().
2.  **Module Callbacks**: If not running as a standalone game, the module listens for module change events via `FModuleManager::Get().OnModulesChanged()` [Source/SkyraEditor/Private/SkyraEditor.cpp:214]().
3.  **GAS Delegate Binding**: The module calls `BindGameplayAbilitiesEditorDelegates()` to configure how GameplayCues are created and searched within the editor [Source/SkyraEditor/Private/SkyraEditor.cpp:216]().
4.  **Menu Registration**: Toolbar extensions are registered via `RegisterGameEditorMenus()` [Source/SkyraEditor/Private/SkyraEditor.cpp:149]().

### GameplayCue Editor Integration
The editor module binds several static functions to the `IGameplayAbilitiesEditorModule` to streamline the creation of GameplayCue notifies:

| Function | Purpose | Implementation Details |
| :--- | :--- | :--- |
| `GetGameplayCueDefaultClasses` | Defines allowed notify classes | Adds `UGameplayCueNotify_Burst`, `AGameplayCueNotify_BurstLatent`, and `AGameplayCueNotify_Looping` [Source/SkyraEditor/Private/SkyraEditor.cpp:33-39](). |
| `GetGameplayCueInterfaceClasses` | Identifies valid targets | Iterates through all `UClass` objects to find `AActor` subclasses implementing `UGameplayCueInterface` [Source/SkyraEditor/Private/SkyraEditor.cpp:42-53](). |
| `GetGameplayCuePath` | Sets default asset paths | Retrieves paths from `UAbilitySystemGlobals::GetGameplayCueNotifyPaths` and appends a `GCN_` prefix to the tag name [Source/SkyraEditor/Private/SkyraEditor.cpp:56-86](). |

**Sources:**
* [Source/SkyraEditor/Private/SkyraEditor.cpp:33-86]()
* [Source/SkyraEditor/Private/SkyraEditor.cpp:204-218]()

---

## Editor Styling (FGameEditorStyle)

`FGameEditorStyle` manages a `FSlateStyleSet` specifically for the Skyra editor tools. It uses SVG-based vector images to ensure high-fidelity icons across different display scales.

### Style Implementation
The style set is registered with the `FSlateStyleRegistry` during `Initialize()` [Source/SkyraEditor/Private/GameEditorStyle.cpp:17](). It defines specific brushes for the toolbar, such as the `GameEditor.CheckContent` icon which points to a local SVG asset [Source/SkyraEditor/Private/GameEditorStyle.cpp:56]().

### Resource Mapping Diagram
This diagram shows how natural language UI concepts map to the internal Slate styling code.

```mermaid
graph TD
    subgraph "Natural Language Space"
        "Check Content Icon"["Check Content Icon"]
        "Toolbar Style"["Toolbar Style"]
    end

    subgraph "Code Entity Space"
        "FGameEditorStyle"["FGameEditorStyle::Create()"]
        "StyleSet"["FSlateStyleSet"]
        "SVG_Brush"["GAME_IMAGE_BRUSH_SVG"]
        "StyleName"["GameEditor.CheckContent"]
    end

    "Check Content Icon" --> "StyleName"
    "Toolbar Style" --> "FGameEditorStyle"
    "FGameEditorStyle" --> "StyleSet"
    "StyleSet" --> "SVG_Brush"
    "SVG_Brush" --> "StyleName"
```

**Sources:**
* [Source/SkyraEditor/Private/GameEditorStyle.cpp:12-60]()
* [Source/SkyraEditor/Public/GameEditorStyle.h:10-29]()

---

## Toolbar Extensions

The editor extends the `LevelEditor.LevelEditorToolBar.PlayToolBar` to add Skyra-specific utilities [Source/SkyraEditor/Private/SkyraEditor.cpp:151]().

### Check Content
This button triggers `UEditorValidator::ValidateCheckedOutContent` [Source/SkyraEditor/Private/SkyraEditor.cpp:144-147](). It is only visible and executable when no PIE session is active (`HasNoPlayWorld`) [Source/SkyraEditor/Private/SkyraEditor.cpp:175-177]().

### Common Maps Dropdown
To speed up development, a "Common Maps" dropdown is provided. This menu is populated dynamically from `USkyraDeveloperSettings::CommonEditorMaps` [Source/SkyraEditor/Private/SkyraEditor.cpp:120]().

*   **Logic**: The dropdown is only shown if maps are defined in the developer settings [Source/SkyraEditor/Private/SkyraEditor.cpp:111-114]().
*   **Action**: Clicking a map entry uses `UAssetEditorSubsystem::OpenEditorForAsset` to switch levels [Source/SkyraEditor/Private/SkyraEditor.cpp:103-109]().

**Sources:**
* [Source/SkyraEditor/Private/SkyraEditor.cpp:103-142]()
* [Source/SkyraEditor/Private/SkyraEditor.cpp:149-199]()

---

## Editor Engine (USkyraEditorEngine)

`USkyraEditorEngine` extends `UUnrealEdEngine` to automate workspace configuration and manage network modes during PIE.

### PIE Lifecycle Management
The engine overrides `PreCreatePIEInstances` to handle specific net mode requirements defined in `ASkyraWorldSettings` [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:56-58]().

*   **Force Standalone**: If `ASkyraWorldSettings::ForceStandaloneNetMode` is true (common for Frontend/Menu maps), the engine automatically switches the `EditorPlaySettings` to `PIE_Standalone` and notifies the user [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:60-72]().
*   **Settings Initialization**: It triggers `OnPlayInEditorStarted` on both `USkyraDeveloperSettings` and `USkyraPlatformEmulationSettings` to ensure developer-specific overrides are applied before the simulation starts [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:76-77]().

### Workspace Automation
On the first tick of the editor engine, `FirstTickSetup()` is called [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:42](). This is used to force `UContentBrowserSettings` to display plugin folders, ensuring that SkyraFramework plugin content is always visible to developers by default [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:52]().

### System Interaction Diagram
This diagram illustrates the relationship between the custom Editor Engine and the framework's settings classes.

```mermaid
graph TD
    subgraph "Editor Engine Logic"
        "USkyraEditorEngine"["USkyraEditorEngine::PreCreatePIEInstances"]
        "FirstTick"["USkyraEditorEngine::FirstTickSetup"]
    end

    subgraph "Data & Settings"
        "WorldSettings"["ASkyraWorldSettings::ForceStandaloneNetMode"]
        "DevSettings"["USkyraDeveloperSettings::OnPlayInEditorStarted"]
        "PlatformSettings"["USkyraPlatformEmulationSettings::OnPlayInEditorStarted"]
        "CBSettings"["UContentBrowserSettings::SetDisplayPluginFolders"]
    end

    "USkyraEditorEngine" -- "Checks" --> "WorldSettings"
    "USkyraEditorEngine" -- "Notifies" --> "DevSettings"
    "USkyraEditorEngine" -- "Notifies" --> "PlatformSettings"
    "FirstTick" -- "Configures" --> "CBSettings"
```

**Sources:**
* [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:35-83]()
* [Source/SkyraEditor/Public/SkyraEditorEngine.h:14-34]()