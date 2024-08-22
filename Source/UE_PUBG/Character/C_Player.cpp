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
#include "GameFramework/SpringArmComponent.h"

#include "Utility/C_Util.h"


AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	MyInputComponent = CreateDefaultSubobject<UC_InputComponent>("MyInputComponent");

	InitTurnAnimMontageMap();

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->InitSphereRadius(100.0f); // Ž�� �ݰ� ����
	DetectionSphere->SetupAttachment(RootComponent);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_Player::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_Player::OnOverlapEnd);
	AimSpringArmTemp = CreateDefaultSubobject<USpringArmComponent>("AimSpringArm");
	//AimSpringArmTemp->SetupAttachment(RootComponent);
	AimSpringArmTemp->SetupAttachment(GetMesh());
	AimCamera = CreateDefaultSubobject<UCameraComponent>("AimCamera");
	AimCamera->SetupAttachment(AimSpringArmTemp);

}

void AC_Player::BeginPlay()
{
	Super::BeginPlay();
	AimCamera->SetActive(false);

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

	MainCamOriginLocalLocation = MainCamera->GetRelativeLocation();
	MainCamOriginLocalRotation = MainCamera->GetRelativeRotation();
}

void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimCamera->SetWorldRotation(GetControlRotation());

	HandleTurnInPlace();
	HandleControllerRotation(DeltaTime);

	HandleCameraAimPunching();

	HandleAimPressCameraLocation();

}

void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyInputComponent->BindAction(PlayerInputComponent, this);

}

void AC_Player::Move(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	//Turn In Place�� �����̸� Tunr In place ��Ÿ�� ���� �ش� �������� �ٷ� �����̰� �ϱ�
	CancelTurnInPlaceMotion();

	// ������ �� ī�޶� �ٶ󺸴� �������� ��ü�� �������� (���� �⺻ StrafeOn ����)
	//Alt Ű �������ƴҋ� �����ؼ� ����
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

		NextSpeed = GetCharacterMovement()->MaxWalkSpeed; // AnimCharacter���� Speed Lerp�� �� setting
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
		GetCharacterMovement()->MaxWalkSpeed = 600;

	}
	else
	{
		PoseState = EPoseState::CROUCH;
		GetCharacterMovement()->MaxWalkSpeed = 200;
		

	}
}

void AC_Player::Crawl()
{
	if (!bCanMove) return;

	if (PoseState == EPoseState::CRAWL)
	{
		PoseState = EPoseState::STAND;
		GetCharacterMovement()->MaxWalkSpeed = 600;

	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 100;

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
		GetCharacterMovement()->MaxWalkSpeed = 200;

		return;
	}

	if (PoseState == EPoseState::CROUCH)
	{
		PoseState = EPoseState::STAND;
		GetCharacterMovement()->MaxWalkSpeed = 600;

		return;
	}

	bPressedJump = true;
	bIsJumping = true;
	JumpKeyHoldTime = 0.0f;
}

