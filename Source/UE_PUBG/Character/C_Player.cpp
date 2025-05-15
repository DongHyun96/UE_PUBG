// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Player.h"

#include "EnhancedInputSubsystems.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_PingSystemComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Character/Component/C_CameraEffectComponent.h"
#include "Character/Component/C_PlayerController.h"

#include "Components/CapsuleComponent.h"

#include "Item/C_Item.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Curves/CurveFloat.h"

#include "Loot/C_BasicLoot.h"

#include "Engine/PostProcessVolume.h"

#include "Blueprint/UserWidget.h"

#include "Utility/C_Util.h"
#include "Utility/C_TimelineUtility.h"
#include "UMG.h"
#include "Airplane/C_AirplaneManager.h"
#include "Styling/SlateBrush.h"
#include "Engine/Texture2D.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_SkyDiveWidget.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"
#include "Component/SkyDivingComponent/C_PlayerSkyDivingComponent.h"
#include "Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "HUD/C_GameOverWidget.h"
#include "HUD/C_InstructionWidget.h"
#include "Singleton/C_GameSceneManager.h"

AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MyInputComponent = CreateDefaultSubobject<UC_InputComponent>("MyInputComponent");
	CrosshairWidgetComponent = CreateDefaultSubobject<UC_CrosshairWidgetComponent>("CrosshairWidgetComponent");


	InitTurnAnimMontageMap();

	//DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	//DetectionSphere->InitSphereRadius(100.0f); // 탐지 반경 설정
	//DetectionSphere->SetupAttachment(RootComponent);
	//
	////DetectionSphere->SetGenerateOverlapEvents(true);
	//DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_Player::OnOverlapBegin);
	//DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_Player::OnOverlapEnd);
	AimSpringArmTemp = CreateDefaultSubobject<USpringArmComponent>("AimSpringArm");
	//AimSpringArmTemp->SetupAttachment(RootComponent);
	AimSpringArmTemp->SetupAttachment(GetMesh());
	AimCamera = CreateDefaultSubobject<UCameraComponent>("AimCamera");
	AimCamera->SetupAttachment(AimSpringArmTemp);


	MainCamera = CreateDefaultSubobject<UCameraComponent>("C_MainCamera");
	C_MainSpringArm = CreateDefaultSubobject<USpringArmComponent>("C_MainSpringArm");
	C_MainSpringArm->SetupAttachment(RootComponent);
	MainCamera->SetupAttachment(C_MainSpringArm);


	//MainCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
	//InitialMainCameraRelativeRotation = MainCamera->GetRelativeRotation().Quaternion();
	//C_MainSpringArm = Cast<USpringArmComponent>(GetDefaultSubobjectByName("MainSpringArm"));

	PingSystemComponent = CreateDefaultSubobject<UC_PingSystemComponent>("PingSystemComponent");

	SetTimeLineComponentForMovingCamera();
	SetRecoilTimeLineComponent();

	InvenSystem = CreateDefaultSubobject<UC_InvenSystem>("C_InvenSystem");
	InvenSystem->SetOwnerCharacter(this);

	CameraEffectComponent = CreateDefaultSubobject<UC_CameraEffectComponent>("CameraEffectComponent");
	CameraEffectComponent->SetOwnerPlayer(this);

	SkyDivingComponent = CreateDefaultSubobject<UC_PlayerSkyDivingComponent>("PlayerSkyDivingComponent");
	SkyDivingComponent->SetOwnerCharcter(this);
	PlayerSkyDivingComponent = Cast<UC_PlayerSkyDivingComponent>(SkyDivingComponent);
}

