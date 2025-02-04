// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_StatComponent.h"
#include "Character/C_BasicCharacter.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_OxygenWidget.h"
#include "Utility/C_Util.h"	

const float UC_StatComponent::MAX_HP		= 100.f;
const float UC_StatComponent::MAX_BOOSTING	= 100.f;
const float UC_StatComponent::HEAL_UP_LIMIT = 75.f; // ���޻���, �ش�� ä�� �� �ִ� �� ���� limit
const float UC_StatComponent::MAX_OXYGEN_HP = 100.f; // �� HP Max

const float UC_StatComponent::BOOST_ONE_BLOCK_EFFECT_TIME = 8.f;
const float UC_StatComponent::BOOST_ONE_BLOCK_AMOUNT		= 2.631f; // �� ���� �� �پ��� Boost ��

// 20 40 30 10
const TArray<float> UC_StatComponent::EACH_BOOST_PHASE_BORDER = { 20.f, 60.f, 90.f, 100.f };

const TArray<FBoostingEffectFactor> UC_StatComponent::BOOSTING_EFFECT_FACTORS =
{
	{1.f, 1.f},		// 1������ 8�ʴ� ü�� ȸ���� & �̵� �ӵ� ����(factor)
	{2.f, 1.01f},	// 2������
	{3.f, 1.025f},	// 3������
	{4.f, 1.0625f}	// 4������
};

const float UC_StatComponent::OXYGEN_EXHAUSTED_DAMAGE_PER_SEC = 20.f;

const TMap<FName, EDamagingPartType> UC_StatComponent::DAMAGINGPARTS_MAP =
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

UC_StatComponent::UC_StatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_StatComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UC_StatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBoostEffect(DeltaTime);
	//UC_Util::Print("CurHP : " + FString::SanitizeFloat(CurHP));
	HandleOxygenExhausted(DeltaTime);
}

void UC_StatComponent::SetCurHP(const float& InCurHP)
{
	CurHP = InCurHP;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateHP(CurHP);
}

void UC_StatComponent::SetCurBoosting(const float& InCurBoosting)
{
	CurBoosting = InCurBoosting;
	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);
}

bool UC_StatComponent::TakeDamage(const float& Damage)
{
	if (CurHP <= 0.f) return false;
	if (Damage < 0.f) return false;

	CurHP -= Damage;
	
	if (CurHP < 0.f) CurHP = 0.f;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateHP(CurHP);

	// ���
	if (CurHP <= 0.f)
	{
		// ��� ó��
		// TODO : OwnwerCharacter���� call back�� �̿��� ��� �˸���
	}

	return true;
}

float UC_StatComponent::TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser)
{
	//FString Str = "Character Damaged on certain damaging part! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);
	//UC_Util::Print(Str, FColor::Cyan, 3.f);

	// TODO : Armor Ȯ���ؼ� Armor �κ��̶�� Damage ���� ����
	// TODO : Armor ���� �� ���

	TakeDamage(DamageAmount);
	return DamageAmount;
}

float UC_StatComponent::TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser)
{
	if (!DAMAGINGPARTS_MAP.Contains(DamagingPhyiscsAssetBoneName))
	{
		UC_Util::Print("From UC_StatComponent::TakeDamage : No Such PhysicsAsset Bone Name exists!");
		return 0.f;
	}

	//UC_Util::Print(DamagingPhyiscsAssetBoneName.ToString() + " Parts damaged! Amount : " + FString::SanitizeFloat(DamageAmount));

	TakeDamage(DamageAmount);
	return DamageAmount;
}

bool UC_StatComponent::ApplyHeal(const float& HealAmount)
{
	if (CurHP >= MAX_HP)  return false; // �̹� ü���� ��� á�� ��
	if (HealAmount < 0.f) return false;

	CurHP += HealAmount;

	if (CurHP > MAX_HP) CurHP = MAX_HP;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateHP(CurHP);

	return true;
}

bool UC_StatComponent::AddBoost(const float& BoostAmount)
{
	if (CurBoosting >= MAX_BOOSTING)	return false;
	if (BoostAmount < 0.f)				return false;

	CurBoosting += BoostAmount;

	if (CurBoosting > MAX_BOOSTING) CurBoosting = MAX_BOOSTING;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);

	return true;
}

void UC_StatComponent::AddOxygen(const float& OxygenAmount)
{
	CurOxygen += OxygenAmount;
	CurOxygen  = FMath::Clamp(CurOxygen, 0.f, MAX_OXYGEN_HP);

	if (OwnerHUDWidget) OwnerHUDWidget->GetOxygenWidget()->SetOxygenPercent(CurOxygen * 0.01f);
}

void UC_StatComponent::UpdateBoostEffect(const float& DeltaTime)
{
	// Boost �������� ���ų� �̹� ��� ó���Ǿ��� ��
	if (CurBoosting <= 0.f || CurHP <= 0.f)
	{
		BoostTimer = 0.f;
		return;
	}

	BoostTimer += DeltaTime;

	if (BoostTimer < BOOST_ONE_BLOCK_EFFECT_TIME) return;

	BoostTimer -= BOOST_ONE_BLOCK_EFFECT_TIME;
	
	FBoostingEffectFactor BoostingEffectFactor = GetBoostingEffectFactorByCurBoostingAmount();

	OwnerCharacter->SetBoostingSpeedFactor(BoostingEffectFactor.MoveSpeedIncreaseFactor);
	ApplyHeal(BoostingEffectFactor.OneBlockHPGainedAmount);
	
	CurBoosting -= BOOST_ONE_BLOCK_AMOUNT;
	if (CurBoosting <= 0.5f) CurBoosting = 0.f;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);
}

void UC_StatComponent::HandleOxygenExhausted(const float& DeltaTime)
{
	if (CurOxygen > 0.f)
	{
		OxygenExhaustedTimer = 0.f;
		return;
	}
	if (OxygenExhaustedTimer >= 1.f)
	{
		OxygenExhaustedTimer -= 1.f;
		TakeDamage(OXYGEN_EXHAUSTED_DAMAGE_PER_SEC);
		return;
	}
	OxygenExhaustedTimer += DeltaTime;
}

FBoostingEffectFactor UC_StatComponent::GetBoostingEffectFactorByCurBoostingAmount() const
{
	for (int i = 0; i < EACH_BOOST_PHASE_BORDER.Num(); i++)
		if (CurBoosting <= EACH_BOOST_PHASE_BORDER[i]) return BOOSTING_EFFECT_FACTORS[i];

	return FBoostingEffectFactor();
}

