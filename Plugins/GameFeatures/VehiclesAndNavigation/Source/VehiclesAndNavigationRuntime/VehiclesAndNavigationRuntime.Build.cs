// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VehiclesAndNavigationRuntime : ModuleRules
{
	public VehiclesAndNavigationRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"VehiclesAndNavigationRuntime/Public"
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				"VehiclesAndNavigationRuntime/Private"
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// "CoreUObject",
				// "Engine",
				// "InputCore",
				// "EnhancedInput",
				"GameplayTags",
				// "GameplayTasks",
				"GameplayAbilities",
				// "GameplayMessageRuntime",
				// "GameplayMessageNodes",
				"ModularGameplay",
				// "GameFeatures",
				"ThirdPersonShooter",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
