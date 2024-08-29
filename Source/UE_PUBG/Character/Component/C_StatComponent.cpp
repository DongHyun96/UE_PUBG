// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_StatComponent.h"

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
}

void UC_StatComponent::TakeDamage(const float& Damage)
{
	CurHP -= Damage;
	
	if (CurHP < 0.f) CurHP = 0.f;

	// TODO : Player의 경우 HUD 업데이트

	// 사망
	if (CurHP <= 0.f)
	{
		// 사망 처리
		// TODO : OwnwerCharacter에게 call back을 이용한 사망 알리기
	}
}

