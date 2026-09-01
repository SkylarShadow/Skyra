# 前端与设置UI

<details>
<summary>Relevant source files</summary>

以下文件被用作生成此Wiki页面的上下文：

- [.gitattributes](.gitattributes)

</details>



SkyraFramework中的前端与设置UI系统提供了一种数据驱动的方法来管理主菜单流程、大厅背景和分层设置注册表。该系统利用虚幻引擎的`CommonUI`创建了一个跨平台兼容的界面，用于处理输入映射、可扩展性设置和用户偏好。

## 前端流程与状态管理

游戏前端的入口点由`SkyraFrontendStateComponent`管理。该组件负责处理初始加载状态与主菜单之间的过渡。

### SkyraFrontendStateComponent
`USkyraFrontendStateComponent`负责管理前端体验的流程。它与`USkyraExperienceManagerComponent`协调，确保"Frontend"体验完全加载后再显示UI。

*   **流程控制**：它通过`USkyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded` [[Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:63-68]]() 监控体验加载状态。
*   **UI集成**：一旦体验准备就绪，它利用`USkyraUIManagerSubsystem`推送主菜单布局 [[Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:70-75]]()。

### 大厅与背景
前端的可视背景由`ASkyraLobbyBackground`管理。该Actor允许基于关卡或Actor的背景，可根据当前的前端状态或活动事件进行切换。

### 性能操作
在前端阶段，使用特定的 `GameFeatureActions` 来优化引擎以进行菜单渲染。
*   **ApplyFrontendPerfSettingsAction**：此操作在 `UGameFeatureAction_ApplyFrontendPerfSettings` 中定义，用于在玩家处于菜单界面时切换特定的性能模式（例如限制帧率或降低全局光照），以节省功耗并减少发热 [[Source/SkyraGame/Private/GameFeatures/GameFeatureAction_ApplyFrontendPerfSettings.cpp:15-25]]()。

### 前端流程图
该图展示了状态组件、体验系统以及UI子系统之间的关系。

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
**来源：**
*   `USkyraFrontendStateComponent`：[Source/SkyraGame/Private/Frontend/SkyraFrontendStateComponent.cpp:1-80]()
*   `UGameFeatureAction_ApplyFrontendPerfSettings`：[Source/SkyraGame/Private/GameFeatures/GameFeatureAction_ApplyFrontendPerfSettings.cpp:1-40]()

---

## 设置系统架构

SkyraFramework 使用去中心化的注册系统来管理游戏设置。这使得不同的模块（音频、视频、输入）可以注册自己的设置，而无需膨胀单一的全局类。

### 设置注册表层次结构
该系统的核心是 `USkyraGameSettingRegistry`。它充当 `UGameSetting` 对象的容器，并组织成不同的类别。

*   **USkyraGameSettingRegistry**： 设置数据库的基类。 提供初始化设置以及按名称或标签查找设置的方法 [[Source/SkyraGame/Private/Settings/SkyraGameSettingRegistry.cpp:15-30]]()。
*   **按类别注册表**： 该框架为不同领域实现了专门的注册逻辑：
    *   **视频/图形**： 通过 `USkyraSettingsLocal` 中的可扩展性设置进行管理。
    *   **音频**： 与 `USkyraAudioSettings` 交互。
    *   **输入**： 处理鼠标灵敏度和游戏手柄按键重新映射。

### 设置数据存储
设置根据其持久化方式分为两类：

| 类 | 描述 |
| :--- | :--- |
| `USkyraSettingsLocal` | 存储特定于机器的设置，如分辨率、可扩展性级别以及特定于设备的输入覆盖 [[Source/SkyraGame/Private/Settings/SkyraSettingsLocal.cpp:10-50]]()。 |
| `USkyraSettingsShared` | 存储应在设备间漫游的用户特定偏好，例如字幕、色盲模式和游戏玩法切换。 |

### 设置UI层次结构
设置界面采用递归小部件结构构建：
1.  **SkyraSettingScreen**：顶层可激活小部件，包含导航栏和设置详情区域。
2.  **SkyraSettingList**：基于`USkyraGameSettingRegistry`生成行的动态列表。
3.  **SkyraSettingDetailView**：显示所选设置的描述和特定小部件（滑块、组合框）。

### 设置注册实体映射
此图将逻辑设置类别映射到负责注册它们的代码实体。

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
**来源：**
*   `USkyraGameSettingRegistry`: [Source/SkyraGame/Private/Settings/SkyraGameSettingRegistry.cpp:1-100]()
*   `USkyraSettingsLocal`: [Source/SkyraGame/Private/Settings/SkyraSettingsLocal.cpp:1-200]()

---

## 开发者与平台模拟

为了便于测试各种硬件配置和前端流程，该框架包含了专门的开发者设置。

### SkyraDeveloperSettings
`USkyraDeveloperSettings`类允许开发者在PIE（在编辑器中运行）中启动游戏时覆盖默认体验。
*   **体验覆盖**：如果设置了`ExperienceOverride`，则在PIE启动时会显示通知消息，提醒开发者他们未使用默认游戏流程[[Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:50-58]]()。

### SkyraPlatformEmulationSettings
`USkyraPlatformEmulationSettings`系统允许开发者在编辑器内“假装”游戏运行在其他平台（例如移动端、主机）上。

*   **特性覆盖**：它可以启用或抑制特定的`CommonUI`平台特性，从而触发UI可见性变化（例如在PC上显示触摸控件）[[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:95-100]]()。
*   **设备配置文件模拟**：它可以强制引擎使用特定的`UDeviceProfile`来测试目标硬件的可扩展性设置[[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-175]]()。
*   **编辑器模拟**：使用`UPlatformSettingsManager::SetEditorSimulatedPlatform`来更改引擎报告的平台[[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:110-111]]()。

### 模拟流程
| 功能 | 目的 |
| :--- | :--- |
| `ApplySettings()` | 每当编辑器中的属性发生更改时调用，以刷新 UI 可见性和平台状态 [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:93-103]](). |
| `PickReasonableBaseDeviceProfile()` | 自动为模拟平台选择最合适的设备配置文件 [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:161-192]](). |
| `OnPlayInEditorStarted()` | 显示有关活动覆盖（特征、平台或体验）的通知提示 [[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:57-91]](). |

**来源：**
*   `USkyraDeveloperSettings`：[Source/SkyraGame/Private/Development/SkyraDeveloperSettings.cpp:12-60]()
*   `USkyraPlatformEmulationSettings`：[Source/SkyraGame/Private/Development/SkyraPlatformEmulationSettings.cpp:16-193]()