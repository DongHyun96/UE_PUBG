// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerController.h"
#include "Framework/Application/NavigationConfig.h"
void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
		Navigation->bKeyNavigation = false;
		Navigation->bTabNavigation = false;
		Navigation->bAnalogNavigation = false;
		FSlateApplication::Get().SetNavigationConfig(Navigation);
	}
}
