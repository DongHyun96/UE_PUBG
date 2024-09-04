// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_StatComponent.h"
#include "Character/C_Player.h"
#include "Utility/C_Util.h"	

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
}

void UC_StatComponent::SetCurHP(const float& InCurHP)
{
	CurHP = InCurHP;

	if (OwnerPlayer) OwnerPlayer->UpdateHPOnHUD();
}

void UC_StatComponent::SetCurBoosting(const float& InCurBoosting)
{
	CurBoosting = InCurBoosting;
	if (OwnerPlayer) OwnerPlayer->OnUpdateBoostingHUD(CurBoosting);
}

bool UC_StatComponent::TakeDamage(const float& Damage)
{
	if (CurHP <= 0.f) return false;
	if (Damage < 0.f) return false;

	CurHP -= Damage;
	
	if (CurHP < 0.f) CurHP = 0.f;

	if (OwnerPlayer) OwnerPlayer->UpdateHPOnHUD();

	// ���
	if (CurHP <= 0.f)
	{
		// ��� ó��
		// TODO : OwnwerCharacter���� call back�� �̿��� ��� �˸���
	}

	return true;
}

bool UC_StatComponent::ApplyHeal(const float& HealAmount)
{
	if (CurHP >= MAX_HP)  return false; // �̹� ü���� ��� á�� ��
	if (HealAmount < 0.f) return false;

	CurHP += HealAmount;

	if (CurHP > MAX_HP) CurHP = MAX_HP;

	if (OwnerPlayer) OwnerPlayer->UpdateHPOnHUD();

	return true;
}

bool UC_StatComponent::AddBoost(const float& BoostAmount)
{
	if (CurBoosting >= MAX_BOOSTING)	return false;
	if (BoostAmount < 0.f)				return false;

	CurBoosting += BoostAmount;

	if (CurBoosting > MAX_BOOSTING) CurBoosting = MAX_BOOSTING;

	if (OwnerPlayer) OwnerPlayer->OnUpdateBoostingHUD(CurBoosting);

	return true;
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

	if (OwnerPlayer) OwnerPlayer->OnUpdateBoostingHUD(CurBoosting);
}

FBoostingEffectFactor UC_StatComponent::GetBoostingEffectFactorByCurBoostingAmount() const
{
	for (int i = 0; i < EACH_BOOST_PHASE_BORDER.Num(); i++)
		if (CurBoosting <= EACH_BOOST_PHASE_BORDER[i]) return BOOSTING_EFFECT_FACTORS[i];

	return FBoostingEffectFactor();
}

