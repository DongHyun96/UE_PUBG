// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_CarRespawnHandler.generated.h"

UCLASS()
class UE_PUBG_API AC_CarRespawnHandler : public AActor
{
	GENERATED_BODY()

public:
	AC_CarRespawnHandler();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	/// <summary>
	/// Spawn Area 영역에서 차량이 들어올 때 처리 
	/// </summary>
	UFUNCTION()
	void OnSpawnAreaBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	/// <summary>
	/// Spawn Area 영역에서 차량이 나갈 때 처리 
	/// </summary>
	UFUNCTION()
	void OnSpawnAreaEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);

	/// <summary>
	/// Respawn할 수 있는 위치들의 Indices 구하기
	/// </summary>
	/// <param name="PossibleIndices"> : 가능한 Indices로 초기화, 만약에 가능한 자리가 없다면 empty array로 set</param>
	void GetPossibleSpaceIndicesForRespawn(TArray<uint8>& PossibleIndices);

protected:

	UPROPERTY(EditInstanceOnly)
	class UBoxComponent* SpawnArea{};

	// SpawnActor 처리시킬 car class 종류
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class AC_Vehicle> VehicleClass{};

	// CarInitialOuterBox의 Collision Object type channel
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "CollisionChannel")
	TEnumAsByte<ECollisionChannel> InitialOuterBoxObjectTypeChannel{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "CollisionChannel")
	TArray<TEnumAsByte<ECollisionChannel>> AllTraceChannelUsed{};
	
private:

	// Init 처리된 이후인지 체크
	bool bInitializedCompletedFlag{};

	uint8 InitialCarCount{}; // Spawn Area에 주차된 첫 차량 대수
	uint8 CurrentCarCount{}; // 현재 SpawnArea에 존재하는 차량 대수


	TArray<FTransform> InitialTransforms{}; // Spawn Area에 주차된 차량들의 초기 Transforms (Index 순으로 위치를 기억해서 관리)

protected:
	
	// Spawn Area에 주차된 차량들의 초기 OuterBox와 동일한 크기의 OuterBox를 두어, 해당 Box Component와 충돌되는 물체가 없는지 조사하여 Spawn처리할 예정
	UPROPERTY(VisibleAnywhere)
	TArray<UBoxComponent*> InitialCarOuterBoxes{};

private:
	
	// 현재 Respawn timer가 동작 중인지
	bool bRespawnTimerSet{};
	
	float RespawnTimer{};
	static const float RespawnDelayMax;
	
};
