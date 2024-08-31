// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_StatComponent.h"

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

	//UC_Util::Print("CurHP : " + FString::SanitizeFloat(CurHP));
}

bool UC_StatComponent::TakeDamage(const float& Damage)
{
	if (CurHP <= 0.f) return false;
	if (Damage < 0.f) return false;

	CurHP -= Damage;
	
	if (CurHP < 0.f) CurHP = 0.f;

	// TODO : Player�� ��� HUD ������Ʈ

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

	return true;
}

bool UC_StatComponent::AddBoost(const float& BoostAmount)
{
	if (CurBoosting >= MAX_BOOSTING)	return false;
	if (BoostAmount < 0.f)				return false;

	CurBoosting += BoostAmount;

	if (CurBoosting > MAX_BOOSTING) CurBoosting = MAX_BOOSTING;

	return true;
}

