// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Player.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_PingSystemComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"

#include "Item/C_Item.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.h"

#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Curves/CurveFloat.h"

#include "Engine/PostProcessVolume.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Blueprint/UserWidget.h"

#include "Utility/C_Util.h"

#include "Styling/SlateBrush.h"
#include "Engine/Texture2D.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

#include "Item/ConsumableItem/Healing/C_FirstAidKit.h"
#include "Item/Weapon/Gun/C_Bullet.h"
#include "Singleton/C_GameSceneManager.h"

AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MyInputComponent = CreateDefaultSubobject<UC_InputComponent>("MyInputComponent");

	InitTurnAnimMontageMap();

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->InitSphereRadius(100.0f); // 탐지 반경 설정
	DetectionSphere->SetupAttachment(RootComponent);

	//DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_Player::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_Player::OnOverlapEnd);
	AimSpringArmTemp = CreateDefaultSubobject<USpringArmComponent>("AimSpringArm");
	//AimSpringArmTemp->SetupAttachment(RootComponent);
	AimSpringArmTemp->SetupAttachment(GetMesh());
	AimCamera = CreateDefaultSubobject<UCameraComponent>("AimCamera");
	AimCamera->SetupAttachment(AimSpringArmTemp);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
	//SceneCaptureComponent->bCaptureEveryFrame = false;

	PingSystemComponent = CreateDefaultSubobject<UC_PingSystemComponent>("PingSystemComponent");

	SetTimeLineComponentForMovingCamera();

}

void AC_Player::BeginPlay()
{
	Super::BeginPlay();

	//GAMESCENE_MANAGER->SetPlayer(this);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
		StatComponent->SetOwnerHUDWidget(HUDWidget);
		PingSystemComponent->SetOwnerPlayer(this);
		HUDWidget->GetMainMapWidget()->SetPlayer(this);
	}

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

	// AimPunching 돌아올 때 쓰일 Local 좌표들
	MainCamOriginLocalLocation	= MainCamera->GetRelativeLocation();
	MainCamOriginLocalRotation	= MainCamera->GetRelativeRotation();
	MainCamPunchingDestLocation = MainCamOriginLocalLocation;
	MainCamPunchingDestRotation = MainCamOriginLocalRotation;

	AimCamOriginLocalLocation	= AimCamera->GetRelativeLocation();
	AimCamOriginLocalRotation	= AimCamera->GetRelativeRotation();
	AimCamPunchingDestLocation	= AimCamOriginLocalLocation;
	AimCamPunchingDestRotation	= AimCamOriginLocalRotation;

	// 자세별 MainSpringArm 위치 초기화
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::STAND, C_MainSpringArm->GetRelativeLocation());
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CROUCH, C_MainSpringArm->GetRelativeLocation() + FVector(0.f, 0.f, -32.f));
	//MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CRAWL, C_MainSpringArm->GetRelativeLocation()  + FVector(0.f, 0.f, -99.f));
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CRAWL, C_MainSpringArm->GetRelativeLocation()  + FVector(0.f, 0.f, -20.f));

	MainSpringArmRelativeLocationDest = MainSpringArmRelativeLocationByPoseMap[EPoseState::STAND];

	// PostProcessVolume 초기화
	TArray<AActor*> PPVolumes{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), PPVolumes);

	if (!PPVolumes.IsEmpty()) PostProcessVolume = Cast<APostProcessVolume>(PPVolumes[0]);
	PostProcessInitialIntensity = PostProcessVolume->Settings.BloomIntensity;

	//ScreenShotWidget->AddToViewport();

	//if (IsValid(C_MainSpringArm))
	//{
	//	//GetCapsuleComponent()
	//	UPrimitiveComponent* MeshToIgnore = GetCapsuleComponent();
	//	if (MeshToIgnore)
	//	{
	//		// 카메라 프로브 채널에 대해 특정 메쉬와 충돌 무시 설정
	//		MeshToIgnore->SetCollisionResponseToChannel(C_MainSpringArm->ProbeChannel, ECR_Ignore);
	//	}
	//}



	//for (UActorComponent* Component : GetComponents())
	//{
	//	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component);
	//	if (PrimitiveComp)
	//	{
	//		if (IsValid(C_MainSpringArm))
	//			PrimitiveComp->SetCollisionResponseToChannel(C_MainSpringArm->ProbeChannel, ECR_Ignore);
	//		// Spring Arm의 ProbeChannel에 대한 충돌을 무시하도록 설정합니다.

	//		// 필요에 따라 다른 채널도 무시하도록 설정할 수 있습니다.
	//		// 예: PrimitiveComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	//	}
	//}

	SpawnConsumableItemForTesting();
	PoolingBullets();

}