void AC_Player::BeginPlay()
{
	Super::BeginPlay();
	//GAMESCENE_MANAGER->SetPlayer(this);

	InitialMainCameraRelativeRotation = MainCamera->GetRelativeRotation().Quaternion();
	AimCamera->SetRelativeLocation(FVector(0));
	AimCamera->SetRelativeRotation(FQuat(0));
	
	if (HUDWidget)
	{
		HUDWidget->AddToViewport(10);
		StatComponent->SetOwnerHUDWidget(HUDWidget);
		PingSystemComponent->SetOwnerPlayer(this);
		HUDWidget->GetSkyDiveWidget()->SetOwnerPlayer(this);
		HUDWidget->GetMiniMapWidget()->SetOwnerPlayer(this);
	}
	else UC_Util::Print("No HUD Widget", FColor::MakeRandomColor(), 10.f);
		

	if (MainMapWidget)
	{
		// MainMapWidget->AddToViewport(9);
		MainMapWidget->SetOwnerPlayer(this);
	} else UC_Util::Print("No MainMapWidget", FColor::MakeRandomColor(), 10.f);

	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport(20);
		GameOverWidget->SetOwnerPlayer(this);
	}
	else UC_Util::Print("No GameOver Widget", FColor::MakeRandomColor(), 10.f);

	if (InvenSystem)
	{
		InvenSystem->GetInvenUI()->AddToViewport();
		InvenSystem->GetInvenUI()->SetVisibility(ESlateVisibility::Hidden);
	}

	CrosshairWidgetComponent->AddToViewport();
	CrosshairWidgetComponent->SetOwnerCharacter(this);

	AimCamera->SetActive(false);
	// AimCamera->bUsePawnControlRotation = false;


	SetPlayerMappingContext();
	
	// 자세별 MainSpringArm 위치 초기화
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::STAND, C_MainSpringArm->GetRelativeLocation());
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CROUCH, C_MainSpringArm->GetRelativeLocation() + FVector(0.f, 0.f, -32.f));
	//MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CRAWL, C_MainSpringArm->GetRelativeLocation()  + FVector(0.f, 0.f, -99.f));
	MainSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CRAWL, C_MainSpringArm->GetRelativeLocation()  + FVector(0.f, 0.f, -20.f));
	MainSpringArmRelativeLocationDest = MainSpringArmRelativeLocationByPoseMap[EPoseState::STAND];
	SetAimPressCameraLocation();

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

	ParkourComponent->SetOwnerPlayer(this);

	SetControllerPitchLimits(PoseState);

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &AC_Player::OnPostWorldBeginPlay);

	// TODO : Player name의 경우, 맨 처음 입력 받아 Name 세팅하기
	CharacterName = "Dongman";
}

void AC_Player::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GAMESCENE_MANAGER->SetPlayer(nullptr);	
}

void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UC_Util::Print(CurveFloatForSwitchCamera->GetFloatValue(GetWorld()->GetDeltaSeconds()));
	//UC_Util::Print(AimCamera->IsActive());
	//UC_Util::Print(MainCamera->IsActive());
	
	HandleTurnInPlace();
	HandleTurnInPlaceWhileAiming();
	HandlePlayerRotationWhileAiming();
	HandleControllerRotation(DeltaTime);

	//ClampControllerRotationPitchWhileAimDownSight();

	HandleLerpMainSpringArmToDestRelativeLocation(DeltaTime);
	HandleStatesWhileMovingCrawl();
	SetCanFireWhileCrawl();

	AC_Item* NewInteractableItem = FindBestInteractable();

	//if (!NewInteractableItem)
	//{
	//	CurOutLinedItem = nullptr;
	//	return;
	//}
	//if (!NewInteractableItem) return;

	// 아이템이 변경되었을 때만 업데이트 수행
	if (NewInteractableItem != CurOutLinedItem)
	{
		UpdateInteractable(NewInteractableItem);
	}

	// 스카이 다이빙 중인 경우에만 다이빙 사운드를 업데이트.
	//if (GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::SKYDIVING)
	//{
	//	UpdateSkyDivingSound();
	//}

	/*switch (HandState)
	{
	case EHandState::UNARMED: UC_Util::Print("UnArmed"); break;
	case EHandState::WEAPON_GUN: UC_Util::Print("Weapon_Gun"); break;
	case EHandState::WEAPON_MELEE: UC_Util::Print("WEAPON_MELEE"); break;
	case EHandState::WEAPON_THROWABLE: UC_Util::Print("WEAPON_THROWABLE"); break;
	case EHandState::HANDSTATE_MAX: UC_Util::Print("HANDSTATE_MAX"); break;
	}*/

	//DrawingItemOutLine();
	//UpdateInteractable(FindBestInteractable());
	//DistanceToGround = GetCharacterMovement()->CurrentFloor.FloorDist;
	
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

void AC_Player::SetPlayerMappingContext()
{
	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetController());

	if (PlayerController != nullptr)
	{
		//if (UEnhancedInputLocalPlayerSubsystem)
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (IsValid(SubSystem))
		{
			SubSystem->AddMappingContext(MyInputComponent->MappingContext, 0);
		}
		//FSlateApplication::Get().SetNavigationConfig(MakeShared<FNavigationConfig>());
	}
}

