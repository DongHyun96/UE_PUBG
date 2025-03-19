// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_CustomNavLinkProxy.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/Velocity/ParticleModuleVelocityOverLifetime.h"
#include "Utility/C_Util.h"

AC_CustomNavLinkProxy::AC_CustomNavLinkProxy()
{
	// PrimaryActorTick.bCanEverTick = true;
}

void AC_CustomNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();
}	

void AC_CustomNavLinkProxy::OnReceiveSmartLinkReached(AActor* Agent, const FVector& Destination)
{
	UC_Util::Print("OnReceiveSmartLinkReached", FColor::MakeRandomColor(), 10.f);
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Agent);
	if (!Enemy) return;

	if (PointLinks.IsEmpty())
	{
		UC_Util::Print("From AC_CustomNavLinkProxy::OnReceiveSmartLinkReached : PointLinks is empty!", FColor::Red, 10.f);
		return;
	}
	
	// 방향 확인하기
	FVector LeftWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Left);
	float LeftToDest			= FVector::Distance(LeftWorldLocation, Destination);
	FVector RightWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Right);
	float RightToDest			= FVector::Distance(RightWorldLocation, Destination);
	
	EDirection CurDirection = (LeftToDest > RightToDest) ? EDirection::LEFT_TO_RIGHT : EDirection::RIGHT_TO_LEFT;

	FVector StartLocation = (CurDirection == EDirection::LEFT_TO_RIGHT) ? LeftWorldLocation : RightWorldLocation;

	UC_Util::Print((CurDirection == EDirection::RIGHT_TO_LEFT) ? "Right_To_Left" : "Left_To_Right", FColor::MakeRandomColor(), 10.f);

	switch (DirectionActionStrategies[CurDirection])
	{
	case ELinkActionStrategy::DEFAULT: case ELinkActionStrategy::MAX:
		return;
	case ELinkActionStrategy::JUMP:
		ExecuteJump(Enemy, Destination);
		return;
	case ELinkActionStrategy::PARKOUR:
	{
		// 파쿠르 할 지점으로 Enemy 회전
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation
		(
		Enemy->GetActorLocation(),
		Destination
		);

		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;
		Enemy->SetActorRotation(LookAtRotation);

		// 파쿠르를 정확히 시도하기 위해 파쿠르 시작 지점으로 위치 지정
		Enemy->SetActorLocation(StartLocation);
		
		FTimerHandle TimerHandle{};
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle,
			[this, Enemy, Destination]()
			{
				ExecuteParkour(Enemy, Destination);
			},
			0.2f,
			false
		);
		// ExecuteParkour(Enemy, Destination);
	}
		return;
	}
}

bool AC_CustomNavLinkProxy::ExecuteJump(class AC_Enemy* Enemy, const FVector& Destination)
{
	UC_Util::Print("Execute Jump", FColor::Red, 10.f);

	// 문제 : 말 그대로 Launch 하는 방식임, Jump를 뛰는 방식이 아님 / Enemy의 MaxWalkSpeed에 따라 비거리가 달라짐
	
	/*static const float LAUNCH_BOOST_FACTOR = 1.5f;
	
	FVector LaunchVelocity{};
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVelocity, Enemy->GetActorLocation(), Destination);
	LaunchVelocity *= LAUNCH_BOOST_FACTOR;
	FTimerHandle TimerHandle{};
	
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[Enemy, LaunchVelocity]()
		{
			Enemy->LaunchCharacter(LaunchVelocity, true, true);
		},
		0.1f,
		false
	);
	*/

	/*FVector Velocity = Enemy->GetVelocity();
	Velocity *= 5.f;	
	Enemy->GetCharacterMovement()->Velocity.X = Velocity.X;
	Enemy->GetCharacterMovement()->Velocity.Y = Velocity.Y;*/

	// TODO : 점프하기 전 Enemy 회전 및 위치 잡아야하는지 확인

	FTimerHandle TimerHandle{};

	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[Enemy]() { Enemy->Jump(); },
		0.1f,
		false
	);
	
	// Enemy->Jump();
	
	return true;
}

bool AC_CustomNavLinkProxy::ExecuteParkour(class AC_Enemy* Enemy, const FVector& Destination)
{
	// TODO : 파쿠르를 실패할 경우도 있음 -> 무조건적으로 성공시켜야 함
	bool ParkourSucceeded = Enemy->GetParkourComponent()->TryExecuteParkourAction();
	UC_Util::Print(ParkourSucceeded ? "ParkourSucceeded" : "Parkour Failed!", FColor::MakeRandomColor(), 10.f);
	return ParkourSucceeded;
}
