// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkyraGame : ModuleRules
{
	public SkyraGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"$(ModuleDir)/Public",
                "$(ModuleDir)/Public/AbilitySystem",
                "$(ModuleDir)/Public/AbilitySystem/Abilities",
                "$(ModuleDir)/Public/AbilitySystem/Attributes",
                "$(ModuleDir)/Public/AbilitySystem/Executions",
                "$(ModuleDir)/Public/AbilitySystem/Phases",
                "$(ModuleDir)/Public/Animation",
                "$(ModuleDir)/Public/Animation/AnimNotify",
                "$(ModuleDir)/Public/Animation/AnimNotifyState",
                "$(ModuleDir)/Public/Archive",
                "$(ModuleDir)/Public/Audio",
                "$(ModuleDir)/Public/Camera",
                "$(ModuleDir)/Public/Character",   
                "$(ModuleDir)/Public/Cosmetics",
                "$(ModuleDir)/Public/DataTable",
                "$(ModuleDir)/Public/Development",
                "$(ModuleDir)/Public/Equipment",
                "$(ModuleDir)/Public/Feedback",
                "$(ModuleDir)/Public/Feedback/ContextEffects",
                "$(ModuleDir)/Public/Feedback/NumberPops",
                "$(ModuleDir)/Public/GameFeatures",
                "$(ModuleDir)/Public/GameModes",
                "$(ModuleDir)/Public/Hotfix",
                "$(ModuleDir)/Public/Input",
                "$(ModuleDir)/Public/Interaction",
                "$(ModuleDir)/Public/Interaction/Abilities",
                "$(ModuleDir)/Public/Interaction/Tasks",
                "$(ModuleDir)/Public/Inventory",
                "$(ModuleDir)/Public/Inventory/Fragments",
                "$(ModuleDir)/Public/Messages",
                "$(ModuleDir)/Public/Messages/MessageProcessors",
                "$(ModuleDir)/Public/Performance",
                "$(ModuleDir)/Public/Physics",
                "$(ModuleDir)/Public/Player",
                "$(ModuleDir)/Public/Replays",
                "$(ModuleDir)/Public/Settings",
                "$(ModuleDir)/Public/Settings/CustomSettings",
                "$(ModuleDir)/Public/Settings/Screens",
                "$(ModuleDir)/Public/Settings/Widgets",
                "$(ModuleDir)/Public/System",
                "$(ModuleDir)/Public/Teams",
                "$(ModuleDir)/Public/UI",
                "$(ModuleDir)/Public/UI/Basic",
                "$(ModuleDir)/Public/UI/Common",
                "$(ModuleDir)/Public/UI/Foundation",
                "$(ModuleDir)/Public/UI/Frontend",
                "$(ModuleDir)/Public/UI/IndicatorSystem",
                "$(ModuleDir)/Public/UI/PerformanceStats",
                "$(ModuleDir)/Public/UI/Subsystem",
                "$(ModuleDir)/Public/UI/Weapons",
                "$(ModuleDir)/Public/Weapons",
				
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				
				"$(ModuleDir)/Public",
                "$(ModuleDir)/Public/AbilitySystem",
                "$(ModuleDir)/Public/AbilitySystem/Abilities",
                "$(ModuleDir)/Public/AbilitySystem/Attributes",
                "$(ModuleDir)/Public/AbilitySystem/Executions",
                "$(ModuleDir)/Public/AbilitySystem/Phases",
                "$(ModuleDir)/Public/Animation",
                "$(ModuleDir)/Public/Animation/AnimNotify",
                "$(ModuleDir)/Public/Animation/AnimNotifyState",
                "$(ModuleDir)/Public/Archive",
                "$(ModuleDir)/Public/Audio",
                "$(ModuleDir)/Public/Camera",
                "$(ModuleDir)/Public/Character",   
                "$(ModuleDir)/Public/Cosmetics",
                "$(ModuleDir)/Public/DataTable",
                "$(ModuleDir)/Public/Development",
                "$(ModuleDir)/Public/Equipment",
                "$(ModuleDir)/Public/Feedback",
                "$(ModuleDir)/Public/Feedback/ContextEffects",
                "$(ModuleDir)/Public/Feedback/NumberPops",
                "$(ModuleDir)/Public/GameFeatures",
                "$(ModuleDir)/Public/GameModes",
                "$(ModuleDir)/Public/Input",
                "$(ModuleDir)/Public/Interaction",
                "$(ModuleDir)/Public/Interaction/Abilities",
                "$(ModuleDir)/Public/Interaction/Tasks",
                "$(ModuleDir)/Public/Inventory",
                "$(ModuleDir)/Public/Inventory/Fragments",
                "$(ModuleDir)/Public/Messages",
                "$(ModuleDir)/Public/Messages/MessageProcessors",
                "$(ModuleDir)/Public/Performance",
                "$(ModuleDir)/Public/Physics",
                "$(ModuleDir)/Public/Player",
                "$(ModuleDir)/Public/Settings",
                "$(ModuleDir)/Public/Settings/CustomSettings",
                "$(ModuleDir)/Public/Settings/Screens",
                "$(ModuleDir)/Public/Settings/Widgets",
                "$(ModuleDir)/Public/System",
                "$(ModuleDir)/Public/Teams",
                "$(ModuleDir)/Public/UI",
                "$(ModuleDir)/Public/UI/Basic",
                "$(ModuleDir)/Public/UI/Common",
                "$(ModuleDir)/Public/UI/Foundation",
                "$(ModuleDir)/Public/UI/Frontend",
                "$(ModuleDir)/Public/UI/IndicatorSystem",
                "$(ModuleDir)/Public/UI/PerformanceStats",
                "$(ModuleDir)/Public/UI/Subsystem",
                "$(ModuleDir)/Public/UI/Weapons",
                "$(ModuleDir)/Public/Weapons",
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
				"DeveloperSettings",
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
