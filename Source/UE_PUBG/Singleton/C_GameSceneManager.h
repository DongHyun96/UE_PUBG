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

private:
	/// <summary>
	/// Level에 배치된 모든 Actor들의 BeginPlay보다 먼저 호출됨
	/// </summary>
	/// <param name="InWorld"></param>
	void OnWorldBeginPlay(UWorld& InWorld) override;

	//void Deinitialize() override;

private:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

private:

	//void OnWorldEndPlay(UWorld* InWorld);
	
public: // Getters and setters

	class AC_Player* GetPlayer() const { return Player; }
	
	TArray<class AC_Enemy*>& GetEnemies() { return Enemies; }

	class AC_MagneticFieldManager* GetMagneticFieldManager() const { return MagneticFieldManager; }
	class AC_AirplaneManager* GetAirplaneManager() const { return AirplaneManager; }

	TArray<class AC_BasicCharacter*>& GetAllCharacters() { return AllCharacters; }
	TArray<class AActor*>& GetAllCharacterActors() { return AllCharacterActors; }

	/// <summary>
	/// GameScene에서 GC로부터 보호된 Object들 추가 -> GameScene 끝날 때 일괄 삭제처리 예정
	/// </summary>
	void AddGCProtectedObject(UObject* Object) { GCProtectedObjects.Add(Object); }

	EHUDMode GetCurrentHUDMode() const { return CurrentHUDMode; }
	void SetCurrentHUDMode(EHUDMode InHUDMode);

	float GetCellWorldSize() const { return CELL_WORLDSIZE; }

public:

	/// <summary>
	/// 100 x 100m 크기 Tile coordinate의 중앙점을 World 좌표로 환산한 값 구하기, 단, Z는 항상 0.f로 고정
	/// </summary>
	/// <param name="TileCoordinate"></param>
	/// <returns></returns>
	FVector ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const;

private:

	class AC_Player*				Player{};
	class AC_MagneticFieldManager*	MagneticFieldManager{};
	class AC_AirplaneManager*		AirplaneManager{};

private:

	// 인게임 모든 캐릭터들(Player + Enemies)
	TArray<class AC_BasicCharacter*> AllCharacters{};
	TArray<AActor*> AllCharacterActors{};

private:

	class TArray<AC_Enemy*> Enemies{};

private:

	// InGameScene내에서 Unreal GC로부터 보호된 객체들 -> GameScene이 끝날 때 해제할 예정
	TSet<UObject*> GCProtectedObjects{};

private:

	// Main Player의 Widget들, 현재 어떤 WidgetMode인지에 따른 Visibility setting 처리
	EHUDMode CurrentHUDMode{};

	TMap<EHUDMode, class UUserWidget*> HUDWidgets{};
	UUserWidget*					   MiniMapWidget{};

private:
	
	const float CELL_WORLDSIZE = 10000.f;

};


