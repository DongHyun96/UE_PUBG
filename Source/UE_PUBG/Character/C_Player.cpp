// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Player.h"

#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

//#include "Engine"

#include "Character/Component/C_InputComponent.h"


AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	MyInputComponent = CreateDefaultSubobject<UC_InputComponent>("MyInputComponent");


}

void AC_Player::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController != nullptr)
	{
		//if (UEnhancedInputLocalPlayerSubsystem)
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (IsValid(SubSystem))
		{
			SubSystem->AddMappingContext(MyInputComponent->MappingContext, 0);
		}
	}
}

void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTurnInPlace();
}

void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyInputComponent->BindAction(PlayerInputComponent, this);

}

void AC_Player::Move(const FInputActionValue& Value)
{
	// 움직일 땐 카메라가 바라보는 방향으로 몸체도 돌려버림 (수업 기본 StrafeOn 세팅)
	if (bIsHoldDirection)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		bUseControllerRotationYaw = false;

	}
	else
	{

		bUseControllerRotationYaw = true; 
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation;


		//if (CurWeaponType == EWeaponType::UNARMED)
		//	Rotation = Controller->GetControlRotation();
		//else
			//Rotation = GetActorRotation();

		//Rotation = Controller->GetControlRotation();
		Rotation = GetActorRotation();

		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);

		NextSpeed = GetCharacterMovement()->MaxWalkSpeed; // AnimCharacter에서 Speed Lerp할 값 setting
	}

}

void AC_Player::MoveEnd(const FInputActionValue& Value)
{
	NextSpeed = 0.f;

	SetStrafeRotationToIdleStop();
}

void AC_Player::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AC_Player::Walk(const FInputActionValue& Value)
{
	//StatComponent->bIsWalking = !StatComponent->bIsWalking;

	//if (StatComponent->bIsWalking)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	//}
	//else
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	//}
}

void AC_Player::Crouch()
{
	if (PoseState == EPoseState::CROUCH)
	{
		PoseState = EPoseState::STAND;
	}
	else
	{
		PoseState = EPoseState::CROUCH;
	}
}

void AC_Player::Crawl()
{
	if (PoseState == EPoseState::CRAWL)
	{
		PoseState = EPoseState::STAND;
	}
	else
	{
		PoseState = EPoseState::CRAWL;
	}
}

void AC_Player::OnJump()
{
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
}

void AC_Player::HoldDirection()
{
	bIsHoldDirection = true;
}

void AC_Player::ReleaseDirection()
{
	bIsHoldDirection = false;
	if (Controller)
	{
		FRotator NewRotation;
		NewRotation.Yaw = GetActorRotation().Yaw;
		NewRotation.Pitch = GetActorRotation().Pitch;
		NewRotation.Roll = Controller->GetControlRotation().Roll;
		Controller->SetControlRotation(NewRotation);
	}
}

void AC_Player::HandleTurnInPlace() // Update함수 안에 있어서 좀 계속 호출이 되어서 버그가 있는듯?
{
	// 현재 멈춰있는 상황이 아니면 처리 x
	if (GetVelocity().Size() > 0.f) return;

	float Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;

	// Debugging
	//FString TheFloatStr = FString::SanitizeFloat(Delta);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);

	if (Delta > 90.f) // Right Turn in place motion
	{
		// Controller
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState와 PoseState에 따른 Right Montage Animation
		UAnimMontage* RightMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(RightMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(RightMontage)) return;

		PlayAnimMontage(RightMontage);

	}
	else if (Delta < -90.f) // Left Turn in place motion
	{
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState와 PoseState에 따른 Left Montage Animation
		UAnimMontage* LeftMontage = TurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!LeftMontage) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LeftMontage)) return;
		
		PlayAnimMontage(LeftMontage);
	}

}

void AC_Player::SetStrafeRotationToIdleStop()
{
	GetCharacterMovement()->bUseControllerDesiredRotation	= false;
	GetCharacterMovement()->bOrientRotationToMovement		= true;

	bUseControllerRotationYaw = false;
}
