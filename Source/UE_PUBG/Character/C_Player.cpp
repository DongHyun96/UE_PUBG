// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Player.h"

#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Components/SphereComponent.h"

#include "Item/C_Item.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Weapon/C_Weapon.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"


AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	MyInputComponent = CreateDefaultSubobject<UC_InputComponent>("MyInputComponent");

	InitTurnAnimMontageMap();

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->InitSphereRadius(100.0f); // 탐지 반경 설정
	DetectionSphere->SetupAttachment(RootComponent);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_Player::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_Player::OnOverlapEnd);
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
	HandleControllerRotation(DeltaTime);
}

void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyInputComponent->BindAction(PlayerInputComponent, this);

}

void AC_Player::Move(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	//Turn In Place중 움직이면 Tunr In place 몽타주 끊고 해당 방향으로 바로 움직이게 하기
	CancelTurnInPlaceMotion();

	// 움직일 땐 카메라가 바라보는 방향으로 몸체도 돌려버림 (수업 기본 StrafeOn 세팅)
	//Alt 키 누를때아닐떄 구분해서 설정
	if (bIsHoldDirection || bIsAltPressed)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		bUseControllerRotationYaw = false;

	}
	else
	{

		bUseControllerRotationYaw = true; 
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation;

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

void AC_Player::Sprint()
{
}

void AC_Player::Crouch()
{
	if (!bCanMove) return;

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
	if (!bCanMove) return;

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
	if (!bCanMove) return;

	CancelTurnInPlaceMotion();

	if (PoseState == EPoseState::CRAWL)
	{
		PoseState = EPoseState::CROUCH;
		return;
	}

	if (PoseState == EPoseState::CROUCH)
	{
		PoseState = EPoseState::STAND;
		return;
	}

	bPressedJump = true;
	bIsJumping = true;
	JumpKeyHoldTime = 0.0f;
}

void AC_Player::CancelTurnInPlaceMotion()
{
	//Turn In Place중 움직이면 Tunr In place 몽타주 끊고 해당 방향으로 바로 움직이게 하기
	UAnimMontage* RightMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState].AnimMontage;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(RightMontage)) return;

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(RightMontage))
		AnimInstance->Montage_Stop(0.2f);

	UAnimMontage* LeftMontage = TurnAnimMontageMap[HandState].LeftMontages[PoseState].AnimMontage;

	if (!IsValid(LeftMontage)) return;

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LeftMontage))
		AnimInstance->Montage_Stop(0.2f);
}

void AC_Player::HoldDirection()
{
	bIsHoldDirection = true;
	bIsAltPressed = false;
	if (Controller)
	{
		//FRotator NewRotation;
		CharacterMovingDirection.Yaw = GetActorRotation().Yaw;
		CharacterMovingDirection.Pitch = GetActorRotation().Pitch;
		CharacterMovingDirection.Roll = Controller->GetControlRotation().Roll;
		//Controller->SetControlRotation(NewRotation);	
	}
}

void AC_Player::ReleaseDirection()
{
	bIsHoldDirection = false;

	bIsAltPressed = true;

}

void AC_Player::HandleControllerRotation(float DeltaTime)
{
	//Alt 를 누른적 없으면 리턴
		// Debugging

	if (!bIsAltPressed) return;
	if (Controller)
	{
		Controller->SetControlRotation(FMath::Lerp(Controller->GetControlRotation(), CharacterMovingDirection, DeltaTime * 10.0f));
	}
	//일정각도 이하로 차이나면 캐릭터 로테이션으로 정해버리기(가끔 적용이 안되는데 이유를 아직 못찾음)
	float DeltaYaw = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Yaw);
	float DeltaPitch = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Pitch);


	if (DeltaYaw < 5 && DeltaPitch< 5)
	{
		Controller->SetControlRotation(CharacterMovingDirection);
		bIsAltPressed = false;
	}
}

void AC_Player::OnNum1()
{
	EquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN);
}

void AC_Player::OnNum2()
{
	EquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN);
}

void AC_Player::OnNum4()
{
	EquippedComponent->ChangeCurWeapon(EWeaponSlot::MELEE_WEAPON);
}

void AC_Player::OnNum5()
{
	EquippedComponent->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON);
}

void AC_Player::OnXKey()
{
	EquippedComponent->ToggleArmed();
}

void AC_Player::OnBKey()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteBKey();
}

void AC_Player::OnRKey()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteRKey();
}

void AC_Player::OnMLBStarted()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMlb_Started();
}

void AC_Player::OnMLBOnGoing()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMlb_OnGoing();
}

void AC_Player::OnMLBCompleted()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMlb_Completed();
}

void AC_Player::OnMRBStarted()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMrb_Started();
}

void AC_Player::OnMRBOnGoing()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMrb_OnGoing();
}

void AC_Player::OnMRBCompleted()
{
	if (!IsValid(EquippedComponent->GetCurWeapon())) return;
	EquippedComponent->GetCurWeapon()->ExecuteMrb_Completed();
}

