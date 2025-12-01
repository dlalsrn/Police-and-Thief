// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PoliceAndThief : ModuleRules
{
	public PoliceAndThief(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"PoliceAndThief",
			"PoliceAndThief/Variant_Platforming",
			"PoliceAndThief/Variant_Platforming/Animation",
			"PoliceAndThief/Variant_Combat",
			"PoliceAndThief/Variant_Combat/AI",
			"PoliceAndThief/Variant_Combat/Animation",
			"PoliceAndThief/Variant_Combat/Gameplay",
			"PoliceAndThief/Variant_Combat/Interfaces",
			"PoliceAndThief/Variant_Combat/UI",
			"PoliceAndThief/Variant_SideScrolling",
			"PoliceAndThief/Variant_SideScrolling/AI",
			"PoliceAndThief/Variant_SideScrolling/Gameplay",
			"PoliceAndThief/Variant_SideScrolling/Interfaces",
			"PoliceAndThief/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
