// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimBasicCharacter.h"

#include "Character/C_BasicCharacter.h"

void UC_AnimBasicCharacter::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(TryGetPawnOwner());
}

void UC_AnimBasicCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter) return;

	Speed = OwnerCharacter->GetVelocity().Size2D();

	//bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();

	//const FRotator Rotation = (OwnerCharacter->GetCurWeaponType() == EWeaponType::UNARMED) ?
	//	OwnerCharacter->Controller->GetControlRotation() : OwnerCharacter->GetActorRotation();

	const FRotator Rotation = OwnerCharacter->GetActorRotation();

	const FRotator YawRotation(0, Rotation.Yaw, 0);

	Direction = CalculateDirection(OwnerCharacter->GetVelocity().GetSafeNormal2D(), YawRotation);

	HandState = OwnerCharacter->GetHandState();
	PoseState = OwnerCharacter->GetPoseState();
}