void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UC_Util::Print(CurveFloatForSwitchCamera->GetFloatValue(GetWorld()->GetDeltaSeconds()));

	HandleTurnInPlace();
	HandleTurnInPlaceWhileAiming();
	HandlePlayerRotationWhileAiming();
	HandleControllerRotation(DeltaTime);

	HandleCameraAimPunching(DeltaTime);
	HandleFlashBangEffect(DeltaTime);

	HandleAimPressCameraLocation();
	//ClampControllerRotationPitchWhileAimDownSight();

	HandleLerpMainSpringArmToDestRelativeLocation(DeltaTime);

	//int TestCount = 0;

	//for (auto& Bullet : PooledBullets)
	//{
	//	if (Bullet->GetIsActive())
	//	{
	//		TestCount++;
	//	}
	//}
	//UC_Util::Print(TestCount);
}

void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyInputComponent->BindAction(PlayerInputComponent, this);

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
	float DeltaYawTemp= FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Yaw);
	float DeltaPitchTemp = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Pitch);


	if (DeltaYawTemp < 5 && DeltaPitchTemp < 5)
	{
		Controller->SetControlRotation(CharacterMovingDirection);
		bIsAltPressed = false;
	}
}

void AC_Player::HandleLerpMainSpringArmToDestRelativeLocation(float DeltaTime)
{
	C_MainSpringArm->SetRelativeLocation
	(
		FMath::Lerp
		(
			C_MainSpringArm->GetRelativeLocation(),
			MainSpringArmRelativeLocationDest,
			DeltaTime * 5.f
		)
	);
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

	FString TheFloatStr = FString::SanitizeFloat(this->Inventory->GetCurVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);

	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);
	
	
	if (IsValid(OverlappedItem) && (OverlappedItem->GetOwnerCharacter() == nullptr))
	{
		UC_Util::Print("OverlappedItem");
		//UC_Util::Print(*OverlappedItem->GetName());
	
		Inventory->GetNearItems().Add(OverlappedItem);
	}
	else
	{
		UC_Util::Print("No item");

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
		Inventory->GetNearItems().Remove(OverlappedItem);
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


void AC_Player::HandleTurnInPlace() // Update함수 안에 있어서 좀 계속 호출이 되어서 버그가 있는듯?
{
	// 현재 멈춰있는 상황이 아니면 처리 x
	if (!bCanMove) return;

	if (GetVelocity().Size() > 0.f) return;
	if (bIsHoldDirection) return;

	// 0 360
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

		// Lower Body도 체크
		if (!LowerBodyTurnAnimMontageMap.Contains(HandState)) return;

		FPriorityAnimMontage LowerRightPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(LowerRightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerRightPriorityMontage.AnimMontage)) return;

		PlayAnimMontage(LowerRightPriorityMontage);

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

		// Lower Body도 체크
		if (!LowerBodyTurnAnimMontageMap.Contains(HandState)) return;

		FPriorityAnimMontage LowerLeftPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!IsValid(LowerLeftPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerLeftPriorityMontage.AnimMontage)) return;

		PlayAnimMontage(LowerLeftPriorityMontage);
	}

}

