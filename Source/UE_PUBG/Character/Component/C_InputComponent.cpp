// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InputComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_StatComponent.h"
#include "Character/Component/C_PingSystemComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_SkyDivingComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_ParkourComponent.h"

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"

#include "UE_PUBG/Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "EnhancedInputComponent.h"

#include "Utility/C_Util.h"

#include "GameFramework/PhysicsVolume.h"

// Sets default values for this component's properties
UC_InputComponent::UC_InputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_InputComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UC_InputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UC_InputComponent::BindAction(UInputComponent* PlayerInputComponent, AC_Player* InPlayer)
{
	Player			= InPlayer;
	PlayerMovement	= Player->GetCharacterMovement();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UC_InputComponent::OnJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Ongoing, this, &UC_InputComponent::OnSwimmingJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnSwimmingJumpCrouchEnd);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UC_InputComponent::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &UC_InputComponent::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UC_InputComponent::Look);

		EnhancedInputComponent->BindAction(CrawlAction, ETriggerEvent::Started, this, &UC_InputComponent::Crawl);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started,   this, &UC_InputComponent::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Ongoing,   this, &UC_InputComponent::OnSwimmingCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnSwimmingJumpCrouchEnd);


		EnhancedInputComponent->BindAction(HoldDirectionAction, ETriggerEvent::Triggered, this, &UC_InputComponent::HoldDirection);
		EnhancedInputComponent->BindAction(HoldDirectionAction, ETriggerEvent::Completed, this, &UC_InputComponent::ReleaseDirection);

		EnhancedInputComponent->BindAction(Num1Action, ETriggerEvent::Started, this, &UC_InputComponent::OnNum1);
		EnhancedInputComponent->BindAction(Num2Action, ETriggerEvent::Started, this, &UC_InputComponent::OnNum2);
		EnhancedInputComponent->BindAction(Num4Action, ETriggerEvent::Started, this, &UC_InputComponent::OnNum4);
		EnhancedInputComponent->BindAction(Num5Action, ETriggerEvent::Started, this, &UC_InputComponent::OnNum5);

		EnhancedInputComponent->BindAction(XAction, ETriggerEvent::Started,     this, &UC_InputComponent::OnXKey);
																	    
		EnhancedInputComponent->BindAction(BAction, ETriggerEvent::Started,		this, &UC_InputComponent::OnBKey);
		EnhancedInputComponent->BindAction(RAction, ETriggerEvent::Started,     this, &UC_InputComponent::OnRKey);

		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Started,   this, &UC_InputComponent::OnMLBStarted);
		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Ongoing,   this, &UC_InputComponent::OnMLBOnGoing);
		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnMLBCompleted);

		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Started,   this, &UC_InputComponent::OnMRBStarted);
		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Ongoing,   this, &UC_InputComponent::OnMRBOnGoing);
		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnMRBCompleted);

		EnhancedInputComponent->BindAction(MMBAction, ETriggerEvent::Started, this, &UC_InputComponent::OnMMBStarted);

		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this, &UC_InputComponent::OnFKey);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &UC_InputComponent::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnSprintReleased);

		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &UC_InputComponent::OnWalkStarted);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnWalkReleased);

		EnhancedInputComponent->BindAction(NKeyAction, ETriggerEvent::Started, this, &UC_InputComponent::OnNKey);
		EnhancedInputComponent->BindAction(MKeyAction, ETriggerEvent::Started, this, &UC_InputComponent::OnMKey);

		EnhancedInputComponent->BindAction(IKeyAction, ETriggerEvent::Started, this, &UC_InputComponent::OnIKey);

		EnhancedInputComponent->BindAction(TabKeyAction, ETriggerEvent::Started, this, &UC_InputComponent::OnTabKey);	

	}
}	

