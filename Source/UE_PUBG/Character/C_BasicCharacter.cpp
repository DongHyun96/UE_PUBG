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
#include "Component/C_SkyDivingComponent.h"
#include "Component/C_InvenSystem.h"

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

	InvenSystem = CreateDefaultSubobject<UC_InvenSystem>("C_InvenSystem");
	InvenSystem->SetOwnerCharacter(this);

	StatComponent = CreateDefaultSubobject<UC_StatComponent>("StatComponent");

	ConsumableUsageMeshComponent = CreateDefaultSubobject<UC_ConsumableUsageMeshComponent>("ConsumableUsageMeshComponent");
	ConsumableUsageMeshComponent->SetOwnerCharacter(this);

	PoseColliderHandlerComponent = CreateDefaultSubobject<UC_PoseColliderHandlerComponent>("PoseColliderHandlerComponent");
	PoseColliderHandlerComponent->SetOwnerCharacter(this);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->InitSphereRadius(100.0f); // 탐지 반경 설정
	DetectionSphere->SetupAttachment(RootComponent);

	//DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_BasicCharacter::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_BasicCharacter::OnOverlapEnd);
	//CrawlCollider->SetupAttachment(GetMesh());


	SwimmingComponent = CreateDefaultSubobject<UC_SwimmingComponent>("SwimmingComponent");
	SwimmingComponent->SetOwnerCharacter(this);

	SkyDivingComponent = CreateDefaultSubobject<UC_SkyDivingComponent>("SkyDivingComponent");
	SkyDivingComponent->SetOwnerCharcter(this);
}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	// For Testing
	//MainState = EMainState::SKYDIVING;
	//SkyDivingComponent->SetSkyDivingState(ESkyDivingState::READY);

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
/// <summary>
/// 아이템이 캐릭터의 근처에 있을 때.
/// </summary>
/// <param name="OverlappedComp"></param>
/// <param name="OtherActor"></param>
/// <param name="OtherComp"></param>
/// <param name="OtherBodyIndex"></param>
/// <param name="bFromSweep"></param>
/// <param name="SweepResult"></param>
void AC_BasicCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FString TheFloatStr = FString::SanitizeFloat(this->Inventory->GetCurVolume());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, TheFloatStr);

	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);


	if (IsValid(OverlappedItem) && (OverlappedItem->GetOwnerCharacter() == nullptr))
	{
		UC_Util::Print("OverlappedItem");
		//UC_Util::Print(*OverlappedItem->GetName());

		//Inventory->GetNearItems().Add(OverlappedItem);
		Inventory->AddItemToAroundList(OverlappedItem);
		//Inventory->InitInvenUI();
		if (!InvenSystem) return;
	    //InvenSystem->InitializeList();
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
void AC_BasicCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AC_Item* OverlappedItem = Cast<AC_Item>(OtherActor);

	if (OverlappedItem)
	{
		//Inventory->GetNearItems().Remove(OverlappedItem);
		Inventory->RemoveItemToAroundList(OverlappedItem);
		//Inventory->InitInvenUI();
		if (!InvenSystem) return;
		//InvenSystem->InitializeList();
	}
}
float AC_BasicCharacter::PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate, FName StartSectionName)
{
	if (!IsValid(PAnimMontage.AnimMontage)) return 0.f;

	FName TargetGroup = PAnimMontage.AnimMontage->GetGroupName();

	// 자신의 group내의 anim montage가 한번도 재생된 적 없을 땐 바로 재생
	if (!CurPriorityAnimMontageMap.Contains(TargetGroup))
	{
		CurPriorityAnimMontageMap.Add(TargetGroup, PAnimMontage);
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	FPriorityAnimMontage TargetGroupCurMontage = CurPriorityAnimMontageMap[TargetGroup];

	// 직전의 AnimMontage의 재생이 이미 끝났을 때
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(TargetGroupCurMontage.AnimMontage))
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// 현재 재생중인 PriorityAnimMontage가 있을 때
	
	// Priority 비교해서 현재 Priority보다 크거나 같은 Priority라면 새로 들어온 AnimMontage 재생
	if (PAnimMontage.Priority >= TargetGroupCurMontage.Priority)
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// Priority가 현재 재생중인 Montage가 더 크다면 새로이 재생하지 않고 그냥 return
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

		if (MovementVector.X == 1.f) // 앞 , 앞 대각선
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 630.f : 470.f; 
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f; 
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 350.f; // 뒷 방향
		break;
	case EPoseState::CROUCH:
		if (bIsWalking || bIsActivatingConsumableItem)
		{
			GetCharacterMovement()->MaxWalkSpeed = 130.f;
			break;
		}
		if (MovementVector.X == 1.f) // 앞 , 앞 대각선
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 480.f : 340.f;
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 290.f;
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 250.f; // 뒷 방향
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
	// TODO : Enemy 캐릭터에 대한 자세 변환 적용 - Player는 작성 완료 BasicCharacter 작성 필요

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
	//bCanMove = true; // PoseBySizeLerp가 끝난 뒤에 움직일 수 있도록 처리
	bIsPoseTransitioning = false;

	// Pose Transition이 끝난 뒤, Delegate call back 처리
	if (Delegate_OnPoseTransitionFin.IsBound()) Delegate_OnPoseTransitionFin.Broadcast();
}

bool AC_BasicCharacter::ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState)
{
	// 다른 PriorityAnimMontage에 의해 자세전환이 안된 상황이면 return false
	if (PlayAnimMontage(TransitionMontage) == 0.f) return false;

	// 이미 자세를 바꾸는 중이라면 return false
	//if (bIsPoseTransitioning) return false;

	NextPoseState			= InNextPoseState;
	bCanMove				= false;
	bIsPoseTransitioning	= true;

	return true;
}
