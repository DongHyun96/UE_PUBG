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

public:

	/// <summary>
	/// 실질적인 CurHP 수치 조정
	/// </summary>
	/// <param name="Damage"> : Damage 량 </param>
	/// <returns></returns>
	bool TakeDamage(const float& Damage);

	/// <summary>
	/// <para> 자체로 만든 TakeDamage 함수, 부위별 Damage를 줄 때 사용 </para>
	/// <para> 주의 : 이 함수는 Armor가 적용된 부위의 데미지 감소만 구현, 실질적인 부위별 Damage량은 외부호출에서 처리 </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage 양 </param>
	/// <param name="DamagingPartType"> : Damage를 줄 부위 </param>
	/// <param name="DamageCauser">		: Damage를 주는 Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser);

	/// <summary>
	/// Bone Name(부위)쪽으로 Damage 주기 시도, 해당 Bone에 Armor가 적용되어 있으면 적절히 Damage량 및 Armor 체력 조절
	/// </summary>
	/// <param name="DamageAmount"> : Damage 량 </param>
	/// <param name="DamagingPhyiscsAssetBoneName"> : Damage를 줄 Bone 쪽 Name </param>
	/// <param name="DamageCauser"> : Damage를 주는 Actor </param>
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

	const float HEAL_UP_LIMIT = 75.f; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f; // 현재 캐릭터 실제 체력
	
	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{}; // 현재 캐릭터의 실제 부스팅 량

private:

	float BoostTimer{};

	const float BOOST_ONE_BLOCK_EFFECT_TIME = 8.f;
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

private:
	// 피격 판정 부위 Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
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
