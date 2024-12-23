

// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UMG.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/PanelWidget.h"
#include "Components/NamedSlotInterface.h"
#include "Utility/C_Util.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CanvasPanelSlot.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Item/AttachmentActors/AttachmentActor.h"

#include "Item/Weapon/Gun/C_Bullet.h"

//UCameraComponent* AC_Gun::AimSightCamera;
// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");
	//ItemType 설정.
	//Magazine = LoadObject<AC_AttachableItem>(nullptr, TEXT("/Game/Project_PUBG/Common/Weapon/GunWeapon/Magazine/BPC_Magazine.BPC_Magazine"));

	ItemDatas.ItemType = EItemTypes::MAINGUN;

	SetHolsterNames();

}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();
	//Add Grip for Test

	//SetHolsterNames();
	//AttachedParts[EPartsName::GRIP] = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("VertgripMesh"));
	IronSightMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName("IronSightMesh"));
	//IronSightMesh->SetHiddenInGame(true);
	SetBulletSpeed();
	AimSightCamera   = FindComponentByClass<UCameraComponent>();
	AimSightSpringArm = FindComponentByClass<USpringArmComponent>();
	if (AimSightCamera)
		AimSightCamera->SetActive(false);
	//블루프린트에서 할당한 Skeletal Mesh 찾아서 변수에 저장
	GunMesh = FindComponentByClass<USkeletalMeshComponent>();
	GunMesh->SetupAttachment(RootComponent);
	
	LoadMagazine();

	if (IsValid(Magazine))
	{

		Magazine->AttachToComponent(
			GunMesh,
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			MAGAZINE_SOCKET_NAME
		);
	}
	SetSightCameraSpringArmLocation(ScopeCameraLocations[EAttachmentNames::MAX]);
}

void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (IsValid(Magazine))
	//{
	//	UC_Util::Print(Magazine->GetActorLocation());	
	//}

	if (!OwnerCharacter) return;

	CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	//GunMesh->SetWorldRotation(OwnerCharacter->GetControlRotation());
	if (IsValid(GunMesh))
	{
		FVector OutLocation{};
		FRotator OutRotation{};
		FTransform TempVec{};
		FRotator AdditionalRotation = FRotator(0, 0, 0); // Yaw를 45도 회전시키는 예제

		if (IsValid(AttachedItem[EPartsName::GRIP]))
			TempVec = AttachedItem[EPartsName::GRIP]->GetAttachmentMesh()->GetSocketTransform("LeftHandSocket");
		else
			TempVec = GunMesh->GetSocketTransform("LeftHandSocket");
		//TempVec = GunMesh->GetSocketTransform("LeftHandSocket");

		OwnerCharacter->GetMesh()->TransformToBoneSpace("RightHand", TempVec.GetLocation(), TempVec.GetRotation().Rotator(), OutLocation, OutRotation);
		LeftHandSocketLocation.SetLocation(OutLocation);
		LeftHandSocketLocation.SetRotation(OutRotation.Quaternion());
		//LeftHandSocketLocation.ConcatenateRotation(AdditionalRotation.Quaternion());

		//FTransform RootTrasnform = OwnerCharacter->GetMesh()->GetSocketTransform("SpineGunSocket");
		//
		//LeftHandSocketLocation = TempVec.GetRelativeTransform(RootTrasnform);
		//FString TheFloatStr = FString::SanitizeFloat(LeftHandSocketLocation.GetLocation().X);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	}
	HandleSpringArmRotation();
	GetPlayerIsAimDownOrNot();
	CheckPlayerIsRunning();
	//CheckBackPackLevelChange();
	ShowAndHideWhileAiming();
}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{

	if (!IsValid(OwnerCharacter)) return false;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::NORIFLE);

	EBackPackLevel BackPackLevel = OwnerCharacter->GetInvenComponent()->GetCurBackPackLevel();
	if (BackPackLevel == EBackPackLevel::LV0)
	{
		switch (CurState)
		{
		case EGunState::MAIN_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				MAIN_HOLSTER_SOCKET_NAME
			);
			break;
		case EGunState::SUB_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_HOLSTER_SOCKET_NAME
			);
			break;
		default:
			return false;
			break;
		}
	}
	else
	{
		switch (CurState)
		{
		case EGunState::MAIN_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				MAIN_HOLSTER_BAG_SOCKET_NAME
			);
			break;
		case EGunState::SUB_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_HOLSTER_BAG_SOCKET_NAME
			);
			break;
		default:
			return false;
			break;
		}
	}

	

}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//if (!IsValid(DrawMontage)) return false;
	// 만일 SUB_GUN의 Draw가 실행된다면 왼손으로 먼저 Attach
	//현재 왼손으로 옮겨지고 있다면 다시 어태치 하지 않고 If문 밑으로 진행
	FName CurSocketName = GetAttachParentSocketName();

	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(DrawMontage) && CurSocketName != SUB_DRAW_SOCKET_NAME)
	{

		//Crawl일 때는 모션이 달라 왼손에 어태치 안함
		if (CurState == EGunState::SUB_GUN && OwnerCharacter->GetPoseState() != EPoseState::CRAWL)
		{
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_DRAW_SOCKET_NAME
			);
		}
	}
	OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::RIFLE);
	
	OwnerPlayer->SetRecoilTimelineValues(BulletRPM);
	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		EQUIPPED_SOCKET_NAME
	);

	
}

