// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"

struct FBoostingEffectFactor
{
	float OneBlockHPGainedAmount{};
	float MoveSpeedIncreaseFactor = 1.f;
};

/// <summary>
/// TODO : 여기에서는 UI 건들지 않고 오로지 Stat 관련한 수치만 건드리기
/// </summary>
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

public: // Getters and setters

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

	UFUNCTION(BlueprintCallable)
	float GetCurHP() const { return CurHP; }

	void SetCurHP(const float& InCurHP);

	UFUNCTION(BlueprintCallable)
	float GetCurBoosting() const { return CurBoosting; }

	void SetCurBoosting(const float& InCurBoosting);

	const float GetHealUpLimit() const { return HEAL_UP_LIMIT; }

public:

	bool TakeDamage(const float& Damage);

	bool ApplyHeal(const float& HealAmount);

	bool AddBoost(const float& BoostAmount);

private:

	void UpdateBoostEffect(const float& DeltaTime);

	FBoostingEffectFactor GetBoostingEffectFactorByCurBoostingAmount() const;

protected:

	class AC_BasicCharacter*	OwnerCharacter{};
	class AC_Player*			OwnerPlayer{};

private:

	const float MAX_HP			= 100.f;
	const float MAX_BOOSTING	= 100.f;

	const float HEAL_UP_LIMIT = 75.f; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{};

private:

	float BoostTimer{};

	// TODO 원래 8초
	//const float BOOST_ONE_BLOCK_EFFECT_TIME = 8.f;
	const float BOOST_ONE_BLOCK_EFFECT_TIME = 2.f;
	const float BOOST_ONE_BLOCK_AMOUNT		= 2.631f; // 한 블록 당 줄어드는 Boost 량

	// 20 40 30 10
	const TArray<float> EACH_BOOST_PHASE_BORDER = { 20.f, 60.f, 90.f, 100.f };
	const TArray<FBoostingEffectFactor> BOOSTING_EFFECT_FACTORS =
	{
		{1.f, 1.f},		// 1페이즈 8초당 체력 회복량 & 이동 속도 증가(factor)
		{2.f, 1.01f},	// 2페이즈
		{3.f, 1.025f},	// 3페이즈
		{4.f, 1.0625f}	// 4페이즈
	};

};
