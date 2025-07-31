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
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

public:
	
	bool ExecuteReloadMontage() override;
	
	bool AIFireBullet(AC_BasicCharacter* InTargetCharacter) override;

	float GetDamageRateByBodyPart(const FName& BodyPart) override;

private:

	void ChangeCurShootingMode() override;
	//특정 행동을 할 때 Reloading 모션 중지
	virtual void CancelReload() override; 
private:

	// 각 피격 부위별 Damage Rate
	static const TMap<FName, float> BODYPARTS_DAMAGERATE;
};


