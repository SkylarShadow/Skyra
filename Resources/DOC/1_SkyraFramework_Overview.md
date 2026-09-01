# SkyraFramework Overview

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)
- [.gitignore](.gitignore)

</details>



SkyraFramework is a comprehensive, modular gameplay framework for Unreal Engine designed to support scalable, data-driven game development. It heavily leverages the **Gameplay Ability System (GAS)**, **Modular Gameplay**, and **Game Features** plugins to create a decoupled architecture where features can be added or removed without modifying core engine classes.

The framework is delivered as a single plugin containing two primary modules: `SkyraGame` (the runtime heart of the framework) and `SkyraEditor` (tooling and validation).

### Module Architecture

The framework is split into two distinct modules defined in the plugin descriptor [SkyraFramework.uplugin:17-28]().

*   **SkyraGame**: The core runtime module. It contains the implementation of the Ability System, Pawn architecture, Experience system, and UI framework. It loads during the `Default` phase [SkyraFramework.uplugin:21]().
*   **SkyraEditor**: An editor-only module providing custom UX for developers, asset validators, and toolbar extensions. It loads during the `PreLoadingScreen` phase [SkyraFramework.uplugin:26]().

For a detailed breakdown of the module configurations and loading phases, see **[Plugin Architecture and Module Structure](#1.1)**.

### Plugin Dependency Graph

SkyraFramework acts as an orchestrator for several advanced Unreal Engine plugins. It establishes a robust dependency graph that includes industry-standard systems for abilities, UI, and networking.

**Framework Dependency Mapping**

```mermaid
graph TD
    subgraph "SkyraFramework Plugin"
        [SkyraGame] --> [SkyraEditor]
    end

    subgraph "External Dependencies"
        [SkyraGame] --> ["GameplayAbilities"]
        [SkyraGame] --> ["ModularGameplay"]
        [SkyraGame] --> ["CommonUI"]
        [SkyraGame] --> ["GameFeatures"]
        [SkyraGame] --> ["EnhancedInput"]
        [SkyraGame] --> ["GameplayMessageRouter"]
    end

    ["ModularGameplay"] --> ["ModularGameplayActors"]
    ["CommonUI"] --> ["CommonGame"]
    ["CommonGame"] --> ["CommonUser"]
```
Sources: [SkyraFramework.uplugin:29-138]()

### Core System Utilities

At the foundation of `SkyraGame` are several "Global" systems that manage the lifecycle of the application and its data. These utilities ensure that assets are loaded correctly before gameplay begins and provide centralized access to global gameplay tags and data.

*   **Asset Management**: `USkyraAssetManager` handles the discovery and synchronous/asynchronous loading of primary assets, including the execution of startup jobs.
*   **Global Data**: `USkyraGameData` acts as a singleton for global Gameplay Effect references, while `SkyraGameplayTags` provides a centralized C++ interface for the framework's tag dictionary.
*   **Engine & Instance**: Custom overrides like `USkyraGameEngine` and `USkyraGameInstance` provide hooks for initialization and global error handling.

For more information on these foundational classes, see **[Core System Utilities](#1.2)**.

### System Integration Overview

The following diagram illustrates how the core code entities within SkyraFramework interact to initialize a gameplay session.

**Code Entity Interaction: Session Startup**

```mermaid
sequenceDiagram
    participant GE as "USkyraGameEngine"
    participant AM as "USkyraAssetManager"
    participant GI as "USkyraGameInstance"
    participant SM as "USkyraSignificanceManager"

    GE->>AM: "Initialize()"
    Note over AM: "Load SkyraGameplayTags"
    AM->>AM: "StartInitialLoading()"
    GE->>GI: "Init()"
    GI->>SM: "Register()"
    Note over GI: "Wait for Experience System"
```
Sources: [SkyraFramework.uplugin:55-57]()

### High-Level Components

Beyond the utilities, the framework is organized into several major functional areas:

| System | Primary Code Entities | Role |
| :--- | :--- | :--- |
| **Experiences** | `ASkyraGameMode`, `USkyraExperienceDefinition` | Defines *what* is being played (rules, maps, actions). |
| **GAS** | `USkyraAbilitySystemComponent`, `USkyraGameplayAbility` | Handles all logic for actions, attributes, and status effects. |
| **Pawn/Hero** | `ASkyraPawn`, `USkyraPawnExtensionComponent` | Modular actor logic that binds GAS, Input, and Visuals together. |
| **Inventory** | `USkyraInventoryManagerComponent`, `USkyraEquipmentInstance` | Manages items, weapons, and their associated gameplay logic. |
| **UI** | `ASkyraHUD`, `USkyraActivatableWidget` | Layered UI system built on top of CommonUI. |

### Repository Structure and Infrastructure

The repository includes standard Unreal Engine project infrastructure for source control and compilation.

*   **Source Control**: The project uses Git LFS for binary assets like `.uasset` files [.gitattributes:3-3]().
*   **Build Artifacts**: The environment is configured to ignore standard build directories such as `Binaries/`, `Intermediate/`, and `Saved/` [.gitignore:49-71]().
*   **Editor Cache**: The `DerivedDataCache/` is excluded from version control to prevent bloat [.gitignore:74-74]().

### Navigation

*   **[Plugin Architecture and Module Structure](#1.1)**: Deep dive into the `.uplugin` and `Build.cs` files.
*   **[Core System Utilities](#1.2)**: Details on Asset Manager, Game Data, and Tag systems.
*   **[Experience and Game Mode System](#2)**: How the framework handles data-driven game rules.
*   **[Gameplay Ability System (GAS)](#3)**: Implementation details of the framework's combat and ability logic.
*   **[SkyraEditor Module](#11)**: Overview of editor-only tooling and validation.

Sources: [.gitattributes:1-4](), [.gitignore:1-75]()