#include "LoadingScreenModule.h"

#include "MoviePlayer.h"
#include "PipelineCacheUtilities.h"
#include "LoadingScreenModule/C_LoadingScreen.h"

void FLoadingScreenModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule"));

	// Load the background texture, which will be used for the loading screen
	// Asset path is relative to the Content folder
	// It should be a 2D texture located in /Content/.. and named PUBG_Loading.uasset
	BackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Project_PUBG/Hyunho/LobbyScene/2DImages/PUBG_Loading.PUBG_Loading'"));

	if (!BackgroundTexture) UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule : BackgroundTexture not found"));
}

bool FLoadingScreenModule::IsGameModule() const
{
	// Identifies this as a runtime module vs editor-only
	return true;
}

void FLoadingScreenModule::StartLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartLoadingScreen"));

	// Create a struct to hold all our loading screen settings
	FLoadingScreenAttributes LoadingScreen{};

	// Don't automatically remove the loading screen when loading completes
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;

	// Set minimum time the loading screen will be displayed (in seconds)
	// This prevents "flash" loading screens for quick loads
	LoadingScreen.MinimumLoadingScreenDisplayTime = 2.f;

	// Set the widget to use for the loading screen
	LoadingScreen.WidgetLoadingScreen = SNew(SC_LoadingScreen).BackgroundTexture(BackgroundTexture);

	// Tell MoviePlayer to display our loading screen with these settings
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	
}

// Registers this module with Unreal Engine's module system
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);

