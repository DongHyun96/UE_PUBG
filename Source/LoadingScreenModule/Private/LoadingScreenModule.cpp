#include "LoadingScreenModule.h"

#include "MoviePlayer.h"
#include "LoadingScreenModule/C_LevelLoadingSettings.h"
#include "LoadingScreenModule/C_LoadingScreen.h"

void FLoadingScreenModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule"));

	// Try to get the loading screen settings
	if (UC_LevelLoadingSettings* Settings = GetMutableDefault<UC_LevelLoadingSettings>())
	{
		// Get the background image path from settings
		const FSoftObjectPath& BGPath = Settings->BackgroundImage;
		// If a background image is specified
		if (!BGPath.IsNull())
		{
			// Load the texture synchronously and store it
			// StaticLoadObject parameters:
			// 1. Class to load (UTexture2D)
			// 2. Outer object (nullptr = default)
			// 3. Path to the asset
			// Editor 켜지다가 여기서 터져서 꺼진다면, 주석처리하고 켠 다음, Project Settings에서 Loading Screen setting 확인할 것
			BackgroundTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *BGPath.ToString()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule : No BGPath in LevelLoadingSettings!"));
		}
	}
}

bool FLoadingScreenModule::IsGameModule() const
{
	// Identifies this as a runtime module vs editor-only
	return true;
}

void FLoadingScreenModule::StartLoadingScreen(const FString& MapName)
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartLoadingScreen"));

	// Get the loading screen settings
	UC_LevelLoadingSettings* Settings = GetMutableDefault<UC_LevelLoadingSettings>();

	// Check if this map should show a loading screen
	bool bShouldShowLoadingScreen{};

	// Loop through all maps that have loading screens
	for (const FSoftObjectPath& MapPath : Settings->MapsWithLoadingScreens)
	{
		// Check if the current map name matches any in our list
		// Uses Contains instead of exact match to be more flexible with path formats
		if (MapPath.GetAssetPathString().Contains(MapName))
		{
			bShouldShowLoadingScreen = true;
			break;
		}
	}

	// If this map shouldn't show a loading screen, exit early
	if (!bShouldShowLoadingScreen) return;
	
	
	// Create a struct to hold all our loading screen settings
	FLoadingScreenAttributes LoadingScreen{};

	// Don't automatically remove the loading screen when loading completes
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;

	// Set minimum time the loading screen will be displayed (in seconds)
	// This prevents "flash" loading screens for quick loads
	LoadingScreen.MinimumLoadingScreenDisplayTime = Settings->MinimumLoadingScreenDisplayTime;

	// Set the widget to use for the loading screen
	LoadingScreen.WidgetLoadingScreen = SNew(SC_LoadingScreen).BackgroundTexture(BackgroundTexture);

	// Tell MoviePlayer to display our loading screen with these settings
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

// Registers this module with Unreal Engine's module system
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);

