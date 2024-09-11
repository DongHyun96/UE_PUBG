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

private:

	void UpdateBoostEffect(const float& DeltaTime);

	FBoostingEffectFactor GetBoostingEffectFactorByCurBoostingAmount() const;

protected:

	class AC_BasicCharacter*	OwnerCharacter{};
	class AC_Player*			OwnerPlayer{};
	class UC_HUDWidget*			OwnerHUDWidget{};

private:

	const float MAX_HP			= 100.f;
	const float MAX_BOOSTING	= 100.f;

	const float HEAL_UP_LIMIT = 75.f; // ���޻���, �ش�� ä�� �� �ִ� �� ���� limit

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f; // ���� ĳ���� ���� ü��
	
	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{}; // ���� ĳ������ ���� �ν��� ��

private:

	float BoostTimer{};

	const float BOOST_ONE_BLOCK_EFFECT_TIME = 8.f;
	const float BOOST_ONE_BLOCK_AMOUNT		= 2.631f; // �� ��� �� �پ��� Boost ��

	// 20 40 30 10
	const TArray<float> EACH_BOOST_PHASE_BORDER = { 20.f, 60.f, 90.f, 100.f };
	const TArray<FBoostingEffectFactor> BOOSTING_EFFECT_FACTORS =
	{
		{1.f, 1.f},		// 1������ 8�ʴ� ü�� ȸ���� & �̵� �ӵ� ����(factor)
		{2.f, 1.01f},	// 2������
		{3.f, 1.025f},	// 3������
		{4.f, 1.0625f}	// 4������
	};

private:
	// �ǰ� ���� ���� Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
	const TMap<FName, EDamagingPartType> DAMAGINGPARTS_MAP =	
	{
		{"Neck",		EDamagingPartType::HEAD},

		{"Hips",		EDamagingPartType::HIPS},

		{"LeftUpLeg",	EDamagingPartType::LEFT_LEG},
		{"LeftFoot",	EDamagingPartType::LEFT_FOOT},
		{"RightUpLeg",	EDamagingPartType::RIGHT_LEG},
		{"RightFoot",	EDamagingPartType::RIGHT_FOOT},

		{"Spine",		EDamagingPartType::LOWER_STOMACH},
		{"Spine1",		EDamagingPartType::UPPER_STOMACH},
		{"Spine2",		EDamagingPartType::SHOULDER},

		{"LeftArm",		EDamagingPartType::LEFT_ARM},
		{"LeftHand",	EDamagingPartType::LEFT_HAND},

		{"RightArm",	EDamagingPartType::RIGHT_ARM},
		{"RightHand",	EDamagingPartType::RIGHT_HAND}
	};

};
