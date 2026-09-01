# 能力消耗、游戏阶段与全局能力系统

<details>
<summary>Relevant source files</summary>

以下文件被用作生成此Wiki页面的上下文：

- [.gitattributes](.gitattributes)

</details>



本页面记录了SkyraFramework中针对游戏玩法能力系统（GAS）的专门扩展。内容包括能力如何通过自定义消耗类消耗资源、如何通过标签驱动的阶段系统管理游戏流程，以及全局能力系统如何管理所有活跃参与者的效果。

## 能力消耗

SkyraFramework通过提供与库存和玩家状态系统交互的数据驱动消耗类，扩展了标准GAS消耗机制。这些类继承自`USkyraAbilityCost`，并实现了`CheckCost`和`ApplyCost`。

### 库存与标签堆叠消耗

该框架提供了三种主要的消耗实现：

| 类 | 资源来源 | 描述 |
| :--- | :--- | :--- |
| `USkyraAbilityCost_InventoryItem` | `USkyraInventoryManagerComponent` | 检查玩家是否拥有由`USkyraInventoryItemDefinition`定义的特定数量的物品 [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_InventoryItem.cpp:14-26]()。 |
| `USkyraAbilityCost_ItemTagStack` | `USkyraInventoryItemInstance` | 用于由装备授予的技能。它检查物品实例本身上的游戏标签堆叠（例如弹药）[Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:19-29]()。 |
| `USkyraAbilityCost_PlayerTagStack` | `ASkyraPlayerState` | 检查存储在玩家状态全局的游戏标签堆叠 [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_PlayerTagStack.cpp:16-28]()。 |

### 消耗流程与失败处理
当技能在`USkyraAbilityCost_ItemTagStack`中未通过消耗检查时，它可以选择性地向`OptionalRelevantTags`容器添加一个`FailureTag`（默认为`Ability.ActivateFail.Cost`）。这使得UI或其他系统能够专门响应"弹药耗尽"或"资源耗尽"状态 [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:11-35]()。

**来源：**
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_InventoryItem.cpp:9-52]()
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_ItemTagStack.cpp:11-59]()
- [Source/SkyraGame/Private/AbilitySystem/Abilities/SkyraAbilityCost_PlayerTagStack.cpp:11-50]()

---

## 游戏阶段系统

游戏阶段系统使用游戏玩法能力来表示高级游戏状态（例如，热身、进行中、突然死亡）。这使得游戏状态能够受益于 GAS 固有的复制和逻辑封装。

### SkyraGamePhaseAbility
一个专门的技能类 `USkyraGamePhaseAbility` 表示一个阶段。
- **服务器启动**：阶段始终由服务器启动 [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:21-22]()
- **标签关联**：每个阶段技能都与一个 `GamePhaseTag` 关联 [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:54]()
- **注册**：激活时，它将自己注册到 `USkyraGamePhaseSubsystem` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:31]()

### SkyraGamePhaseSubsystem
这个世界子系统管理活动阶段的生命周期并处理转换。

- **阶段转换**：当新阶段开始时，子系统检查传入的标签是否与现有的活动阶段匹配。如果推送了一个新阶段（例如，`Game.GameOver`），它会取消不兼容的活动阶段（例如，`Game.Playing`） [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:156-177]()
- **子阶段**：系统支持层级阶段。多个能力可共享一个父标签；启动`Game.Playing.SuddenDeath`不一定结束`Game.Playing` [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:164-167]()。
- **观察者**：系统可通过`WhenPhaseStartsOrIsActive`和`WhenPhaseEnds`注册以在阶段开始或结束时收到通知 [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:99-120]()。

