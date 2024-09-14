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

	Bullet = LoadObject<AC_Bullet>(nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet"));
	//ItemType 설정.
	MyItemType = EItemTypes::MAINGUN;
}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();
	SpawnBulletForTest();
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
	if (IsValid(Bullet))
	{
		UC_Util::Print("FindBullet");

		UProjectileMovementComponent* ProjectileMovement = Bullet->FindComponentByClass<UProjectileMovementComponent>();
		if (ProjectileMovement)
		{
			ProjectileMovement->InitialSpeed = BulletSpeed;

		}
	}
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
	OwnerCharacter->SetHandState(EHandState::WEAWPON_GUN);
	
	
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

	HitResult = {};
	FVector WorldLocation, WorldDirection;
	APlayerController* WolrdContorller = GetWorld()->GetFirstPlayerController();
	//WolrdContorller->DeprojectScreenPositionToWorld(0.5, 0.5, WorldLocation,WorldDirection);
	//Controller->ActorLineTraceSingle(HitResult, StartLocation, StartLocation + ForwardVector, ECollisionChannel::ECC_Visibility,FCollisionQueryParams::DefaultQueryParam);
	//UC_Util::Print(HitResult.Distance);
	//UC_Util::Print(WorldLocation);
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
		//UC_Util::Print("FoundImage");

		ImageSlot = Cast<UCanvasPanelSlot>(MyImage->Slot);
		if (ImageSlot)
		{
			// 이때, Cast<UPanelSlot>(ImageSlot)으로 다른 타입으로 캐스팅할 수 있음
			FVector2D ImageSize = ImageSlot->GetSize();

			// 이미지 크기 확인
			//UC_Util::Print("FoundImage");
			//UC_Util::Print(float(ImageSize.X));
			//UC_Util::Print(float(ImageSize.Y));
			RandomPoint = FMath::RandPointInCircle(ImageSize.X / 2.0f);
			//UC_Util::Print(float(RandomPoint.X));
			//UC_Util::Print(float(RandomPoint.Y));
			//UE_LOG(LogTemp, Log, TEXT("Image Size: X = %f, Y = %f"), ImageSize.X, ImageSize.Y);
		}
		else
			return false;
	}
	else
		return false;
	//UC_Util::Print(float(RandomPoint.X));
	//UC_Util::Print(float(RandomPoint.Y));
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	//UC_Util::Print(float(ViewportSize.X));
	//UC_Util::Print(float(ViewportSize.Y));

	FVector2D RandomPointOnScreen;
	RandomPointOnScreen.X = (0.5 * ViewportSize.X + RandomPoint.X);
	RandomPointOnScreen.Y = (0.5 * ViewportSize.Y + RandomPoint.Y);

	//UC_Util::Print(float(RandomPointOnScreen.X));
	//UC_Util::Print(float(RandomPointOnScreen.Y));

	WolrdContorller->DeprojectScreenPositionToWorld(RandomPointOnScreen.X, RandomPointOnScreen.Y, WorldLocation, WorldDirection);
	//WolrdContorller->DeprojectScreenPositionToWorld(0.5, 0.5, WorldLocation, WorldDirection);
	//UC_Util::Print(float(WorldDirection.X));
	//UC_Util::Print(float(WorldDirection.Y));
	//UC_Util::Print(float(WorldDirection.Z));
	FVector DestLocation = WorldLocation + WorldDirection * 10000;

	//UC_Util::Print(float(WorldLocation.X));
	//UC_Util::Print(float(WorldLocation.Y));
	//UC_Util::Print(float(WorldLocation.Z));
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, DestLocation, ECC_Visibility, CollisionParams);

	DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Red, true);
	UC_Util::Print(float(HitResult.Distance));
	//UC_Util::Print(float(HitResult.Location.Y));
	//Controller->ActorLineTraceSingle(nullptr, ),)
	if (IsValid(Bullet))
	{
		//UC_Util::Print("FindBullet");

		UProjectileMovementComponent* ProjectileMovement = Bullet->BulletProjectileMovement;
		if (ProjectileMovement)
		{
			UC_Util::Print("Fire!");
			Bullet->SetActorLocation(this->GetActorLocation());
			//ProjectileMovement->Velocity = WorldDirection * ProjectileMovement->InitialSpeed;
			Bullet->BulletProjectileMovement->Velocity = WorldDirection * 10;
			Bullet->BulletProjectileMovement->SetActive(true);
		}
	}
	return true;
}

void AC_Gun::SpawnBulletForTest()
{

	FActorSpawnParameters Param2{};
	Param2.Owner = OwnerCharacter;
	Bullet = GetWorld()->SpawnActor<AC_Bullet>(AC_Bullet::StaticClass(), Param2);
	if (IsValid(Bullet))
		UC_Util::Print("Created Bullet");

}