void UC_InputComponent::Move(const FInputActionValue& Value)
{
	if (Player->GetSwimmingComponent()->IsSwimming()) Player->SetCanMove(true);
	if (!Player->GetCanMove()) return;

	//Turn In Place중 움직이면 Tunr In place 몽타주 끊고 해당 방향으로 바로 움직이게 하기
	CancelTurnInPlaceMotion();

	// 움직일 땐 카메라가 바라보는 방향으로 몸체도 돌려버림 (수업 기본 StrafeOn 세팅)
	//Alt 키 누를때아닐떄 구분해서 설정

	FVector2D MovementVector = Value.Get<FVector2D>();

	Player->GetParkourComponent()->SetHasTryVaulting(MovementVector);

	if (Player->GetSwimmingComponent()->IsSwimming())
	{
		Player->GetSwimmingComponent()->HandlePlayerMovement(MovementVector);
		return;
	}


	if (Player->GetIsHoldDirection() || Player->GetIsAltPressed() || Player->GetIsAimDown()) //GetIsAimDown() -> bIsAimDownSight
	{
		PlayerMovement->bUseControllerDesiredRotation	= false;
		PlayerMovement->bOrientRotationToMovement		= false;
		Player->bUseControllerRotationYaw				= false;
	}
	else
	{
		Player->bUseControllerRotationYaw				= true;
		PlayerMovement->bUseControllerDesiredRotation	= false;
		PlayerMovement->bOrientRotationToMovement		= false;
	}

	if (Player->GetMainState() == EMainState::SKYDIVING)
	{
		Player->GetSkyDivingComponent()->HandlePlayerMovement(MovementVector);
		return;
	}

	// Update Max walk speed
	Player->UpdateMaxWalkSpeed(MovementVector);

	if (Player->Controller != nullptr)
	{
		const FRotator Rotation = FRotator(0, Player->GetActorRotation().Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		Player->AddMovementInput(ForwardDirection, MovementVector.X);
		Player->AddMovementInput(RightDirection, MovementVector.Y);

		Player->SetNextSpeed(PlayerMovement->MaxWalkSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
	}
}

void UC_InputComponent::MoveEnd(const FInputActionValue& Value)
{
	Player->SetNextSpeed(0.f);

	Player->SetStrafeRotationToIdleStop();

	if (Player->GetSwimmingComponent()->IsSwimming())
		Player->GetSwimmingComponent()->OnSwimmingMoveEnd();

	if (Player->GetMainState() == EMainState::SKYDIVING)
		Player->GetSkyDivingComponent()->OnSkyMoveEnd();

	Player->GetParkourComponent()->SetHasTryVaulting(false);
}

void UC_InputComponent::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Player->Controller != nullptr)
	{
		Player->AddControllerYawInput(LookAxisVector.X);
		Player->AddControllerPitchInput(LookAxisVector.Y);
		//TODO : Aim Down Sight 일 때 메쉬 숨기기
		//if (PoseState == EPoseState::CRAWL)
		//{
		//if (GetControlRotation().Pitch >= 350.f && GetControlRotation().Pitch < 360.f)
		//	{
		//		GetMesh()->SetOwnerNoSee(true);
		//		UC_Util::Print(float(GetControlRotation().Pitch));
		//	}
		//	else
		//		GetMesh()->SetOwnerNoSee(false);


		//}
	}
}

void UC_InputComponent::Crouch()
{
	if (Player->GetSwimmingComponent()->IsSwimming()) return;
	//if (Player->GetParkourComponent()->GetIsCurrentlyWarping()) return;

	switch (Player->GetPoseState())
	{
	case EPoseState::STAND: // Stand to crouch (Pose transition 없이 바로 처리)
		Player->SetPoseState(EPoseState::STAND, EPoseState::CROUCH);
		return;
	case EPoseState::CROUCH: // Crouch to stand (Pose transition 없이 바로 처리)
		Player->SetPoseState(EPoseState::CROUCH, EPoseState::STAND);
		return;
	case EPoseState::CRAWL: // Crawl to crouch
		Player->SetPoseState(EPoseState::CRAWL, EPoseState::CROUCH);
		return;
	case EPoseState::POSE_MAX: default:
		UC_Util::Print("From AC_Player::Crouch : UnAuthorized current pose!");
		return;
	}
}

