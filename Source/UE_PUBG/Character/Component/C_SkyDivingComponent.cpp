// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Utility/C_Util.h"

UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_SkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UC_SkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerCharacter->GetCharacterMovement()->IsFlying())
		UC_Util::Print("Flying");
}

void UC_SkyDivingComponent::HandlePlayerMovement(const FVector2D& MovementVector)
{
	if (MovementVector.X == -1)  // 뒷방향 input 처리하지 않기 or 낙하속도 줄이기
	{
		//OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, -300.f);
		return;
	}

	// Update MaxWalkSpeed
	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
	OwnerMovement->MaxWalkSpeed = 2000.f;

	//Player->bUseControllerRotationYaw = true;
	//PlayerMovement->bUseControllerDesiredRotation = false;
	//PlayerMovement->bOrientRotationToMovement = false;

	if (!OwnerCharacter->Controller) return;

	/*FRotator Rotation = (OwnerPlayer->GetIsHoldDirection() || OwnerPlayer->GetIsAltPressed()) ?
		FRotator(0.f, OwnerPlayer->GetActorRotation().Yaw, 0.f) : OwnerPlayer->GetController()->GetControlRotation();*/

	//const FRotator Rotation = FRotator(0, OwnerCharacter->GetActorRotation().Yaw, 0);
	const FRotator Rotation = OwnerCharacter->GetController()->GetControlRotation();

	//OwnerCharacter->GetController()->GetControlRotation();


	const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
	const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	OwnerCharacter->AddMovementInput(ForwardDirection, MovementVector.X);
	OwnerCharacter->AddMovementInput(RightDirection, MovementVector.Y);
	OwnerCharacter->SetNextSpeed(OwnerMovement->MaxWalkSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
}

void UC_SkyDivingComponent::OnSkyMoveEnd()
{
	FVector Velocity = OwnerCharacter->GetCharacterMovement()->Velocity;
	Velocity.X = 0.f;
	Velocity.Y = 0.f;
	
	OwnerCharacter->GetCharacterMovement()->Velocity = Velocity;
	//OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, -300.f);
	//OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;

}

void UC_SkyDivingComponent::SetSkyDivingState(ESkyDivingState InSkyDivingState)
{
	SkyDivingState = InSkyDivingState;

	switch (SkyDivingState)
	{
	case ESkyDivingState::READY:
		// TODO : Player일 경우 비행기 위치로 카메라 잡기
		OwnerCharacter->SetActorHiddenInGame(true);
		return;
	case ESkyDivingState::SKYDIVING:
	{

		// TODO : 비행기 위치에 Character spawn(Visibility on) & Player일 경우 MainCamera Player로 다시 잡아주기
		OwnerCharacter->SetActorHiddenInGame(false);

		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

		OwnerMovement->AirControl   = 1.f;
		OwnerMovement->GravityScale = 0.2f;

		//OwnerMovement->AirControlBoostMultiplier = 10.0f;
		//OwnerMovement->MaxFlySpeed = 1000.f;
		//OwnerMovement->BrakingDecelerationFlying = 2048.f;
		return;
	}
	case ESkyDivingState::PARACHUTING:
		return;
	case ESkyDivingState::LANDING:
	{
		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
		OwnerMovement->AirControl   = 0.f;
		OwnerMovement->GravityScale = 1.f;
		return;
	}
	default: return;
		
	}
}