void AC_Player::CancelTurnInPlaceMotion()
{
	//Turn In Place�� �����̸� Tunr In place ��Ÿ�� ���� �ش� �������� �ٷ� �����̰� �ϱ�
	UAnimMontage*  RightMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState].AnimMontage;
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
	//Alt �� ������ ������ ����
		// Debugging

	if (!bIsAltPressed) return;
	if (Controller)
	{
		Controller->SetControlRotation(FMath::Lerp(Controller->GetControlRotation(), CharacterMovingDirection, DeltaTime * 10.0f));
	}
	//�������� ���Ϸ� ���̳��� ĳ���� �����̼����� ���ع�����(���� ������ �ȵǴµ� ������ ���� ��ã��)
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
/// ��ȣ�ۿ�(F)�� �����Ǵ� Ű�� ������.
/// ���� ��ȣ�ۿ���.
/// </summary>
void AC_Player::Interaction()
{
	UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), this->BPC_InvenSystemInstance->GetMaxVolume());
	FString TheFloatStr = FString::SanitizeFloat(this->BPC_InvenSystemInstance->GetMaxVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);

	FString TheFloatStr1 = FString::SanitizeFloat((float)this->BPC_InvenSystemInstance->GetCurBackPackLevel());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr1);

	//UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), NearInventory[0]);


	if (BPC_InvenSystemInstance->GetNearItems().Num() > 0)
	{
		//AC_Item* Item = *NearInventory.CreateIterator();
		AC_Item* item = BPC_InvenSystemInstance->GetNearItems()[0];
	
		//NearInventory.Add(Item);
		item->Interaction(this);
		//item->SetActorHiddenInGame(true); // Hide item from the world
		item->SetActorEnableCollision(false); // Disable collision
		BPC_InvenSystemInstance->GetNearItems().Remove(item);
		
	}
	else
	{
		UC_Util::Print("NONE");
	}
}
/// <summary>
/// �������� ĳ������ ��ó�� ���� ��.
/// </summary>
/// <param name="OverlappedComp"></param>
/// <param name="OtherActor"></param>
/// <param name="OtherComp"></param>
/// <param name="OtherBodyIndex"></param>
/// <param name="bFromSweep"></param>
/// <param name="SweepResult"></param>
void AC_Player::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FString TheFloatStr = FString::SanitizeFloat(this->BPC_InvenSystemInstance->GetCurVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);


	//AC_BackPack* OverlappedItem = Cast<AC_BackPack>(OtherActor);
	//AC_Item* OverlappedItem = CastChecked<AC_Item>(OtherActor);
	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);
	
	//UC_Util::Print(*OverlappedItem->GetName());


	//UE_LOG(LogTemp, Log, TEXT("Overlapped actor class: %s"), *OverlappedItem->GetClass()->GetName());
	//if (OverlappedItem == nullptr || OverlappedItem->GetClass() != AC_Item::StaticClass())
	//	return;
	
	if (IsValid(OverlappedItem))
	{
	
		//NearInventory.Add(OverlappedItem);
		//OverlappedItem->Interaction(this);
		UC_Util::Print("OverlappedItem");
		//UC_Util::Print(*OverlappedItem->GetName());
	
		//GetInvenComponent()->GetNearItems().Add(OverlappedItem);
		//TArray<AC_Item*> TestNearItems = GetInvenComponent()->GetNearItems();
		//TestNearItems.Add(OverlappedItem);
		BPC_InvenSystemInstance->GetNearItems().Add(OverlappedItem);
		//AC_BackPack* TestBackPack = Cast<AC_BackPack>(OverlappedItem);
		//BPC_InvenSystemInstance->GetNearItems().Add(TestBackPack);
		//NearInventory.Add(TestBackPack);
		//TLqkf dho dksemfdjrksirh TLdldldldldldldldldlldlQKf
		//if (IsValid(InvenComponent->GetNearItems()[0]))
		//	UC_Util::Print(InvenComponent->GetNearItems()[0]->GetName());
	}
	else
	{
		UC_Util::Print("No item");

		return;
	}
}
/// <summary>
/// �������� ĳ������ ���������� ����� ��.
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
		BPC_InvenSystemInstance->GetNearItems().Remove(OverlappedItem);
	}
}

void AC_Player::HandleAimPressCameraLocation()
{
	FVector RootLocation = GetActorLocation();
	FVector HeadLocation = GetMesh()->GetBoneLocation("Head" ,EBoneSpaces::ComponentSpace);
	FVector HipLocation = GetMesh()->GetBoneLocation("Hips");
	FVector NewLocation = FVector(0, 0, 0);
	NewLocation.Z += HeadLocation.Z;

	//FString TheFloatStr = FString::SanitizeFloat(RootLocation.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, *TheFloatStr);
	//TheFloatStr = FString::SanitizeFloat(HeadLocation.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, *TheFloatStr);
	//TheFloatStr = FString::SanitizeFloat(NewLocation.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	AimSpringArmTemp->SetRelativeLocation(NewLocation);
}


void AC_Player::HandleTurnInPlace() // Update�Լ� �ȿ� �־ �� ��� ȣ���� �Ǿ ���װ� �ִµ�?
{
	// ���� �����ִ� ��Ȳ�� �ƴϸ� ó�� x
	if (!bCanMove) return;

	if (GetVelocity().Size() > 0.f) return;
	if (bIsHoldDirection) return;

	float Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;

	if (Delta > 90.f) // Right Turn in place motion
	{
		// Controller
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState�� PoseState�� ���� Right Montage Animation
		FPriorityAnimMontage RightPriorityMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(RightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(RightPriorityMontage.AnimMontage)) return;

		PlayAnimMontage(RightPriorityMontage);

	}
	else if (Delta < -90.f) // Left Turn in place motion
	{
		GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		GetCharacterMovement()->bOrientRotationToMovement		= false;

		// HandState�� PoseState�� ���� Left Montage Animation
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
			// ���� static���� �δ°� �����̶�� ��
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


void AC_Player::HandleCameraAimPunching()
{
}

void AC_Player::ExecuteCameraAimPunching(FVector CamPunchingDirection, float CamPunchIntensity, float CamRotationPunchingXDelta)
{
	// TODO : ���� AimDownSight�̸� �ٸ��� ó�� (Character Animation���� ó���ؾ� �� ��)
	// TODO : Aim Down�� ���� Aim Camera�� ������ ������� ��

	IsAimPunching = true;

	//CamPunchingDestLocation = 
}


void AC_Player::SetToAimKeyPress()
{
	MainCamera->SetActive(false);
	AimCamera->SetActive(true);
	bIsAimDownSight = true;
}

void AC_Player::SetToAimDownSight()
{
	MainCamera->SetActive(false);
	AimCamera->SetActive(false);
	bIsAimDownSight = true;
}

void AC_Player::BackToMainCamera()
{
	AimCamera->SetActive(false);
	MainCamera->SetActive(true);
	bIsAimDownSight = false;
}
