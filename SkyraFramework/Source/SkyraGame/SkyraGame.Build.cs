// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkyraGame : ModuleRules
{
	public SkyraGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Public",
				ModuleDirectory + "/Public/AbilitySystem",
				ModuleDirectory + "/Public/AbilitySystem/Abilities",
				ModuleDirectory + "/Public/AbilitySystem/Attributes",
				ModuleDirectory + "/Public/AbilitySystem/Executions",
				ModuleDirectory + "/Public/AbilitySystem/Phases",
				ModuleDirectory + "/Public/Animation",
				ModuleDirectory + "/Public/Audio",
				ModuleDirectory + "/Public/Camera",
				ModuleDirectory + "/Public/Character",
				ModuleDirectory + "/Public/Cosmetics",
				ModuleDirectory + "/Public/Development",
				ModuleDirectory + "/Public/Equipment",
				ModuleDirectory + "/Public/Equipment/SubEquipment",
				ModuleDirectory + "/Public/Feedback",
				ModuleDirectory + "/Public/Feedback/ContextEffects",
				ModuleDirectory + "/Public/Feedback/NumberPops",
				ModuleDirectory + "/Public/GameFeatures",
				ModuleDirectory + "/Public/GameModes",
				ModuleDirectory + "/Public/Hotfix",
				ModuleDirectory + "/Public/Input",
				ModuleDirectory + "/Public/Interaction",
				ModuleDirectory + "/Public/Interaction/Abilities",
				ModuleDirectory + "/Public/Interaction/Tasks",
				ModuleDirectory + "/Public/Inventory",
				ModuleDirectory + "/Public/Messages",
				ModuleDirectory + "/Public/Performance",
				ModuleDirectory + "/Public/Physics",
				ModuleDirectory + "/Public/Player",
				ModuleDirectory + "/Public/Replays",
				ModuleDirectory + "/Public/Settings",
				ModuleDirectory + "/Public/Settings/CustomSettings",
				ModuleDirectory + "/Public/Settings/Screens",
				ModuleDirectory + "/Public/Settings/Widgets",
				ModuleDirectory + "/Public/System",
				ModuleDirectory + "/Public/Teams",
				ModuleDirectory + "/Public/Tests",
				ModuleDirectory + "/Public/UI",
				ModuleDirectory + "/Public/UI/Basic",
				ModuleDirectory + "/Public/UI/Common",
				ModuleDirectory + "/Public/UI/Foundation",
				ModuleDirectory + "/Public/UI/Frontend",
				ModuleDirectory + "/Public/UI/IndicatorSystem",
				ModuleDirectory + "/Public/UI/PerformanceStats",
				ModuleDirectory + "/Public/UI/Subsystem",
				ModuleDirectory + "/Public/Weapons",
				
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				
				ModuleDirectory + "/Public",
				ModuleDirectory + "/Public/AbilitySystem",
				ModuleDirectory + "/Public/AbilitySystem/Abilities",
				ModuleDirectory + "/Public/AbilitySystem/Attributes",
				ModuleDirectory + "/Public/AbilitySystem/Executions",
				ModuleDirectory + "/Public/AbilitySystem/Phases",
				ModuleDirectory + "/Public/Animation",
				ModuleDirectory + "/Public/Audio",
				ModuleDirectory + "/Public/Camera",
				ModuleDirectory + "/Public/Character",
				ModuleDirectory + "/Public/Cosmetics",
				ModuleDirectory + "/Public/Development",
				ModuleDirectory + "/Public/Equipment",
				ModuleDirectory + "/Public/Equipment/SubEquipment",
				ModuleDirectory + "/Public/Feedback",
				ModuleDirectory + "/Public/Feedback/ContextEffects",
				ModuleDirectory + "/Public/Feedback/NumberPops",
				ModuleDirectory + "/Public/GameFeatures",
				ModuleDirectory + "/Public/GameModes",
				ModuleDirectory + "/Public/Hotfix",
				ModuleDirectory + "/Public/Input",
				ModuleDirectory + "/Public/Interaction",
				ModuleDirectory + "/Public/Interaction/Abilities",
				ModuleDirectory + "/Public/Interaction/Tasks",
				ModuleDirectory + "/Public/Inventory",
				ModuleDirectory + "/Public/Messages",
				ModuleDirectory + "/Public/Performance",
				ModuleDirectory + "/Public/Physics",
				ModuleDirectory + "/Public/Player",
				ModuleDirectory + "/Public/Replays",
				ModuleDirectory + "/Public/Settings",
				ModuleDirectory + "/Public/Settings/CustomSettings",
				ModuleDirectory + "/Public/Settings/Screens",
				ModuleDirectory + "/Public/Settings/Widgets",
				ModuleDirectory + "/Public/System",
				ModuleDirectory + "/Public/Teams",
				ModuleDirectory + "/Public/Tests",
				ModuleDirectory + "/Public/UI",
				ModuleDirectory + "/Public/UI/Basic",
				ModuleDirectory + "/Public/UI/Common",
				ModuleDirectory + "/Public/UI/Foundation",
				ModuleDirectory + "/Public/UI/Frontend",
				ModuleDirectory + "/Public/UI/IndicatorSystem",
				ModuleDirectory + "/Public/UI/PerformanceStats",
				ModuleDirectory + "/Public/UI/Subsystem",
				ModuleDirectory + "/Public/Weapons",
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreOnline",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"PhysicsCore",
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				"AIModule",
				"ModularGameplay",
				"ModularGameplayActors",
				"DataRegistry",
				"DeveloperSettings",
				"ReplicationGraph",
				"GameFeatures",
				"SignificanceManager",
				"Hotfix",
				"CommonGame",
				"CommonLoadingScreen",
				"Niagara",
				"AsyncMixin",
				"ControlFlows",
				"PropertyPath"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
				"RenderCore",
				"EnhancedInput",
				"NetCore",
				"RHI",
				"Projects",
				"Gauntlet",
				"UMG",
				"CommonUI",
				"CommonInput",
				"GameSettings",
				
				"CommonUser",
				"GameSubtitles",
				"GameplayMessageRuntime",
				"AudioMixer",
				"NetworkReplayStreaming",
				"UIExtension",
				"ClientPilot",
				"AudioModulation",
				"EngineSettings",
				"DTLSHandlerComponent", 
				"AnimGraphRuntime",
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);

		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
		

		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
