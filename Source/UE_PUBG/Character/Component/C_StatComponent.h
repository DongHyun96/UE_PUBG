// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"

/// <summary>
/// �ǰ� ���� ����
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
/// TODO : ���⿡���� UI �ǵ��� �ʰ� ������ Stat ������ ��ġ�� �ǵ帮��
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
	/// �������� CurHP ��ġ ����
	/// </summary>
	/// <param name="Damage"> : Damage �� </param>
	/// <returns></returns>
	bool TakeDamage(const float& Damage);

	/// <summary>
	/// <para> ��ü�� ���� TakeDamage �Լ�, ������ Damage�� �� �� ��� </para>
	/// <para> ���� : �� �Լ��� Armor�� ����� ������ ������ ���Ҹ� ����, �������� ������ Damage���� �ܺ�ȣ�⿡�� ó�� </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage �� </param>
	/// <param name="DamagingPartType"> : Damage�� �� ���� </param>
	/// <param name="DamageCauser">		: Damage�� �ִ� Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser);

	/// <summary>
	/// Bone Name(����)������ Damage �ֱ� �õ�, �ش� Bone�� Armor�� ����Ǿ� ������ ������ Damage�� �� Armor ü�� ����
	/// </summary>
	/// <param name="DamageAmount"> : Damage �� </param>
	/// <param name="DamagingPhyiscsAssetBoneName"> : Damage�� �� Bone �� Name </param>
	/// <param name="DamageCauser"> : Damage�� �ִ� Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser);

public:

	bool ApplyHeal(const float& HealAmount);

	bool AddBoost(const float& BoostAmount);

	/// <summary>
	/// Oxygen�� ���ϰ� ���� ����
	/// </summary>
	/// <param name="OxygenAmount"> : ���� Oxygen �� </param>
	void AddOxygen(const float& OxygenAmount);

private:

	void UpdateBoostEffect(const float& DeltaTime);

	/// <summary>
	/// ��� ���� �� Damage �ֱ� ���, 1�ʿ� ���ط� 20�� �ֱ�
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
	static const float HEAL_UP_LIMIT; // ���޻���, �ش�� ä�� �� �ִ� �� ���� limit
	static const float MAX_OXYGEN_HP; // �� HP Max

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f; // ���� ĳ���� ���� ü��
	
	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{}; // ���� ĳ������ ���� �ν��� ��

	UPROPERTY(BlueprintReadOnly)
	float CurOxygen = 100.f;

private:

	float BoostTimer{};

	static const float BOOST_ONE_BLOCK_EFFECT_TIME;
	static const float BOOST_ONE_BLOCK_AMOUNT; // �� ���� �� �پ��� Boost ��

	// 20 40 30 10
	static const TArray<float> EACH_BOOST_PHASE_BORDER;
	static const TArray<FBoostingEffectFactor> BOOSTING_EFFECT_FACTORS;

private: // Oxygen Exhausted ����

	static const float OXYGEN_EXHAUSTED_DAMAGE_PER_SEC;

	float OxygenExhaustedTimer{};

private:
	// �ǰ� ���� ���� Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
	static const TMap<FName, EDamagingPartType> DAMAGINGPARTS_MAP;

};
