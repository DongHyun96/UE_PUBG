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

public:
	
	bool GetIsPlayingMontagesOfAny() override;
	
	void SetIsCurrentlyReloadingSRMagazine(bool InIsCurrentlyReloading) { bIsCurrentlyReloadingSRMagazine = InIsCurrentlyReloading; }
	
	FRotator GetNewRelativeRotationOnCrawl() { return RelativeRotationOnCrawl; }

	TMap<EPoseState, FPriorityAnimMontage>& GetSniperReloadMontages() { return SniperReloadMontages; }

public:

	/// <summary>
	/// SR Bolt action 장전 처리
	/// </summary>
	/// <returns> : Bolt action 장전 처리를 할 상황이 아니거나 제대로 처리가 되지 않았다면 return false </returns>
	bool ExecuteBoltActionReloadMontage();
	
protected:
	
	void SetRelativeRotationOnCrawl();
	
private:
	
	bool AIFireBullet(AC_BasicCharacter* InTargetCharacter) override;

	float GetDamageRateByBodyPart(const FName& BodyPart) override;

protected:
	
	void ChangeCurShootingMode() override;

	/// <summary>
	/// 특정 행동을 할 때 Reloading 모션 중지
	/// </summary>
	void CancelReload() override;

public:
	
	/// <summary>
	/// AN_SniperReloadEnd Callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnSniperReloadEnd();

private:

	static const FName SR_RELOAD_LEFT_HAND_SOCKET_NAME;

protected:
	
	// 각 피격 부위별 Damage Rate
	static const TMap<FName, float> BODYPARTS_DAMAGERATE;

	// 볼트액션 장전 Montages
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SniperReloadMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RelativeRotationOnCrawl = FRotator(0.0f, 0.0f, 0.0f); // Pitch, Yaw, Roll

private:
	
	bool bIsCurrentlyReloadingSRMagazine{};
};


