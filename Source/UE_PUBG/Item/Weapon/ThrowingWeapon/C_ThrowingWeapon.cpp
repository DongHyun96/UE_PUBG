// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_AnimBasicCharacter.h"

#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.h"
#include "C_FlashBangExplode.h"
#include "C_SmokeGrndExplode.h"

#include "Utility/C_Util.h"


TArray<AC_ThrowingWeapon*> AC_ThrowingWeapon::ThrowablePool{};

USkeletalMeshComponent* AC_ThrowingWeapon::OwnerMeshTemp{};

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Collider = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PathSpline = CreateDefaultSubobject<USplineComponent>("PredictedPathSpline");
	PredictedEndPoint = CreateDefaultSubobject<UStaticMeshComponent>("PredictedPathEndPointMesh");
	PredictedEndPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PredictedEndPoint->SetVisibility(false);
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();
}

void AC_ThrowingWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed) return;

	if (OwnerMeshTemp)
	{
		OwnerMeshTemp->DestroyComponent();
		OwnerMeshTemp = nullptr;
	}

	for (auto& item : ThrowablePool)
		item->Destroy();

	ThrowablePool.Empty();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(OwnerCharacter))
		return;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	// TODO : 투척류를 핀까지만 뽑았고 쿠킹을 안했을 시 다시 집어넣음
	// TODO : 투척류를 안전손잡이까지 뽑았다면 현재 위치에 현재 투척류 그냥 바닥에 떨굼

	SetActorHiddenInGame(true);

	ProjectileMovement->Deactivate();

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	SetActorHiddenInGame(false);

	ProjectileMovement->Deactivate();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	UINT numOfGrenade =  ThrowablePool.Num() + 1;
	FString DebugMessage = "Left Grenade Count : " + FString::FromInt(numOfGrenade);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::InitTestPool(AC_BasicCharacter* InOwnerCharacter, UClass* Class, UC_EquippedComponent* EquippedComponent)
{
	ThrowablePool.Empty();

	// 배낭에 있는 것처럼 세팅
	for (int i = 0; i < TESTPOOLCNT; i++)
	{
		FActorSpawnParameters Param{};
		Param.Owner = InOwnerCharacter;

		AC_ThrowingWeapon* ThrowWeapon = EquippedComponent->GetWorld()->SpawnActor<AC_ThrowingWeapon>(Class, Param);

		if (ThrowWeapon)
		{
			ThrowWeapon->SetOwnerCharacter(InOwnerCharacter);
			ThrowablePool.Add(ThrowWeapon);
		}
	}

	if (OwnerMeshTemp)
	{
		OwnerMeshTemp->DestroyComponent();
		OwnerMeshTemp = nullptr;
	}
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	if (bIsCharging) HandlePredictedPath();
	else
	{
		// 다음 던지기 동작 실행
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
		ClearSpline();
	}
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Direction 구하는 방법 1
	/*FVector ActorForward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	UC_AnimBasicCharacter* OwnerAnim = Cast<UC_AnimBasicCharacter>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	if (!IsValid(OwnerAnim))
	{
		FString DebugMessage = "AC_ThrowingWeapon::OnThrowThrowable : OwnerAnim not valid!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		Direction = ActorForward;
	}
	else
	{
		FRotator SpineRotation	  = OwnerAnim->GetCSpineRotation();
		FVector SpineForward	  = FRotationMatrix(SpineRotation).GetUnitAxis(EAxis::X);
		FVector CombinedDirection = FRotationMatrix(OwnerCharacter->GetActorRotation()).TransformVector(SpineForward);
		Direction = CombinedDirection;
	}*/

	UpdateProjectileLaunchValues();

	//UC_Util::Print(ProjStartLocation);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	this->SetActorLocation(ProjStartLocation);
	
	ProjectileMovement->Velocity = ProjLaunchVelocity;
	ProjectileMovement->Activate();

	if (!bIsCooked) StartCooking();
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	bIsOnThrowProcess = false;

	bIsCharging = false;

	// Ready 도중 ProcessEnd가 될 수 있기 때문에 Predicted Path spline 모두 지워주기
	ClearSpline();

	// 현재 Throw AnimMontage 멈추기 (우선순위 때문에 멈춰야 함)
	OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// 현재 투척류 장착 해제 바로 하기
	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// TODO : 가방에 투척류 있는지 확인해서 바로 다음 무기 장착하기 (우선순위 - 같은 종류의 투척류)

	if (!ThrowablePool.IsEmpty())
	{
		AC_ThrowingWeapon* ThrowWeapon = ThrowablePool[0];

		OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowWeapon);

		// 가방에 있는 투척류 하나 지우기
		ThrowablePool.RemoveAt(0);

		// 바로 다음 투척류 꺼내기
		OwnerCharacter->PlayAnimMontage(ThrowWeapon->GetCurDrawMontage());

		return;
	}

	/*
	남아 있는 투척류가 없다면 주무기1, 주무기2 순으로 장착 시도, 없다면 UnArmed인 상태로 돌아가기
	*/

	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN)) return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN)) return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);

}

