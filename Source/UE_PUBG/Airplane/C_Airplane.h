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

	bool GetIsFlying() const { return IsFlying; }

public:

	/// <summary>
	/// ���� �����ϱ�
	/// </summary>
	void StartFlight();

private:

	void Move(const float& DeltaTime);

	/// <summary>
	/// Player�� Map�� ����� ǥ�� Update
	/// </summary>
	void UpdatePlayerMapHUD();

	/// <summary>
	/// ����� ���� ����( Dest�� ���� �� ���� ��)�� �����ߴ��� ����
	/// </summary>
	void CheckFlightFinished();

	/// <summary>
	/// �°��� ��ġ ����
	/// </summary>
	void UpdatePassengersPosition();

	/// <summary>
	/// �����緯 ȸ����Ű��
	/// </summary>
	void UpdateProps(const float& DeltaTime);

private:

	const float FLIGHT_SPEED = 2000.f;

	FVector StartPosition{};
	FVector FlightDirection{};

private:
	
	bool IsFlying{};

private:
	TArray<class UStaticMeshComponent*> Props{};

	const float PROP_ROT_SPEED = 1e3;
	float PropRotZValue{};

private:

	class UC_HUDWidget* PlayerHUDWidget{};

};
