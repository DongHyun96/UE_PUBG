// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"

/// <summary>
/// 占실곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙
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
/// TODO : 占쏙옙占썩에占쏙옙占쏙옙 UI 占실듸옙占쏙옙 占십곤옙 占쏙옙占쏙옙占쏙옙 Stat 占쏙옙占쏙옙占쏙옙 占쏙옙치占쏙옙 占실드리占쏙옙
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
	/// 占쏙옙占쏙옙占쏙옙占쏙옙 CurHP 占쏙옙치 占쏙옙占쏙옙
	/// </summary>
	/// <param name="Damage"> : Damage 占쏙옙 </param>
	/// <returns></returns>
	bool TakeDamage(const float& Damage);

	/// <summary>
	/// <para> 占쏙옙체占쏙옙 占쏙옙占쏙옙 TakeDamage 占쌉쇽옙, 占쏙옙占쏙옙占쏙옙 Damage占쏙옙 占쏙옙 占쏙옙 占쏙옙占?</para>
	/// <para> 占쏙옙占쏙옙 : 占쏙옙 占쌉쇽옙占쏙옙 Armor占쏙옙 占쏙옙占쏙옙占?占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌀몌옙 占쏙옙占쏙옙, 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 Damage占쏙옙占쏙옙 占쌤븝옙호占썩에占쏙옙 처占쏙옙 </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage 占쏙옙 </param>
	/// <param name="DamagingPartType"> : Damage占쏙옙 占쏙옙 占쏙옙占쏙옙 </param>
	/// <param name="DamageCauser">		: Damage占쏙옙 占쌍댐옙 Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser);

	/// <summary>
	/// Bone Name(占쏙옙占쏙옙)占쏙옙占쏙옙占쏙옙 Damage 占쌍깍옙 占시듸옙, 占쌔댐옙 Bone占쏙옙 Armor占쏙옙 占쏙옙占쏙옙퓸占?占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 Damage占쏙옙 占쏙옙 Armor 체占쏙옙 占쏙옙占쏙옙
	/// </summary>
	/// <param name="DamageAmount"> : Damage 占쏙옙 </param>
	/// <param name="DamagingPhyiscsAssetBoneName"> : Damage占쏙옙 占쏙옙 Bone 占쏙옙 Name </param>
	/// <param name="DamageCauser"> : Damage占쏙옙 占쌍댐옙 Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser);

public:

	bool ApplyHeal(const float& HealAmount);

	bool AddBoost(const float& BoostAmount);

	/// <summary>
	/// Oxygen占쏙옙 占쏙옙占싹곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	/// </summary>
	/// <param name="OxygenAmount"> : 占쏙옙占쏙옙 Oxygen 占쏙옙 </param>
	void AddOxygen(const float& OxygenAmount);

private:

	void UpdateBoostEffect(const float& DeltaTime);

	/// <summary>
	/// 占쏙옙占?占쏙옙占쏙옙 占쏙옙 Damage 占쌍깍옙 占쏙옙占? 1占십울옙 占쏙옙占쌔뤄옙 20占쏙옙 占쌍깍옙
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
	static const float HEAL_UP_LIMIT; // 占쏙옙占쌨삼옙占쏙옙, 占쌔댐옙占?채占쏙옙 占쏙옙 占쌍댐옙 占쏙옙 占쏙옙占쏙옙 limit
	static const float MAX_OXYGEN_HP; // 占쏙옙 HP Max

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f; // 占쏙옙占쏙옙 캐占쏙옙占쏙옙 占쏙옙占쏙옙 체占쏙옙
	
	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{}; // 占쏙옙占쏙옙 캐占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占싸쏙옙占쏙옙 占쏙옙

	UPROPERTY(BlueprintReadOnly)
	float CurOxygen = 100.f;

private:

	float BoostTimer{};

	static const float BOOST_ONE_BLOCK_EFFECT_TIME;
	static const float BOOST_ONE_BLOCK_AMOUNT; // 占쏙옙 占쏙옙占쏙옙 占쏙옙 占쌕억옙占쏙옙 Boost 占쏙옙

	// 20 40 30 10
	static const TArray<float> EACH_BOOST_PHASE_BORDER;
	static const TArray<FBoostingEffectFactor> BOOSTING_EFFECT_FACTORS;

private: // Oxygen Exhausted 占쏙옙占쏙옙

	static const float OXYGEN_EXHAUSTED_DAMAGE_PER_SEC;

	float OxygenExhaustedTimer{};

private:
	// 占실곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙 Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
	static const TMap<FName, EDamagingPartType> DAMAGINGPARTS_MAP;

};


