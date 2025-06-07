// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InputComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_StatComponent.h"
#include "Character/Component/C_PingSystemComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Character/Component/C_PlayerController.h"

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"

#include "Character/C_Enemy.h"

#include "UE_PUBG/Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "EnhancedInputComponent.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Service/C_BTServiceIdle.h"

#include "UMG.h"

#include "Utility/C_Util.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "GameFramework/PhysicsVolume.h"
#include "HUD/C_GameOverWidget.h"
#include "HUD/C_InformWidget.h"
#include "HUD/C_MagneticFieldIndicatorWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "SkyDivingComponent/C_PlayerSkyDivingComponent.h"
#include "InputMappingContext.h"
#include "EnhancedActionKeyMapping.h"

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
	if (!IsValid(EnhancedInputComponent)) return;

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

	EnhancedInputComponent->BindAction(ToggleThrowableWheelAction, ETriggerEvent::Started, this, &UC_InputComponent::OnGKey);
	EnhancedInputComponent->BindAction(ToggleThrowableWheelAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnGKey);

	EnhancedInputComponent->BindAction(ToggleConsumableAction, ETriggerEvent::Started, this, &UC_InputComponent::OnTKey);
	EnhancedInputComponent->BindAction(ToggleConsumableAction, ETriggerEvent::Completed, this, &UC_InputComponent::OnTKey);

	// MappingContext->GetMappings().FindByKey(FEnhancedActionKeyMapping(Num1Action, EKeys::One))->Action;
}	

void UC_InputComponent::Move(const FInputActionValue& Value)
{
	if (Player->GetSwimmingComponent()->IsSwimming()) Player->SetCanMove(true);
	if (!Player->GetCanMove()) return;
	if (Player->GetIsActivatingConsumableItem() && Player->GetPoseState() == EPoseState::CRAWL) return;

	//Turn In Place중 움직이면 Turn In place 몽타주 끊고 해당 방향으로 바로 움직이게 하기
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
	else // Alt키 누르고 있지 않은 상황에서의 Move
	{
		Player->bUseControllerRotationYaw				= true;
		PlayerMovement->bUseControllerDesiredRotation	= false;
		PlayerMovement->bOrientRotationToMovement		= false;
	}

	// SkyDiving Movement 처리
	if (Player->GetMainState() == EMainState::SKYDIVING)
	{
		Player->GetPlayerSkyDivingComponent()->HandlePlayerMovement(MovementVector);
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
		Player->GetPlayerSkyDivingComponent()->OnPlayerSkyMoveEnd();

	Player->GetParkourComponent()->SetHasTryVaulting(false);
}

void UC_InputComponent::Look(const FInputActionValue& Value)
{
	if (!Player->Controller) return;
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

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
	
	Player->Jump();
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
	
	UAnimMontage* RightMontage	= Player->GetPoseTurnAnimMontage(Player->GetHandState()).RightMontages[Player->GetPoseState()].AnimMontage;
	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();

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
	AC_Gun* TempGun = Cast<AC_Gun>(Player->GetEquippedComponent()->GetCurWeapon());
	if (Player->GetIsAimDown() || Player->GetIsWatchingSight())
		TempGun->BackToMainCamera();
	
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
	OnNumKey(EWeaponSlot::MAIN_GUN);
}

void UC_InputComponent::OnNum2()
{
	OnNumKey(EWeaponSlot::SUB_GUN);
}

void UC_InputComponent::OnNum4()
{
	OnNumKey(EWeaponSlot::MELEE_WEAPON);
}

void UC_InputComponent::OnNum5()
{
	OnNumKey(EWeaponSlot::THROWABLE_WEAPON);
}

void UC_InputComponent::OnNumKey(EWeaponSlot ChangeTo)
{
	if (Player->GetEquippedComponent()->GetWeapons()[ChangeTo] && Player->GetIsActivatingConsumableItem())
	{
		// 이전에 무기를 들고 있었던 상황이었을 경우(== 잠시 Holster에 붙인 경우) 현재 무기로 바꾸기 위한 처리 필요
		if (Player->GetEquippedComponent()->GetCurWeaponType() != EWeaponSlot::NONE)
			Player->GetEquippedComponent()->SetCurWeaponTypeToNone();

		Player->GetCurActivatingConsumableItem()->CancelActivating();
		Player->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("ITEM USE INTERRUPTED");
	}

	Player->GetEquippedComponent()->ChangeCurWeapon(ChangeTo);
}

void UC_InputComponent::OnXKey()
{
	Player->GetEquippedComponent()->ToggleArmed();
}

void UC_InputComponent::OnBKey()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteBKey();
}

void UC_InputComponent::OnRKey()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteRKey();
}

void UC_InputComponent::OnMLBStarted()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_Started();
}

void UC_InputComponent::OnMLBOnGoing()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	//if (Player->GetInvenSystem()->GetIsPanelOpend()) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_OnGoing();
}

void UC_InputComponent::OnMLBCompleted()
{
	if (!IsValid(Player->GetEquippedComponent()->GetCurWeapon())) return;
	Player->GetEquippedComponent()->GetCurWeapon()->ExecuteMlb_Completed();
}

void UC_InputComponent::OnMRBStarted()
{
	if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return;

	if (IsValid(Player->GetEquippedComponent()->GetCurWeapon()))  
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

	// Consumable Item 관련 F키 - Consumable 활성화 취소
	if (Player->GetIsActivatingConsumableItem() && Player->GetCurActivatingConsumableItem())
	{
		Player->GetCurActivatingConsumableItem()->CancelActivating();
		return;
	}

	if (Player->GetCurOutLinedItem())
	{
		Player->GetCurOutLinedItem()->Interaction(Player);
		Player->GetInvenSystem()->GetInvenUI()->UpdateWidget();
	}
	
	// TODO : Consumable Item 사용 중이라면 취소 시키기
	// Testing용 ConsumableItem 작동 취소 TODO : 이 라인 지우기
	//if (IsValid(Player->ConsumableItems[Player->ConsumableIterator]))
	//	Player->ConsumableItems[Player->ConsumableIterator]->CancelActivating();
}

void UC_InputComponent::OnNKey()
{
	Player->GetHUDWidget()->ToggleMiniMapEnlarged();
}

void UC_InputComponent::OnMKey()
{
	if (GAMESCENE_MANAGER->GetCurrentHUDMode() == EHUDMode::MAINMAP)
		GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::IDLE);
	else GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::MAINMAP);
}

void UC_InputComponent::OnIKey()
{
	OnTabKey(); // Inventory 토글 기능 똑같음
}

void UC_InputComponent::OnTabKey()
{
	// Inven 켜기 / 끄기 기능

	if (GAMESCENE_MANAGER->GetCurrentHUDMode() == EHUDMode::MAINMENU) return;

	if (GAMESCENE_MANAGER->GetCurrentHUDMode() == EHUDMode::INVEN)
		GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::IDLE);
	else
	{
		GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::INVEN);

		AC_Gun* CurGun = Cast<AC_Gun>(Player->GetEquippedComponent()->GetCurWeapon());
		if (IsValid(CurGun))
		{
			CurGun->BackToMainCamera();
		}
	}
}

void UC_InputComponent::OnGKey()
{
	Player->ToggleThrowablegWeaponWheel();
}

void UC_InputComponent::OnTKey()
{
	Player->ToggleConsumableWheel();
}
