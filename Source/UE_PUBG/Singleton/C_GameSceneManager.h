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
/// GameScene���� ����� Singleton class
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
	/// GameScene���� GC�κ��� ��ȣ�� Object�� �߰� -> GameScene ���� �� �ϰ� ����ó�� ����
	/// </summary>
	void AddNonGCObject(UObject* Object) { NonGCObjects.Add(Object); }

private:

	class AC_Player*				Player{};
	class AC_MagneticFieldManager*	MagneticFieldManager{};
	class AC_AirplaneManager*		AirplaneManager{};

private:

	// �ΰ��� ��� ĳ���͵�(Player + Enemies)
	TArray<class AC_BasicCharacter*> AllCharacters{};

private:

	// InGameScene������ Unreal GC�κ��� ��ȣ�� ��ü�� -> GameScene�� ���� �� ������ ����
	TSet<UObject*> NonGCObjects{};

};
