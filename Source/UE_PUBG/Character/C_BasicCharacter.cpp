// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_BasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Component/C_StatComponent.h"
#include "Component/C_ConsumableUsageMeshComponent.h"

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

	CrawlCollider = CreateDefaultSubobject<UCapsuleComponent>("CrawlCollider");
	CrawlCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//CrawlCollider->SetupAttachment(GetMesh());

	//CharacterMeshComponent->SetRelativeLocation();
}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
	InitialMainCameraRelativeRotation = MainCamera->GetRelativeRotation().Quaternion();
	C_MainSpringArm = Cast<USpringArmComponent>(GetDefaultSubobjectByName("MainSpringArm")); 

	StatComponent->SetOwnerCharacter(this);

	CrawlCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetSimulatePhysics(false);
}

// Called every frame
void AC_BasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UC_Util::Print(GetCharacterMovement()->GetMovementName());
	//UC_Util::Print(GetActorLocation().Z);
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
	// TODO : 도핑 했을 시, 도핑 계수 곱해주기

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
	SetColliderByPoseState(PoseState);
	if (InPoseState == EPoseState::CRAWL) GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AC_BasicCharacter::SetColliderByPoseState(EPoseState InPoseState)
{
	FVector MeshLocation = GetMesh()->GetRelativeTransform().GetLocation();

	switch (InPoseState)
	{
	case EPoseState::STAND:

		GetCapsuleComponent()->SetSimulatePhysics(true);

		GetCapsuleComponent()->SetCapsuleSize
		(
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Value,
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Key
		);
		
		GetMesh()->SetRelativeLocation(FVector(MeshLocation.X, MeshLocation.Y, POSE_BY_MESH_Z_POS[InPoseState]));
		
		CrawlCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetSimulatePhysics(false);

		return;
	case EPoseState::CROUCH:

		GetCapsuleComponent()->SetSimulatePhysics(true);

		GetCapsuleComponent()->SetCapsuleSize
		(
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Value,
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Key
		);

		GetMesh()->SetRelativeLocation(FVector(MeshLocation.X, MeshLocation.Y, POSE_BY_MESH_Z_POS[InPoseState]));

		CrawlCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetSimulatePhysics(false);

		//DrawDebugLine(GetWorld(), GetMesh()->GetBoneLocation("Neck"), Get)

		/*
		On input action Prone 1 - capsule component - set simulate physics = true, 2 - ProneCollider's Collision enabled = Collision enabled, 3 - capsule component - set simulate physics = false.
		*/

		return;
	case EPoseState::CRAWL:

		GetCapsuleComponent()->SetSimulatePhysics(true);

		GetCapsuleComponent()->SetCapsuleSize
		(
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Value,
			POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState].Key
		);

		GetMesh()->SetRelativeLocation(FVector(MeshLocation.X, MeshLocation.Y, POSE_BY_MESH_Z_POS[InPoseState]));

		CrawlCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetSimulatePhysics(false);

		return;
	case EPoseState::POSE_MAX:
		return;
	default:
		return;
	}
}

void AC_BasicCharacter::OnPoseTransitionGoing()
{
	PoseState = NextPoseState;
	SetColliderByPoseState(PoseState);
}

