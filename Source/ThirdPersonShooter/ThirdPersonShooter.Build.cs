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
			// "CoreOnline",
			"CoreUObject",
			// "ApplicationCore",
			"Engine",
			"PhysicsCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// "AIModule",
			"ModularGameplay",
			"ModularGameplayActors",
			// "DataRegistry",
			// "ReplicationGraph",
			"GameFeatures",
			// "SignificanceManager",
			// "Hotfix",
			// "CommonLoadingScreen",
			"Niagara",
			// "AsyncMixin",
			// "ControlFlows",
			// "PropertyPath"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"InputCore",
			"Slate",
			"SlateCore",
			// "RenderCore",
			"DeveloperSettings",
			// "EnhancedInput",
			"NetCore",
			// "RHI",
			// "Projects",
			// "Gauntlet",
			"UMG",
			"CommonUI",
			"CommonInput",
			// "GameSettings",
			"CommonGame",
			"CommonUser",
			// "GameSubtitles",
			"GameplayMessageRuntime",
			"AudioMixer",
			// "NetworkReplayStreaming",
			"UIExtension",
			// "ClientPilot",
			"AudioModulation",
			// "EngineSettings",
			"DTLSHandlerComponent",
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		SetupIrisSupport(Target);
	}
}
