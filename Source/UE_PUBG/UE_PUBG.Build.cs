// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE_PUBG : ModuleRules
{
	public UE_PUBG(ReadOnlyTargetRules Target) : base(Target)
	{
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "GameplayCameras" });

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "GameplayCameras",
            "Niagara",
            "Slate",
            "SlateCore",
            "Json",
            "JsonUtilities",
            "MotionWarping",
            "AIModule",
            "Water",
            "NavigationSystem",
            "PhysicsCore",
            "LoadingScreenModule"
            // "MoviePlayer"
        });

        /*
         PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AnimGraphRuntime",
            "AIModule",
            "GameplayTasks",
            "AnimationCore", // Add this for advanced animation functionality
            "AnimGraph", // Add this for animation graph support
        });
        */

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add(ModuleDirectory);

        bUseUnity = false;
	}
}