void AC_BasicCharacter::OnPoseTransitionFinish()
{
	//UC_Util::Print("Transition pose finished!", FColor::Cyan, 5.f);
	//PoseState = NextPoseState;
	bCanMove = true;
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

bool AC_BasicCharacter::CanChangePoseOnCurrentSurroundEnvironment(EPoseState InChangeTo)
{
	if (PoseState == InChangeTo) return false;

	const float CRAWL_START_OFFSET = 20.f;

	switch (InChangeTo)
	{
	case EPoseState::STAND:
	{
		// Crouch to stand or Crawl to Stand
		FVector StartLocation	 = GetActorLocation();
		StartLocation.Z			+= (PoseState == EPoseState::CROUCH) ? POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::CROUCH].Key - SWEEP_SPHERE_RAD : CRAWL_START_OFFSET;
		FVector DestLocation	 = StartLocation + FVector::UnitZ() * ((PoseState == EPoseState::CROUCH) ? CROUCH_TO_STAND_SWEEP_DIST : CRAWL_TO_STAND_SWEEP_DIST);

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(this);
		
		TArray<AActor*> AttachedActors{};
		GetAttachedActors(AttachedActors);
		CollisionParams.AddIgnoredActors(AttachedActors);

		FHitResult HitResult{};

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);

		if (HasHit) UC_Util::Print(HitResult.GetActor()->GetName());
		
		DrawDebugSphere(GetWorld(), StartLocation, SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		DrawDebugSphere(GetWorld(), DestLocation,  SWEEP_SPHERE_RAD, 10, FColor::Red, true);

		return !HasHit;
	}
	case EPoseState::CROUCH: // Crawl to Crouch만 확인 하면 됨
	{
		if (PoseState == EPoseState::STAND) return true; // Stand to crouch -> 언제든 자세를 바꿀 수 있음

		// Crawl to Crouch test

		FVector StartLocation	 = GetActorLocation();
		StartLocation.Z			+= CRAWL_START_OFFSET;
		FVector DestLocation	 = StartLocation + FVector::UnitZ() * CRAWL_TO_CROUCH_SWEEP_DIST;

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(this);

		TArray<AActor*> AttachedActors{};
		GetAttachedActors(AttachedActors);
		CollisionParams.AddIgnoredActors(AttachedActors);

		FHitResult HitResult{};

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);

		if (HasHit) UC_Util::Print(HitResult.GetActor()->GetName());

		DrawDebugSphere(GetWorld(), StartLocation, SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		DrawDebugSphere(GetWorld(), DestLocation,  SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		
		return !HasHit;
	}
	case EPoseState::CRAWL: // TODO : 지형 경사도 확인 & Crawl 충돌체가 충분히 들어갈 수 있는 상황인지 확인
	{
		FVector HeadStartLocation	= GetActorLocation() + GetActorForwardVector() * 75.f;
		FVector HeadDestLocation	= HeadStartLocation - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;
		FVector PelvisStartLocation = GetActorLocation();
		FVector PelvisDestLocation	= PelvisStartLocation - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(this);

		TArray<AActor*> AttachedActors{};
		GetAttachedActors(AttachedActors);
		CollisionParams.AddIgnoredActors(AttachedActors);

		FHitResult HeadHitResult{};
		FHitResult PelvisHitResult{};

		bool HasHeadHit		= GetWorld()->LineTraceSingleByChannel(HeadHitResult, HeadStartLocation, HeadDestLocation, ECC_Visibility, CollisionParams);
		bool HasPelvisHit	= GetWorld()->LineTraceSingleByChannel(PelvisHitResult, PelvisStartLocation, PelvisDestLocation, ECC_Visibility, CollisionParams);

		UC_Util::Print(HeadHitResult.ImpactPoint);
		DrawDebugLine(GetWorld(), HeadStartLocation, HeadHitResult.ImpactPoint, FColor::Red, true);
		DrawDebugLine(GetWorld(), PelvisStartLocation, PelvisHitResult.ImpactPoint, FColor::Red, true);

		if (!HasHeadHit || !HasPelvisHit) return false;
		// TODO : Length 체크
		//if (HeadHitResult.ImpactPoint.Length() )

		// 경사도 체크
		//HeadHitResult.ImpactPoint
		float A				= (HeadHitResult.ImpactPoint - PelvisHitResult.ImpactPoint).Length();
		float B				= FMath::Abs(HeadHitResult.ImpactPoint.Z - PelvisHitResult.ImpactPoint.Z);
		float SlopeDegree	= FMath::RadiansToDegrees(FMath::Asin(B / A));

		UC_Util::Print(SlopeDegree);

		//FRotator CrawlRelativeRotation = CrawlCollider->GetRelativeRotation();
		//CrawlRelativeRotation.Pitch += SlopeDegree;
		//CrawlCollider->SetRelativeRotation(CrawlRelativeRotation);

		return SlopeDegree < CRAWL_DEGREE_LIMIT;
	}
	case EPoseState::POSE_MAX: default: return false;
	}
}
