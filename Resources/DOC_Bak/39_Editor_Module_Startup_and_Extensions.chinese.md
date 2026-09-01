# Editor 模块启动与扩展

<details>
<summary>Relevant source files</summary>

以下文件用作生成本 wiki 页面的上下文：

- [.gitattributes](.gitattributes)

</details>



`SkyraEditor` 模块为 SkyraFramework 提供了基础工具和编辑器特定重写。这包括自定义 Slate 样式、用于内容验证和地图导航的工具栏扩展、专门的 Gameplay Ability System (GAS) 编辑器集成，以及一个自定义编辑器引擎来管理 Play-In-Editor (PIE) 生命周期和工作区自动化。

## 模块生命周期与初始化

`FSkyraEditorModule` 类处理编辑器特定系统的启动和关闭。它确保框架的自定义样式已注册，并且 Gameplay Ability System 的专用委托已绑定到编辑器的 UI。

### 启动与关闭流程
1.  **初始化样式**: 初始化 `FGameEditorStyle` 以为自定义 UI 提供图标和画刷 [Source/SkyraEditor/Private/SkyraEditor.cpp:210]().
2.  **模块回调**: 如果未作为独立游戏运行，该模块会通过 `FModuleManager::Get().OnModulesChanged()` 监听模块更改事件 [Source/SkyraEditor/Private/SkyraEditor.cpp:214]().
3.  **GAS 委托绑定**: 该模块调用 `BindGameplayAbilitiesEditorDelegates()` 来配置 GameplayCue 在编辑器中的创建和搜索方式 [Source/SkyraEditor/Private/SkyraEditor.cpp:216]().
4.  **菜单注册**: 工具栏扩展通过 `RegisterGameEditorMenus()` 注册 [Source/SkyraEditor/Private/SkyraEditor.cpp:149]().

### GameplayCue 编辑器集成
编辑器模块将多个静态函数绑定到`IGameplayAbilitiesEditorModule`，以简化GameplayCue通知的创建：

| 函数 | 用途 | 实现细节 |
| :--- | :--- | :--- |
| `GetGameplayCueDefaultClasses` | 定义允许的通知类 | 添加`UGameplayCueNotify_Burst`、`AGameplayCueNotify_BurstLatent`和`AGameplayCueNotify_Looping` [Source/SkyraEditor/Private/SkyraEditor.cpp:33-39]()。 |
| `GetGameplayCueInterfaceClasses` | 识别有效目标 | 遍历所有`UClass`对象，以查找实现`UGameplayCueInterface`的`AActor`子类 [Source/SkyraEditor/Private/SkyraEditor.cpp:42-53]()。 |
| `GetGameplayCuePath` | 设置默认资源路径 | 从 `UAbilitySystemGlobals::GetGameplayCueNotifyPaths` 检索路径，并在标签名称前添加 `GCN_` 前缀 [Source/SkyraEditor/Private/SkyraEditor.cpp:56-86](). |

**来源：**
* [Source/SkyraEditor/Private/SkyraEditor.cpp:33-86]()
* [Source/SkyraEditor/Private/SkyraEditor.cpp:204-218]()

---

## 编辑器样式 (FGameEditorStyle)

`FGameEditorStyle` 管理专用于 Skyra 编辑器工具的 `FSlateStyleSet`。它使用基于 SVG 的矢量图像，以确保在不同显示比例下图标的高保真度。

### 样式实现
样式集在 `Initialize()` 期间注册到 `FSlateStyleRegistry` [Source/SkyraEditor/Private/GameEditorStyle.cpp:17]()。它为工具栏定义了特定的笔刷，例如指向本地 SVG 资源的 `GameEditor.CheckContent` 图标 [Source/SkyraEditor/Private/GameEditorStyle.cpp:56]()。

### 资源映射图
该图展示了自然语言 UI 概念如何映射到内部的 Slate 样式代码。

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

**来源：**
* [Source/SkyraEditor/Private/GameEditorStyle.cpp:12-60]()
* [Source/SkyraEditor/Public/GameEditorStyle.h:10-29]()

---

## 工具栏扩展

编辑器扩展了`LevelEditor.LevelEditorToolBar.PlayToolBar`以添加Skyra特定的实用工具 [Source/SkyraEditor/Private/SkyraEditor.cpp:151]().

### 检查内容
此按钮触发`UEditorValidator::ValidateCheckedOutContent` [Source/SkyraEditor/Private/SkyraEditor.cpp:144-147]()。仅当没有PIE会话处于活动状态时（`HasNoPlayWorld`），它才可见且可执行 [Source/SkyraEditor/Private/SkyraEditor.cpp:175-177]()。

### 常用地图下拉菜单
为了加速开发，提供了一个“常用地图”下拉菜单。该菜单从`USkyraDeveloperSettings::CommonEditorMaps`动态填充 [Source/SkyraEditor/Private/SkyraEditor.cpp:120]()。

*   **逻辑**：仅当在开发者设置中定义了地图时，才会显示此下拉菜单 [Source/SkyraEditor/Private/SkyraEditor.cpp:111-114]()。
*   **操作**：点击地图条目会使用`UAssetEditorSubsystem::OpenEditorForAsset`切换关卡 [Source/SkyraEditor/Private/SkyraEditor.cpp:103-109]()。

**来源：**
* [Source/SkyraEditor/Private/SkyraEditor.cpp:103-142]()
* [Source/SkyraEditor/Private/SkyraEditor.cpp:149-199]()

---

## 编辑器引擎（USkyraEditorEngine）

`USkyraEditorEngine` 扩展了 `UUnrealEdEngine`，以自动化工作区配置并在 PIE 期间管理网络模式。

### PIE 生命周期管理
该引擎重写 `PreCreatePIEInstances` 以处理 `ASkyraWorldSettings` 中定义的特定网络模式要求 [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:56-58]()。

*   **强制独立模式**：如果 `ASkyraWorldSettings::ForceStandaloneNetMode` 为 true（通常用于前端/菜单地图），引擎会自动将 `EditorPlaySettings` 切换为 `PIE_Standalone` 并通知用户 [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:60-72]()。
*   **设置初始化**：它会在 `USkyraDeveloperSettings` 和 `USkyraPlatformEmulationSettings` 上触发 `OnPlayInEditorStarted`，以确保在模拟开始前应用开发者特定的覆盖 [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:76-77]()。

### 工作区自动化
在编辑器引擎首次 Tick 时，会调用 `FirstTickSetup()` [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:42]()。这用于强制 `UContentBrowserSettings` 显示插件文件夹，确保 SkyraFramework 插件内容默认始终对开发者可见 [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:52]()。

### 系统交互图
该图展示了自定义编辑器引擎与框架的设置类之间的关系。

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

**来源：**
* [Source/SkyraEditor/Private/SkyraEditorEngine.cpp:35-83]()
* [Source/SkyraEditor/Public/SkyraEditorEngine.h:14-34]()