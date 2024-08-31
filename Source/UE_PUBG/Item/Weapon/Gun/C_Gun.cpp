// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"
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


// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

	//ItemType ����.
	MyItemType = EItemTypes::MAINGUN;
}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();
	AimSightCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
	AimSightSpringArm = Cast<USpringArmComponent>(GetDefaultSubobjectByName("RifleSightSpringArm"));
	//if(IsValid(AimSightCamera))
	AimSightCamera->SetActive(false);
	//�������Ʈ���� �Ҵ��� Skeletal Mesh ã�Ƽ� ������ ����
	GunMesh = FindComponentByClass<USkeletalMeshComponent>();
	GunMesh->SetupAttachment(RootComponent);
	//AimSightCamera->SetupAttachment(GunMesh);

	

	//if (IsValid(MyMesh))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh: %s"), *MyMesh->GetName());
	//	FString TheFloatStr = "Found Mesh";
	//	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	//	// �߰����� ���� ���̷�Ż �޽��� ó���մϴ�.
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh not found!"));
	//}
	
}

void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	// ���� SUB_GUN�� Draw�� ����ȴٸ� �޼����� ���� Attach
	//���� �޼����� �Ű����� �ִٸ� �ٽ� ����ġ ���� �ʰ� If�� ������ ����
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

	//AimDown �� �� �Ӹ������
	//TODO : �� ī�޶󿡸� �Ⱥ��̰� ���� ī�޶󿡼� ���̰� �����
	OwnerCharacter->GetMesh()->HideBoneByName(FName("Head"), EPhysBodyOp::PBO_None);
	AimSightCamera->SetActive(true);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2);
	return true;
}
//���� ���ظ��� �� Player AimKePress�Լ��� ����ī�޶󿡼� ���� ī�޶�� �ٲ��ֱ�
bool AC_Gun::SetAimingPress()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (OwnerPlayer->GetIsAimDown()) return false;
	OwnerPlayer->SetToAimKeyPress();
	bIsAimPress = true;
	//OwnerCharacter->bUseControllerRotationYaw = true;

	return true;
}

bool AC_Gun::BackToMainCamera()
{
	AimSightCamera->SetActive(false);
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget() == this)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(OwnerCharacter, 0.2);
	}
	OwnerCharacter->GetMesh()->UnHideBoneByName(FName("Head"));
	OwnerCharacter->bUseControllerRotationYaw = false;

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

void AC_Gun::OnOwnerCharacterPoseTransitionFin()
{
}


