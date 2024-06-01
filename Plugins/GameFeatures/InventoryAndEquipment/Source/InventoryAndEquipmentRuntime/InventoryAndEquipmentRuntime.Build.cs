// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventoryAndEquipmentRuntime : ModuleRules
{
	public InventoryAndEquipmentRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"InventoryAndEquipmentRuntime/Public",
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				"InventoryAndEquipmentRuntime/Private",
				// "ThirdPersonShooter",
				// ... add public include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"GameplayTags",
				"GameplayAbilities",
				"GameplayMessageRuntime",
				"ModularGameplay",
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
