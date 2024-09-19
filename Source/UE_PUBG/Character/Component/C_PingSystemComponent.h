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
	/// World Ping Actor spawn ��Ű��
	/// </summary>
	/// <param name="SpawnPos"> : Spawn world ��ġ</param>
	/// <returns> : Spawn�� ���������� �Ͼ�ٸ� return true </returns>
	bool SpawnWorldPingActor(FVector SpawnPos);

	/// <summary>
	/// World ping Hidden���� ���߱�
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