void AC_Player::HandleTurnInPlaceWhileAiming()
{
	// 현재 멈춰있는 상황이 아니면 처리 x
	
	if (!bCanMove) return;
	if (HandState != EHandState::WEAPON_GUN) return;
	if (!bIsAimDownSight)
	{
		AimingTurnInPlaceTimeCount = 0;
		UAnimMontage* RightMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState].AnimMontage;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		FPriorityAnimMontage LowerRightPriorityMontage;
		if (LowerBodyTurnAnimMontageMap[HandState].RightMontages.Contains(PoseState))
			LowerRightPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].RightMontages[PoseState];
		else
			return;
		if (!IsValid(LowerRightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerRightPriorityMontage.AnimMontage))
		{
			AnimInstance->Montage_Stop(0.1f, LowerRightPriorityMontage.AnimMontage);
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}

		FPriorityAnimMontage LowerLeftPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!IsValid(LowerLeftPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerLeftPriorityMontage.AnimMontage))
		{
			AnimInstance->Montage_Stop(0.1f, LowerLeftPriorityMontage.AnimMontage);
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		return;
	}
	if (GetVelocity().Size() > 0.f) return;
	if (bIsHoldDirection) return;

	float Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;
	float ControlRotation = GetControlRotation().Yaw;
	float CapsuleRotation = GetCapsuleComponent()->GetComponentRotation().Yaw;
	if (AimingTurnInPlaceTimeCount <= 0)
	{
		SavedYaw = CapsuleRotation;
	}
	AimingTurnInPlaceTimeCount += GetWorld()->DeltaTimeSeconds;
	DeltaYaw = CapsuleRotation - SavedYaw;
	DeltaYaw = UKismetMathLibrary::FClamp(DeltaYaw, -90.0f, 90.0f);
	//DeltaYaw = UKismetMathLibrary::Abs(DeltaYaw);
	if (AimingTurnInPlaceTimeCount >= 0.5f)
	{
		//UKismetMathLibrary::Lerp(AimingTurnInPlaceTimeCount, -0.1, 0.5);
		AimingTurnInPlaceTimeCount = 0;
		UAnimMontage* RightMontage = TurnAnimMontageMap[HandState].RightMontages[PoseState].AnimMontage;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	

		FPriorityAnimMontage LowerRightPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(LowerRightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerRightPriorityMontage.AnimMontage))
		{
			AnimInstance->Montage_Stop(0.1f, LowerRightPriorityMontage.AnimMontage);
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}

		FPriorityAnimMontage LowerLeftPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!IsValid(LowerLeftPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerLeftPriorityMontage.AnimMontage))
		{
			AnimInstance->Montage_Stop(0.1f, LowerLeftPriorityMontage.AnimMontage);
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		return;
	}
	if (DeltaYaw > 0.f) // Right Turn in place motion
	{
		// Controller
		//GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;

		
		if (!LowerBodyTurnAnimMontageMap.Contains(HandState)) return;

		FPriorityAnimMontage LowerRightPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].RightMontages[PoseState];

		if (!IsValid(LowerRightPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerRightPriorityMontage.AnimMontage)) return;
		float CurPlayRate = (DeltaYaw / 90.0f) * 2.0f / AimingTurnInPlaceTimeCount;
		CurPlayRate = UKismetMathLibrary::FClamp(CurPlayRate, 0.5f, 1.5f);

		//UC_Util::Print(CurPlayRate);

		PlayAnimMontage(LowerRightPriorityMontage, CurPlayRate);

	}
	if (DeltaYaw < 0.f) // Left Turn in place motion
	{
		//GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;


		// Lower Body도 체크
		if (!LowerBodyTurnAnimMontageMap.Contains(HandState)) return;

		FPriorityAnimMontage LowerLeftPriorityMontage = LowerBodyTurnAnimMontageMap[HandState].LeftMontages[PoseState];

		if (!IsValid(LowerLeftPriorityMontage.AnimMontage)) return;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerLeftPriorityMontage.AnimMontage)) return;
		float CurPlayRate = (UKismetMathLibrary::Abs(DeltaYaw) * 2.0f / 90.0f) / AimingTurnInPlaceTimeCount;
		CurPlayRate = UKismetMathLibrary::FClamp(CurPlayRate, 0.5f, 1.5f);
		//UC_Util::Print(CurPlayRate);
		PlayAnimMontage(LowerLeftPriorityMontage, CurPlayRate);
	}
}

