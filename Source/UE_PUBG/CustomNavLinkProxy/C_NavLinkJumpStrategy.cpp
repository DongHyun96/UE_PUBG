// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_NavLinkJumpStrategy.h"

#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

bool UC_NavLinkJumpStrategy::ExecuteStartPointAction(AC_CustomNavLinkProxy* CustomNavLinkProxy, AC_Enemy* EnemyAgent,
                                                     const FVector& StartPoint)
{
	UC_Util::Print("Execute Jump", FColor::Red, 10.f);

	// TODO : 점프하기 전 Enemy 회전 및 위치 잡아야하는지 확인

	FTimerHandle TimerHandle{};

	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[EnemyAgent]() { EnemyAgent->Jump(); },
		0.1f,
		false
	);
	// Enemy->Jump();
	return true;
}
