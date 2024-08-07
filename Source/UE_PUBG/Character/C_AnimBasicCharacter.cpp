// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimBasicCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/C_BasicCharacter.h"

void UC_AnimBasicCharacter::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(TryGetPawnOwner());
	//CSpineRotation = FRotator(0);
	//CHeadLookAtRotation = FQuat(0);

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
	bIsJumping = OwnerCharacter->GetIsJumping();
	bCanCharacterMove = OwnerCharacter->GetCanMove();
	bIsHoldDirection = OwnerCharacter->GetIsHoldDirection();
	bIsAimDownSight = OwnerCharacter->GetIsAimDown();
	ControlHeadRotation();
	SetAimOfssetRotation();
}

void UC_AnimBasicCharacter::AnimNotify_OnStartTransition_Stand_To_Falling()
{
	AnimNotify_OnStartTransition_RunningJump_To_Falling();
	OwnerCharacter->GetCharacterMovement()->Velocity.X = 0;
	OwnerCharacter->GetCharacterMovement()->Velocity.Y = 0;
	//OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0;
	//OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 1000000000000;
}

void UC_AnimBasicCharacter::AnimNotify_OnStartTransition_RunningJump_To_Falling()
{
	// Start Transition 시 수행할 로직 추가
	FString TheFloatStr = "Start Transition";
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	UE_LOG(LogTemp, Warning, TEXT("Transition Started"));
	OwnerCharacter->SetCanMove(false);

}

void UC_AnimBasicCharacter::AnimNotify_OnEndTransition_HardLand_To_Stand()
{
	// End Transition 시 수행할 로직 추가
	FString TheFloatStr = "End Transition";
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	UE_LOG(LogTemp, Warning, TEXT("Transition Ended"));
	OwnerCharacter->SetCanMove(true);
	AnimNotify_OnEndTransition_Falling_To_Standing();


}

void UC_AnimBasicCharacter::AnimNotify_OnEndTransition_Falling_To_Standing()
{
	OwnerCharacter->SetIsJumping(false);
	OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 600;
	OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 2048;


}

void UC_AnimBasicCharacter::ControlHeadRotation()
{
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator CapsuleRotation = OwnerCharacter->GetCapsuleComponent()->GetComponentRotation();

	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, CapsuleRotation);
	DeltaRotation.Roll  /= -5.0f;
	DeltaRotation.Pitch /= -5.0f;
	DeltaRotation.Yaw   /=  5.0f;

	DeltaRotation.Roll  = FMath::Clamp(DeltaRotation.Roll,  -90, 90);	
	DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, -90, 90);	
	DeltaRotation.Yaw   = FMath::Clamp(DeltaRotation.Yaw,   -90, 90);

	//DeltaRotation.Roll  = 0;
	//DeltaRotation.Pitch = 0;
	//DeltaRotation.Yaw   = 0;
	
	CSpineRotation = UKismetMathLibrary::RInterpTo(CSpineRotation, DeltaRotation, 1.0, 0.3);
	//CSpineRotation = DeltaRotation;

	CHeadLookAtRotation = UKismetMathLibrary::MakeRotator(CSpineRotation.Pitch, CSpineRotation.Yaw, CSpineRotation.Roll).Quaternion();
	//CHeadLookAtRotation = FQuat::Identity;
	DeltaRotation.Roll  *= 5.0f / 3.0f;
	DeltaRotation.Pitch *= 5.0f / 3.0f;
	DeltaRotation.Yaw   *= 5.0f / 3.0f;

	DeltaRotation.Roll = FMath::Clamp(DeltaRotation.Roll, -30, 30);
	DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, -30, 30);
	DeltaRotation.Yaw = FMath::Clamp(DeltaRotation.Yaw, -30, 30);
	//CSpineRotation = DeltaRotation;
	CAimingRotation.Roll = DeltaRotation.Pitch;
	CAimingRotation.Pitch = DeltaRotation.Yaw;
	CAimingRotation.Yaw = DeltaRotation.Roll;
}

void UC_AnimBasicCharacter::RilfeLeftHandIK()
{
}

void UC_AnimBasicCharacter::SetAimOfssetRotation()
{
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator CapsuleRotation = OwnerCharacter->GetCapsuleComponent()->GetComponentRotation();
	CAimOffsetRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, CapsuleRotation);

	FString TheFloatStr = FString::SanitizeFloat(CAimOffsetRotation.Yaw);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, *TheFloatStr);
	
}

