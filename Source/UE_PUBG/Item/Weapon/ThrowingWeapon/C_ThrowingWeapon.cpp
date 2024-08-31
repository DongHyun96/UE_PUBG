   // Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

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
	SetRootComponent(Collider);

	PathSpline = CreateDefaultSubobject<USplineComponent>("PredictedPathSpline");
	PredictedEndPoint = CreateDefaultSubobject<UStaticMeshComponent>("PredictedPathEndPointMesh");
	PredictedEndPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PredictedEndPoint->SetVisibility(false);

	//ItemType ����.
	MyItemType = EItemTypes::THROWABLE;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>("ExplosionSphere");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();

	// Spawn actor�� ������ �������� OwnerCharacter�� Setting�ϴ����̶� ������ �ȸ���
	//if (OwnerCharacter)
	//	OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(this, &AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin);
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
	// TODO : ��ô���� �ɱ����� �̾Ұ� ��ŷ�� ������ �� �ٽ� �������
	// TODO : ��ô���� ���������̱��� �̾Ҵٸ� ���� ��ġ�� ���� ��ô�� �׳� �ٴڿ� ����

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

	UC_Util::Print("Left Grenade Count : " + FString::FromInt(ThrowablePool.Num() + 1));

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

	// �賶�� �ִ� ��ó�� ����
	for (int i = 0; i < TESTPOOLCNT; i++)
	{
		FActorSpawnParameters Param{};
		Param.Owner = InOwnerCharacter;

		AC_ThrowingWeapon* ThrowWeapon = EquippedComponent->GetWorld()->SpawnActor<AC_ThrowingWeapon>(Class, Param);

		if (ThrowWeapon)
		{
			ThrowWeapon->SetOwnerCharacter(InOwnerCharacter);
			//ThrowWeapon->OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(ThrowWeapon, &AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin);
			ThrowablePool.Add(ThrowWeapon);
		}
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
		// ���� ������ ���� ����
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
		ClearSpline();
	}
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Direction ���ϴ� ��� 1
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
	//UC_Util::Print("OnThrowProcessEnd", FColor::Cyan, 5.f);

	bIsOnThrowProcess = false;

	bIsCharging = false;

	// Ready ���� ProcessEnd�� �� �� �ֱ� ������ Predicted Path spline ��� �����ֱ�
	ClearSpline();

	// ���� Throw AnimMontage ���߱� (�켱���� ������ ����� ��)
	//OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowMontage.AnimMontage);
	for (uint8 p = 0; p < static_cast<uint8>(EPoseState::POSE_MAX); p++)
	{
		EPoseState		PoseState			= static_cast<EPoseState>(p);
		UAnimMontage*	ThrowMontage		= ThrowProcessMontages[PoseState].ThrowMontage.AnimMontage;
		UAnimInstance*	OwnerAnimInstance	= OwnerCharacter->GetMesh()->GetAnimInstance();

		if (OwnerAnimInstance->Montage_IsPlaying(ThrowMontage))
		{
			OwnerAnimInstance->Montage_Stop(0.2f, ThrowMontage);
			break;
		}
	}

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// ���� ��ô�� ���� ���� �ٷ� �ϱ� -> �̹� ���� ���⿡ ���� PoseTransitionEnd Delegate ���� �̷����
	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// TODO : ���濡 ��ô�� �ִ��� Ȯ���ؼ� �ٷ� ���� ���� �����ϱ� (�켱���� - ���� ������ ��ô��)
	if (!ThrowablePool.IsEmpty())
	{
		AC_ThrowingWeapon* ThrowWeapon = ThrowablePool[0];

		// ���� slot�� �����ϸ鼭 ���ο� ��ô�� �Լ��� Delegate ��� �̷����
		OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowWeapon);

		// ���濡 �ִ� ��ô�� �ϳ� �����
		// TODO : ���������� ���濡�� �����
		ThrowablePool.RemoveAt(0);

		// �ٷ� ���� ��ô�� ������
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		OwnerCharacter->PlayAnimMontage(ThrowWeapon->GetCurDrawMontage());

		return;
	}

	//���� �ִ� ��ô���� ���ٸ� �ֹ���1, �ֹ���2 ������ ���� �õ�, ���ٸ� UnArmed�� ���·� ���ư���
	// TODO : MeleeWeapon �� �����
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MELEE_WEAPON)) return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN))		return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN))		return;

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

	// TODO : Ready ���¿��� Cooking �����ϸ� ���� �ð� HUD ����
	//UC_Util::Print("Throwable Starts cooking");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking�� �ȵǾ��ٸ� (���������̰� ���ư��� �ʾҴٸ�)
	if (ProjectileMovement->IsActive()) return false;	// �̹� �������ٸ�

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f�� �ӷ����� ������
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

	if (GetAttachParentActor()) ReleaseOnGround(); // �տ��� ���� ������ �ʾ��� ��
		
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
			UC_Util::Print("From AC_ThrowingWeapon::GetPredictedThrowStartLocation : OwnerTempMesh Not inited!");
			return FVector::ZeroVector;
		}

		OwnerMeshTemp->SetVisibility(false);
		OwnerMeshTemp->RegisterComponent();
		OwnerMeshTemp->SetSkeletalMesh(OwnerCharacter->GetMesh()->SkeletalMesh);
		OwnerMeshTemp->SetAnimInstanceClass(OwnerCharacter->GetMesh()->GetAnimInstance()->GetClass());
		OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());
	}

	// �ڼ��� �´� Montage�� ��������� �ʴٸ�, �ش� Montage�� ���� �ڿ� ���߱�
	static EPoseState PredictedPoseState = EPoseState::POSE_MAX;

	UAnimInstance* MeshAnimInstance = OwnerMeshTemp->GetAnimInstance();

	// ���� posing�� �ٸ��ٸ� ���� ��� �������� �޶���
	
	float ThrowThrowableTime{};

	for (const FAnimNotifyEvent& NotifyEvent : CurThrowProcessMontages.ThrowMontage.AnimMontage->Notifies)
	{
		if (NotifyEvent.NotifyName == "AN_OnThrowThrowable_C")
		{
			ThrowThrowableTime = NotifyEvent.GetTime();
			break;
		}
	}

	if (PredictedPoseState != OwnerCharacter->GetPoseState() ||
		MeshAnimInstance->Montage_GetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage) != ThrowThrowableTime)
	{
		PredictedPoseState = OwnerCharacter->GetPoseState();
		
		MeshAnimInstance->Montage_Play(CurThrowProcessMontages.ThrowMontage.AnimMontage);
		MeshAnimInstance->Montage_SetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage, ThrowThrowableTime);
		MeshAnimInstance->Montage_Pause();

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
	// �÷��̾��� ��쿡�� �׸��� (����, GameManager ��������� Player�� ��ü �����غ� ��)
	AC_Player* Player = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(Player)) return;

	// ���� OwnerCharacter�� �տ� ������ ��Ȳ���� Ȯ��
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
	// TODO : Enemy AI�� ��� ����ź ������ ������ �ٸ� ������� ������� ��

	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	FVector Direction     = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X).GetSafeNormal();	
	ProjStartLocation     = OwnerCharacter->GetMesh()->GetSocketLocation(THROW_START_SOCKET_NAME);
	ProjLaunchVelocity    = Direction * Speed;
	ProjLaunchVelocity.Z += UP_DIR_BOOST_OFFSET;
}

void AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin()
{
	if (bIsCharging)
	{
		// ���ο� �ڼ��� �´� Anim Montage�� �ٽ� ���
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
		return;
	}


	//CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()];

	//// ThrowProcessEnd ���� OnDrawEnd�� ����� ȣ���� �ȵǾ��� ���
	//// ���ǽ��� �Ȱɸ� ���� ����
	//if (
	//	OwnerCharacter->GetHandState() == EHandState::WEAPON_THROWABLE &&
	//	OwnerCharacter->GetEquippedComponent()->GetNextWeaponType() != EWeaponSlot::NONE //&&
	//	//!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurDrawMontage.AnimMontage)
	//	)
	//{
	//	UC_Util::Print("Call OnDrawEnd Manually", FColor::Cyan, 5.f);
	//	OwnerCharacter->GetEquippedComponent()->OnDrawEnd();
	//}

}

void AC_ThrowingWeapon::ClearSpline()
{
	PredictedEndPoint->SetVisibility(false);

	for (auto& item : SplineMeshes) item->DestroyComponent();
	SplineMeshes.Empty();

	PathSpline->ClearSplinePoints();
}