void UC_InputComponent::OnSwimmingCrouch()
{
	if (!Player->GetSwimmingComponent()->IsSwimming()) return;
	Player->GetSwimmingComponent()->OnSwimmingCKey();
}

void UC_InputComponent::Crawl()
{
	//if (Player->GetParkourComponent()->GetIsCurrentlyWarping()) return;

	switch (Player->GetPoseState())
	{
	case EPoseState::STAND: // Stand to Crawl
		Player->SetPoseState(EPoseState::STAND, EPoseState::CRAWL);
		return;
	case EPoseState::CROUCH: // Crouch to Crawl
		Player->SetPoseState(EPoseState::CROUCH, EPoseState::CRAWL);
		return;
	case EPoseState::CRAWL: // Crawl to Stand
		Player->SetPoseState(EPoseState::CRAWL, EPoseState::STAND);
		return;
	case EPoseState::POSE_MAX: default:
		UC_Util::Print("From AC_Player::Crawl : UnAuthorized current pose!");
		return;
	}
}

void UC_InputComponent::OnJump()
{
	if (!Player->GetCanMove())									return;
	if (Player->GetIsJumping() || PlayerMovement->IsFalling())	return;
	if (Player->GetSwimmingComponent()->IsSwimming())			return;
	//if (Player->GetParkourComponent()->GetIsCurrentlyWarping()) return;

	CancelTurnInPlaceMotion();

	// 파쿠르 Action에 성공했다면 return
	if (Player->GetParkourComponent()->TryExecuteParkourAction()) return;

	if (Player->GetPoseState() == EPoseState::CRAWL) // Crawl to crouch
	{
		SetToNonAimCamera();
		Player->SetPoseState(EPoseState::CRAWL, EPoseState::CROUCH);
		return;
	}

	if (Player->GetPoseState() == EPoseState::CROUCH) // Crouch to stand
	{
		Player->SetPoseState(EPoseState::CROUCH, EPoseState::STAND);
		return;
	}
	SetToNonAimCamera();
	//Player->GetPressedJum
	Player->bPressedJump = true;
	Player->SetIsJumping(true);
	Player->JumpKeyHoldTime = 0.0f;
}

void UC_InputComponent::OnSwimmingJump()
{
	if (!Player->GetSwimmingComponent()->IsSwimming()) return;
	Player->GetSwimmingComponent()->OnSwimmingSpaceBarKey();
}

void UC_InputComponent::OnSwimmingJumpCrouchEnd()
{
	if (!Player->GetSwimmingComponent()->IsSwimming()) return;
	MoveEnd(0.f);
}

void UC_InputComponent::CancelTurnInPlaceMotion()
{
	//Turn In Place중 움직이면 Turn In place 몽타주 끊고 해당 방향으로 바로 움직이게 하기
	
	UAnimMontage* RightMontage = Player->GetPoseTurnAnimMontage(Player->GetHandState()).RightMontages[Player->GetPoseState()].AnimMontage;
	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();

	if (!AnimInstance) return;

	if (!IsValid(RightMontage)) return;

	if (AnimInstance->Montage_IsPlaying(RightMontage))
	{ 
		Player->SetStrafeRotationToIdleStop();
		AnimInstance->Montage_Stop(0.2f, RightMontage);
	}

	UAnimMontage* LeftMontage = Player->GetPoseTurnAnimMontage(Player->GetHandState()).LeftMontages[Player->GetPoseState()].AnimMontage;
	if (!IsValid(LeftMontage)) return;

	if (AnimInstance->Montage_IsPlaying(LeftMontage))
	{
		Player->SetStrafeRotationToIdleStop();
		AnimInstance->Montage_Stop(0.2f, LeftMontage);
	}

	// Lower body part도 확인
	if (!Player->GetLowerBodyTurnAnimMontageMap().Contains(Player->GetHandState())) return;

	UAnimMontage* LowerRightMontage = Player->GetLowerPoseTurnAnimMontage(Player->GetHandState()).RightMontages[Player->GetPoseState()].AnimMontage;
	if (!IsValid(LowerRightMontage)) return;

	if (AnimInstance->Montage_IsPlaying(LowerRightMontage))
	{
		Player->SetStrafeRotationToIdleStop();
		AnimInstance->Montage_Stop(0.2f, LowerRightMontage);
	}

	UAnimMontage* LowerLeftMontage = Player->GetLowerPoseTurnAnimMontage(Player->GetHandState()).LeftMontages[Player->GetPoseState()].AnimMontage;
	if (!IsValid(LowerLeftMontage)) return;

	if (AnimInstance->Montage_IsPlaying(LowerLeftMontage))
	{
		Player->SetStrafeRotationToIdleStop();
		AnimInstance->Montage_Stop(0.2f, LowerLeftMontage);
	}
}

