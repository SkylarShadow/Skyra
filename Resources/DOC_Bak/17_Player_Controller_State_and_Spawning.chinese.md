# 玩家控制器、状态与生成

<details>
<summary>Relevant source files</summary>

以下文件用作生成本维基页面的上下文：

- [.gitattributes](.gitattributes)

</details>



本页面详述了SkyraFramework中核心玩家基础结构。涵盖了针对人类玩家和机器人的专用玩家控制器，持久化玩家状态，以及负责玩家生成和管理体验中玩家生命周期的模块化系统。

## ASkyraPlayerController

`ASkyraPlayerController`作为人类玩家与游戏世界之间的主要接口。它扩展了基础`APlayerController`以集成Skyra的团队系统、摄像机管理和游戏玩法系统。

### 主要职责
*   **团队集成**：实现`ISkyraTeamAgentInterface`以允许玩家属于特定团队并处理基于团队的查询。
*   **摄像机辅助**：与摄像机系统集成，通过`ASkyraPlayerCameraManager`提供平滑过渡和辅助逻辑。
*   **输入路由**：将输入转发到受控Pawn上附加的`USkyraHeroComponent`。
*   **作弊管理**：提供对`USkyraCheatManager`的访问，用于开发和调试命令。

### 回放与录制
控制器包含`USkyraReplaySubsystem`的钩子，支持录制和回放游戏会话，以便审查或调试。

## ASkyraPlayerState

`ASkyraPlayerState` 管理玩家的持久数据，这些数据在 pawn 死亡和重生后依然保留。它是玩家控制实体的 `USkyraAbilitySystemComponent` (ASC) 的主要所有者。

### 数据流：Player State 和 ASC
Player State 初始化 ASC 和属性集（例如 `USkyraHealthSet` 和 `USkyraCombatSet`）。当控制一个 pawn 时，ASC 会更新为指向新的 pawn 作为其物理化身，而 Player State 仍然是所有者。

| 组件/类 | 职责 |
| :--- | :--- |
| `USkyraAbilitySystemComponent` | 处理游戏玩法标签、能力和效果。 |
| `USkyraHealthSet` | 跟踪生命值、最大生命值和护盾属性。 |
| `USkyraCombatSet` | 追踪伤害和治疗属性。 |

## 玩家生成与机器人管理

Skyra 利用模块化方法通过 `USkyraPlayerSpawningManagerComponent`（位于游戏状态上）和专门的 `ASkyraPlayerStart` Actor 进行生成。

### 机器人生成逻辑
`USkyraBotCreationComponent` 在体验加载期间自动创建 AI 控制的玩家。

#### 实现流程：机器人创建
1.  **体验加载**：该组件通过 `USkyraExperienceManagerComponent` 监听 `OnExperienceLoaded`。
2.  **数量确定**：它根据 `NumBotsToCreate`、开发者设置覆盖或 URL 选项（`?NumBots=X`）计算机器人数量。
3.  **控制器生成**：生成一个 `AAIController`（或 `ASkyraPlayerBotController`）。
4.  **初始化**：调用游戏模式上的 `GenericPlayerInitialization` 来设置队伍和玩家状态。
5.  **Pawn 重启**：触发 `RestartPlayer` 以生成实际的 Pawn。

### 系统架构：生成与机器人
下图展示了游戏模式、Bot 创建组件以及生成 AI 实体之间的关系。

标题：生成与 Bot 架构
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
来源：[Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:21-40](), [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:98-129]()

## 开发与调试

### SkyraCheatManager 与 Bot 作弊
`USkyraBotCheats` 类扩展了作弊管理器，以提供用于 Bot 管理的运行时命令。这些命令在作弊管理器创建时自动注册。

*   **AddPlayerBot**：触发创建组件上的 `Cheat_AddBot`。
*   **RemovePlayerBot**：触发 `Cheat_RemoveBot` 从世界中移除一个随机 Bot。

### 调试摄像机
`ASkyraDebugCameraController` 提供了增强的调试视图，允许开发者从玩家 Pawn 分离并检查世界，同时保持对 Skyra 特定调试信息的访问。

### 代码实体映射：调试与作弊
该图将作弊命令连接到内部组件逻辑。

标题：作弊管理器到机器人逻辑映射
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
来源：[Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:27-58](), [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:131-163]()

## 生成类概述

| 类 | 角色 |
| :--- | :--- |
| `ASkyraPlayerStart` | 使用队伍特定的标签和需求扩展了`APlayerStart`。 |
| `SkyraPlayerSpawningManagerComponent` | 处理为玩家查找最佳`ASkyraPlayerStart`的逻辑。 |
| `USkyraBotCreationComponent` | 服务器端组件，用于管理AI机器人的数量。 |
| `ASkyraPlayerBotController` | 用于机器人的 AI 控制器，旨在模仿人类玩家的状态设置。 |

来源：
* [Source/SkyraGame/Private/GameModes/SkyraBotCreationComponent.cpp:1-183]()
* [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:1-60]()
* [.gitattributes:1-4]()