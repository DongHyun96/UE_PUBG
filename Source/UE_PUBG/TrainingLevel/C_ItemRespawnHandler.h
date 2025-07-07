// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_ItemRespawnHandler.generated.h"

/// <summary>
/// Level에 바로 배치 처리 -> SpawnArea에 잡힌 아이템들에 대해 자동적으로 Respawn 처리를 도와주는 클래스
/// </summary>
UCLASS()
class UE_PUBG_API AC_ItemRespawnHandler : public AActor
{
	GENERATED_BODY()

public:
	AC_ItemRespawnHandler();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 이 RespawnHandler에 의해 Spawn된 아이템이 파밍되었을 시 Callback될 Delegate 함수
	/// </summary>
	/// <param name="PickedItem"></param>
	void OnItemPickedUp(class AC_Item* PickedItem);

protected:

	UPROPERTY(EditInstanceOnly)
	class UBoxComponent* SpawnArea{};

private:

	// Respawn area에 배치된 아이템들의 초기 Transform 값 저장
	TMap<TSubclassOf<AC_Item>, TArray<FTransform>> InitialTransforms{};

	// RespawnHandler에 의해 Spawn 처리 되었고, 아직 파밍되지 않은 아이템들 저장 (파밍이 되었다면 해당 Index번 째 아이템 nullptr로 저장하여, Vacant 여부 표시)
	TMap<TSubclassOf<AC_Item>, TArray<AC_Item*>> SpawnedItems{};

private:

	// 현재 Respawn timer가 동작 중인지
	bool bRespawnTimerSet{};
	
	float RespawnTimer{};
	float RespawnDelayMax = 5.f;
};
