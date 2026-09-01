# 外观与角色部件

<details>
<summary>Relevant source files</summary>

以下文件被用作生成本维基页面的上下文：

- [.gitattributes](.gitattributes)

</details>



SkyraFramework中的外观系统提供了一种模块化、数据驱动的方法来定制角色外观和动画行为。它采用“角色部件”架构，其中视觉组件通过插槽附加到Pawn的骨骼网格体上，并且动画层根据游戏标签动态选择。

## 角色部件架构

该系统在Controller（管理哪些部件应处于活动状态的“意图”）和Pawn（处理这些部件的实际生成与复制）之间进行了划分。

### 数据结构
*   **`FSkyraCharacterPart`**：一个定义要生成的部件的结构体。它包含`PartClass`（要生成的Actor）、用于附着的`SocketName`以及`CollisionMode` [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:44-61]()。
*   **`FSkyraCharacterPartHandle`**：用于跟踪和移除特定部件的不透明句柄 [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:20-33]()。

### SkyraControllerComponent_CharacterParts
此组件位于`APlayerController`上，并作为玩家外观选择的持久存储。
*   它通过`FSkyraControllerCharacterPartEntry`列表跟踪角色部件 [Source/SkyraGame/Public/Cosmetics/SkyraControllerComponent_CharacterParts.h:47-56]()。
*   当棋子被控制时，它会通过调用棋子定制器组件上的`AddCharacterPart`，自动将存储的部件应用到新棋子[Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:119-130]()。
*   它处理“自然”部件（标准装备）与“作弊”部件（来自开发者设置的覆盖）[Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:60-74]()。

### SkyraPawnComponent_CharacterParts
该组件存在于`APawn`上，并处理部件的物理表现和网络同步。
*   **复制**：它使用`FSkyraCharacterPartList`（一个`FFastArraySerializer`结构体）来高效地将部件数据复制到客户端[Source/SkyraGame/Public/Cosmetics/SkyraPawnComponent_CharacterParts.h:116-130]()。
*   **生成**：在客户端和服务器（非专用）上，它为每个条目生成`UChildActorComponent`实例并将它们附加到网格上[Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:153-196]()。
*   **标签收集**：如果生成的Actor实现了`IGameplayTagAssetInterface`，它可以从所有生成的部件中聚合游戏标签[Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:135-151]()。

### 角色部件流程
下面的图表说明了部件如何从控制器的意图传递到棋子上的物理Actor。

| 系统层 | 实体 | 角色 |
| :--- | :--- | :--- |
| **请求** | `USkyraControllerComponent_CharacterParts` | 调用 `AddCharacterPartInternal` 使用 `ECharacterPartSource` [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:60-64](). |
| **管理** | `FSkyraCharacterPartList` | 生成一个 `FSkyraCharacterPartHandle` 并添加到 `Entries` 数组 [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:76-96](). |
| **复制** | `FFastArraySerializer` | 在客户端触发 `PostReplicatedAdd` [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:42-55](). |
| **执行** | `SpawnActorForEntry` | 创建 `UChildActorComponent` 并附加到 `SocketName` [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:153-172](). |

**来源：** [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:14-130](), [Source/SkyraGame/Private/Cosmetics/SkyraPawnComponent_CharacterParts.cpp:76-200](), [Source/SkyraGame/Public/Cosmetics/SkyraCharacterPartTypes.h:44-61]()

## 装饰动画选择

Skyra 使用基于标签的选择系统来确定使用哪些动画层或骨骼网格体。这主要通过 `FSkyraAnimLayerSelectionSet` 处理。

### FSkyraAnimLayerSelectionSet
此结构允许系统根据 `FGameplayTagContainer` 来选择 `UAnimInstance` 层。
*   **`SelectBestLayer`**：遍历规则列表。如果提供的 `CosmeticTags` 与某条规则的 `RequiredTags` 匹配，则返回该规则的 `Layer`。若无规则匹配，返回 `DefaultLayer` [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:10-21]()

### FSkyraAnimBodyStyleSelectionSet
与层选择类似，此结构根据标签选择 `USkeletalMesh`，允许角色动态更改体型或基础网格体 [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:23-34]()。

### 动画选择逻辑
标题：动画层选择逻辑
```mermaid
graph TD
    "InputTags[FGameplayTagContainer]" --> "SelectBestLayer[FSkyraAnimLayerSelectionSet::SelectBestLayer]"
    "SelectBestLayer" --> "LoopRules[Iterate LayerRules]"
    "LoopRules" -- "Match Found" --> "ReturnRuleLayer[Return Rule.Layer]"
    "LoopRules" -- "No Match" --> "ReturnDefault[Return DefaultLayer]"
    "ReturnRuleLayer" --> "AnimInstance[UAnimInstance Class]"
    "ReturnDefault" --> "AnimInstance"
```
**来源：** [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticAnimationTypes.cpp:1-35](), [Source/SkyraGame/Public/Cosmetics/SkyraCosmeticAnimationTypes.h:18-40]()

## 开发者设置与作弊

框架包含强大的工具，用于在无需完整后端集成的情况下测试外观物品。

### SkyraCosmeticDeveloperSettings
一个开发者设置类（在项目设置中可见），允许开发者：
*   定义一个 `CheatMode`（例如，`ReplaceParts`）[Source/SkyraGame/Public/Cosmetics/SkyraCosmeticDeveloperSettings.h:35-40]()。
*   指定一个 `CheatCosmeticCharacterParts` 列表，在 PIE 期间自动应用[Source/SkyraGame/Public/Cosmetics/SkyraCosmeticDeveloperSettings.h:43-44]()。
*   这些设置会在 PIE 启动或属性更改时应用于服务器世界中的所有玩家控制器[Source/SkyraGame/Private/Cosmetics/SkyraCosmeticDeveloperSettings.cpp:58-80]()。

### SkyraCosmeticCheats
一个 `UCheatManagerExtension`，提供用于运行时外观物品测试的控制台命令：
*   `AddCharacterPart <AssetName>`：通过搜索类名添加一个部件[Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:28-43]()。
*   `ReplaceCharacterPart <AssetName>`：清除现有覆盖项并添加一个新部件[Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:45-49]()。
*   `ClearCharacterPartOverrides`：移除所有通过作弊管理器添加的部件[Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:51-59]()。

### 作弊集成流程
标题：装饰品作弊执行流程
```mermaid
graph LR
    "ConsoleCommand[AddCharacterPart]" --> "USkyraCosmeticCheats"
    "USkyraCosmeticCheats" --> "FindClass[USkyraDevelopmentStatics::FindClassByShortName]"
    "FindClass" --> "ControllerComp[USkyraControllerComponent_CharacterParts]"
    "ControllerComp" --> "AddCheatPart[AddCheatPart]"
    "AddCheatPart" --> "Internal[AddCharacterPartInternal]"
    "Internal" -- "Source: AppliedViaCheatManager" --> "PawnComp[USkyraPawnComponent_CharacterParts]"
```
**来源：** [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticCheats.cpp:14-69](), [Source/SkyraGame/Private/Cosmetics/SkyraCosmeticDeveloperSettings.cpp:18-91](), [Source/SkyraGame/Private/Cosmetics/SkyraControllerComponent_CharacterParts.cpp:164-192]()