void AC_Player::HandlePlayerRotationWhileAiming()
{
	if (!bIsAimDownSight) return;
	//bUseControllerRotationYaw = true;


	// 현재 캐릭터 회전
	//if (FMath::Abs(GetActorRotation().Yaw - GetControlRotation().Yaw) <= 3.0f)
	//{
	//	bUseControllerRotationYaw = true;
	//	return;
	//}
	//else
	//	bUseControllerRotationYaw = false;

	// 목표 회전값으로 Lerp
	float DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), GetControlRotation()).Yaw;

	float LerpAlpha = UKismetMathLibrary::Abs(DeltaRotation);
	//UC_Util::Print(LerpAlpha,FColor::Blue);
	LerpAlpha = UKismetMathLibrary::FClamp(LerpAlpha, 0.6, 1);

	FRotator NewRotationTemp = UKismetMathLibrary::RLerp(GetActorRotation(), GetControlRotation(), GetWorld()->DeltaTimeSeconds * 15.f, true);
	float NewRotationYaw = NewRotationTemp.Yaw;
	// 캐릭터 회전 설정
	FRotator NewRotation = FRotator(GetActorRotation().Pitch, NewRotationYaw, GetActorRotation().Roll);
	SetActorRotation(NewRotation);
	//UC_Util::Print(float(GetActorRotation().Yaw));
}

void AC_Player::ClampControllerRotationPitchWhileCrawl(EPoseState InCurrentState)
{
	APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	switch (InCurrentState)
	{
	case EPoseState::STAND:
		Camera->ViewPitchMax = 30;
		Camera->ViewPitchMin = -10;
		break;
	case EPoseState::CROUCH:
		Camera->ViewPitchMax = 30;
		Camera->ViewPitchMin = -10;
		break;
	case EPoseState::CRAWL:
		Camera->ViewPitchMax = 90;
		Camera->ViewPitchMin = -90;
		break;
	case EPoseState::POSE_MAX:
		Camera->ViewPitchMax = 90;
		Camera->ViewPitchMin = -90;
		break;
	default:
		break;
	}

	//FRotator NewRotation = GetController()->GetControlRotation();
	//if (PoseState == EPoseState::CRAWL)
	//{
	//	APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	//	Camera->ViewPitchMax = 30;
	//	Camera->ViewPitchMin = -10;

	//	//AimCamera->GetCameraView();
	//	if (NewRotation.Pitch > 180.f)
	//		NewRotation.Pitch -= 360.0f;
	//	NewRotation.Pitch = UKismetMathLibrary::FClamp(NewRotation.Pitch, -10, 30);
	//	GetController()->SetControlRotation(NewRotation);
	//}


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
		FPoseTurnInPlaceAnimMontage CurrenteHandStateTurnInPlaces{};

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


void AC_Player::HandleCameraAimPunching(float DeltaTime)
{
	if (!IsValid(MainCamera) || !IsValid(AimCamera)) return;

	FVector		MainCamPos = MainCamera->GetRelativeLocation();
	FVector		AimCamPos  = AimCamera->GetRelativeLocation();
	FRotator	MainCamRot = MainCamera->GetRelativeRotation();
	FRotator	AimCamRot  = AimCamera->GetRelativeRotation();

	if (FVector::Dist(MainCamPos, MainCamPunchingDestLocation) < 0.1f) // Destination으로 도달했다고 판단
	{
		// 다시금 원위치로 조정
		MainCamPunchingDestLocation = MainCamOriginLocalLocation;
		MainCamPunchingDestRotation.Roll = MainCamOriginLocalRotation.Roll;
	}

	if (FVector::Dist(AimCamPos, AimCamPunchingDestLocation) < 0.1f)
	{
		AimCamPunchingDestLocation = AimCamOriginLocalLocation;
		AimCamPunchingDestRotation.Roll = AimCamOriginLocalRotation.Roll;
	}

	//if (FMath::Abs(MainCamRot.Roll - MainCamPunchingDestRotation.Roll) < 0.1f)
	//if (FMath::Abs(AimCamRot.Roll - AimCamPunchingDestRotation.Roll) < 0.1f)

	MainCamPos = FMath::Lerp(MainCamPos, MainCamPunchingDestLocation, PunchingLerpFactor * DeltaTime);
	AimCamPos = FMath::Lerp(AimCamPos, AimCamPunchingDestLocation, PunchingLerpFactor * DeltaTime);

	MainCamRot.Roll = FMath::Lerp(MainCamRot.Roll, MainCamPunchingDestRotation.Roll, PunchingLerpFactor * DeltaTime);
	AimCamRot.Roll = FMath::Lerp(AimCamRot.Roll, AimCamPunchingDestRotation.Roll, PunchingLerpFactor * DeltaTime);

	MainCamera->SetRelativeLocation(MainCamPos);
	AimCamera->SetRelativeLocation(AimCamPos);
	MainCamera->SetRelativeRotation(MainCamRot);
	AimCamera->SetRelativeRotation(AimCamRot);

	//UC_Util::Print(float(AimCamera->GetRelativeRotation().Roll));
	//UC_Util::Print(float(MainCamera->GetRelativeRotation().Roll));

}

void AC_Player::ExecuteCameraAimPunching
(
	FVector CamPunchingDirection,
	float CamPunchIntensity,
	float CamRotationPunchingXDelta,
	float InPunchingLerpFactor
)
{
	// TODO : 현재 AimDownSight이면 다르게 처리 (Character Animation으로 처리해야 할 듯)
	// TODO : Aim Down일 때와 Main Camera는 동일하게 모두 일괄 처리 (자세 바뀔 때도 다음 카메라 또한 에임 펀칭이 일어나는 중으로 해줘야 함)

	CamPunchingDirection.Normalize();

	MainCamPunchingDestLocation			= MainCamOriginLocalLocation + CamPunchingDirection * CamPunchIntensity;
	AimCamPunchingDestLocation			= AimCamOriginLocalLocation + CamPunchingDirection * CamPunchIntensity;

	MainCamPunchingDestRotation.Roll	= MainCamOriginLocalRotation.Roll + CamRotationPunchingXDelta;
	AimCamPunchingDestRotation.Roll		= AimCamOriginLocalRotation.Roll + CamRotationPunchingXDelta;

	PunchingLerpFactor = InPunchingLerpFactor;
}

void AC_Player::ExecuteCameraShake(float ShakeScale)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController && PlayerController->PlayerCameraManager && CameraShakeClass)
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass, ShakeScale);
}

