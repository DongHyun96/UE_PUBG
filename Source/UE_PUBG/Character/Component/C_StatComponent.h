// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_StatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	float GetCurHP() const { return CurHP; }
	float GetCurBoosting() const { return CurBoosting; }

	void TakeDamage(const float& Damage);

private:

	const float MAX_HP			= 100.f;
	const float MAX_BOOSTING	= 100.f;
	const float HEAL_UP_LIMIT	= 75.f; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit

private:

	float CurHP = 100.f;
	float CurBoosting{};

};
