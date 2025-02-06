// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_WorldPingActor.generated.h"

UCLASS()
class UE_PUBG_API AC_WorldPingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AC_WorldPingActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	bool SpawnPingActorToWorld(FVector SpawnPos);
	
	void HideWorldPing();

	const float GetWorldPingDistanceLimit() const { return WORLD_PING_DISTANCE_LIMIT; }

	//bool IsActive() const {return !}

private:

	void HandleUpdateWorldPingScale();

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UWidgetComponent* PingWidgetComponent{};

private:

	// 500m 찍히는 거리 및 보이는 거리 제한
	static const float WORLD_PING_DISTANCE_LIMIT;

};