bool AC_Gun::SetAimingDown()
{
	//스프린트 중이라면 Return
	if (OwnerCharacter->GetNextSpeed() > 600) return false;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))	              return false;
	OwnerPlayer->SetToAimDownSight();
	//CharacterMesh->HideBoneByName(FName("HeadBoneName"), EPhysBodyOp::PBO_None);

	//OwnerCharacter->bUseControllerRotationYaw = true;

	//AimDown 일 때 머리숨기기
	//TODO : 내 카메라에만 안보이고 상대방 카메라에선 보이게 만들기
	OwnerCharacter->GetMesh()->HideBoneByName(FName("Head"), EPhysBodyOp::PBO_None);
	AimSightCamera->SetActive(true);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2);
	bIsAimDown = true;
	if (IsValid(AttachedItem[EPartsName::SCOPE]))
		AttachedItem[EPartsName::SCOPE]->UseMrbStrategy();
	return true;
}
//견착 조준만할 때 Player AimKePress함수로 메인카메라에서 에임 카메라로 바꿔주기
bool AC_Gun::SetAimingPress()
{
	//if (OwnerCharacter->GetInvenSystem()->GetIsPanelOpend()) return false;
	//스프린트 중이라면 Return
	if (OwnerCharacter->GetNextSpeed() > 600) return false;

	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))       return false;
	if (OwnerPlayer->GetIsAimDown()) return false;
	OwnerPlayer->SetToAimKeyPress();
	//bIsAimDown = true;
	//OwnerCharacter->bUseControllerRotationYaw = true;

	return true;
}

bool AC_Gun::BackToMainCamera()
{

	AimSightCamera->SetActive(false);
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget() != OwnerCharacter)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(OwnerCharacter, 0.2);
	}
	OwnerCharacter->GetMesh()->UnHideBoneByName(FName("Head"));
	OwnerCharacter->bUseControllerRotationYaw = false;
	bIsAimDown = false;
	Cast<AC_Player>(OwnerCharacter)->BackToMainCamera();
	if (IsValid(AttachedItem[EPartsName::SCOPE]))
		AttachedItem[EPartsName::SCOPE]->UseMrbStrategy();
	return true;
	
	//FString TheFloatStr = FString::SanitizeFloat(MrbPressTimeCount);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	
}

void AC_Gun::HandleSpringArmRotation()
{
	//if (!bIsAimPress) return;
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator NewRotation = FRotator(ControlRotation.Pitch, ControlRotation.Yaw, 0);
	AimSightSpringArm->SetWorldRotation(OwnerCharacter->GetControlRotation());
	//AimSightSpringArm->SetWorldRotation(NewRotation);
}

void AC_Gun::GetPlayerIsAimDownOrNot()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (IsValid(OwnerPlayer))
	{
		bIsPlayerAimDownPress = OwnerPlayer->GetIsAimDown();
	}
}


