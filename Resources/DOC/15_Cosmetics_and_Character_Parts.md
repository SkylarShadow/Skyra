# Cosmetics and Character Parts

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [.gitattributes](.gitattributes)

</details>



The cosmetics system in SkyraFramework provides a modular, data-driven approach to customizing character appearance and animation behavior. It utilizes a "Character Part" architecture where visual components are attached to a pawn's skeletal mesh via sockets, and animation layers are selected dynamically based on gameplay tags.

## Character Part Architecture

The system is split between the Controller (which manages the "intent" of what parts should be active) and the Pawn (which handles the actual spawning and replication of those parts).

### Data Structures
*   **`FSkyraCharacterPart`**: A struct defining a part to be spawned. It contains the `PartClass` (the Actor to spawn), a `SocketName` for attachment, and a `CollisionMode` [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:44-61]().
*   **`FSkyraCharacterPartHandle`**: An opaque handle used to track and remove specific parts [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:20-33]().

### SkyraControllerComponent_CharacterParts
This component lives on the `APlayerController` and serves as the persistent storage for a player's cosmetic choices.
*   It tracks character parts in a list of `FSkyraControllerCharacterPartEntry` [Source/SkyraGame/Public/Cosmetics/SkyraControllerComponent_CharacterParts.h:47-56]().
*   When a pawn is possessed, it automatically applies the stored parts to the new pawn by calling `AddCharacterPart` on the pawn's customizer component [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:119-130]().
*   It handles "natural" parts (standard loadout) vs "cheat" parts (overrides from developer settings) [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:60-74]().

### SkyraPawnComponent_CharacterParts
This component lives on the `APawn` and handles the physical manifestation and networking of parts.
*   **Replication**: It uses a `FSkyraCharacterPartList` (a `FFastArraySerializer` struct) to replicate part data efficiently to clients [Source/SkyraGame/Public/Cosmetics/SkyraPawnComponent_CharacterParts.h:116-130]().
*   **Spawning**: On clients and servers (non-dedicated), it spawns `UChildActorComponent` instances for each entry and attaches them to the mesh [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:153-196]().
*   **Tag Collection**: It can aggregate gameplay tags from all spawned parts if the spawned actors implement `IGameplayTagAssetInterface` [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:135-151]().

### Character Part Flow
The following diagram illustrates how a part moves from the Controller's intent to a physical actor on the Pawn.

| System Layer | Entity | Role |
| :--- | :--- | :--- |
| **Request** | `USkyraControllerComponent_CharacterParts` | Calls `AddCharacterPartInternal` with `ECharacterPartSource` [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:60-64](). |
| **Management** | `FSkyraCharacterPartList` | Generates a `FSkyraCharacterPartHandle` and adds to `Entries` array [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:76-96](). |
| **Replication** | `FFastArraySerializer` | Triggers `PostReplicatedAdd` on clients [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:42-55](). |
| **Execution** | `SpawnActorForEntry` | Creates `UChildActorComponent` and attaches to `SocketName` [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:153-172](). |

**Sources:** [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:14-130](), [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:76-200](), [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:44-61]()

## Cosmetic Animation Selection

Skyra uses a tag-based selection system to determine which animation layers or skeletal meshes to use. This is primarily handled via `FSkyraAnimLayerSelectionSet`.

### FSkyraAnimLayerSelectionSet
This structure allows the system to select a `UAnimInstance` layer based on a `FGameplayTagContainer`.
*   **`SelectBestLayer`**: Iterates through a list of rules. If the provided `CosmeticTags` match the `RequiredTags` of a rule, that rule's `Layer` is returned. If no rules match, it returns the `DefaultLayer` [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:10-21]().

### FSkyraAnimBodyStyleSelectionSet
Similar to the layer selection, this selects a `USkeletalMesh` based on tags, allowing characters to change body types or base meshes dynamically [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:23-34]().

### Animation Selection Logic
Title: Animation Layer Selection Logic
```mermaid
graph TD
    "InputTags[FGameplayTagContainer]" --> "SelectBestLayer[FSkyraAnimLayerSelectionSet::SelectBestLayer]"
    "SelectBestLayer" --> "LoopRules[Iterate LayerRules]"
    "LoopRules" -- "Match Found" --> "ReturnRuleLayer[Return Rule.Layer]"
    "LoopRules" -- "No Match" --> "ReturnDefault[Return DefaultLayer]"
    "ReturnRuleLayer" --> "AnimInstance[UAnimInstance Class]"
    "ReturnDefault" --> "AnimInstance"
```
**Sources:** [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:1-35](), [Source/SkyraGame/Public/Cosmetics/SkyraCosmeticAnimationTypes.h:18-40]()

## Developer Settings and Cheats

The framework includes robust tooling for testing cosmetics without requiring full backend integration.

### SkyraCosmeticDeveloperSettings
A developer settings class (visible in Project Settings) that allows developers to:
*   Define a `CheatMode` (e.g., `ReplaceParts`) [Source/SkyraGame/Public/Cosmetics/SkyraCosmeticDeveloperSettings.h:35-40]().
*   Specify a list of `CheatCosmeticCharacterParts` to apply automatically during PIE [Source/SkyraGame/Public/Cosmetics/SkyraCosmeticDeveloperSettings.h:43-44]().
*   The settings are applied to all player controllers in the server world during PIE startup or property changes [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticDeveloperSettings.cpp:58-80]().

### SkyraCosmeticCheats
A `UCheatManagerExtension` that provides console commands for runtime cosmetic testing:
*   `AddCharacterPart <AssetName>`: Adds a part by searching for its class name [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:28-43]().
*   `ReplaceCharacterPart <AssetName>`: Clears existing overrides and adds a new part [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:45-49]().
*   `ClearCharacterPartOverrides`: Removes all parts added via the cheat manager [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:51-59]().

### Cheat Integration Flow
Title: Cosmetic Cheat Execution Flow
```mermaid
graph LR
    "ConsoleCommand[AddCharacterPart]" --> "USkyraCosmeticCheats"
    "USkyraCosmeticCheats" --> "FindClass[USkyraDevelopmentStatics::FindClassByShortName]"
    "FindClass" --> "ControllerComp[USkyraControllerComponent_CharacterParts]"
    "ControllerComp" --> "AddCheatPart[AddCheatPart]"
    "AddCheatPart" --> "Internal[AddCharacterPartInternal]"
    "Internal" -- "Source: AppliedViaCheatManager" --> "PawnComp[USkyraPawnComponent_CharacterParts]"
```
**Sources:** [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:14-69](), [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticDeveloperSettings.cpp:18-91](), [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:164-192]()