void AC_Player::OnPostWorldBeginPlay()
{
	// Player HUD에 비행기 경로 추가 (여기서 추가하는 이유는 BeginPlay 상호 Dependency 해결을 위함

	if (!IsValid(GAMESCENE_MANAGER))						return;
	if (!IsValid(GAMESCENE_MANAGER->GetAirplaneManager())) return;
	
	TPair<FVector, FVector> PlaneStartDestPair = GAMESCENE_MANAGER->GetAirplaneManager()->GetPlaneRouteStartDestPair();
	MainMapWidget->SetAirplaneRoute(PlaneStartDestPair);
	HUDWidget->GetMiniMapWidget()->SetAirplaneRoute(PlaneStartDestPair);
}

void AC_Player::HandleControllerRotation(float DeltaTime)
{
	//Alt 를 누른적 없으면 리턴
		// Debugging

	if (!bIsAltPressed) return;

	//if (GetCharacterMovement()->IsSwimming())
	//{
	//	GetCharacterMovement()->bUseControllerDesiredRotation	= false;
	//	GetCharacterMovement()->bOrientRotationToMovement		= true;
	//}

	if (Controller)
	{
		Controller->SetControlRotation(FMath::Lerp(Controller->GetControlRotation(), CharacterMovingDirection, DeltaTime * 10.0f));
	}
	//일정각도 이하로 차이나면 캐릭터 로테이션으로 정해버리기(가끔 적용이 안되는데 이유를 아직 못찾음)
	float DeltaYawTemp = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Yaw);
	float DeltaPitchTemp = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), CharacterMovingDirection).Pitch);


	if (DeltaYawTemp < 5 && DeltaPitchTemp < 5)
	{
		Controller->SetControlRotation(CharacterMovingDirection);
		bIsAltPressed = false;
	}
}

void AC_Player::HandleLerpMainSpringArmToDestRelativeLocation(const float& DeltaTime)
{
	// SkyDiving & Parachuting 중에서의 Lerp Destination 적용


	C_MainSpringArm->SetRelativeLocation
	(
		FMath::Lerp
		(
			C_MainSpringArm->GetRelativeLocation(),
			MainSpringArmRelativeLocationDest,
			DeltaTime * SpringArmRelativeLocationLerpSpeed
		)
	);
	//AimSpringArmTemp->SocketOffset
	AimSpringArmTemp->SetRelativeLocation
	(
		FMath::Lerp
		(
			AimSpringArmTemp->GetRelativeLocation(),
			AimingSpringArmRelativeLocationDest,
			DeltaTime * SpringArmRelativeLocationLerpSpeed
		)
	);
}

//void AC_Player::SetCanFireWhileCrawl()
//{
//	float ControllerPitchAngle = 360 - GetControlRotation().Pitch;
//	if (ControllerPitchAngle >= 0 && ControllerPitchAngle <= 10 && PoseState == EPoseState::CRAWL)
//		bCanFireBullet = false;
//	else
//		bCanFireBullet = true;
//
//	
//}

void AC_Player::SetCanFireWhileCrawl()
{
	float ControllerPitchAngle = 360 - GetControlRotation().Pitch;
	if (ControllerPitchAngle >= 10 && ControllerPitchAngle <= 20 && PoseState == EPoseState::CRAWL)
		bCanFireBullet = false;
	else
		bCanFireBullet = true;
}

bool AC_Player::GetIsHighEnoughToFall()
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);

	//CollisionParams.AddIgnoredActor(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane());

	FHitResult HitResult{};

	FVector StartLocation = GetActorLocation();
	FVector DestLocation = StartLocation - FVector::UnitZ() * (90.f+250.f);

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECollisionChannel::ECC_Visibility, CollisionParams);
	//UC_Util::Print(HitResult.Distance);	
	return !HasHit;
}



