# 热修复、回放和开发工具

<details>
<summary>Relevant source files</summary>

以下文件被用作生成此维基页面的上下文：

- [.gitattributes](.gitattributes)

</details>



本页面记录了SkyraFramework中用于实时更新（热修复）、游戏过程录制（回放）以及用于模拟平台和自动化测试的开发工具套件的基础架构。

## 热修复与运行时配置

SkyraFramework采用多层方法进行运行时配置和热修复，使开发人员能够在不进行完整客户端更新的情况下修补文本、调整游戏常量以及重写引擎级别的设置。

### SkyraHotfixManager
`USkyraHotfixManager`是运行时应用补丁的中央权威。它扩展了基础引擎的热修复功能，以提供用于数据验证和应用的框架特定钩子。

### SkyraRuntimeOptions
`USkyraRuntimeOptions`作为一个数据驱动的容器，用于存储可以通过热修复调整的全局游戏常量。它为设计师可能需要频繁更改的“可调参数”提供了一个集中位置。

### SkyraTextHotfixConfig
`USkyraTextHotfixConfig`类处理本地化文本的重定向。这允许通过将现有文本键映射到热修复数据资产中提供的新值来动态修复错别字或更新UI字符串。

| 类 | 职责 |
| :--- | :--- |
| `USkyraHotfixManager` | 编排热修复补丁的加载和应用。 |
| `USkyraRuntimeOptions` | 存储可在运行时调整的游戏参数和标记。 |
| `USkyraTextHotfixConfig` | 管理本地化文本字符串的动态覆盖。 |

**来源：**
- [Source/SkyraGame/Private/System/SkyraHotfixManager.cpp:1-20]()
- [Source/SkyraGame/Private/System/SkyraRuntimeOptions.cpp:1-20]()

---

## 回放子系统

SkyraFramework 通过专门的子系统和异步操作与 Unreal Replay 系统集成，以管理游戏会话的录制和查询。

### SkyraReplaySubsystem
`USkyraReplaySubsystem` 管理回放录制的生命周期。它处理流媒体的初始化，并提供接口供游戏根据游戏状态或开发者命令开始和停止录制。

### AsyncAction_QueryReplays
为了防止在搜索可用回放时阻塞主线程（尤其是在使用基于网络的流媒体时），框架提供了 `UAsyncAction_QueryReplays`。此操作将内部的回放元数据查询封装成蓝图友好的延迟节点。

**回放查询流程：**
1. **调用：** 蓝图调用 `QueryReplays`。
2. **执行：** 该操作与 `USkyraReplaySubsystem` 接口以获取元数据。
3. **完成：** 成功或失败时，相应的执行引脚被触发，并返回回放信息列表。

**来源：**
- [Source/SkyraGame/Private/Replays/SkyraReplaySubsystem.cpp:1-30]()
- [Source/SkyraGame/Private/Replays/AsyncAction_QueryReplays.cpp:1-30]()

---

## 开发与模拟工具

SkyraFramework 包含强大的工具，用于模拟不同的硬件环境，并简化编辑器内的开发流程。

### SkyraDeveloperSettings
`USkyraDeveloperSettings` 在项目设置中提供了一个持久的配置界面，用于开发者特定的重写。一个关键功能是 **体验重写（Experience Override）**，它允许开发者强制游戏加载特定的 `USkyraExperienceDefinition`，而不管默认地图设置如何。

*   **体验重写通知：** 当 PIE（在编辑器中播放）启动时，如果有一个重写处于激活状态，会向开发者显示一条 toast 通知 [Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:47-59]()。

### SkyraPlatformEmulationSettings
`USkyraPlatformEmulationSettings` 类允许开发者“假装”编辑器正运行在不同的平台上（例如，移动端或主机）。这对于测试 UI 可见性和性能缩放至关重要。

*   **平台特性：** 开发者可以手动启用或禁用表示平台特征的特定 Gameplay 标签 [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:60-79]()。
*   **设备配置文件模拟：** 系统可以模拟特定设备配置文件以测试可伸缩性设置 [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:127-159]()。
*   **可见性集成：** 它更新 `UCommonUIVisibilitySubsystem` 以反映模拟的特征 [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:95-95]()。

### 平台模拟数据流
下图说明了模拟设置如何从开发者的配置传播到 UI 和引擎系统。

**图表：平台模拟流程**
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
**来源：**
- [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:93-112]()
- [Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-193]()

---

## 测试与自动化

### SkyraTestControllerBootTest
`USkyraTestControllerBootTest` 是一个专用控制器，用于自动化的“启动测试”。它验证游戏能否成功初始化、加载体验，并在不崩溃或遇到严重错误的情况下进入可玩状态。

### SkyraBotCheats
`USkyraBotCheats` 提供了一个作弊管理器扩展，以便于使用 AI 实体进行测试。它与 `USkyraBotCreationComponent` 交互，在会话期间动态添加或移除机器人。

*   **注册：** 它在创建时自动向 `UCheatManager` 注册自身 [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:13-25]()。
*   **机器人管理：** 它提供了 `AddPlayerBot` 和 `RemovePlayerBot` 命令，用于在 `AGameStateBase` 上定位 `USkyraBotCreationComponent` [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:27-58]()。

**图表：作弊管理器扩展逻辑**
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

**来源：**
- [Source/SkyraGame/Private/Development/SkyraBotCheats.cpp:13-58]()

---

## 物理与材质

### PhysicalMaterialWithTags
SkyraFramework 通过 `UPhysicalMaterialWithTags` 扩展了标准物理材质系统。这允许开发人员将游戏标签直接与物理材质关联。

这些标签主要由 **情境效果系统**（见第10.1节）使用，以确定当特定表面发生碰撞或脚步时播放哪种视觉或音频效果（例如，`Surface.Type.Grass` 或 `Surface.Type.Metal`）。

**来源：**
- [Source/SkyraGame/Private/Physics/PhysicalMaterialWithTags.cpp:1-15]()