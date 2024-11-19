// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_AirplaneManager.generated.h"

UCLASS()
class UE_PUBG_API AC_AirplaneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_AirplaneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	TPair<FVector, FVector> GetPlaneRouteStartDestPair() const { return { PlaneRouteStart, PlaneRouteDest }; }

private:
	/// <summary>
	/// Random Start, Dest position �����ϱ�
	/// </summary>
	void InitRandomStartDestPosition();

	/// <summary>
	/// Start Dest pos �ʱ�ȭ �� Border�� valid���� üũ
	/// </summary>
	/// <param name="PositionValue"></param>
	/// <returns></returns>
	bool IsValueValidInBorder(float PositionValue);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class AC_Airplane* Airplane{};

private:

	const float PLANE_ALTITUDE = 60000.f; // 600m

	const float PLANE_START_DEST_BORDER_VALUE = 40000.f;

	// ����� ��� ������, ����
	FVector PlaneRouteStart{};
	FVector PlaneRouteDest{};


};
