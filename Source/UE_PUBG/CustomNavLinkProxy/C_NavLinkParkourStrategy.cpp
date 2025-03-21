// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_NavLinkParkourStrategy.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UC_NavLinkParkourStrategy::ExecuteStartPointAction(AC_CustomNavLinkProxy* CustomNavLinkProxy, AC_Enemy* EnemyAgent,
                                                        const FVector& StartPoint)
{
	// TODO : 파쿠르 위치조정 Descriptor같은 거 만들어서 링크 위치 말고 좀 더 정확한 파쿠르 시도 위치를 두어야 할 수도 있음
		
	// EnemyAIController->StopMovement();
		
	/*FTimerHandle TimerHandle{};
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[this, EnemyAgent]()
		{
			EnemyAgent->GetParkourComponent()->TryExecuteParkourAction();
		},
		0.2f,
		false
	);*/
	return EnemyAgent->GetParkourComponent()->TryExecuteParkourAction();
}
