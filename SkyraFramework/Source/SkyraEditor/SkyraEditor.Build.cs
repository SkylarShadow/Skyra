// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkyraEditor : ModuleRules
{
    public SkyraEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Public",
				ModuleDirectory + "/Public/Commandlets",
				ModuleDirectory + "/Public/Utilities",
				ModuleDirectory + "/Public/Utilities/TexImport",
				ModuleDirectory + "/Public/Validation",
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Public",
				ModuleDirectory + "/Public/Commandlets",
				ModuleDirectory + "/Public/Utilities",
				ModuleDirectory + "/Public/Utilities/TexImport",
				ModuleDirectory + "/Public/Validation",
			}
		);

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "UnrealEd",
				"PhysicsCore",
				"GameplayTagsEditor",
				"GameplayTasksEditor",
				"GameplayAbilities",
				"GameplayAbilitiesEditor",
				"StudioTelemetry",
				"SkyraGame",
				
				"InterchangeCore",
				"InterchangeEngine",
				"InterchangeFactoryNodes",
				"InterchangeNodes",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
				"AssetTools",
				"EditorScriptingUtilities",
				"Blutility",
				"InputCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"DataValidation",
				"MessageLog",
				"Projects",
				"DeveloperToolSettings",
				"CollectionManager",
				"SourceControl",
				"Chaos",
				
				"InterchangeCommonParser",
			}
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);
		
		
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"TextureUtilitiesCommon",
				}
			);
		}
		
		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
    }
}
