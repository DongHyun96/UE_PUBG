using UnrealBuildTool;

// Critical: Module must have "PreLoadingScreen" load timing in .uproject
// Otherwise module won't initialize early enough to handle loading screens
public class LoadingScreenModule : ModuleRules
{
	public LoadingScreenModule(ReadOnlyTargetRules Target) : base(Target)
	{
		// Specify the primary header file for precompiled headers
		PrivatePCHHeaderFile = "Public/LoadingScreenModule.h";
		
		// Use shared precompiled headers for better compile times
		PCHUsage = PCHUsageMode.UseSharedPCHs;
		
		// Add core Unreal Engine modules as private dependencies
		PrivateDependencyModuleNames.AddRange
			(new string[]
				{
					"Core",			// Core UE functionality
					"CoreUObject",	// Base UObject system
					"MoviePlayer",	// Movie player for loading screens
					"Engine",		// Main UE Engine module for UTexture2D
					"Slate",		// UI framework
					"SlateCore",		// Core Slate functionality
					"DeveloperSettings"
				}
			);
		
		//PublicIncludePaths.Add("Public/LoadingScreenModule");
		//PrivateIncludePaths.Add("Private/LoadingScreenModule");
	}
}