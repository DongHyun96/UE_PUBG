// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "UMG.h"


#include "Components/PanelWidget.h"
#include "Components/NamedSlotInterface.h"
#include "Utility/C_Util.h"

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
#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"

#include "Item/Weapon/Gun/C_Bullet.h"


// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

	//Bullet = LoadObject<AC_Bullet>(nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet"));
	//ItemType 설정.
	MyItemType = EItemTypes::MAINGUN;
}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();
	SetBulletSpeed();

	AimSightCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
	AimSightSpringArm = Cast<USpringArmComponent>(GetDefaultSubobjectByName("RifleSightSpringArm"));
	//if(IsValid(AimSightCamera))
	AimSightCamera->SetActive(false);
	//블루프린트에서 할당한 Skeletal Mesh 찾아서 변수에 저장
	GunMesh = FindComponentByClass<USkeletalMeshComponent>();
	GunMesh->SetupAttachment(RootComponent);
	//AimSightCamera->SetupAttachment(GunMesh);
	//CurveFloatForSwitchCameraChange = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/Project_PUBG/Hyunho/CameraMoving/CF_CameraMoving"));

	UUserWidget* AimWidget = LoadObject<UUserWidget>(nullptr,TEXT("/All/Game/Project_PUBG/Hyunho/TempWidget/WBP_CrossHair"));
	//Bullet = LoadObject<AC_Bullet>(nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet"));

	//UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>
	

	//if (IsValid(MyMesh))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh: %s"), *MyMesh->GetName());
	//	FString TheFloatStr = "Found Mesh";
	//	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	//	// 추가적인 논리로 스켈레탈 메쉬를 처리합니다.
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh not found!"));
	//}
	
}

void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerCharacter) return;

	CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	//GunMesh->SetWorldRotation(OwnerCharacter->GetControlRotation());
	if (IsValid(GunMesh))
	{
		FTransform TempVec = GunMesh->GetSocketTransform("LeftHandSocket");
		FVector OutLocation{};
		FRotator OutRotation{};
		OwnerCharacter->GetMesh()->TransformToBoneSpace("RightHand", TempVec.GetLocation(), TempVec.GetRotation().Rotator(), OutLocation, OutRotation);
		LeftHandSocketLocation.SetLocation(OutLocation);
		LeftHandSocketLocation.SetRotation(OutRotation.Quaternion());
		//FTransform RootTrasnform = OwnerCharacter->GetMesh()->GetSocketTransform("SpineGunSocket");
		//
		//LeftHandSocketLocation = TempVec.GetRelativeTransform(RootTrasnform);
		//FString TheFloatStr = FString::SanitizeFloat(LeftHandSocketLocation.GetLocation().X);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	}
	HandleSpringArmRotation();
	GetPlayerIsAimDownOrNot();

}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{
	switch (CurState)
	{
	case EGunState::MAIN_GUN:
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			MAIN_HOLSTER_SOCKET_NAME
		);
		break;
	case EGunState::SUB_GUN:
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			SUB_HOLSTER_SOCKET_NAME
		);
		break;
	default:
		return false;
		break;
	}

}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[EGunState::SUB_GUN].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//if (!IsValid(DrawMontage)) return false;
	// 만일 SUB_GUN의 Draw가 실행된다면 왼손으로 먼저 Attach
	//현재 왼손으로 옮겨지고 있다면 다시 어태치 하지 않고 If문 밑으로 진행
	FName CurSocketName = GetAttachParentSocketName();
	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(DrawMontage) && CurSocketName != SUB_DRAW_SOCKET_NAME)
	{
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			SUB_DRAW_SOCKET_NAME
		);
	}
	OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
	
	
	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
	
}