void AC_Gun::SetOwnerCharacter(AC_BasicCharacter * InOwnerCharacter)
{
	Super::SetOwnerCharacter(InOwnerCharacter);
	if (!InOwnerCharacter)
	{
		if (AimWidget)
		{
			AimWidget->SetVisibility(ESlateVisibility::Hidden);
			AimWidget = nullptr;
		}
		return;
	}
	SetAimSightWidget();
}

bool AC_Gun::MoveToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	//equipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, nullptr);

	//SpawnItem(Character);

	this->SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);

	ItemDatas.ItemPlace = EItemPlace::AROUND;
	DetachmentItem();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	return true;
}

bool AC_Gun::MoveToSlot(AC_BasicCharacter* Character)
{
	//TODO : PickUpItem 내용으로 우선 만든거라 다시 만들어야함. 그리고 MainGun과 SubGun slot의 이동을 고민해야 함.
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	EWeaponSlot Slot = EWeaponSlot::MAIN_GUN;

	switch (Slot)
	{
	case EWeaponSlot::NONE:
		break;
	case EWeaponSlot::MAIN_GUN:
		//Main Gun Slot에 총이 없다면 실행
		if (!equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN])
		{
			equipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, this);

			//켰다 끄는 이유는 OwnerCharacter에서 인벤컴포넌트에서 RemoveItemAroundList를 써도 안되서 사용함.
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
			return true;
		}
		//Slot = EWeaponSlot::SUB_GUN;
	case EWeaponSlot::SUB_GUN:
		//Sub Gun Slot에 총이 없다면 실행
		if (!equipComp->GetWeapons()[EWeaponSlot::SUB_GUN])
		{
			equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);

			//켰다 끄는 이유는 OwnerCharacter에서 인벤컴포넌트에서 RemoveItemAroundList를 써도 안되서 사용함.
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);

			return true;
		}
		break;
	case EWeaponSlot::MELEE_WEAPON:
		break;
	case EWeaponSlot::THROWABLE_WEAPON:
		break;
	default:
		break;
	}

	//Main, Sub 모두 총이 있는 경우.
	EHandState HandState = Character->GetHandState();
	AC_Weapon* DropGun = nullptr;
	if (HandState == EHandState::WEAPON_GUN)
	{
		EWeaponSlot curSlot = equipComp->GetCurWeaponType();

		//제대로 총을 바꾸는지 확인해야함, SetSlotWeapon과 DetachmentItem의 순서를 바꿔야 할 수 도 있음.
		DropGun = equipComp->SetSlotWeapon(curSlot, this);
		DropGun->DetachmentItem();
		return true;
	}
	else
	{
		DropGun = equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		DropGun->DetachmentItem();
		return true;
	}
}

void AC_Gun::PickUpItem(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	EWeaponSlot Slot = EWeaponSlot::MAIN_GUN;

	switch (Slot)
	{
	case EWeaponSlot::NONE:
		break;
	case EWeaponSlot::MAIN_GUN:
		//Main Gun Slot에 총이 없다면 실행
		if (!equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN])
		{
			equipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, this);
			
			//켰다 끄는 이유는 OwnerCharacter에서 인벤컴포넌트에서 RemoveItemAroundList를 써도 안되서 사용함.
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
			return;
		}
		//Slot = EWeaponSlot::SUB_GUN;
	case EWeaponSlot::SUB_GUN:
		//Sub Gun Slot에 총이 없다면 실행
		if (!equipComp->GetWeapons()[EWeaponSlot::SUB_GUN])
		{
			equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);

			//켰다 끄는 이유는 OwnerCharacter에서 인벤컴포넌트에서 RemoveItemAroundList를 써도 안되서 사용함.
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);

			return;
		}
		break;
	case EWeaponSlot::MELEE_WEAPON:
		break;
	case EWeaponSlot::THROWABLE_WEAPON:
		break;
	default:
		break;
	}

	//Main, Sub 모두 총이 있는 경우.
	EHandState HandState = Character->GetHandState();
	AC_Weapon* DropGun = nullptr;
	if (HandState == EHandState::WEAPON_GUN)
	{
		EWeaponSlot curSlot = equipComp->GetCurWeaponType();
		
		//제대로 총을 바꾸는지 확인해야함, SetSlotWeapon과 DetachmentItem의 순서를 바꿔야 할 수 도 있음.
		DropGun = equipComp->SetSlotWeapon(curSlot, this);
		DropGun->DetachmentItem();
	}
	else
	{
		DropGun = equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		DropGun->DetachmentItem();
	}

}

