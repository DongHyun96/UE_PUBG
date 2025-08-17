// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PingSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_PingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UC_PingSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void OnMMBStarted();

	//void SpawnWorldPing()

	/// <summary>
	/// World Ping Actor spawn 시키기
	/// </summary>
	/// <param name="SpawnLocation"> : Spawn world 위치</param>
	void SpawnWorldPingActor(FVector SpawnLocation);

	/// <summary>
	/// World ping Hidden으로 감추기
	/// </summary>
	void OnCancelWorldPing();


public:

	void SetOwnerPlayer(class AC_Player* InPlayer) { OwnerPlayer = InPlayer; }

private:

	class AC_Player* OwnerPlayer{};

private:

	class AC_WorldPingActor* WorldPingActor{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class TSubclassOf<AC_WorldPingActor> WorldPingActorClass{};
		
};