/// <summary>
/// 상호작용(F)와 대응되는 키로 구상중.
/// 봇도 상호작용함.
/// </summary>
void AC_Player::Interaction()
{
	UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), this->InvenComponent->GetMaxVolume());
	FString TheFloatStr = FString::SanitizeFloat(this->InvenComponent->GetMaxVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);

	FString TheFloatStr1 = FString::SanitizeFloat((float)this->InvenComponent->GetCurBackPackLevel());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr1);

	//UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), NearInventory[0]);

	if (NearInventory.Num() > 0)
	{
		//AC_Item* Item = *NearInventory.CreateIterator();
		AC_Item* item = NearInventory[0];

		//NearInventory.Add(Item);
		item->Interaction(this);
		//item->SetActorHiddenInGame(true); // Hide item from the world
		item->SetActorEnableCollision(false); // Disable collision
		NearInventory.Remove(item);
		
	}
}
/// <summary>
/// 아이템이 캐릭터의 근처에 있을 때.
/// </summary>
/// <param name="OverlappedComp"></param>
/// <param name="OtherActor"></param>
/// <param name="OtherComp"></param>
/// <param name="OtherBodyIndex"></param>
/// <param name="bFromSweep"></param>
/// <param name="SweepResult"></param>
void AC_Player::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString TheFloatStr = FString::SanitizeFloat(this->InvenComponent->GetCurVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);

	//AC_BackPack* OverlappedItem = Cast<AC_BackPack>(OtherActor);
	//AC_Item* OverlappedItem = CastChecked<AC_Item>(OtherActor);
	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);
	
	//UE_LOG(LogTemp, Log, TEXT("Overlapped actor class: %s"), *OverlappedItem->GetClass()->GetName());

	if (IsValid(OverlappedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("Item overlapped: %s"), *OverlappedItem->GetName());
		NearInventory.Add(OverlappedItem);
		//OverlappedItem->Interaction(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap with non-item: %s"), *OtherActor->GetName());
		return;
	}
}
/// <summary>
/// 아이템이 캐릭터의 감지범위를 벗어났을 때.
/// </summary>
/// <param name="OverlappedComp"></param>
/// <param name="OtherActor"></param>
/// <param name="OtherComp"></param>
/// <param name="OtherBodyIndex"></param>
void AC_Player::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);

	if (OverlappedItem)
	{
		NearInventory.Remove(OverlappedItem);
	}
}


void AC_Player::HandleTurnInPlace() // Update함수 안에 있어서 좀 계속 호출이 되어서 버그가 있는듯?
{
	// 현재 멈춰있는 상황이 아니면 처리 x
	if (!bCanMove) return;

	if (GetVelocity().Size() > 0.f) return;
	if (bIsHoldDirection) return;

	float Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;

	if (Delta > 90.f) // Right Turn in place motion
	{
		// Controller
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState와 PoseState에 따른 Right Montage Animation
		FPriorityAnimMontage RightPriorityMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(RightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(RightPriorityMontage.AnimMontage)) return;

		PlayAnimMontage(RightPriorityMontage);

	}
	else if (Delta < -90.f) // Left Turn in place motion
	{
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState와 PoseState에 따른 Left Montage Animation
		FPriorityAnimMontage LeftPriorityMontage = TurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!IsValid(LeftPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LeftPriorityMontage.AnimMontage)) return;
		
		PlayAnimMontage(LeftPriorityMontage);
	}

}

void AC_Player::SetStrafeRotationToIdleStop()
{
	GetCharacterMovement()->bUseControllerDesiredRotation	= false;
	GetCharacterMovement()->bOrientRotationToMovement		= true;

	bUseControllerRotationYaw = false;
}

void AC_Player::InitTurnAnimMontageMap()
{
	for (uint8 handState = 0; handState < static_cast<uint8>(EHandState::HANDSTATE_MAX); handState++)
	{
		FPoseAnimMontage CurrenteHandStateTurnInPlaces{};

		for (uint8 poseState = 0; poseState < static_cast<uint8>(EPoseState::POSE_MAX); poseState++)
		{
			EPoseState currentPose = static_cast<EPoseState>(poseState);

			// Left side
			// 원래 static으로 두는게 정석이라고 함
			FString leftMontagePath = FString::Printf
			(
				TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/Turn_In_Place/Montages/%u%u_TurnLeft_Montage"),
				handState,
				poseState
			);

			ConstructorHelpers::FObjectFinder<UAnimMontage> TurnLeftMontage(*leftMontagePath);

			FPriorityAnimMontage PriorityLeftMontage{};

			PriorityLeftMontage.Priority	= EMontagePriority::TURN_IN_PLACE;
			PriorityLeftMontage.AnimMontage = (TurnLeftMontage.Succeeded()) ? TurnLeftMontage.Object : nullptr;
			
			CurrenteHandStateTurnInPlaces.LeftMontages.Emplace(currentPose, PriorityLeftMontage);

			// Right side
			FString rightMontagePath = FString::Printf
			(
				TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/Turn_In_Place/Montages/%u%u_TurnRight_Montage"),
				handState,
				poseState
			);
			ConstructorHelpers::FObjectFinder<UAnimMontage> TurnRightMontage(*rightMontagePath);

			FPriorityAnimMontage PriorityRightMontage{};

			PriorityRightMontage.Priority    = EMontagePriority::TURN_IN_PLACE;
			PriorityRightMontage.AnimMontage = (TurnRightMontage.Succeeded()) ? TurnRightMontage.Object : nullptr;

			CurrenteHandStateTurnInPlaces.RightMontages.Emplace(currentPose, PriorityRightMontage);
		}

		TurnAnimMontageMap.Emplace(static_cast<EHandState>(handState), CurrenteHandStateTurnInPlaces);
	}

}