bool AC_Player::SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo)
{
	if (!bCanMove)											return false;
	if (bIsJumping || GetCharacterMovement()->IsFalling())	return false;
	if (InChangeFrom == InChangeTo)							return false;
	if (SwimmingComponent->IsSwimming())					return false;

	switch (InChangeTo)
	{
	case EPoseState::STAND:
		switch (InChangeFrom)
		{
		case EPoseState::CROUCH: // Crouch To Stand (Pose transition 없이 바로 처리)

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("STANDING BLOCKED!");
				return false;
			}

			SetMainSpringArmRelativeLocationDest(EPoseState::STAND);
			SetAimingSpringArmRelativeLocationDest(EPoseState::STAND);
			SetControllerPitchLimits(EPoseState::STAND);
			Super::SetPoseState(EPoseState::STAND);
			return true;

		case EPoseState::CRAWL: // Crawl To Stand

			if (bIsActivatingConsumableItem)
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("STANDING BLOCKED!");
				return false;
			}
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("STANDING BLOCKED!");
				return false;
			}
			SetControllerPitchLimits(EPoseState::STAND);
			SetMainSpringArmRelativeLocationDest(EPoseState::STAND);

			if (SwimmingComponent->GetSwimmingState() != ESwimmingState::ON_GROUND)
			{
				Super::SetPoseState(EPoseState::STAND);
				return true;
			}


			SetAimingSpringArmRelativeLocationDest(EPoseState::STAND);

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToStand, EPoseState::STAND);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CROUCH:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand To Crouch

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CROUCH BLOCKED!");
				return false;
			}

			SetMainSpringArmRelativeLocationDest(EPoseState::CROUCH);
			SetAimingSpringArmRelativeLocationDest(EPoseState::CROUCH);
			SetControllerPitchLimits(EPoseState::CROUCH);
			Super::SetPoseState(EPoseState::CROUCH);
			return true;

		case EPoseState::CRAWL: // Crawl To Crouch

			if (bIsActivatingConsumableItem)
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CROUCH BLOCKED!");
				return false;
			}
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CROUCH BLOCKED!");
				return false;
			}
			SetControllerPitchLimits(EPoseState::CROUCH);
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToCrouch, EPoseState::CROUCH);
			SetMainSpringArmRelativeLocationDest(EPoseState::CROUCH);
			SetAimingSpringArmRelativeLocationDest(EPoseState::CROUCH);

			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CRAWL:
		switch (InChangeFrom)	
		{
		case EPoseState::STAND: // Stand to Crawl

			if (bIsActivatingConsumableItem)
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CRAWL BLOCKED!");
				return false;
			}
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CRAWL BLOCKED!");
				return false;
			}
			SetControllerPitchLimits(EPoseState::CRAWL);

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).StandToCrawl, EPoseState::CRAWL);
			SetMainSpringArmRelativeLocationDest(EPoseState::CRAWL);
			SetAimingSpringArmRelativeLocationDest(EPoseState::CRAWL);

			return true;

		case EPoseState::CROUCH: // Crouch to Crawl

			if (bIsActivatingConsumableItem)
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CRAWL BLOCKED!");
				return false;
			}
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL))
			{
				HUDWidget->GetInstructionWidget()->AddPlayerWarningLog("CRAWL BLOCKED!");
				return false;
			}
			SetControllerPitchLimits(EPoseState::CRAWL);
			SetMainSpringArmRelativeLocationDest(EPoseState::CRAWL);
			SetAimingSpringArmRelativeLocationDest(EPoseState::CRAWL);

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrouchToCrawl, EPoseState::CRAWL);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::POSE_MAX: default: return false;
	}
}

void AC_Player::HandleOverlapBegin(AActor* OtherActor)
{
	AC_Item*		OverlappedItem		= Cast<AC_Item>(OtherActor);
	AC_BasicLoot*	OverlappedLootBox	= Cast<AC_BasicLoot>(OtherActor);
	
	//if (IsValid(OverlappedItem) && (OverlappedItem->GetOwnerCharacter() == nullptr))
	if (!IsValid(OverlappedItem) && !IsValid(OverlappedLootBox)) return;

	if (IsValid(OverlappedItem) && !OverlappedItem->IsHidden())
	{
		// UC_Util::Print("OverlappedItem");
		//UC_Util::Print(*OverlappedItem->GetName());

		//Inventory->GetNearItems().Add(OverlappedItem);
		//Inventory->AddItemToAroundList(OverlappedItem);

		if (OverlappedItem->GetOwnerCharacter() == nullptr)
		{
			if (!IsValid(InvenComponent)) return;//이 부분들에서 계속 터진다면 아예 없을때 생성해버리기.
			if (InvenComponent->GetAroundItems().Contains(OverlappedItem)) return;
			InvenComponent->AddItemToAroundList(OverlappedItem);
			
			//Inventory->InitInvenUI();
			//if (!IsValid(InvenSystem)) return;
		}
		InvenSystem->InitializeList();
		return;
	}

	// Loot Box 처리
	if (IsValid(OverlappedLootBox))
	{
		if (OverlappedLootBox->GetLootItems().Num() == 0) return;
			
		OverlappedLootBox->SetActorTickEnabled(true);

		InvenComponent->GetAroundItems().Append(OverlappedLootBox->GetLootItems());

		InvenSystem->GetInvenUI()->UpdateAroundItemPanelWidget();
	}

	//TArray<AC_Item*> SortedItems = this->GetInvenComponent()->GetAroundItems();
	//
	////오름차순 정렬
	//SortedItems.Sort([](AC_Item* A, AC_Item* B) 
	//    {
	//        if (!A && !B) return false;
	//        if (!A) return false;
	//        if (!B) return true;
	//        return A->GetItemDatas()->ItemType < B->GetItemDatas()->ItemType;
	//    });
	//GetInvenComponent()->SetAroundItems(SortedItems);
}

