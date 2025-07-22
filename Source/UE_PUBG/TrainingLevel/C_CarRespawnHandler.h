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

	UFUNCTION()
	void OnSpawnAreaEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);

private:

	

protected:

	UPROPERTY(EditInstanceOnly)
	class UBoxComponent* SpawnArea{};

private:

	// Init 처리된 이후인지 체크
	bool bInitializedCompletedFlag{};

	uint8 InitialCarCount{}; // Spawn Area에 주차된 첫 차량 대수
	uint8 CurrentCarCount{}; // 현재 SpawnArea에 존재하는 차량 대수


	TArray<FTransform> InitialTransforms{}; // Spawn Area에 주차된 차량들의 초기 Transforms (Index 순으로 위치를 기억해서 관리)
	
	// Spawn Area에 주차된 차량들의 초기 OuterBox와 동일한 크기의 OuterBox를 두어, 해당 Box Component와 충돌되는 물체가 없는지 조사하여 Spawn처리할 예정
	TArray<UBoxComponent*> InitialCarOuterBoxes{};

	// 해당 SpawnArea에 주차된 Vehicles (만약에 SpawnArea를 이탈한 차량이 존재한다면, 해당 Index는 nullptr로 체크)
	TArray<class AC_Vehicle*> Vehicles{};

private:
	
	// 현재 Respawn timer가 동작 중인지
	bool bRespawnTimerSet{};
	
	float RespawnTimer{};
	static const float RespawnDelayMax;
	
};