void AC_Player::HandleFlashBangEffect(float DeltaTime)
{
	FlashBangEffectDuration -= DeltaTime;

	// End of Effect
	if (FlashBangEffectDuration <= 0.f)
	{
		FlashBangEffectDuration = 0.f;

		PostProcessVolume->Settings.BloomIntensity = FMath::Lerp(PostProcessVolume->Settings.BloomIntensity, PostProcessInitialIntensity, DeltaTime * 10.f);

		// TODO : Capture된 잔상 남기기

		return;
	}

	PostProcessVolume->Settings.BloomIntensity = 1000.f;
	//PostProcessVolume->Settings.BloomIntensity = 500.f;
}

void AC_Player::CaptureScene()
{
	if (!SceneCaptureComponent)
	{
		UC_Util::Print("From AC_Player::CaptureScene : SceneCaptureComponent not initialized!", FColor::Red, 5.f);
		return;
	}
	if (!RenderTarget)
	{
		UC_Util::Print("From AC_Player::CaptureScene : RenderTarget not initialized!", FColor::Red, 5.f);
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		UC_Util::Print("From AC_Player::CaptureScene : PlayerController Invalid!", FColor::Red, 5.f);
		return;
	}

	AActor* ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget();
	if (!IsValid(ViewTarget))
	{
		UC_Util::Print("From AC_Player::CaptureScene : ViewTarget Invalid!", FColor::Red, 5.f);
		return;
	}

	UCameraComponent* CameraComponent = ViewTarget->FindComponentByClass<UCameraComponent>();

	if (!CameraComponent)
	{
		UC_Util::Print("From AC_Player::CaptrueScene : Camera Component not found!", FColor::Red, 5.f);
		return;
	}

	SceneCaptureComponent->SetWorldTransform(CameraComponent->GetComponentTransform());
	SceneCaptureComponent->FOVAngle = CameraComponent->FieldOfView;
	
	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureScene();

	UTexture2D* NewTexture = RenderTarget->ConstructTexture2D(SceneCaptureComponent, TEXT("CapturedImage"), EObjectFlags::RF_NoFlags, CTF_DeferCompression);

	if (!NewTexture)
	{
		UC_Util::Print("Texture not created!", FColor::Red, 5.f);
		return;
	}

	// 필요에 따라 텍스쳐를 더 수정할 수 있음 (무슨 소린지 잘 모르겠음)
	NewTexture->UpdateResource();

	//FSlateBrush Brush{};
    //Brush.SetResourceObject(NewTexture);
	//ScreenShotWidget->GetDisplayImage()->SetBrush(Brush);
}