void AC_Player::HandleOverlapEnd(AActor* OtherActor)
{
	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);

	if (OverlappedItem)
	{
		//Inventory->GetNearItems().Remove(OverlappedItem);
		//Inventory->RemoveItemToAroundList(OverlappedItem);
		if (!IsValid(InvenComponent)) return;
		InvenComponent->RemoveItemToAroundList(OverlappedItem);
		//Inventory->InitInvenUI();
		//if (!IsValid(InvenSystem)) return;
		//InvenSystem->InitializeList();
		InvenSystem->GetInvenUI()->RemoveItemInList(OverlappedItem);
	}

	AC_BasicLoot* OverlappedLootBox = Cast<AC_BasicLoot>(OtherActor);

	if (IsValid(OverlappedLootBox))
	{
		TArray<AC_Item*> LootItemList = OverlappedLootBox->GetLootItems();
		OverlappedLootBox->SetActorTickEnabled(false);
		for (AC_Item* LootItem : LootItemList)
		{
			InvenComponent->RemoveItemToAroundList(LootItem);
			InvenSystem->GetInvenUI()->RemoveItemInList(LootItem);
		}
	}

	//TArray<AC_Item*> SortedItems = this->GetInvenComponent()->GetAroundItems();
	//
	////오름차순 정렬
	//SortedItems.Sort([](AC_Item* A, AC_Item* B)
	//	{
	//		if (!A && !B) return false;
	//		if (!A) return false;
	//		if (!B) return true;
	//		return A->GetItemDatas()->ItemType < B->GetItemDatas()->ItemType;
	//	});
	//GetInvenComponent()->SetAroundItems(SortedItems);
}

AC_Item* AC_Player::FindBestInteractable()
{
	if (InvenComponent->GetAroundItems().IsEmpty()) return nullptr;
	if (bIsActivatingConsumableItem) return nullptr; // ConsumableItem을 사용중이라면 nullptr 처리
	AC_Item* TargetInteractableItem = nullptr;

	float ItemDotProduct = 0.0f;

	//UGameplayStatics::controller

	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	for (auto& AroundItem : InvenComponent->GetAroundItems())
	{
		AC_Item* CachedInteractableItem = AroundItem;

		if (!CachedInteractableItem) continue;

		//밖으로 빼야 될 수도.
		FVector PlayerToItemVector = (CachedInteractableItem->GetActorLocation() - PlayerCamera->GetCameraLocation()).GetSafeNormal();
		
		FVector CameraForwardVector = PlayerCamera->GetCameraRotation().Vector().GetSafeNormal();

		double CachedDot = FVector::DotProduct(PlayerToItemVector, CameraForwardVector);
		//
		if (CachedDot > .9f && CachedDot > ItemDotProduct)
		{
			if (CachedInteractableItem)
			{
				ItemDotProduct = CachedDot;
				TargetInteractableItem = CachedInteractableItem;
			}
		}
	}

	return TargetInteractableItem;
}

void AC_Player::UpdateInteractable(AC_Item* InteractableItem)
{
	// UC_Util::Print("UpdateInteractable");

	if (CurOutLinedItem == InteractableItem)
	{
		return;
	}

	// 이전 아웃라인 제거
	if (CurOutLinedItem)
	{
		HUDWidget->GetInstructionWidget()->DeActivateFKeyInstruction();
		CurOutLinedItem->SetOutlineEffect(false);
	}

	// 새 아이템이 존재하면 아웃라인 적용
	CurOutLinedItem = InteractableItem;
	if (CurOutLinedItem)
	{
		FString Instruction = "PICK " + CurOutLinedItem->GetItemName(); 
		HUDWidget->GetInstructionWidget()->ActivateFKeyInstruction(Instruction);
		CurOutLinedItem->SetOutlineEffect(true);
	}
}