void AC_ThrowingWeapon::InitExplodeStrategy(EThrowableType ThrowableType)
{
	switch (ThrowableType)
	{
	case EThrowableType::GRENADE:
		ExplodeStrategy = NewObject<AC_GrenadeExplode>(this);
		return;
	case EThrowableType::FLASH_BANG:
		ExplodeStrategy = NewObject<AC_FlashBangExplode>(this);
		return;
	case EThrowableType::SMOKE:
		ExplodeStrategy = NewObject<AC_SmokeGrndExplode>(this);
		return;
	default:
		break;
	}
}

void AC_ThrowingWeapon::StartCooking()
{
	bIsCooked = true;

	// TODO : Ready 상태에서 Cooking 시작하면 남은 시간 HUD 띄우기
	UC_Util::Print("Throwable Starts cooking");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking이 안되었다면 (안전손잡이가 날아가지 않았다면)
	if (ProjectileMovement->IsActive()) return false;	// 이미 던졌었다면

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f의 속력으로 날리기
	ProjectileMovement->Activate();

	OnThrowProcessEnd();

	return true;
}

void AC_ThrowingWeapon::Explode()
{
	if (!ExplodeStrategy)
	{
		UC_Util::Print("From AC_ThrowingWeapon::Explode : Explode Strategy nullptr!");
		return;
	}

	bool Exploded = ExplodeStrategy->UseStrategy(this);


	if (ExplodeEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation());

	if (GetAttachParentActor()) ReleaseOnGround(); // 손에서 아직 떠나지 않았을 때
		
	//if (Exploded) this->Destroy();
	if (Exploded)
	{
		this->SetActorHiddenInGame(true);
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				this->Destroy();
			}, 10.f, false);
	}
}

FVector AC_ThrowingWeapon::GetPredictedThrowStartLocation()
{
	if (!IsValid(OwnerMeshTemp))
	{
		OwnerMeshTemp = NewObject<USkeletalMeshComponent>(OwnerCharacter);

		if (!IsValid(OwnerMeshTemp))
		{
			UC_Util::Print("From AC_ThrowingWeapon::DrawDebugPredictedPath : OwnerTempMesh Not inited!");
			return FVector::ZeroVector;
		}

		OwnerMeshTemp->SetVisibility(false);
		OwnerMeshTemp->RegisterComponent();
		OwnerMeshTemp->SetSkeletalMesh(OwnerCharacter->GetMesh()->SkeletalMesh);
		OwnerMeshTemp->SetAnimInstanceClass(OwnerCharacter->GetMesh()->GetAnimInstance()->GetClass());
		OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());

		OwnerMeshTemp->GetAnimInstance()->Montage_Play(CurThrowProcessMontages.ThrowMontage.AnimMontage);
		OwnerMeshTemp->GetAnimInstance()->Montage_SetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage, 0.33f);
		OwnerMeshTemp->GetAnimInstance()->Montage_Pause();

		OwnerMeshTemp->TickPose(GetWorld()->GetDeltaSeconds(), true);
		OwnerMeshTemp->RefreshBoneTransforms();
		OwnerMeshTemp->RefreshFollowerComponents();
		OwnerMeshTemp->UpdateComponentToWorld();
	}

	OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());

	return OwnerMeshTemp->GetSocketLocation(THROW_START_SOCKET_NAME);
}

void AC_ThrowingWeapon::DrawDebugPredictedPath()
{
	static const float MaxSimTime	= 1.0f; // Max time to simulate
	static const float TimeStep		= 0.1f; // Time step for simulation
	FVector Gravity					= GetWorld()->GetGravityZ() * FVector::UpVector;

	TArray<FVector> PathPoints{};

	FVector CurrentLocation = GetPredictedThrowStartLocation();
	FVector CurrentVelocity = ProjLaunchVelocity;

	PathPoints.Add(CurrentLocation);

	for (float Time = 0; Time < MaxSimTime; Time += TimeStep)
	{
		FVector NextLocation = CurrentLocation + CurrentVelocity * TimeStep + 0.5f * Gravity * TimeStep * TimeStep;
		CurrentVelocity += Gravity * TimeStep;

		PathPoints.Add(NextLocation);
		CurrentLocation = NextLocation;

		if (PathPoints.Num() > 1)
		{
			DrawDebugLine
			(
				GetWorld(),
				PathPoints[PathPoints.Num() - 2], PathPoints[PathPoints.Num() - 1],
				FColor::Green,
				false,
				TimeStep * 2.0f,
				0,
				1.0f
			);
		}
	}
}

