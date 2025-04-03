// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_NavLinkJumpStrategy.h"

#include "C_CustomNavLinkProxy.h"
#include "Character/C_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/C_Util.h"

bool UC_NavLinkJumpStrategy::ExecuteStartPointAction(AC_CustomNavLinkProxy* CustomNavLinkProxy, AC_Enemy* EnemyAgent,
                                                     const FVector& StartPoint, const EDirection& Direction)
{
	UC_Util::Print("Execute Jump", FColor::Red, 10.f);

	FTimerHandle TimerHandle{};
	EnemyAgent->GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[CustomNavLinkProxy, EnemyAgent, Direction]()
		{
			float JumpVelocityZ = (CustomNavLinkProxy->IsUsingJumpDescriptorSettings()) ?
									CustomNavLinkProxy->GetJumpVelocityZ(Direction) :
									AC_Enemy::GetJumpVelocityZOrigin();
			
			EnemyAgent->GetCharacterMovement()->JumpZVelocity = JumpVelocityZ;
			EnemyAgent->Jump();
		},
		0.1f,
		false
	);
	// EnemyAgent->Jump();
	return true;
}
