
# 部署Skyra

## 编辑器内设置：

### Project Settings

#### Maps & Modes（可以根据项目实际需要）
![alt text](./Resources/Doc5_8/image.png)


#### Gameplaytags
![alt text](./Resources/Doc5_8/image-1.png)
可以 GameFeatures的Tags 或者例如StandardCharacter里的tag datatable

#### AssetManager

##### PrimaryAssetTypesToScan
- Map(World)
- SkyraGameData
- PrimaryAssetLabel
- GameFeatureData
- SkyraExperienceDefinition
- SkyraUserFacingExperienceDefinition
- SkyraLobbyBackground
- SkyraExperienceActionSet

例如World还有DataAsset之类无蓝图类的,Has Blueprint Classes 填false, 
PrimaryAssetLabel Cook Rule = unknown

#### CommonGame
![alt text](./Resources/Doc5_8/image-18.png)

#### Common Input Settings
![alt text](./Resources/Doc5_8/image-2.png)

#### Common Loading Screen 根据项目实际需求填写
![alt text](./Resources/Doc5_8/image-3.png)

#### Game Features
![alt text](./Resources/Doc5_8/image-4.png)

#### Gameplay Abilities Settings 根据项目实际需求填写 特别是GameCuePath
![alt text](./Resources/Doc5_8/image-5.png)

####
![alt text](./Resources/Doc5_8/image-17.png)

#### SkyraAudioSettings
![alt text](./Resources/Doc5_8/image-6.png)

#### SkyraContextEffects 根据项目实际需求填写
参考lyra
![alt text](./Resources/Doc5_8/image-7.png)

#### Audio
![alt text](./Resources/Doc5_8/image-8.png)

#### Collision 
这个不至于不会吧，参考lyra
![alt text](./Resources/Doc5_8/image-9.png)
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

#### Enhanced Input
![alt text](./Resources/Doc5_8/image-10.png)

#### GC
Lyra关了这个，我要研究下是为什么
![alt text](./Resources/Doc5_8/image-11.png)

#### General Settings
![alt text](./Resources/Doc5_8/image-12.png)

#### Input
![alt text](./Resources/Doc5_8/image-13.png)
可以去掉移动端虚拟按键
bEnableLegacyInputScales =false可以处理鼠标移动上下翻转
#### Interchange
![alt text](./Resources/Doc5_8/image-14.png)

#### Physics
参考lyra,也有一些其他好玩的可以调下
![alt text](./Resources/Doc5_8/image-15.png)

### Editor Settings
![alt text](./Resources/Doc5_8/image-16.png)
也要在AssetManager添加


## 配置文件

### DefaultEngine
```ini
[/Script/SignificanceManager.SignificanceManager]
SignificanceManagerClassName=/Script/SkyraGame.SkyraSignificanceManager

[/Script/Hotfix.OnlineHotfixManager]
HotfixManagerClassName=/Script/SkyraGame.SkyraHotfixManager
```

[/Script/SkyraGame.SkyraPlayerController] ;注意这里要设置否则鼠标上下会反转，或者在设置关闭bEnableLegacyInputScales
InputYawScale=1.0
InputPitchScale=1.0
InputRollScale=1.0
ForceFeedbackScale=1.0


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
![alt text](Resources/MarkdownImg/./Resources/Doc5_8/image-20.png)