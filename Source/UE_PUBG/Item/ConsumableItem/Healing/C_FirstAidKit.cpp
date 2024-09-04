
// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_FirstAidKit.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "Utility/C_Util.h"

AC_FirstAidKit::AC_FirstAidKit()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 6.f;

	//0.24�ʾ� -> +15 : �� 5�� �ؼ� �� 2�ʿ� �ִ� 75��ŭ ��
	OneBlockHealAmount	= 15.f;
	OneBlockTime		= 0.24f;
	TotalUsingBlockCnt	= 5;
	HealUpLimit			= 75.f;
	
}

void AC_FirstAidKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_FirstAidKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_FirstAidKit::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser)) 
		Player->OnActivatingHealUp(75.f, UsageTime, UsingTimer);
}

void AC_FirstAidKit::InitStartVariables()
{
	UsingTimer	= 0.f;
	UsageTime	= 6.f;
	BlockUsed	= 0;
	UC_Util::Print("Starts to use FirstAidKit!");
}