void AC_Player::DrawingItemOutLine()
{
	// 카메라의 위치 및 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CameraLocation = PlayerCamera->GetCameraLocation();
	// 트레이스 시작 및 끝 지점 설정
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 1000.0f); // 1000cm(10m) 거리까지 탐색

	// 충돌 정보
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 플레이어는 무시
	UC_Util::Print("Start LineTrace!");

	// 라인 트레이스 실행
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		// 감지된 액터가 아이템인지 확인
		AC_Item* DetectedItem = Cast<AC_Item>(HitResult.GetActor());
		if (DetectedItem && DetectedItem != CurOutLinedItem)
		{
			// 기존 아이템의 아웃라인 비활성화
			if (CurOutLinedItem)
			{
				CurOutLinedItem->SetOutlineEffect(false);
			}

			// 새 아이템의 아웃라인 활성화
			DetectedItem->SetOutlineEffect(true);
			CurOutLinedItem = DetectedItem;
			UC_Util::Print(DetectedItem->GetItemDatas()->ItemName);
		}
	}
	else if (CurOutLinedItem)
	{
		// 아이템이 감지되지 않으면 기존 아이템의 아웃라인 제거
		CurOutLinedItem->SetOutlineEffect(false);
		CurOutLinedItem = nullptr;
	}

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams) ? FColor::Green : FColor::Red, // 감지 여부에 따라 색상 변경
		false,
		0.1f,  // 지속 시간
		0,
		2.0f   // 선 두께
	);

}

void AC_Player::CharacterDead(const FKillFeedDescriptor& KillFeedDescriptor)
{
	Super::CharacterDead(KillFeedDescriptor);
	HUDWidget->GetInstructionWidget()->ActivateMiddleKillFeedLog(KillFeedDescriptor);

	GAMESCENE_MANAGER->SetIsGameOver(true);

	// TODO : if else 문으로 대체하기
	//if (Ranking == 1) GameOverWidget->ActivateWinningSequence();
	//else GameOverWidget->ActivateLoseSequence();
	GameOverWidget->ActivateLoseSequence();
	
	//if (AC_Gun* Gun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon()))
	//{
	//	UC_Util::Print("Dead : Current Gun valid!", FColor::Red, 10.f);
	//}
}

void AC_Player::EnableRagdoll()
{
	Super::EnableRagdoll();
	
	// 💡 컨트롤러 제거
	//DetachFromControllerPendingDestroy();//이걸로 해도 계속 인풋이 발생하는 것 같음.
	if (AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetController()))
		DisableInput(PlayerController);
		
	HUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	SetActorTickEnabled(false);
}

void AC_Player::SetAimPressCameraLocation()
{
	

	FVector HeadLocation = GetMesh()->GetBoneLocation("Head" ,EBoneSpaces::ComponentSpace);
	FVector NewLocation = FVector(0, 0, 0);
	NewLocation.Z += HeadLocation.Z;

	AimingSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::STAND , NewLocation);
	AimingSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CROUCH, NewLocation + FVector(0.f,   0.f, -50.f));
	AimingSpringArmRelativeLocationByPoseMap.Emplace(EPoseState::CRAWL , NewLocation + FVector(0.f, -50.f, -90.f));

	AimingSpringArmRelativeLocationDest = AimingSpringArmRelativeLocationByPoseMap[EPoseState::STAND];

	//AimSpringArmTemp->SetRelativeLocation(NewLocation);
}


void AC_Player::HandleTurnInPlace() // Update함수 안에 있어서 좀 계속 호출이 되어서 버그가 있는듯?
{
	// 현재 멈춰있는 상황이 아니면 처리 x
	if (!bCanMove)								return;

	//if (MainState != EMainState::IDLE)			return;
	if (GetCharacterMovement()->IsSwimming())	return;
	if (GetVelocity().Size() > 0.f)				return;
	if (bIsHoldDirection)						return;
	if (bIsActivatingConsumableItem)			return;
	
	//if (ParkourComponent->GetIsCurrentlyWarping()) return;

	// 0 360
	float Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;

	if (-90.f <= Delta && Delta <= 90.f) return;

	// Crawl Slope 예외처리
	if (PoseState == EPoseState::CRAWL)
	{
		//UC_Util::Print(Controller->GetControlRotation());
		FRotator ControlRotation	= Controller->GetControlRotation();
		ControlRotation				= FRotator(0.f, ControlRotation.Yaw, 0.f);
		FQuat ControlRotQuat		= FQuat(ControlRotation);
		FVector RotatedDirection	= ControlRotQuat * FVector::UnitX();

		FVector HeadStartLocation   = GetActorLocation() + RotatedDirection * 75.f;
		FVector PelvisStartLocation = GetActorLocation();

		DrawDebugLine(GetWorld(), PelvisStartLocation, HeadStartLocation, FColor::Red, true);

		if (!PoseColliderHandlerComponent->CanCrawlOnSlope(HeadStartLocation, PelvisStartLocation)) return;
	}

	// Controller
	GetCharacterMovement()->bUseControllerDesiredRotation	= true;
	GetCharacterMovement()->bOrientRotationToMovement		= false;

	FPriorityAnimMontage TurnInPlaceMontage = (Delta > 90.f) ? TurnAnimMontageMap[HandState].RightMontages[PoseState] :
															   TurnAnimMontageMap[HandState].LeftMontages[PoseState];

	if (!IsValid(TurnInPlaceMontage.AnimMontage))											return;
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(TurnInPlaceMontage.AnimMontage))	return;

	PlayAnimMontage(TurnInPlaceMontage);

	// Lower Body도 체크
	if (!LowerBodyTurnAnimMontageMap.Contains(HandState)) return;

	FPriorityAnimMontage LowerMontage = (Delta > 90.f) ? LowerBodyTurnAnimMontageMap[HandState].RightMontages[PoseState] :
														 LowerBodyTurnAnimMontageMap[HandState].LeftMontages[PoseState];

	if (!IsValid(LowerMontage.AnimMontage))											return;
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(LowerMontage.AnimMontage))	return;

	PlayAnimMontage(LowerMontage);
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