void AC_Gun::OnOwnerCharacterPoseTransitionFin()
{
}

void AC_Gun::CheckPlayerIsRunning()
{
	//Aim Press 혹은 Aim Down 상태일 때 스프린트 실행하면 다시 main 카메라로 돌아가기
	float NextSpeed = OwnerCharacter->GetNextSpeed();
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	if (NextSpeed > 600 && OwnerPlayer->GetIsAimDown())
		BackToMainCamera();


}

void AC_Gun::CheckBackPackLevelChange()
{
	if (PrevBackPackLevel != OwnerCharacter->GetInvenComponent()->GetCurBackPackLevel())
	{
		if(OwnerCharacter->GetEquippedComponent()->GetCurWeapon() != this)
			AttachToHolster(OwnerCharacter->GetMesh());
	}
	PrevBackPackLevel = OwnerCharacter->GetInvenComponent()->GetCurBackPackLevel();

}

bool AC_Gun::GetIsPlayingMontagesOfAny()
{
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	bool IsPlayingMontagesOfAny = 
		CurAnimInstance->Montage_IsPlaying(DrawMontage)   || 
		CurAnimInstance->Montage_IsPlaying(SheathMontage) ||
		CurAnimInstance->Montage_IsPlaying(ReloadMontage);
	UC_Util::Print(IsPlayingMontagesOfAny, FColor::Magenta, 10);
	return IsPlayingMontagesOfAny;
}

bool AC_Gun::GetCanGunAction()
{
	bool CanAim = OwnerCharacter->GetCharacterMovement()->IsFalling();
	return CanAim;
}

void AC_Gun::ChangeCurShootingMode()
{
	int CurMode = int(CurrentShootingMode);
	++CurMode %= 3;
	CurrentShootingMode = EShootingMode(CurMode);
	UC_Util::Print(CurMode);
}

bool AC_Gun::ExecuteReloadMontage()
{
	//AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);
	//if (CurBulletCount == MaxBulletCount) return;
	//if (!CurPlayer->GetCanMove()) return;
	//if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return;
	//SetMagazineVisibility(false);
	//OwnerCharacter->SetIsReloadingBullet(true);
	//OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
	//BackToMainCamera();	
	return true;
}


FVector2D AC_Gun::GetRecoilFactors()
{
	float VerticalFactor   = RecoilFactorVertical * RecoilMultiplierByGripVert * RecoilMultiplierMuzzleVert;
	float HorizontalFactor = RecoilFactorHorizontal * RecoilMultiplierByGripHorizon * RecoilMultiplierMuzzleHorizon;

	return FVector2D(HorizontalFactor, VerticalFactor);
}

bool AC_Gun::FireBullet()
{
	bool OnScreen = (OwnerCharacter->GetNextSpeed() < 600) && OwnerCharacter->GetCanMove();
	if (!OnScreen) return false;
	//ExecuteReloadMontage();


	FVector FireLocation;
	FVector FireDirection;
	FVector HitLocation;
	bool HasHit;
	if (!SetBulletDirection(FireLocation, FireDirection,HitLocation, HasHit)) return false;
	UC_Util::Print(FireLocation);
	UC_Util::Print(FireDirection);
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	bool ApplyGravity = OwnerPlayer->GetIsWatchingSight() || !HasHit;
	int BulletCount = 0;
	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (Bullet->GetIsActive())
		{
			//UC_Util::Print("Can't fire");
			continue;
		}
		BulletCount++;
		//UC_Util::Print("FIRE!!!!!!!");
		CurBulletCount--;
		OwnerPlayer->RecoilController();
		if (HasHit)
			return Bullet->Fire(this, FireLocation, FireDirection, ApplyGravity, HitLocation);
		else
			return Bullet->Fire(this, FireLocation, FireDirection, ApplyGravity);

		//Bullet->Fire(this, FireLocation, FireDirection);
		//if (BulletCount > 100)
		//	return true;
	}
	UC_Util::Print("No More Bullets in Pool");
	return false;
}

