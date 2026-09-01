
# 部署Skyra

## 编辑器内设置：

### Project Settings

![](Resources/MarkdownImg/image-1.png)
设置Modes,Default Maps 和GameInstance

### Gameplaytags
![alt text](Resources/MarkdownImg/image-21.png)
可以添加上Skyra中的Tags,或者GameFeatures的Tags

### AssetManager

#### PrimaryAssetTypesToScan
Map:
![alt text](Resources/MarkdownImg/image-2.png)

PrimaryAssetLabel:
![alt text](Resources/MarkdownImg/image-3.png)

GameData:
![alt text](Resources/MarkdownImg/image-4.png)

GameFeatureData:
![alt text](Resources/MarkdownImg/image-5.png)

Experience:
![alt text](Resources/MarkdownImg/image-6.png)

UserFacingExperience:
![alt text](Resources/MarkdownImg/image-7.png)

LobbyBackground:
![alt text](Resources/MarkdownImg/image-8.png)
其实不一定会用到后端，可以不填


SkyraExperienceActionSet
![alt text](Resources/MarkdownImg/image-9.png)

#### Common Input Settings
![alt text](Resources/MarkdownImg/image-10.png)

#### Common Loading Screen
![alt text](Resources/MarkdownImg/image-11.png)

#### Game Features
![alt text](Resources/MarkdownImg/image-12.png)

#### SkyraAudioSettings
![alt text](Resources/MarkdownImg/image-13.png)

#### SkyraContextEffects

![alt text](Resources/MarkdownImg/image-14.png)

参考：
![alt text](Resources/MarkdownImg/image-19.png)

#### Enhanced Input
![alt text](Resources/MarkdownImg/image-15.png)

#### General Settings
![alt text](Resources/MarkdownImg/image-16.png)

#### Input
![alt text](Resources/MarkdownImg/image-18.png)

### Editor Settings
![alt text](Resources/MarkdownImg/image-17.png)
可以选上默认的Experience，保险点（前提要在AssetManager添加）

### Collision
![alt text](Resources/MarkdownImg/image-22.png)
参考代码[SkyraCollisionChannels.h](./SkyraFramework/Source/SkyraGame/Public/Physics/SkyraCollisionChannels.h)进行配置, 注意要按照顺序
![alt text](Resources/MarkdownImg/image-23.png)


```cpp
#pragma once
/**
 * when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list
 **/

// Trace against Actors/Components which provide interactions.
#define Skyra_TraceChannel_Interaction					ECC_GameTraceChannel1

// Trace used by weapons, will hit physics assets instead of capsules
#define Skyra_TraceChannel_Weapon						ECC_GameTraceChannel2

// Trace used by by weapons, will hit pawn capsules instead of physics assets
#define Skyra_TraceChannel_Weapon_Capsule				ECC_GameTraceChannel3

// Trace used by by weapons, will trace through multiple pawns rather than stopping on the first hit
#define Skyra_TraceChannel_Weapon_Multi					ECC_GameTraceChannel4

// Allocated to aim assist by the ShooterCore game feature
//#define Skyra_TraceChannel_AimAssist					ECC_GameTraceChannel5
// ECC_GameTraceChannel5
```
另外可以参考lyra的碰撞
![alt text](Resources/MarkdownImg/image-24.png)

## 配置文件

### DefaultEngine
```ini
[/Script/SignificanceManager.SignificanceManager]
SignificanceManagerClassName=/Script/SkyraGame.SkyraSignificanceManager

[/Script/Hotfix.OnlineHotfixManager]
HotfixManagerClassName=/Script/SkyraGame.SkyraHotfixManager
```

### DefaultGame
```ini

[/Script/SkyraGame.SkyraUIManagerSubsystem]
DefaultUIPolicyClass=/SkyraFramework/Blueprints/UI/B_SkyraUIPolicy.B_SkyraUIPolicy_C

[/Script/CommonInput.CommonInputSettings]
InputData=/SkyraFramework/Blueprints/UI/B_CommonInputData.B_CommonInputData_C

[/Script/CommonLoadingScreen.CommonLoadingScreenSettings]
LoadingScreenWidget=/SkyraFramework/Blueprints/UI/Foundation/LoadingScreen/W_LoadingScreen_Host.W_LoadingScreen_Host_C

[/Script/GameFeatures.GameFeaturesSubsystemSettings]
GameFeaturesManagerClassName=/Script/SkyraGame.SkyraGameFeaturePolicy

[/Script/SkyraGame.SkyraContextEffectsSettings]
SurfaceTypeToContextMap=((SurfaceType_Default, ()))


[/Script/SkyraGame.SkyraPlayerController] ;注意这里要设置否则鼠标上下会反转，或者在设置关闭bEnableLegacyInputScales
InputYawScale=1.0
InputPitchScale=1.0
InputRollScale=1.0
ForceFeedbackScale=1.0

[/Script/GameplayAbilities.AbilitySystemGlobals]
AbilitySystemGlobalsClassName=/Script/SkyraGame.SkyraAbilitySystemGlobals
GlobalGameplayCueManagerClass=/Script/SkyraGame.SkyraGameplayCueManager
PredictTargetGameplayEffects=false
bUseDebugTargetFromHud=true
ActivateFailIsDeadName=Ability.ActivateFail.IsDead
ActivateFailCooldownName=Ability.ActivateFail.Cooldown
ActivateFailCostName=Ability.ActivateFail.Cost
ActivateFailTagsBlockedName=Ability.ActivateFail.TagsBlocked
ActivateFailTagsMissingName=Ability.ActivateFail.TagsMissing
ActivateFailNetworkingName=Ability.ActivateFail.Networking
+GameplayCueNotifyPaths=/SkyraFramework/Blueprints/GameplayCueNotifies
+GameplayCueNotifyPaths=/Game
;最好进行配置，可以优化性能
; +GameplayCueNotifyPaths=/Game/GameplayCueNotifies
; +GameplayCueNotifyPaths=/Game/GameplayCues

[/Script/SkyraGame.SkyraAssetManager]
SkyraGameDataPath=/SkyraFramework/DefaultGameData.DefaultGameData
DefaultPawnData=/SkyraFramework/DefaultPawnData_EmptyPawn.DefaultPawnData_EmptyPawn
```

如果需要解决
**You may only reference assets from EngineContent...**
需要在GameFeature插件添加对Skyra的依赖
```json
	"Plugins": [
		{
			"Name": "SkyraFramework",
			"Enabled": true
		}
	]
```

注意，如果项目需要依赖GameFeature并采用下面的设置方法，则会导致GameFeature的加载特性失效：
![alt text](Resources/MarkdownImg/image-20.png)