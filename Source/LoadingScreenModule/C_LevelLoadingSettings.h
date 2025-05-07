// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "C_LevelLoadingSettings.generated.h"

// Loading Screen settings for the game project
UCLASS(Config = "Game", DefaultConfig, meta = (DisplayName = "Loading Screen"))
class LOADINGSCREENMODULE_API UC_LevelLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// Array of soft references to map assets that should display loading screens
	// Config - Property will be saved in config file
	// AllowClasses = "World" - Only World assets can be selected
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowClasses = "World"))
	TArray<FSoftObjectPath> MapsWithLoadingScreens{};

	// Soft reference to the background image asset for the loading screen
	// Config - Property will be saved in config file
	// AllowClasses = "World" - Only Texture assets can be selected
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowClasses = "Texture"))
	FSoftObjectPath BackgroundImage{};

	// Minimum duration (in seconds) that the loading screen will be displayed
	// Default value of 2.0 seconds
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen")
	float MinimumLoadingScreenDisplayTime = 2.f;
	
};