bool AC_Gun::ReloadBullet()
{
	OwnerCharacter->SetIsReloadingBullet(false);
	CurBulletCount = MaxBulletCount;
	return true;
}

void AC_Gun::SetBulletSpeed()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))
	{
		return;
	}
	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (IsValid(Bullet))
		{
			UC_Util::Print("FindBullet");

			UProjectileMovementComponent* ProjectileMovement = Bullet->FindComponentByClass<UProjectileMovementComponent>();
			if (ProjectileMovement)
			{
				ProjectileMovement->InitialSpeed = BulletSpeed;

			}
		}
	}
}

bool AC_Gun::SetBulletDirection(FVector &OutLocation, FVector &OutDirection, FVector &OutHitLocation, bool& OutHasHit)
{
	FHitResult HitResult;
	//AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	AController* Controller = OwnerCharacter->GetController();
	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector StartLocation = PlayerCamera->GetCameraLocation();
	FRotator CameraRotation = PlayerCamera->GetCameraRotation();

	FVector ForwardVector = CameraRotation.Vector().GetSafeNormal() * 10000;
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.AddIgnoredActor(PlayerCamera);

	HitResult = {};
	FVector WorldLocation, WorldDirection;
	APlayerController* WolrdContorller = GetWorld()->GetFirstPlayerController();
	TArray<UUserWidget*> FoundWidgets;
	UUserWidget* MyWidget = AimWidget;
	UImage* MyImage;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), true);

	MyImage = Cast<UImage>(MyWidget->WidgetTree->FindWidget(FName("Crosshair_Base")));

	UCanvasPanelSlot* ImageSlot;
	FVector2D RandomPoint;
	if (IsValid(MyImage))
	{

		ImageSlot = Cast<UCanvasPanelSlot>(MyImage->Slot);
		if (ImageSlot)
		{
			// 이때, Cast<UPanelSlot>(ImageSlot)으로 다른 타입으로 캐스팅할 수 있음
			FVector2D ImageSize = ImageSlot->GetSize();
			RandomPoint = FMath::RandPointInCircle(ImageSize.X * 0.5f * 0.373f);
			UC_Util::Print("Find Slot");
		}
		else
		{
			UC_Util::Print("Cant Find Slot");

			return false;
		}
	}
	else
		return false;
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D RandomPointOnScreen;

	//TODO: 캐릭터 상태에 따라 탄퍼짐 or 직선
	if (OwnerCharacter->GetIsWatchingSight())
	{
		RandomPointOnScreen.X = (IronSightWindowLocation.X * ViewportSize.X );
		RandomPointOnScreen.Y = (IronSightWindowLocation.Y * ViewportSize.Y );
	}
	else
	{
		RandomPointOnScreen.X = (0.5 * ViewportSize.X + RandomPoint.X);
		RandomPointOnScreen.Y = (0.5 * ViewportSize.Y + RandomPoint.Y);
	}

	WolrdContorller->DeprojectScreenPositionToWorld(RandomPointOnScreen.X, RandomPointOnScreen.Y, WorldLocation, WorldDirection);
	FVector DestLocation = WorldLocation + WorldDirection * 100000;
	if (GetIsPartAttached(EPartsName::SCOPE))
	{
		if (
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE4 ||
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE8
			)
		{
			FVector AttachmentLocation = AttachedItem[EPartsName::SCOPE]->GetActorLocation();
			FRotator AttachmentRotation = AttachedItem[EPartsName::SCOPE]->GetActorRotation();
			FVector AttachmentForward = AttachmentRotation.Vector().GetSafeNormal();
			DestLocation = AttachmentLocation + AttachmentForward * 100000;
		}
	}

	//FVector ChildForward = TestDirection.Vector();
	//DestLocation = TestLocation + ChildForward * 100000;
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, DestLocation, ECC_Visibility, CollisionParams);

	DrawDebugSphere(GetWorld(), HitResult.Location, 1.0f, 12, FColor::Red, true);
	UC_Util::Print(float(HitResult.Distance));
	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	//FVector FireLocation2 = GunMesh->GetSocketLocation(FName("MuzzleSocke"));
	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireLocation2, FColor::Blue);
	FVector FireDirection;

	if (HitResult.Distance <= 1000 && HitResult.Distance >0)
	{
		FireDirection = HitResult.Location - FireLocation;
		FireDirection = FireDirection.GetSafeNormal();
		
		UC_Util::Print(HitResult.Location, FColor::Emerald);
		UC_Util::Print(HitResult.Distance, FColor::Cyan);
	}
	else
	{
		FireDirection = DestLocation - FireLocation;
		FireDirection = FireDirection.GetSafeNormal();
		//HasHit = false;
		//UC_Util::Print(FireDirection, FColor::Blue);

	}
	//FRotator LocalRotation(0, 0, 5.6);  

	float RadianValue = FMath::DegreesToRadians(0.06f);

	// 기존 벡터의 크기를 저장
	float OriginalLength = FireDirection.Size();

	// Z 값을 변화시키기 위해 현재 XY 평면에서의 크기를 유지
	FireDirection.Z += FMath::Tan(RadianValue);

	// 벡터를 다시 정규화하여 크기를 유지
	FireDirection = FireDirection.GetSafeNormal();
	UC_Util::Print(FireDirection, FColor::Blue);

	FireDirection *= 100;
	FireDirection *= BulletSpeed;
	OutLocation = FireLocation;
	OutDirection = FireDirection;
	OutHasHit = HasHit;
	OutHitLocation = HitResult.Location;
	return true;
}