void AC_Player::ExecuteFlashBangEffect(float Duration)
{
	// 현재의 남은 Duration 이하인 Duration이 들어왔다면 새로이 FlashBangEffect를 실행하지 않음
	if (FlashBangEffectDuration >= Duration) return;

	if (!IsValid(PostProcessVolume))
	{
		UC_Util::Print("From AC_Player::ExecuteFlashBangEffect : PostProcessVolume Invalid!", FColor::Cyan, 5.f);
		return;
	}
	
	FlashBangEffectDuration = Duration;

	//CaptureScene();
	//FString Temp{};
	//FScreenshotRequest::RequestScreenshot(Temp, false, false);
}



void AC_Player::SetToAimDownSight()
{
	MainCamera->SetActive(false);
	AimCamera->SetActive(false);

	bIsAimDownSight = true;
}

void AC_Player::BackToMainCamera()
{
	//AimCamera->SetActive(false);
	//MainCamera->SetActive(true);
	InitialCameraLocation = AimCamera->GetComponentLocation();
	InitialCameraRotation = AimCamera->GetComponentRotation();

	if (CameraTransitionTimeline)
	{
		bIsAimDownSight = false;

		CameraTransitionTimeline->PlayFromStart();
		//UC_Util::Print(CameraTransitionTimeline->IsPlaying());
	}
	//bIsAimDownSight = false;
}

void AC_Player::SetToAimKeyPress()
{

	InitialCameraLocation = MainCamera->GetComponentLocation();
	InitialCameraRotation = MainCamera->GetComponentRotation();

	// 타임라인 실행
	if (CameraTransitionTimeline)
	{
		bIsAimDownSight = true;

		CameraTransitionTimeline->PlayFromStart();
		//UC_Util::Print(CameraTransitionTimeline->IsPlaying());
	}
	//CameraTransitionTimeline->Get
	//AimCamera->SetActive(true);
	//MainCamera->SetActive(false);
	//bIsAimDownSight = true;
}
void AC_Player::HandleInterpolation(float Value)
{
	FVector  NewLocation;
	FRotator NewRotation;
	if (bIsAimDownSight)
	{
		NewLocation = FMath::Lerp(InitialCameraLocation, AimCamera->GetComponentLocation(), Value);
		NewRotation = FMath::Lerp(InitialCameraRotation, AimCamera->GetComponentRotation(), Value);
		MainCamera->SetWorldLocation(NewLocation);
		MainCamera->SetWorldRotation(NewRotation);
	}
	else
	{
		NewLocation = FMath::Lerp(InitialCameraLocation, MainCamera->GetComponentLocation(), Value);
		NewRotation = FMath::Lerp(InitialCameraRotation, MainCamera->GetComponentRotation(), Value);
		AimCamera->SetWorldLocation(NewLocation);
		AimCamera->SetWorldRotation(NewRotation);
		//UC_Util::Print(Value);
	}
}

