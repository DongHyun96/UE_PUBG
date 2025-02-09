// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"

/// <summary>
/// 피격 판정 부위
/// </summary>
UENUM(BlueprintType)
enum class EDamagingPartType : uint8
{
	HEAD,			// Physics Asset Neck

	HIPS,

	LEFT_ARM,
	LEFT_HAND,
	RIGHT_ARM,
	RIGHT_HAND,

	LEFT_LEG,
	LEFT_FOOT,
	RIGHT_LEG,
	RIGHT_FOOT,

	SHOULDER,		// Physics Asset spine2
	UPPER_STOMACH,	// Physics Asset spine1
	LOWER_STOMACH	// Phyiscs Asset spine
};

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
	//void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }
	void SetOwnerHUDWidget(class UC_HUDWidget* InHUDWidget) { OwnerHUDWidget = InHUDWidget; }

	UFUNCTION(BlueprintCallable)
	float GetCurHP() const { return CurHP; }
	void SetCurHP(const float& InCurHP);

	UFUNCTION(BlueprintCallable)
	float GetCurBoosting() const { return CurBoosting; }
	void SetCurBoosting(const float& InCurBoosting);

	const float GetHealUpLimit() const { return HEAL_UP_LIMIT; }

	float GetCurOxygen() const { return CurOxygen; }

public:

	/// <summary>
	/// 실질적인 CurHP 수치 조정
	/// </summary>
	/// <param name="Damage"> : Damage 량 </param>
	/// <param name="DamageCauser"> : Damage를 주는 BasicCharacter </param>
	/// <returns> : Damage를 추가적으로 입을 수 없는 상황이라면(CurHP <= 0.f) return false </returns>
	bool TakeDamage(const float& Damage, AC_BasicCharacter* DamageCauser);

	/// <summary>
	/// <para> 자체로 만든 TakeDamage 함수, 부위별 Damage를 줄 때 사용 </para>
	/// <para> 주의 : 이 함수는 Armor가 적용된 부위의 데미지 감소만 구현, 실질적인 부위별 Damage량은 외부호출에서 처리 </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage 양 </param>
	/// <param name="DamagingPartType"> : Damage를 줄 부위 </param>
	/// <param name="DamageCauser">		: Damage를 주는 BasicCharacter </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AC_BasicCharacter* DamageCauser);

	/// <summary>
	/// Bone Name(부위)쪽으로 Damage 주기 시도, 해당 Bone에 Armor가 적용되어 있으면 적절히 Damage량 및 Armor 체력 조절
	/// </summary>
	/// <param name="DamageAmount"> : Damage 량 </param>
	/// <param name="DamagingPhysicsAssetBoneName"> : Damage를 줄 Bone 쪽 Name </param>
	/// <param name="DamageCauser"> : Damage를 주는 BasicCharacter </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, FName DamagingPhysicsAssetBoneName, AC_BasicCharacter* DamageCauser);

public:

	bool ApplyHeal(const float& HealAmount);

	bool AddBoost(const float& BoostAmount);

	/// <summary>
	/// Oxygen량 더하고 빼기 통합
	/// </summary>
	/// <param name="OxygenAmount"> : 더할 Oxygen 량 </param>
	void AddOxygen(const float& OxygenAmount);

private:

	void UpdateBoostEffect(const float& DeltaTime);

	/// <summary>
	/// 산소 고갈 시 Damage 주기 담당, 1초에 피해량 20씩 주기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleOxygenExhausted(const float& DeltaTime);

	FBoostingEffectFactor GetBoostingEffectFactorByCurBoostingAmount() const;

protected:

	class AC_BasicCharacter*	OwnerCharacter{};
	class UC_HUDWidget*			OwnerHUDWidget{};
private:

	static const float MAX_HP;
	static const float MAX_BOOSTING;
	static const float HEAL_UP_LIMIT; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit
	static const float MAX_OXYGEN_HP; // 숨 HP Max
	
protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f; // 현재 캐릭터 실제 체력
	
	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{}; // 현재 캐릭터의 실제 부스팅 량

	UPROPERTY(BlueprintReadOnly)
	float CurOxygen = 100.f;

private:

	float BoostTimer{};

	static const float BOOST_ONE_BLOCK_EFFECT_TIME;
	static const float BOOST_ONE_BLOCK_AMOUNT; // 한 블록 당 줄어드는 Boost 량

	// 20 40 30 10
	static const TArray<float> EACH_BOOST_PHASE_BORDER;
	static const TArray<FBoostingEffectFactor> BOOSTING_EFFECT_FACTORS;

private: // Oxygen Exhausted 관련

	static const float OXYGEN_EXHAUSTED_DAMAGE_PER_SEC;

	float OxygenExhaustedTimer{};

private:
	// 피격 판정 부위 Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
	static const TMap<FName, EDamagingPartType> DAMAGINGPARTS_MAP;

};