void AC_Player::SetControllerPitchLimits(EPoseState InCurrentState)
{
	APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	switch (InCurrentState)
	{
	case EPoseState::STAND:
		Camera->ViewPitchMax = 90;
		Camera->ViewPitchMin = -65;
		return;
	case EPoseState::CROUCH:
		Camera->ViewPitchMax = 90;
		Camera->ViewPitchMin = -65;
		return;
	case EPoseState::CRAWL:
		Camera->ViewPitchMax = 30;
		Camera->ViewPitchMin = -15;
		return;
	case EPoseState::POSE_MAX: default: return;
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
		FPoseTurnInPlaceAnimMontage CurrentHandStateTurnInPlaces{};

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
			
			CurrentHandStateTurnInPlaces.LeftMontages.Emplace(currentPose, PriorityLeftMontage);

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

			CurrentHandStateTurnInPlaces.RightMontages.Emplace(currentPose, PriorityRightMontage);
		}

		TurnAnimMontageMap.Emplace(static_cast<EHandState>(handState), CurrentHandStateTurnInPlaces);
	}

}

void AC_Player::SetToAimDownSight()
{
	CrosshairWidgetComponent->SetCrosshairState(ECrosshairState::RIFLEAIMDOWNSIGHT);
	MainCamera->SetActive(false);
	AimCamera->SetActive(false);
	bIsWatchingSight = true;
	bIsAimDownSight = true;
	UC_Util::Print("AimDownNow");
}

void AC_Player::BackToMainCamera()
{
	//AimCamera->SetActive(false);
	//MainCamera->SetActive(true);
	InitialCameraLocation = AimCamera->GetComponentLocation();
	InitialCameraRotation = AimCamera->GetComponentRotation();

	CrosshairWidgetComponent->SetCrosshairState(ECrosshairState::RIFLE);
	if (CameraTransitionTimelineComponent)
	{

		bIsAimDownSight = false;
		bIsWatchingSight = false;
		//CameraTransitionTimeline->PlayFromStart();
		CameraTransitionTimelineComponent->PlayFromStart();
		//		UC_Util::Print(CameraTransitionTimelineComponent->IsPlaying());
	}
	//bIsAimDownSight = false;
}

void AC_Player::HandleStatesWhileMovingCrawl()
{
	if (PoseState != EPoseState::CRAWL || !bIsAimDownSight)
	{
		return;
	}
	//UC_Util::Print(PrevMoveSpeed);
	//UC_Util::Print(GetNextSpeed());
	if (PrevMoveSpeed <= 10 && GetNextSpeed() != PrevMoveSpeed)
	{
		Cast<AC_Gun>(EquippedComponent->GetCurWeapon())->BackToMainCamera();
	}
	PrevMoveSpeed = GetNextSpeed();
}

bool AC_Player::GetIsTooCloseToAimGun()
{
	bool HasHit = Super::GetIsTooCloseToAimGun();
	if (HasHit)
	{
		if(!CurrentCanAimGun)
			Cast<AC_Gun>(EquippedComponent->GetCurWeapon())->BackToMainCamera();
	}
	CurrentCanAimGun = HasHit;
	return HasHit;

}

