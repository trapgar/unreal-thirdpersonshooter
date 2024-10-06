// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdPersonShooter : ModuleRules
{
	public ThirdPersonShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"ThirdPersonShooter"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"PhysicsCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// "GameplayMessageNodes",
			"ModularGameplay",
			"ModularGameplayActors",
			"GameFeatures",
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"InputCore",
			"Slate",
			"SlateCore",
			"DeveloperSettings",
			"UMG",
			"CommonUI",
			"CommonInput",
			// "GameSettings",
			"CommonGame",
			"CommonUser",
			"GameplayMessageRuntime",
			"AudioMixer",
			"UIExtension",
			"AudioModulation",
			"DTLSHandlerComponent",
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		SetupIrisSupport(Target);
	}
}
