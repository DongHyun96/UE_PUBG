// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TrainingShootingTarget.generated.h"

/*UENUM(BlueprintType)
enum class ETargetPartType : uint8
{
	Head,
	Hip,
	Torso,
	Arm,
	Leg,
	Max
};*/

UCLASS()
class UE_PUBG_API AC_TrainingShootingTarget : public AActor
{
	GENERATED_BODY()

public:
	AC_TrainingShootingTarget();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnCollisionPartHit
	(
		UPrimitiveComponent*	HitComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		FVector					NormalImpulse,
		const FHitResult&		Hit
	);
	

private:

	// 캐릭터의 실질적인 Hit bodyParts 이름 저장 / Damage 계산 시 사용
	TMap<UShapeComponent*, FName> CorrespondingBodyPartNames{};

private:

	// Shooting Target 들이 모두 돌아가며 사용할 예정
	static class AC_ShootingTargetWidgetsHolder* ShootingTargetWidgetsHolder;

};