void UC_InputComponent::HoldDirection()
{
	// 수류탄 던지는 process 중이라면 Alt키 중지
	if (Player->GetIsFiringBullet()) return;

	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Player->GetEquippedComponent()->GetCurWeapon());
	if (ThrowingWeapon)
		if (ThrowingWeapon->GetIsOnThrowProcess())
		{
			Player->SetIsHoldDirection(false);
			Player->SetIsAltPressed(false);
			return;
		}

	Player->SetIsHoldDirection(true);
	Player->SetIsAltPressed(false);
	if (Player->Controller)
	{
		//FRotator NewRotation;
		FRotator CharacterMovingDirection{};
		CharacterMovingDirection.Yaw	= Player->GetActorRotation().Yaw;
		CharacterMovingDirection.Pitch	= Player->GetActorRotation().Pitch;
		CharacterMovingDirection.Roll	= Player->Controller->GetControlRotation().Roll;

		Player->SetCharacterMovingDirection(CharacterMovingDirection);

		//Controller->SetControlRotation(NewRotation);	
	}
}

void UC_InputComponent::ReleaseDirection()
{
	if (Player->GetIsFiringBullet()) return;

	Player->SetIsHoldDirection(false);
	Player->SetIsAltPressed(true);
}

void UC_InputComponent::SetToNonAimCamera()
{

	if (Player->GetIsAimDown()) // bIsAimDownSight
	{
		AC_Gun* CurGun = Cast<AC_Gun>(Player->GetEquippedComponent()->GetCurWeapon());
		if (IsValid(CurGun))
		{
			CurGun->BackToMainCamera();
			Player->SetIsAimDown(false);
		}
	}
}

void UC_InputComponent::OnNum1()
{
	Player->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::MAIN_GUN);
	Player->SetRecoilTimelineValues();
}

void UC_InputComponent::OnNum2()
{
	// Testing 용 Boosting TODO : 이 라인 지우기
	Player->GetStatComponent()->AddBoost(40.f);

	Player->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::SUB_GUN);
	Player->SetRecoilTimelineValues();

}

void UC_InputComponent::OnNum4()
{
	Player->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::MELEE_WEAPON);
}

void UC_InputComponent::OnNum5()
{
	Player->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON);
}

void UC_InputComponent::OnXKey()
{
	// Testing 용 Damage 주기 TODO : 이 라인 지우기
	//TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor * DamageCauser);
	//static bool SwimFlag = false;

	/*
	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = true;

	bIsSwimming = true;

	OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorUpVector() * 0.005f, false, false);
	*/

	//if (!SwimFlag)
	//{
	//	Player->GetPhysicsVolume()->bWaterVolume = true;
	//	Player->LaunchCharacter(Player->GetActorUpVector() * 0.005f, false, false);
	//}
	//else
	//{
	//	Player->GetPhysicsVolume()->bWaterVolume = false;
	//}
	//
	//SwimFlag = !SwimFlag;

	// For testing
	Player->GetParkourComponent()->Vault();
	
	Player->GetStatComponent()->TakeDamage(10.f, EDamagingPartType::HEAD, Player);
	Player->GetEquippedComponent()->ToggleArmed();
}

