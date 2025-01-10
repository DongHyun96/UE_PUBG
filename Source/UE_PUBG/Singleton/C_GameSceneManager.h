// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "C_GameSceneManager.generated.h"

#define GAMESCENE_MANAGER GetWorld()->GetSubsystem<UC_GameSceneManager>()

/// <summary>
/// GameScene 현 HUD Mode
/// </summary>
enum class EHUDMode : uint8
{
	IDLE,
	INVEN,
	MAINMAP,
	MAX
};


/// <summary>
/// GameScene에서 사용할 Singleton class
/// </summary>
UCLASS()
class UE_PUBG_API UC_GameSceneManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void OnWorldBeginPlay(UWorld& InWorld) override;

	//void Deinitialize() override;

private:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

private:

	//void OnWorldEndPlay(UWorld* InWorld);
	
public: // Getters and setters

	class AC_Player* GetPlayer() const { return Player; }

	class AC_MagneticFieldManager* GetMagneticFieldManager() const { return MagneticFieldManager; }
	class AC_AirplaneManager* GetAirplaneManager() const { return AirplaneManager; }

	TArray<class AC_BasicCharacter*>& GetAllCharacters() { return AllCharacters; }

	/// <summary>
	/// GameScene에서 GC로부터 보호된 Object들 추가 -> GameScene 끝날 때 일괄 삭제처리 예정
	/// </summary>
	void AddNonGCObject(UObject* Object) { NonGCObjects.Add(Object); }

	EHUDMode GetCurrentHUDMode() const { return CurrentHUDMode; }
	void SetCurrentHUDMode(EHUDMode InHUDMode);

private:

	class AC_Player*				Player{};
	class AC_MagneticFieldManager*	MagneticFieldManager{};
	class AC_AirplaneManager*		AirplaneManager{};

private:

	// 인게임 모든 캐릭터들(Player + Enemies)
	TArray<class AC_BasicCharacter*> AllCharacters{};

private:

	// InGameScene내에서 Unreal GC로부터 보호된 객체들 -> GameScene이 끝날 때 해제할 예정
	TSet<UObject*> NonGCObjects{};

private:

	// Main Player의 Widget들, 현재 어떤 WidgetMode인지에 따른 Visibility setting 처리
	EHUDMode CurrentHUDMode{};

	TMap<EHUDMode, class UUserWidget*> HUDWidgets{};
	UUserWidget*					   MiniMapWidget{};

};