void AC_Gun::SetAimSightWidget()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (OwnerPlayer)
		AimWidget = OwnerPlayer->GetCrosshairWidgetComponent()->GetAimWidget();
}

void AC_Gun::ShowAndHideWhileAiming()
{
	if (!OwnerCharacter) return;
	if (OwnerCharacter->GetEquippedComponent()->GetCurWeapon() != this) return;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!OwnerPlayer->GetIsWatchingSight()) return;
	FVector StartLocation = AimSightCamera->GetComponentLocation();
	FVector ForwardVector = AimSightCamera->GetForwardVector() * 15;
	FVector EndLocation = StartLocation + ForwardVector;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	//if (GetIsPartAttached(EPartsName::SCOPE))
	//	CollisionParams.AddIgnoredActor(AttachedItem[EPartsName::SCOPE]);
	CollisionParams.AddIgnoredComponent(GunMesh);
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (HasHit )
	{
		
		AimWidget->SetVisibility(ESlateVisibility::Visible);
		//UC_Util::Print(HitResult.GetActor()->GetName());
	}
	else
	{
		AimWidget->SetVisibility(ESlateVisibility::Hidden);

	}
		//UC_Util::Print("No Hit");

}

void AC_Gun::LoadMagazine()
{
	FString ClassPath = TEXT("/Game/Project_PUBG/Common/Weapon/GunWeapon/Magazine/BPC_Magazine.BPC_Magazine_C");
	//Magazine = LoadObject<AC_AttachableItem>(nullptr, ));

	// UClass로 불러오기
	UClass* MagazineClass = LoadObject<UClass>(nullptr, *ClassPath);
	if (MagazineClass)
	{
		//	// 인스턴스 생성
		FActorSpawnParameters Param{};
		Param.Owner = this;
		//ConsumableItem = GetWorld()->SpawnActor<AC_FirstAidKit>(ConsumableItemClass, Param);
		Magazine = GetWorld()->SpawnActor<AC_AttachableItem>(MagazineClass, Param);
	}
	if (IsValid(Magazine))
	{
		// 인스턴스를 GunMesh에 Attach
		Magazine->SetOwnerCharacter(OwnerCharacter);
		Magazine->SetIsAttached(true);
		//UC_Util::Print("Success To Load Magazine", FColor::Blue, 10.0f);

	}
	else
	{
		UC_Util::Print("Failed To Load Magazine", FColor::Blue, 10.0f);
	}

//	else
//	{
//		UC_Util::Print("Fail To Load Magazine Class", FColor::Blue, 10.0f);
//
//	}
}