void UC_InputComponent::OnBKey()
{
	// Testing 용 Heal 주기 TODO : 이 라인 지우기
	if (IsValid(Player->ConsumableItems[Player->ConsumableIterator])) 
		Player->ConsumableItems[Player->ConsumableIterator]->StartUsingConsumableItem(Player);

	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteBKey();
}

void UC_InputComponent::OnRKey()
{
	// Testing용 ConsumableItem 작동 취소 TODO : 이 라인 지우기
	if (IsValid(Player->ConsumableItems[Player->ConsumableIterator]))
		Player->ConsumableItems[Player->ConsumableIterator]->CancelActivating();

	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteRKey();
}

void UC_InputComponent::OnMLBStarted()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_Started();
}

void UC_InputComponent::OnMLBOnGoing()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_OnGoing();
}

void UC_InputComponent::OnMLBCompleted()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_Completed();
}

void UC_InputComponent::OnMRBStarted()
{
	UC_Util::Print("Switching Consumable");

	// Test용 Consumable switching
	if (Player->ConsumableIterator >= Player->ConsumableItems.Num() - 1) Player->ConsumableIterator = 0;
	else Player->ConsumableIterator++;
		
	
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMrb_Started();
}

void UC_InputComponent::OnMRBOnGoing()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMrb_OnGoing();
}

void UC_InputComponent::OnMRBCompleted()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMrb_Completed();
}

void UC_InputComponent::OnMMBStarted()
{
	Player->GetPingSystemComponent()->OnMMBStarted();
}

void UC_InputComponent::OnSprintStarted()
{
	Player->SetIsSprinting(true);
}

void UC_InputComponent::OnSprintReleased()
{
	Player->SetIsSprinting(false);
}

void UC_InputComponent::OnWalkStarted()
{
	Player->SetIsWalking(true);
}

void UC_InputComponent::OnWalkReleased()
{
	Player->SetIsWalking(false);
}

/// <summary>
/// 상호작용(F)와 대응되는 키로 구상중.
/// 봇도 상호작용함.
/// </summary>
void UC_InputComponent::OnFKey()
{
	//UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), this->Inventory->GetMaxVolume());
	//FString TheFloatStr = FString::SanitizeFloat(this->Inventory->GetMaxVolume());
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);
	//
	//FString TheFloatStr1 = FString::SanitizeFloat((float)this->Inventory->GetCurBackPackLevel());
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr1);

	//UE_LOG(LogTemp, Log, TEXT("Max Volume: %d"), NearInventory[0]);

	// SkyDiving 관련 F키
	if (Player->GetMainState() == EMainState::SKYDIVING)
	{
		if (Player->GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::READY)
			Player->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::SKYDIVING);
		else if (Player->GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::SKYDIVING)
			Player->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::PARACHUTING);
		return;
	}

	if (Player->GetInventory()->GetNearItems().Num() > 0)
	{
		//AC_Item* Item = *NearInventory.CreateIterator();
		AC_Item* item = Player->GetInventory()->GetNearItems()[0];

		//NearInventory.Add(Item);
		//item->Interaction(Player);
		//item->SetActorHiddenInGame(true); // Hide item from the world
		item->SetActorEnableCollision(false); // Disable collision
		Player->GetInventory()->GetNearItems().Remove(item);

	}
	else
	{
		UC_Util::Print("NONE");
	}
}

void UC_InputComponent::OnNKey()
{
	static bool Flag{};
	
	if (!Flag) ChangeTestWeapon = Player->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	else Player->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ChangeTestWeapon);
	
	Flag = !Flag;

	Player->GetHUDWidget()->ToggleMiniMapEnlarged();
}

void UC_InputComponent::OnMKey()
{
	Player->GetHUDWidget()->GetMainMapWidget()->OnMKey();
}

void UC_InputComponent::OnIKey()
{
	//Player->GetInvenComponent()->OpenInvenUI();
	Player->GetInvenSystem()->OpenInvenUI();
}

void UC_InputComponent::OnTabKey()
{
	Player->GetInvenSystem()->OpenInvenUI();
}

