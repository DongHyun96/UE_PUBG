// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "C_SR.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_SR : public AC_Gun
{
	GENERATED_BODY()
public:
	AC_SR();
protected:
	
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual bool ExecuteReloadMontage() override;

public:
	
	virtual bool GetIsPlayingMontagesOfAny();
	
	void SetIsReloadingSR(bool InIsReloadingSR) { bIsReloadingSR = InIsReloadingSR; }
	bool GetIsReloadingSR() { return bIsReloadingSR; }
	
	FRotator GetNewRelativeRotationOnCrawl() { return RelativeRotationOnCrawl; }

	TMap<EPoseState, FPriorityAnimMontage>& GetSniperReloadMontages() { return SniperReloadMontages; }
	
protected:
	
	void SetRelativeRotationOnCrawl();
	
public:
	
	virtual bool ExecuteAIAttackTickTask(AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;
	virtual bool AIFireBullet(AC_BasicCharacter* InTargetCharacter) override;

	float GetDamageRateByBodyPart(const FName& BodyPart) override;

protected:
	
	void ChangeCurShootingMode() override;

	/// <summary>
	/// 특정 행동을 할 때 Reloading 모션 중지
	/// </summary>
	virtual void CancelReload() override;

private:

	static const FName SR_RELOAD_LEFT_HAND_SOCKET_NAME;

protected:
	
	// 각 피격 부위별 Damage Rate
	static const TMap<FName, float> BODYPARTS_DAMAGERATE;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SniperReloadMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RelativeRotationOnCrawl = FRotator(0.0f, 0.0f, 0.0f); // Pitch, Yaw, Roll

private:
	
	bool bIsReloadingSR = false;
};


