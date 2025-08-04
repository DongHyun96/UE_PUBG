// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "C_GameSceneManager.generated.h"

// 주의 : GetWorld() 부분 (World가 맞지 않다면 GetInstance로 GameSceneManager 객체 불러오기 처리하기)
#define GAMESCENE_MANAGER GetWorld()->GetSubsystem<UC_GameSceneManager>()
//
/// <summary>
/// GameScene 현 HUD Mode
/// </summary>
UENUM(BlueprintType)
enum class EHUDMode : uint8
{
	IDLE,
	INVEN,
	MAINMAP,
	MAINMENU,
	MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetVehiclesCollisoin); 
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
	class AC_LootCrate* SpawnLootCrateAt(FVector SpawnLocation, class AC_BasicCharacter* DeadCharacter);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AC_LootCrate> LootCrateClass;

private:

	//void OnWorldEndPlay(UWorld* InWorld);

public:

	/// <summary>
	/// 해당 Location으로부터 가장 가까운 NavMesh가 존재하는지 조사 
	/// </summary>
	/// <param name="Location"> : 조사할 위치 지점 </param>
	/// <param name="Extent"> : 조사 반경 </param>
	/// <param name="ProjectedLocation"> : NavMesh위로 보정된 가장 가까운 위치 </param>
	/// <returns> : Extent반경에 NavMesh가 없다면 return false </returns>
	bool FindNearestNavMeshAtLocation(const FVector& Location, const FVector& Extent, FVector& ProjectedLocation);
	
public: // Getters and setters

	static UC_GameSceneManager* GetInstance(UWorld* World) { return World->GetSubsystem<UC_GameSceneManager>(); }

	UFUNCTION(BlueprintCallable)
	class AC_Player* GetPlayer() const { return Player; }

	void SetPlayer(AC_Player* InPlayer) { Player = InPlayer; }


public: // Widget 초기화 관련 처리

	void SetHUDWidgetByHUDMode(EHUDMode Mode, UUserWidget* InUserWidget) { HUDWidgets.Add(Mode, InUserWidget); }
	void SetMiniMapWidget(UUserWidget* InMiniMapWidget) { MiniMapWidget = InMiniMapWidget; }

public:

	bool GetIsGameOver() const { return bIsGameOver; }
	void SetIsGameOver(bool InIsGameOver) { bIsGameOver = InIsGameOver; }
	
	TArray<class AC_Enemy*>& GetEnemies() { return Enemies; }

	class AC_MagneticFieldManager* GetMagneticFieldManager() const { return MagneticFieldManager; }
	class AC_AirplaneManager* GetAirplaneManager() const { return AirplaneManager; }

	UFUNCTION(BlueprintCallable)
	class AC_ItemManager* GetItemManager() const { return ItemManager; }
	UFUNCTION(BlueprintCallable)
	class AC_SoundManager* GetSoundManager() const { return SoundManager; }

	UFUNCTION(BlueprintCallable)
	TArray<AC_BasicCharacter*>& GetAllCharacters() { return AllCharacters; }
	TArray<AActor*>& GetAllCharacterActors() { return AllCharacterActors; }

	class AC_TutorialManager* GetTutorialManager() const { return TutorialManager; }
	class AC_TrainingGroundManager* GetTrainingGroundManager() const { return TrainingGroundManager; }
	
	/// <summary>
	/// GameScene에서 GC로부터 보호된 Object들 추가 -> GameScene 끝날 때 일괄 삭제처리 예정
	/// </summary>
	void AddGCProtectedObject(UObject* Object) { GCProtectedObjects.Add(Object); }

	/// <summary>
	/// Object 
	/// </summary>
	/// <returns></returns>
	FTimerHandle& GetTimerHandle();

	UFUNCTION(BlueprintCallable)
	EHUDMode GetCurrentHUDMode() const { return CurrentHUDMode; }

	UFUNCTION(BlueprintCallable)
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

	/// <summary>
	/// CurrentRanking을 받음과 동시에 CurrentRanking 업데이트 처리
	/// </summary>
	/// <returns></returns>
	int GetCurrentRankingAndUpdateCurrentRanking() { return CurrentRanking--; }

	int GetTotalPlayedCharacterCount() const { return TotalPlayedCharacterCount; }

	/// <summary>
	/// CurrentAliveCount에서 하나 제거 
	/// </summary>
	/// <returns> : 제거한 이후 CurrentAliveCharacterCount 수 </returns>
	int RemoveOneFromCurrentAliveCharacterCount() { return --CurrentAliveCharacterCount; }
	
	int GetCurrentAliveCharacterCount() const { return CurrentAliveCharacterCount; }

public:
	
	UFUNCTION(BlueprintCallable)
	void AddSpawnedItemToContainer(class AC_Item* InItem);
	//{
	//	if (!ItemManager) return;
	//	ItemManager->AddSpawnedItemToContainer(InItem);
	//}

	UFUNCTION(BlueprintCallable)
	void DeleteSpawnedItemToContainer(AC_Item* InItem);

	void ToggleItemsHiddenInGame(bool InHiddenInGame);

	UFUNCTION(BlueprintCallable)
	void AddVehiclesToContainer(class APawn* InVehicle);

	
private:
	void SetVehiclesCollision(); // 델리게이트 실행 함수

private:

	AC_Player*					Player{};
	AC_MagneticFieldManager*	MagneticFieldManager{};
	AC_AirplaneManager*			AirplaneManager{};
	AC_ItemManager*				ItemManager{};
	AC_SoundManager*			SoundManager{};
	AC_TutorialManager*			TutorialManager{};
	AC_TrainingGroundManager*	TrainingGroundManager{};
	
	
private:

	// 인게임 모든 캐릭터들(Player + Enemies)
	TArray<AC_BasicCharacter*> AllCharacters{};
	TArray<AActor*> AllCharacterActors{};

private:

	TArray<AC_Enemy*> Enemies{};

private:

	// InGameScene내에서 Unreal GC로부터 보호된 객체들 -> GameScene이 끝날 때 해제할 예정
	TSet<UObject*> GCProtectedObjects{};

	// InGameScene내에서 사용되는 TimerHandle
	TArray<FTimerHandle> GameSceneTimerHandles{};
	const int TimerHandlePoolCount = 50;

private: /* Widget UI 관련 */

	// Main Player의 Widget들, 현재 어떤 WidgetMode인지에 따른 Visibility setting 처리
	EHUDMode CurrentHUDMode{};

	TMap<EHUDMode, UUserWidget*> HUDWidgets{};
	UUserWidget*			     MiniMapWidget{};

private:
	
	const float CELL_WORLDSIZE = 10000.f; // Shanty_Town Cell world size

private:

	class AC_TickRandomColorGenerator* TickRandomColorGenerator{};

private:

	int CurrentRanking{}; // 죽었을 때 부여받을 Ranking
	int TotalPlayedCharacterCount{}; // 게임 시작했을 때 살아있었던 총 Character 수
	int CurrentAliveCharacterCount{}; // 현재 살아있는 캐릭터 수

private:

	bool bIsGameOver{};

private:
	
	TArray<AC_Item*> ItemContainer{};
	TArray<APawn*> VehicleContainer{};

protected:
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnSetVehiclesCollisoin OnSetVehiclesCollision; // 블루프린트에서 바인딩할 수 있게 함

};


