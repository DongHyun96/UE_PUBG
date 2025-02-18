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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame

	virtual void Tick(float DeltaTime) override;
	virtual bool ExecuteReloadMontage() override;
	FName SR_RELOAD_LEFT_HAND_SOCKET_NAME ="Kar98ReloadSocket";
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SniperReloadMontages{};
	virtual bool GetIsPlayingMontagesOfAny();
	bool IsReloadingSR = false;
	void SetIsReloadingSR(bool InIsReloadingSR) { IsReloadingSR = InIsReloadingSR; }
	bool GetIsReloadingSR() { return IsReloadingSR; }
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RelativeRotationOnCrawl = FRotator(0.0f, 0.0f, 0.0f); // Pitch, Yaw, Roll
	void SetRelativeRotationOnCrawl();
public:
	FRotator GetNewRelativeRotationOnCrawl() { return RelativeRotationOnCrawl; }

protected:
	//bool ReloadBullet() override;
public:
	virtual bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;

	float GetDamageRateByBodyPart(const FName& BodyPart) override;

private:
	
	// 각 피격 부위별 Damage Rate
	static const TMap<FName, float> BODYPARTS_DAMAGERATE;
};