void AC_Gun::SetMagazineVisibility(bool InIsVisible)
{
	if (IsValid(Magazine))
	{
		Magazine->SetMeshVisibility(InIsVisible);
		//UC_Util::Print("ASDaisujdfhaiseuehfaiesuhfaweiufh");
	}
}

bool AC_Gun::GetGunHasGrip()
{
	return 	IsValid(AttachedItem[EPartsName::GRIP]);
}

void AC_Gun::SetHolsterNames()
{
	for (int32 i = 0; i < (int32)EAttachmentNames::MAX; ++i) // EAttachmentNames에 MAX가 있다면
	{
		EAttachmentNames AttachmentName = (EAttachmentNames)i;
		AttachmentPartsHolsterNames.Add(AttachmentName);
		ScopeCameraLocations.Add(AttachmentName);
	}


	//AttachmentPartsHolsterNames.Add(EAttachmentNames::REDDOT, FName("Red_Dot_Socket"));
	//AttachmentPartsHolsterNames.Add(EAttachmentNames::SCOPE4, FName("4X_Scope_Socket"));
	//

	//ScopeCameraLocations.Add(EAttachmentNames::REDDOT, FVector4(7.f,    0.f, 15.f, 6.f   ));
	//ScopeCameraLocations.Add(EAttachmentNames::SCOPE4, FVector4(10.89f, 0.f, 14.75f, 6.5f));
	ScopeCameraLocations.Add(EAttachmentNames::MAX);


	for (int32 i = 0; i < (int32)EPartsName::MAX; ++i) // EAttachmentNames에 MAX가 있다면
	{
		EPartsName AttachmentName = (EPartsName)i;
		AttachedItem.Add(AttachmentName, nullptr);
		IsPartAttached.Add(AttachmentName, false);
		AttachedItemName.Add(AttachmentName, EAttachmentNames::MAX);
	}
}

void AC_Gun::SetIronSightMeshHiddenInGame(bool bInIsHidden)
{
	if (!IsValid(IronSightMesh)) return;
	IronSightMesh->SetHiddenInGame(bInIsHidden);
}

void AC_Gun::SetIsPartAttached(EPartsName InAttachmentName, bool bInIsAttached)
{
	IsPartAttached[InAttachmentName] = bInIsAttached;
}

void AC_Gun::SetSightCameraSpringArmLocation(FVector4 InLocationAndArmLength)
{
	FVector Location{};
	Location.X = InLocationAndArmLength.X;
	Location.Y = InLocationAndArmLength.Y;
	Location.Z = InLocationAndArmLength.Z;
	AimSightSpringArm->SetRelativeLocation(Location);
	AimSightSpringArm->TargetArmLength = InLocationAndArmLength.W;
}

void AC_Gun::SetScopeCameraMode(EAttachmentNames InAttachmentName)
{
	switch (InAttachmentName)
	{
	case EAttachmentNames::MAX:
	case EAttachmentNames::REDDOT:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 0.f;
		break;
	case EAttachmentNames::SCOPE4:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 6.5f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 5.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 4.0f;
		break;
	case EAttachmentNames::SCOPE8:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 6.5f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 5.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 4.0f;
		break;
	default:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 0.f;
		break;
	}


}

//void AC_Gun::SpawnBulletForTest()
//{
//
//	//FActorSpawnParameters Param2{};
//	//Param2.Owner = this;
//	//UClass* BulletBPClass = StaticLoadClass(AC_Bullet::StaticClass(), nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet.BPC_Bullet_C"));
//	//Bullet = GetWorld()->SpawnActor<AC_Bullet>(BulletBPClass, Param2);
//	//if (IsValid(Bullet))
//	//	UC_Util::Print("Created Bullet");
//
//}