#### 游戏阶段代码实体关联
标题：游戏阶段代码实体关联
```mermaid
graph TD
    subgraph "Logic Space"
        "StartPhase()" --> "ActivateAbility()"
        "EndAbility()" --> "OnEndPhase()"
    end

    subgraph "Code Entities"
        "StartPhase()" -- "calls" --> Subsystem["USkyraGamePhaseSubsystem"]
        Subsystem -- "grants" --> PhaseAbility["USkyraGamePhaseAbility"]
        PhaseAbility -- "has" --> Tag["GamePhaseTag (FGameplayTag)"]
        Subsystem -- "manages" --> Map["ActivePhaseMap (TMap)"]
    end

    subgraph "State Management"
        Map -- "tracks" --> Entry["FSkyraGamePhaseEntry"]
        Entry -- "holds" --> Callback["PhaseEndedCallback"]
    end
```
**源码：**
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseAbility.cpp:16-47]()
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:50-134]()
- [Source/SkyraGame/Private/AbilitySystem/Phases/SkyraGamePhaseSubsystem.cpp:136-194]()

---

## 全局能力系统

`USkyraGlobalAbilitySystem`提供了一种机制，将游戏性能力和游戏性效果应用于所有拥有`USkyraAbilitySystemComponent`的Actor。

### 注册与应用
- **ASC注册**：当角色或Pawn初始化其ASC时，它通过`RegisterASC`向该系统注册。系统立即将当前激活的“全局”能力或效果应用于该新ASC [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:126-140]()。
- **批量应用**：`ApplyAbilityToAll` 和 `ApplyEffectToAll` 等函数遍历所有 `RegisteredASCs` 以授予指定类 [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:82-104]().
- **清理**：当通过 `UnregisterASC` 取消注册 ASC 时（例如 actor 销毁），系统会从该特定 ASC 中移除所有全局应用的处理程序 [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:142-155]().

#### 全局能力分发流程
标题：全局能力分发流程
```mermaid
graph LR
    "Manager"["USkyraGlobalAbilitySystem"] -- "ApplyAbilityToAll()" --> "List"["FGlobalAppliedAbilityList"]
    "List" -- "Iterates" --> "RegisteredASCs"["TArray<USkyraAbilitySystemComponent*>"]
    "RegisteredASCs" -- "GiveAbility()" --> "Handles"["TMap<ASC, FGameplayAbilitySpecHandle>"]
    
    "NewActor" -- "RegisterASC()" --> "Manager"
    "Manager" -- "BackfillsAbilities" --> "NewActor"
```
**来源：**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:9-41]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:82-124]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGlobalAbilitySystem.cpp:126-155]()

---

## Gameplay Cue 管理器

`USkyraGameplayCueManager` 优化了 Gameplay Cue（VFX/SFX）的加载和执行。

- **异步加载**：它支持不同的加载模式，包括用于编辑器稳定性的 `LoadUpfront` 和用于运行时性能的 `PreloadAsCuesAreReferenced` [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:16-28]().
- **标签驱动预加载**：当引擎加载 gameplay 标签时，管理器会拦截以 `GameplayCue` 开头的标签，并为相应的 Notify 资产启动异步加载 [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:180-202]().
- **内存管理**：它追踪“始终加载”的提示与按需加载的提示，并提供调试命令，如`Skyra.DumpGameplayCues`用以检查内存使用情况 [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:32-35]()。

**来源：**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:59-93]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:126-178]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayCueManager.cpp:180-202]()

---

## 游戏效果上下文

`FSkyraGameplayEffectContext`是一个自定义结构体，扩展了基础GAS上下文，以在执行管线中携带Skyra特定的数据。

- **技能来源**：它可以存储一个指向实现了`ISkyraAbilitySourceInterface`的对象的弱指针，让伤害计算能够识别来源（例如，特定的武器或装置）[Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:48-57]()。
- **物理材质**：它提供了一个辅助函数`GetPhysicalMaterial()`，从上下文中存储的命中结果中提取物理材质，便于在执行计算中实现特定于表面的逻辑 [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:59-66]()。
- **序列化**：实现了`NetSerialize`，并与Iris复制系统集成，以实现高效的网络传输 [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:29-46]()。

**来源：**
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:18-27]()
- [Source/SkyraGame/Private/AbilitySystem/SkyraGameplayEffectContext.cpp:48-66]()