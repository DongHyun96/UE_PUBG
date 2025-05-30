// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Airplane.generated.h"

UCLASS()
class UE_PUBG_API AC_Airplane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Airplane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	void SetStartPosition(FVector InStartPosition) { StartPosition = InStartPosition; }
	void SetFlightDirection(FVector InDirection);

	void SetIsFlying(bool InIsFlying);
	bool GetIsFlying() const { return bIsFlying; }

public:

	/// <summary>
	/// 비행 시작하기
	/// </summary>
	void StartFlight();

	/// <summary>
	/// 비행기 엔진음 시작.
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void StartEngineSound();


	/// <summary>
	/// 비행기 엔진음 중지.
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void StopEngineSound();

private:

	/// <summary>
	/// True  : 엔진음 play
	/// False : 엔진음 Stop
	/// </summary>
	void OnEngineSound(bool bIsPlaySound);

	/// <summary>
	/// Player의 Map에 비행기 표시 Update
	/// </summary>
	void UpdatePlayerMapHUD();

	/// <summary>
	/// 프로펠러 회전시키기
	/// </summary>
	void UpdateProps(const float& DeltaTime);

private:

	const float FLIGHT_SPEED = 2000.f;

	FVector StartPosition{};
	FVector FlightDirection{};

private:
	
	bool bIsFlying{};

private:
	TArray<UStaticMeshComponent*> Props{};

	const float PROP_ROT_SPEED = 2000.f;
	float PropRotZValue{};

};