void AC_ThrowingWeapon::DrawPredictedPath()
{
	FPredictProjectilePathParams ProjectilePathParams{};
	ProjectilePathParams.StartLocation			= GetPredictedThrowStartLocation();
	ProjectilePathParams.LaunchVelocity			= ProjLaunchVelocity;
	ProjectilePathParams.bTraceWithCollision	= true;
	ProjectilePathParams.ProjectileRadius		= 5.f;
	ProjectilePathParams.MaxSimTime				= 1.f;
	ProjectilePathParams.bTraceWithChannel		= true;
	ProjectilePathParams.TraceChannel			= ECollisionChannel::ECC_Visibility;
	ProjectilePathParams.ObjectTypes			= {};
	ProjectilePathParams.ActorsToIgnore			= { this };
	ProjectilePathParams.SimFrequency			= 20.f;
	ProjectilePathParams.OverrideGravityZ		= ProjectileMovement->GetGravityZ();
	ProjectilePathParams.DrawDebugType			= EDrawDebugTrace::None;
	ProjectilePathParams.DrawDebugTime			= 1.f;
	ProjectilePathParams.bTraceComplex			= false;

	
	FPredictProjectilePathResult Result{};

	bool IsHit = UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, Result);

	ClearSpline();

	TArray<FPredictProjectilePathPointData> PathData = Result.PathData;

	if (IsHit)
	{
		PredictedEndPoint->SetWorldLocation(PathData.Last().Location);
		PredictedEndPoint->SetVisibility(true);
	}

	for (auto& pointData : PathData) 
		PathSpline->AddSplinePoint(pointData.Location, ESplineCoordinateSpace::Local);

	PathSpline->SetSplinePointType(PathData.Num() - 1, ESplinePointType::CurveClamped);
	
	for (int i = 0; i < PathSpline->GetNumberOfSplinePoints() - 2; i++)
	{
		FVector FirstLocation{};
		FVector SecondLocation{};
		FVector FirstTangent{};
		FVector SecondTangent{};

		PathSpline->GetLocationAndTangentAtSplinePoint(i, FirstLocation, FirstTangent, ESplineCoordinateSpace::Local);
		PathSpline->GetLocationAndTangentAtSplinePoint(i + 1, SecondLocation, SecondTangent, ESplineCoordinateSpace::Local);

		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
		SplineMeshComponent->SetStaticMesh(SplineMesh);
		SplineMeshComponent->SetStartAndEnd(FirstLocation, FirstTangent, SecondLocation, SecondTangent);
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
		//SplineMeshComponent->SetMaterial();
		
		SplineMeshes.Add(SplineMeshComponent);
	}
}

void AC_ThrowingWeapon::HandlePredictedPath()
{
	// Projectile Path
	// TODO : 플레이어일 경우에만 그리기 (추후, GameManager 멤버변수의 Player와 객체 대조해볼 것)

	// 현재 OwnerCharacter의 손에 장착된 상황인지 확인
	if (!IsValid(this->GetAttachParentActor()) || this->GetAttachParentSocketName() != EQUIPPED_SOCKET_NAME) return;

	if (!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurThrowProcessMontages.ThrowReadyMontage.AnimMontage))
		return;

	UpdateProjectileLaunchValues();
	DrawPredictedPath();
	//DrawDebugPredictedPath();
}

void AC_ThrowingWeapon::UpdateProjectileLaunchValues()
{
	// TODO : Init ProjStartLocation & ProjLaunchVelocity
	// TODO : Enemy AI의 경우 수류탄 던지는 방향을 다른 방법으로 정해줘야 함

	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	FVector Direction     = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X).GetSafeNormal();	
	ProjStartLocation     = OwnerCharacter->GetMesh()->GetSocketLocation(THROW_START_SOCKET_NAME);
	ProjLaunchVelocity    = Direction * Speed;
	ProjLaunchVelocity.Z += UP_DIR_BOOST_OFFSET;
}

void AC_ThrowingWeapon::ClearSpline()
{
	PredictedEndPoint->SetVisibility(false);

	for (auto& item : SplineMeshes) item->DestroyComponent();
	SplineMeshes.Empty();

	PathSpline->ClearSplinePoints();
}