void AC_Player::OnTimelineFinished()
{
	UC_Util::Print("Finished");
	if (bIsAimDownSight)
	{
		AimCamera->SetActive(true);
		MainCamera->SetActive(false);   
	}
	else
	{
		AimCamera->SetActive(false);
		MainCamera->SetActive(true);
	}
	MainCamera->SetRelativeLocation(FVector(0));
	
	MainCamera->SetRelativeRotation(InitialMainCameraRelativeRotation);
	//UC_Util::Print(MainCamera->GetRelativeRotation());
	AimCamera->SetRelativeLocation(FVector(0));
	AimCamera->SetRelativeRotation(FQuat(0));
	
}

void AC_Player::SetTimeLineComponentForMovingCamera()
{
	CameraTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("CameraTransitionTimeline"));
	//CameraTransitionTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	//this->BlueprintCreatedComponents.Add(CameraTransitionTimeline);
	CameraTransitionTimeline->SetNetAddressable();  // 리플리케이션 가능
	//FName name = "CF_CameraMoving";
	
	//CurveFloatForSwitchCamera = CreateCurveFloatForSwitchCamera();
	//CurveFloatForSwitchCameraChange = Cast<UCurveFloat>(GetWorld()->GetDefaultSubobjectByName(name));
	CurveFloatForSwitchCameraChange = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Project_PUBG/Hyunho/CameraMoving/CF_CameraMoving"));
	// 타임라인 초기화
	if (CameraTransitionTimeline && CurveFloatForSwitchCameraChange)
	{
		//CurveFloatForSwitchCamera->Get
		//UC_Util::Print("Success");
		InterpFunction.BindUFunction(this, FName("HandleInterpolation"));
		TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
		CameraTransitionTimeline->AddInterpFloat(CurveFloatForSwitchCameraChange, InterpFunction);
		CameraTransitionTimeline->SetTimelineFinishedFunc(TimelineFinished);
		CameraTransitionTimeline->SetPlayRate(1.0f);  // 재생 속도 설정
		CameraTransitionTimeline->SetLooping(false);  // 반복하지 않도록 설정
		CameraTransitionTimeline->SetTimelineLength(0.2f);
		//UC_Util::Print("Timeline Length: " + FString::SanitizeFloat(CameraTransitionTimeline->GetTimelineLength()));
	
	}

}

UCurveFloat* AC_Player::CreateCurveFloatForSwitchCamera()
{
	UCurveFloat* NewCurve = NewObject<UCurveFloat>();

	// 커브의 KeyFrame을 설정합니다.
	if (NewCurve)
	{
		FRichCurve& Curve = NewCurve->FloatCurve;

		// 커브의 키프레임 추가 (예: 0초에 0, 1초에 1)
		Curve.AddKey(0.0f, 0.0f);
		Curve.AddKey(0.2f, 1.0f);
	}

	return NewCurve;
}

void AC_Player::SpawnConsumableItemForTesting()
{
	FActorSpawnParameters Param{};
	Param.Owner = this;
	//ConsumableItem = GetWorld()->SpawnActor<AC_FirstAidKit>(ConsumableItemClass, Param);
	//ConsumableItem = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClass, Param);

	for (auto& ItemClass : ConsumableItemClasses)
		ConsumableItems.Add(GetWorld()->SpawnActor<AC_ConsumableItem>(ItemClass, Param));

}

void AC_Player::PoolingBullets()
{

	FActorSpawnParameters Param2{};
	Param2.Owner = this;
	for (int i = 0; i < 10; i++)
	{
		UClass* BulletBPClass = StaticLoadClass(AC_Bullet::StaticClass(), nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet.BPC_Bullet_C"));
		AC_Bullet* Bullet = GetWorld()->SpawnActor<AC_Bullet>(BulletBPClass, Param2);

		
		if (IsValid(Bullet))
		{

			UC_Util::Print("Created Bullet");
			PooledBullets.Add(Bullet);
		}
	}
	
}

void AC_Player::SetLineTraceCollisionIgnore()
{
	LineTraceCollisionParams.AddIgnoredActor(this);
	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

	//LineTraceCollisionParams.AddIgnoredActor(OwnerCharacter);
	LineTraceCollisionParams.AddIgnoredActor(PlayerCamera);
	for (auto& Bullet : Bullets)
	{
		LineTraceCollisionParams.AddIgnoredActor(Bullet);
	}
}