void AC_Player::SetToAimKeyPress()
{
	CrosshairWidgetComponent->SetCrosshairState(ECrosshairState::RIFLE);
	InitialCameraLocation = MainCamera->GetComponentLocation();
	InitialCameraRotation = MainCamera->GetComponentRotation();

	// 타임라인 실행
	if (CameraTransitionTimelineComponent)
	{
		bIsAimDownSight = true;
		bIsWatchingSight = false;
		CameraTransitionTimelineComponent->PlayFromStart();
		//CameraTransitionTimeline->PlayFromStart();
		//UC_Util::Print(CameraTransitionTimeline->IsPlaying());
	}
	//CameraTransitionTimeline->Get
	//AimCamera->SetActive(true);
	//MainCamera->SetActive(false);
	//bIsAimDownSight = true;
}

void AC_Player::HandleCameraTransitionInterpolation(float Value)
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
		//UC_Util::Print(NewLocation);
		//UC_Util::Print(NewRotation);
	}
}

void AC_Player::OnCameraTransitionTimelineFinished()
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

	CameraTransitionTimelineComponent = CreateDefaultSubobject<UC_TimelineUtility>("CameraTransitionTimelineComponent");
	//RecoilTimeline = NewObject<AC_TimelineUtility>(this,FName("RecoilTimeline"),EObjectFlags::RF_NoFlags, AC_TimelineUtility::StaticClass());
	CameraTransitionTimelineComponent->InitializeTimelines
	(
		this,
		TEXT("/Game/Project_PUBG/Hyunho/Timelines/CameraMoving/CF_CameraMoving"),	
		ECurveType::CURVEFLOAT,
		FName("HandleCameraTransitionInterpolation"),
		FName("OnCameraTransitionTimelineFinished"),
		1.0f,
		0.2f,
		false
	);


}



//////////////////////////////////////////////////////
void AC_Player::HandleRecoilInterpolation(float Value)
{
	AC_Gun* CurGun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon());
	if (!IsValid(CurGun)) return;
	//UC_Util::Print("Recoiling!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	//UC_Util::Print(Value);
	float GunRecoilFactor = CurGun->GetRecoilFactors().Y;
	SetRecoilFactorByPose();
	float RecoilFactor = GunRecoilFactor * PlayerRecoilFactorByPose;
	AddControllerPitchInput(-Value * RecoilFactor);
}

void AC_Player::OnRecoilTimelineFinished()
{
	bIsFiringBullet = false;
}

void AC_Player::SetRecoilTimeLineComponent()
{
	RecoilTimeline = CreateDefaultSubobject<UC_TimelineUtility>("RecoilTimeline");
	//RecoilTimeline = NewObject<AC_TimelineUtility>(this,FName("RecoilTimeline"),EObjectFlags::RF_NoFlags, AC_TimelineUtility::StaticClass());
	RecoilTimeline->InitializeTimelines
	(
		this,
		TEXT("/Game/Project_PUBG/Hyunho/Timelines/Recoil/CF_Recoil"),
		ECurveType::CURVEFLOAT,
		FName("HandleRecoilInterpolation"),
		FName("OnRecoilTimelineFinished"),
		1.0f / 0.08f,
		2.0f,
		false
	);
}



void AC_Player::RecoilController()
{
	// UC_Util::Print("Start Recoil");
	AC_Gun* CurGun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon());
	if (!IsValid(CurGun)) return;
	RecoilTimeline->PlayFromStart();
	bIsFiringBullet = true;
	float GunRecoilFactor = CurGun->GetRecoilFactors().X;

	//ControllerRecoilTimeline->PlayFromStart();
	float Value = FMath::FRandRange(-1.0, 1.0) * PlayerRecoilFactorByPose * GunRecoilFactor;
	AddControllerYawInput(Value);


}

void AC_Player::SetRecoilTimelineValues(float InGunRPM)
{
	//AC_Gun* CurGun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon());
	//UC_Util::Print("SetRecoilTimelineValues", FColor::Red, 10);

	//if (!IsValid(CurGun)) return;
	float PlayRate = 1.0f;
	if (InGunRPM != 0)
		PlayRate /= InGunRPM;
	// UC_Util::Print(PlayRate, FColor::Red, 10);
	RecoilTimeline->SetTimelinePlayRate(PlayRate);
	//CameraTransitionTimeline->SetPlayRate(PlayRate);  // 재생 속도 설정
	//CameraTransitionTimeline->SetLooping(false);  // 반복하지 않도록 설정
}

void AC_Player::SetRecoilFactorByPose()
{
	switch (PoseState)
	{
	case EPoseState::STAND:
		PlayerRecoilFactorByPose = 1;
		break;
	case EPoseState::CROUCH:
		PlayerRecoilFactorByPose = 0.5;

		break;
	case EPoseState::CRAWL:
		PlayerRecoilFactorByPose = 0.3;

		break;
	case EPoseState::POSE_MAX:
		break;
	default:
		break;
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



