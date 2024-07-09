// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimBasicCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	//Speed = OwnerCharacter->GetVelocity().Size2D();
	Speed = FMath::Lerp(Speed, OwnerCharacter->GetNextSpeed(), DeltaSeconds * 15.f);

	const FRotator Rotation = OwnerCharacter->GetActorRotation();

	const FRotator YawRotation(0, Rotation.Yaw, 0);

	Direction = CalculateDirection(OwnerCharacter->GetVelocity().GetSafeNormal2D(), YawRotation);

	//FString TheFloatStr = FString::SanitizeFloat(Direction);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);

	HandState = OwnerCharacter->GetHandState();
	PoseState = OwnerCharacter->GetPoseState();
	bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
}
