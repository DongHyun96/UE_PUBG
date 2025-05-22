// Fill out your copyright notice in the Description page of Project Settings.


#include "C_GameInstance.h"
#include "LoadingScreenModule.h"
#include "Utility/C_Util.h"

void UC_GameInstance::Init()
{
	Super::Init();
	
	// This ensures our function is called right before a map begins loading
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UC_GameInstance::BeginLoadingScreen);

	// This ensures our function is called after a map has finished loading
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UC_GameInstance::EndLoadingScreen);


}

void UC_GameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::BeginLoadingScreen: %s"), *InMapName);

	// Try to get the loading screen module
	FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
	if (LoadingScreenModule)
	{
		// Module found - Start the loading screen
		LoadingScreenModule->StartLoadingScreen(InMapName);
	}
	else
	{
		// Module not found
		UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::BeginLoadingScreen: LoadingScreenModule not found"));
	}
	
	
}

void UC_GameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::EndLoadingScreen: %s"), *InLoadedWorld->GetName());
}
