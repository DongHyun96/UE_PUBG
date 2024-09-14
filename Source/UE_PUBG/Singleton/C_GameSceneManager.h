// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "C_GameSceneManager.generated.h"

#define GAMESCENE_MANAGER GetWorld()->GetSubsystem<UC_GameSceneManager>()

/**
 * 
 */

/// <summary>
/// GameScene에서 사용할 Singleton class
/// </summary>
UCLASS()
class UE_PUBG_API UC_GameSceneManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void OnWorldBeginPlay(UWorld& InWorld) override;
	
public: // Getters and setters

	class AC_Player* GetPlayer() const { return Player; }

	void SetMagneticFieldManager(class AC_MagneticFieldManager* InMagneticFieldManager) { MagneticFieldManager = InMagneticFieldManager; }
	class AC_MagneticFieldManager* GetMagneticFieldManager() const { return MagneticFieldManager; }

private:

	class AC_Player* Player{};
	class AC_MagneticFieldManager* MagneticFieldManager{};

};
