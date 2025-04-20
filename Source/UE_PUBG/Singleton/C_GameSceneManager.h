// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
//#include "Item/ItemManager/C_ItemManager.h"
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
	UC_GameSceneManager();

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

public:
	UFUNCTION(BlueprintCallable)
	class AC_LootCrate* SpawnLootCrateAt(FVector SpawnLocation, AC_BasicCharacter* DeadCharacter);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AC_LootCrate> LootCrateClass;

private:

	//void OnWorldEndPlay(UWorld* InWorld);

public:

	
	
public: // Getters and setters

	static UC_GameSceneManager* GetInstance(UWorld* World) { return World->GetSubsystem<UC_GameSceneManager>(); }

	class AC_Player* GetPlayer() const { return Player; }
	void SetPlayer(AC_Player* InPlayer) { Player = InPlayer; }

public: // Widget 초기화 관련 처리

	void SetHUDWidgetByHUDMode(EHUDMode Mode, UUserWidget* InUserWidget) { HUDWidgets.Add(Mode, InUserWidget); }
	void SetMiniMapWidget(UUserWidget* InMiniMapWidget) { MiniMapWidget = InMiniMapWidget; }

public:
	
	TArray<class AC_Enemy*>& GetEnemies() { return Enemies; }

	class AC_MagneticFieldManager* GetMagneticFieldManager() const { return MagneticFieldManager; }
	class AC_AirplaneManager* GetAirplaneManager() const { return AirplaneManager; }

	UFUNCTION(BlueprintCallable)
	class AC_ItemManager* GetItemManager() const { return ItemManager; }
	UFUNCTION(BlueprintCallable)
	class AC_SoundManager* GetSoundManager() const { return SoundManager; }

	UFUNCTION(BlueprintCallable)
	TArray<class AC_BasicCharacter*>& GetAllCharacters() { return AllCharacters; }
	TArray<AActor*>& GetAllCharacterActors() { return AllCharacterActors; }

	/// <summary>
	/// GameScene에서 GC로부터 보호된 Object들 추가 -> GameScene 끝날 때 일괄 삭제처리 예정
	/// </summary>
	void AddGCProtectedObject(UObject* Object) { GCProtectedObjects.Add(Object); }

	EHUDMode GetCurrentHUDMode() const { return CurrentHUDMode; }
	void SetCurrentHUDMode(EHUDMode InHUDMode);

	float GetCellWorldSize() const { return CELL_WORLDSIZE; }

	/// <summary>
	/// <para> 현재 Tick에서의 RandomColor 받아오기 </para>
	/// <para> (Level에 TickRandomColorGenerator Actor가 배치되어 있지 않다면 FColor::Red 반환) </para>
	/// </summary>
	FColor GetTickRandomColor() const;

public:

	/// <summary>
	/// 100 x 100m 크기 Tile coordinate의 중앙점을 World 좌표로 환산한 값 구하기, 단, Z는 항상 0.f로 고정
	/// </summary>
	FVector ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const;

	/// <summary>
	/// 해당 X, Y좌표를 포함시키는 Tile의 좌표 구하기
	/// </summary>
	TPair<uint8, uint8> GetContainingTileCoordinate(const FVector2D& WorldPositionXY) const;

private:

	class AC_Player*				Player{};
	class AC_MagneticFieldManager*	MagneticFieldManager{};
	class AC_AirplaneManager*		AirplaneManager{};
	class AC_ItemManager*			ItemManager{};
	class AC_SoundManager*			SoundManager{}; 
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

	TMap<EHUDMode, UUserWidget*> HUDWidgets{};
	UUserWidget*			     MiniMapWidget{};

private:

	class UDataTable* RandomNameDataTable{};

private:
	
	const float CELL_WORLDSIZE = 10000.f;

private:

	class AC_TickRandomColorGenerator* TickRandomColorGenerator{};

};