bool AC_Gun::SetAimingDown()
{
	Cast<AC_Player>(OwnerCharacter)->SetToAimDownSight();
	//CharacterMesh->HideBoneByName(FName("HeadBoneName"), EPhysBodyOp::PBO_None);

	//OwnerCharacter->bUseControllerRotationYaw = true;

	//AimDown 일 때 머리숨기기
	//TODO : 내 카메라에만 안보이고 상대방 카메라에선 보이게 만들기
	OwnerCharacter->GetMesh()->HideBoneByName(FName("Head"), EPhysBodyOp::PBO_None);
	AimSightCamera->SetActive(true);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2);
	bIsAimDown = true;

	return true;
}
//견착 조준만할 때 Player AimKePress함수로 메인카메라에서 에임 카메라로 바꿔주기
bool AC_Gun::SetAimingPress()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer)) return false;
	if (OwnerPlayer->GetIsAimDown()) return false;
	OwnerPlayer->SetToAimKeyPress();
	//bIsAimDown = true;
	//OwnerCharacter->bUseControllerRotationYaw = true;

	return true;
}

bool AC_Gun::BackToMainCamera()
{
	AimSightCamera->SetActive(false);
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget() == this)
	{
		UC_Util::Print("Fuck");
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(OwnerCharacter, 0.2);
	}
	OwnerCharacter->GetMesh()->UnHideBoneByName(FName("Head"));
	OwnerCharacter->bUseControllerRotationYaw = false;
	bIsAimDown = false;
	Cast<AC_Player>(OwnerCharacter)->BackToMainCamera();
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

void AC_Gun::OnOwnerCharacterPoseTransitionFin()
{
}


bool AC_Gun::FireBullet()
{
	FHitResult HitResult;

	AController* Controller = OwnerCharacter->GetController();
	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector StartLocation = PlayerCamera->GetCameraLocation();
	FRotator CameraRotation = PlayerCamera->GetCameraRotation();

	FVector ForwardVector = CameraRotation.Vector().GetSafeNormal() * 10000;
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.AddIgnoredActor(PlayerCamera);
	//CollisionParams.AddIgnoredActor(Bullet);

	HitResult = {};
	FVector WorldLocation, WorldDirection;
	APlayerController* WolrdContorller = GetWorld()->GetFirstPlayerController();
	TArray<UUserWidget*> FoundWidgets;
	UUserWidget* MyWidget;
	UImage* MyImage;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), true);
	if (FoundWidgets.Num() > 0)
	{
		MyWidget = FoundWidgets[0];  // 첫 번째 위젯 가져오기
		//UC_Util::Print("FoundWidget");

	}
	else 
		return false;

	MyImage = Cast<UImage>(MyWidget->WidgetTree->FindWidget(FName("Image_53")));
	UCanvasPanelSlot* ImageSlot;
	FVector2D RandomPoint;
	if (IsValid(MyImage))
	{

		ImageSlot = Cast<UCanvasPanelSlot>(MyImage->Slot);
		if (ImageSlot)
		{
			// 이때, Cast<UPanelSlot>(ImageSlot)으로 다른 타입으로 캐스팅할 수 있음
			FVector2D ImageSize = ImageSlot->GetSize();
			RandomPoint = FMath::RandPointInCircle(ImageSize.X / 2.0f);

		}
		else
			return false;
	}
	else
		return false;
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D RandomPointOnScreen;
	//RandomPointOnScreen.X = (0.5 * ViewportSize.X + RandomPoint.X);
	//RandomPointOnScreen.Y = (0.5 * ViewportSize.Y + RandomPoint.Y);
	RandomPointOnScreen.X = (0.5 * ViewportSize.X );
	RandomPointOnScreen.Y = (0.5 * ViewportSize.Y );

	WolrdContorller->DeprojectScreenPositionToWorld(RandomPointOnScreen.X, RandomPointOnScreen.Y, WorldLocation, WorldDirection);

	FVector DestLocation = WorldLocation + WorldDirection * 10000;
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, DestLocation, ECC_Visibility, CollisionParams);

	DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Red, true);
	UC_Util::Print(float(HitResult.Distance));
	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	FVector FireDirection;

	if (HasHit)
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
	FireDirection *= 940;	
	//UC_Util::Print(float(HitResult.Location.Y));
	//Controller->ActorLineTraceSingle(nullptr, ),)
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
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

		//return Bullet->Fire(this, FireLocation, FireDirection);
		Bullet->Fire(this, FireLocation, FireDirection);
		if (BulletCount > 100)
			return true;
	}
	UC_Util::Print("No More Bullets in Pool");
	return false;
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


