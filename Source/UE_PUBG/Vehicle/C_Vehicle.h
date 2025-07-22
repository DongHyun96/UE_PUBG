// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WheeledVehiclePawn.h"
#include "C_Vehicle.generated.h"

UCLASS(Abstract)
class UE_PUBG_API AC_Vehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AC_Vehicle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:

	class UBoxComponent* GetMainOuterBoxComponent() const { return MainOuterBoxComponent; }

protected:

	// 차량의 가장 바깥 쪽 BoxComponent (외관 전체를 감싸는 BoxComponent) -> 이 프로젝트에서 BP_Vehicle05_Car의 BeginPlay에서만 초기화시킴
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBoxComponent* MainOuterBoxComponent{};
	
};
