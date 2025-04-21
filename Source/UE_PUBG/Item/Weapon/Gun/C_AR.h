// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "C_AR.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_AR : public AC_Gun
{
	GENERATED_BODY()
	
public:
	AC_AR();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ExecuteReloadMontage() override;

public:
	virtual bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;
	virtual bool ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;
	virtual bool AIFireBullet(class AC_BasicCharacter* InTargetCharacter) override;

	float GetDamageRateByBodyPart(const FName& BodyPart) override;

private:

	void ChangeCurShootingMode() override;

private:

	// 각 피격 부위별 Damage Rate
	static const TMap<FName, float> BODYPARTS_DAMAGERATE;
};


