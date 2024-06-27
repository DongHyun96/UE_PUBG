// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Player.h"

#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyInputComponent->BindAction(PlayerInputComponent, this);

}

void AC_Player::Move(const FInputActionValue& Value)
{

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation;


		//if (CurWeaponType == EWeaponType::UNARMED)
		//	Rotation = Controller->GetControlRotation();
		//else
			Rotation = GetActorRotation();

		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);

	}

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
