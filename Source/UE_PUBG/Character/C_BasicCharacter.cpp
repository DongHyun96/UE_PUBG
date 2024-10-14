// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_BasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PhysicsVolume.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Component/C_StatComponent.h"
#include "Component/C_ConsumableUsageMeshComponent.h"
#include "Component/C_PoseColliderHandlerComponent.h"
#include "Component/C_SwimmingComponent.h"

#include "Components/CapsuleComponent.h"

#include "Utility/C_Util.h"


// Sets default values
AC_BasicCharacter::AC_BasicCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EquippedComponent = CreateDefaultSubobject<UC_EquippedComponent>("EquippedComponent");
	EquippedComponent->SetOwnerCharacter(this);

	Inventory = CreateDefaultSubobject<UC_InvenComponent>("C_Inventory");
	Inventory->SetOwnerCharacter(this);

	StatComponent = CreateDefaultSubobject<UC_StatComponent>("StatComponent");

	ConsumableUsageMeshComponent = CreateDefaultSubobject<UC_ConsumableUsageMeshComponent>("ConsumableUsageMeshComponent");
	ConsumableUsageMeshComponent->SetOwnerCharacter(this);

	PoseColliderHandlerComponent = CreateDefaultSubobject<UC_PoseColliderHandlerComponent>("PoseColliderHandlerComponent");
	PoseColliderHandlerComponent->SetOwnerCharacter(this);

	SwimmingComponent = CreateDefaultSubobject<UC_SwimmingComponent>("SwimmingComponent");
	SwimmingComponent->SetOwnerCharacter(this);
}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
	InitialMainCameraRelativeRotation = MainCamera->GetRelativeRotation().Quaternion();
	C_MainSpringArm = Cast<USpringArmComponent>(GetDefaultSubobjectByName("MainSpringArm")); 

	GetPhysicsVolume()->FluidFriction = 2.5f;
	StatComponent->SetOwnerCharacter(this);
}

// Called every frame
void AC_BasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AC_BasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

float AC_BasicCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	// Deprecated in UE_PUBG Project
	// Do Nothing
	UC_Util::Print
	(
		"From AC_BasicCharacter:PlayAnimMontage : This function is deprecated. Use PriorityAnimMontage parameter function instead",
		FColor::Cyan,
		10.f
	);

	return 0.0f;
}

float AC_BasicCharacter::PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate, FName StartSectionName)
{
	if (!IsValid(PAnimMontage.AnimMontage)) return 0.f;

	FName TargetGroup = PAnimMontage.AnimMontage->GetGroupName();

	// �ڽ��� group���� anim montage�� �ѹ��� ����� �� ���� �� �ٷ� ���
	if (!CurPriorityAnimMontageMap.Contains(TargetGroup))
	{
		CurPriorityAnimMontageMap.Add(TargetGroup, PAnimMontage);
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	FPriorityAnimMontage TargetGroupCurMontage = CurPriorityAnimMontageMap[TargetGroup];

	// ������ AnimMontage�� ����� �̹� ������ ��
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(TargetGroupCurMontage.AnimMontage))
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// ���� ������� PriorityAnimMontage�� ���� ��
	
	// Priority ���ؼ� ���� Priority���� ũ�ų� ���� Priority��� ���� ���� AnimMontage ���
	if (PAnimMontage.Priority >= TargetGroupCurMontage.Priority)
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// Priority�� ���� ������� Montage�� �� ũ�ٸ� ������ ������� �ʰ� �׳� return
	return 0.0f;
}

float AC_BasicCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FString Str = "Character Damaged! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);

	UC_Util::Print(Str, FColor::Cyan, 3.f);

	StatComponent->TakeDamage(DamageAmount);

	return DamageAmount;
}

void AC_BasicCharacter::UpdateMaxWalkSpeed(const FVector2D& MovementVector)
{
	//GetCharacterMovement()->MaxWalkSpeed =	(PoseState == EPoseState::STAND)  ? 370.f :
	//										(PoseState == EPoseState::CROUCH) ? 200.f :
	//										(PoseState == EPoseState::CRAWL)  ? 100.f : 600.f;
	// TODO : ���� ���� ��, ���� ��� �����ֱ�
	if (SwimmingComponent->IsSwimming())
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		return;
	}

	switch (PoseState)
	{
	case EPoseState::STAND:
		if (bIsWalking || bIsActivatingConsumableItem)
		{
			GetCharacterMovement()->MaxWalkSpeed = 170.f;
			break;
		}

		if (MovementVector.X == 1.f) // �� , �� �밢��
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 630.f : 470.f; 
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f; 
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 350.f; // �� ����
		break;
	case EPoseState::CROUCH:
		if (bIsWalking || bIsActivatingConsumableItem)
		{
			GetCharacterMovement()->MaxWalkSpeed = 130.f;
			break;
		}
		if (MovementVector.X == 1.f) // �� , �� �밢��
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 480.f : 340.f;
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 290.f;
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 250.f; // �� ����
		break;
	case EPoseState::CRAWL:
		GetCharacterMovement()->MaxWalkSpeed =	bIsActivatingConsumableItem ? 0.f :
												bIsWalking ? 60.f : 120.f;
		break;
	case EPoseState::POSE_MAX:
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed *= BoostingSpeedFactor;
}

void AC_BasicCharacter::SetPoseState(EPoseState InPoseState)
{
	PoseState = InPoseState;
	PoseColliderHandlerComponent->SetColliderByPoseState(PoseState);
}

bool AC_BasicCharacter::SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo)
{
	// TODO : Enemy ĳ���Ϳ� ���� �ڼ� ��ȯ ����

	return false;
}

void AC_BasicCharacter::OnPoseTransitionGoing()
{
	PoseState = NextPoseState;
	PoseColliderHandlerComponent->SetColliderByPoseState(PoseState);
}

void AC_BasicCharacter::OnPoseTransitionFinish()
{
	//UC_Util::Print("Transition pose finished!", FColor::Cyan, 5.f);
	//PoseState = NextPoseState;
	//bCanMove = true; // PoseBySizeLerp�� ���� �ڿ� ������ �� �ֵ��� ó��
	bIsPoseTransitioning = false;

	// Pose Transition�� ���� ��, Delegate call back ó��
	if (Delegate_OnPoseTransitionFin.IsBound()) Delegate_OnPoseTransitionFin.Broadcast();
}

bool AC_BasicCharacter::ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState)
{
	// �ٸ� PriorityAnimMontage�� ���� �ڼ���ȯ�� �ȵ� ��Ȳ�̸� return false
	if (PlayAnimMontage(TransitionMontage) == 0.f) return false;

	// �̹� �ڼ��� �ٲٴ� ���̶�� return false
	//if (bIsPoseTransitioning) return false;

	NextPoseState			= InNextPoseState;
	bCanMove				= false;
	bIsPoseTransitioning	= true;

	return true;
}
