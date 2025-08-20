// Fill out your copyright notice in the Description page of Project Settings.


#include "C_SpectatorPawn.h"

#include "Utility/C_Util.h"


AC_SpectatorPawn::AC_SpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_SpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AC_SpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_SpectatorPawn::MoveRight(float Val)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// 현재 눌린 키 확인
		if (PC->IsInputKeyDown(EKeys::Left) || PC->IsInputKeyDown(EKeys::Right))
			return; // 방향키 입력은 무시

		// 나머지는 정상 처리 (W/S 등)
		FRotator const ControlSpaceRot = PC->GetControlRotation();

		// transform to world space and add it
		AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::Y ), Val );
	}
}

void AC_SpectatorPawn::MoveForward(float Val)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC->IsInputKeyDown(EKeys::Up) || PC->IsInputKeyDown(EKeys::Down))
			return; // 방향키 입력은 무시
		
		FRotator const ControlSpaceRot = PC->GetControlRotation();

		// transform to world space and add it
		AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::X ), Val );
	}
}

void AC_SpectatorPawn::TurnAtRate(float Rate)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// 현재 눌린 키 확인
		if (PC->IsInputKeyDown(EKeys::Left) || PC->IsInputKeyDown(EKeys::Right))
			return; // 방향키 입력은 무시
	}
	
	Super::TurnAtRate(Rate);
}

/*void AC_SpectatorPawn::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::Y ), Val );
		}
	}
}

void AC_SpectatorPawn::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::X ), Val );
		}
	}
}*/

