// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulsGame : ModuleRules
{
	public SoulsGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","HairStrandsCore", "Niagara", "UMG", "NavigationSystem", "AIModule" });
	}
}
