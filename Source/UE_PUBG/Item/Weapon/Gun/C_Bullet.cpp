// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "NiagaraComponent.h"

#include "UMG.h"


#include "Character/C_BasicCharacter.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Utility/C_Util.h"

AC_Bullet::AC_Bullet()
{
	PrimaryActorTick.bCanEverTick = true;
	//WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");
	//if (!RootComponent)
	//{
	//	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//}
	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("BulletProjectileMovement");
	BulletProjectileMovement->MaxSpeed = 620 * 100.0f;
	BulletProjectileMovement->ProjectileGravityScale = 1.0;
	BulletProjectileMovement->OnProjectileStop.AddDynamic(this, &AC_Bullet::OnProjectileStop);
}



void AC_Bullet::BeginPlay()
{
	Super::BeginPlay();
	//UC_Util::Print("NewBullet");
	//if (!RootComponent)
	//	UC_Util::Print("NoRoot");
	BulletProjectileMovement->SetUpdatedComponent(RootComponent);
	BulletProjectileMovement->SetActive(false);
	//BulletMesh = FindComponentByClass<UStaticMeshComponent>();
	//InstancedBulletMesh = FindComponentByClass<UInstancedStaticMeshComponent>();
	SphereCollider = FindComponentByClass<USphereComponent>();
	
	//if (IsValid(SphereCollider))
	//	UC_Util::Print("Found Collider");
	if (IsValid(InstancedBulletMesh))
	{
		InstancedBulletMesh->SetupAttachment(RootComponent);
		//UC_Util::Print("Found Mesh");
		
	}
	InitializeItem(ItemCode);
	//DeactivateInstance();
		
}

void AC_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateTravelDistanceAndDeactivate(DeltaTime);

	///adsdasdsadsadsadw
}

void AC_Bullet::InitializeItem(FName NewItemCode)
{
	static const FString ContextString(TEXT("Sound Lookup"));
	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 BulletImpactSoundData 정의해 주기. -> FiredBullet(실제 총알)은 ItemManager에서 안 할 수도.
	UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_Bullet_Impact_Sound_Data.DT_Bullet_Impact_Sound_Data"));

	if (ItemDataTable)
	{
		const FBulletImpactSoundData* ItemData = ItemDataTable->FindRow<FBulletImpactSoundData>(ItemCode, ContextString);
		if (ItemData)
		{
			BulletImpactSoundData = ItemData;  // 원본 참조 저장
		}
	}
}

void AC_Bullet::DeactivateInstance()
{

	//FString Message = "Deactivated" + FString::FromInt(InstanceNum);
	//UC_Util::Print(Message);

	//InstancedBulletMesh->SetActive(false);
	//InstancedBulletMesh->SetComponentTickEnabled(false);
	//	InstancedBulletMesh->SetVisibility(false);
	
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	BulletProjectileMovement->Velocity = FVector(0);
	BulletProjectileMovement->SetActive(false);
	BulletProjectileMovement->SetComponentTickEnabled(false);

	SphereCollider->SetActive(false);
	SphereCollider->SetComponentTickEnabled(false);
	IsActive = false;
	UNiagaraComponent* BulletParticle = FindComponentByClass<UNiagaraComponent>();
	if (BulletParticle)
	{
		BulletParticle->Deactivate();
	}
}

void AC_Bullet::ActivateInstance()
{

	//InstancedBulletMesh->SetActive(true);
	//InstancedBulletMesh->SetVisibility(true);
	//InstancedBulletMesh->SetComponentTickEnabled(true);



	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	SphereCollider->SetActive(true);
	SphereCollider->SetComponentTickEnabled(true);

	BulletProjectileMovement->SetActive(true, true);
	BulletProjectileMovement->SetComponentTickEnabled(true);
	IsActive = true;
}

bool AC_Bullet::Fire(AC_Gun* InOwnerGun, FVector InLocation, FVector InDirection, bool EnableGravity, FVector InHitLocation)
{
	FiredGun = InOwnerGun;
	UNiagaraComponent* BulletParticle = FindComponentByClass<UNiagaraComponent>();
	if (BulletParticle)
	{
		// UC_Util::Print("Found Effect");
		BulletParticle->Deactivate();
	}
	if (EnableGravity)
		BulletProjectileMovement->ProjectileGravityScale = 1.0f;
	else
		BulletProjectileMovement->ProjectileGravityScale = 0;
	LineTraceHitLocation = InHitLocation;
	TestTickCount = 0;
	TestTimeCount = 0;
	InstanceLifeTime = 10.0f;
	if (!RootComponent)
	{
		//UC_Util::Print("No Root!");
		return false;
	}
	//SetActorHiddenInGame(false);
	if (BulletProjectileMovement)
	{
		FireLocation = InLocation;
		if (BulletProjectileMovement->UpdatedComponent == NULL)
		{
			BulletProjectileMovement->SetUpdatedComponent(RootComponent);
		}

		ActivateInstance();
		//UC_Util::Print(BulletProjectileMovement->InitialSpeed);
		USkeletalMeshComponent* GunMesh = InOwnerGun->GetGunMesh();

		SetActorLocation(InLocation);
		BulletProjectileMovement->Velocity = InDirection;
		//BulletProjectileMovement->UpdateComponentVelocity();
		float BulletSpeedCheck = BulletProjectileMovement->Velocity.Size();
		//UC_Util::Print(BulletSpeedCheck, FColor::Blue);

		//Bullet->BulletProjectileMovement->SetActive(true);
		InitialVelocityNormalized = InDirection.GetSafeNormal();
		
		if (BulletParticle)
		{
			// UC_Util::Print("Found Effect");
			BulletParticle->Activate();
		}
		return true;
	}
	else
	{
		//UC_Util::Print("No BulletProjectileMovement");
	}
	
	return false;
}

void AC_Bullet::SubSteppingMovementPhysics(float SebStepDeltaTime)
{
	
	float Speed = BulletProjectileMovement->Velocity.Size() * 0.01f;


	float Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(Speed);

	FVector Gravity = FVector(0.0f, 0.0f, -9.81f); // 중력 가속도 (Z축 방향)
	// 1. k1 계산
	//UC_Util::Print(Drag_Force);
	FVector k1_Velocity = BulletProjectileMovement->Velocity * 0.01f;
	FVector k1_Acceleration = -k1_Velocity * Drag_Force / Bullet_Mass;// + Gravity;

	// 2. k2 계산 (절반 시간 스텝)
	FVector k2_Velocity = BulletProjectileMovement->Velocity * 0.01f + k1_Acceleration * (SebStepDeltaTime * 0.5f);
	float k2_Speed = k2_Velocity.Size() / 100.0f;
	float k2_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k2_Speed);
	FVector k2_Acceleration = -k2_Velocity * k2_Drag_Force / Bullet_Mass;// + Gravity;

	// 3. k3 계산 (다시 절반 시간 스텝)
	FVector k3_Velocity = BulletProjectileMovement->Velocity * 0.01f + k2_Acceleration * (SebStepDeltaTime * 0.5f);
	float k3_Speed = k3_Velocity.Size() / 100.0f;
	float k3_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k3_Speed);
	FVector k3_Acceleration = -k3_Velocity * k3_Drag_Force / Bullet_Mass;// + Gravity;

	// 4. k4 계산 (전체 시간 스텝)
	FVector k4_Velocity = BulletProjectileMovement->Velocity * 0.01f + k3_Acceleration * SebStepDeltaTime;
	float k4_Speed = k4_Velocity.Size() / 100.0f;
	float k4_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k4_Speed);
	FVector k4_Acceleration = -k4_Velocity * k4_Drag_Force / Bullet_Mass;//+Gravity;

	// 최종 속도 업데이트 (룽게-쿠타 공식에 따른 평균 계산)
	FVector NewVelocity = BulletProjectileMovement->Velocity + (SebStepDeltaTime / 6.0f) * (k1_Acceleration + 2.0f * k2_Acceleration + 2.0f * k3_Acceleration + k4_Acceleration) * 100.0f;

	// 최종 속도 적용
	
	BulletProjectileMovement->Velocity = NewVelocity;	

	BulletProjectileMovement->UpdateComponentVelocity();



	
}

void AC_Bullet::CustomPhysics(float DeltaTime)
{
	float FixedDeltaTime = 0.005f;

	int32 NumSteps = FMath::CeilToInt(DeltaTime / FixedDeltaTime);
	for (int32 i = 0; i < NumSteps; ++i)
	{
		// 남은 DeltaTime에서 서브스텝 시간 구하기
		float SubstepTime = FMath::Min(FixedDeltaTime, DeltaTime - i * FixedDeltaTime);

		// 공기 저항 등 물리 계산 적용
		SubSteppingMovementPhysics(SubstepTime);
	}
}

void AC_Bullet::CalculateTravelDistanceAndDeactivate(float DeltaTime)
{


	if (!IsValid(BulletProjectileMovement))
		return;
	IsActive = BulletProjectileMovement->IsActive();
	InstanceLifeTime -= DeltaTime;
	if (InstanceLifeTime <= 0)
	{
		InstanceLifeTime = 0;
		DeactivateInstance();
	}
	if (BulletProjectileMovement->IsActive())
	{
		CustomPhysics(DeltaTime);
		//UC_Util::Print(BulletProjectileMovement->Velocity.Size());
		float TravelDistance = (GetActorLocation() - FireLocation).Size();
		if (TravelDistance > 100000.0f  || GetActorLocation().Z < -900)
		{
			DeactivateInstance();
		}
	}
}

void AC_Bullet::OnProjectileStop(const FHitResult& ImpactResult)
{
	AC_BasicCharacter* HittedCharacter = Cast<AC_BasicCharacter>(ImpactResult.GetActor());
	// 피격판정된 Actor가 캐릭터가 아닐 때

	PlaySound(ImpactResult);

	if (!HittedCharacter) return;

	FName HittedBoneName = ImpactResult.BoneName;

	float DamageRate 	= FiredGun->GetDamageRateByBodyPart(HittedBoneName);
	float DamageBase 	= FiredGun->GetDamageBase();
	float TotalDamage	= DamageRate * DamageBase;

	float DamageDistance = FVector::Distance(HittedCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation()); 
	DamageDistance *= 0.01f;
	
	FKillFeedDescriptor KillFeedDesc =
	{
		EDamageType::Weapon,
		OwnerCharacter,
		HittedCharacter,
		FiredGun,
		false, // HeadShot 여부는 TakeDamage 내에서 검사해서 조정
		static_cast<int>(DamageDistance)
	};

	if (HittedCharacter->GetStatComponent()->TakeDamage(TotalDamage, HittedBoneName, KillFeedDesc) != 0.f)
		HittedCharacter->ActivateBloodParticle(ImpactResult.Location);
}

void AC_Bullet::PlaySound(const FHitResult& ImpactResult)
{

	// UC_Util::Print(ImpactResult.GetActor());

	//if (!ImpactResult.PhysMaterial.IsValid())
	//{
	//	UC_Util::Print("PhysMaterial is NULL!");
	//	return;
	//}

	// 충돌이 실제로 발생했는지 확인
	if (!ImpactResult.bBlockingHit)
	{
		// UC_Util::Print("ImpactResult is not a blocking hit!");
		return;
	}

	EPhysicalSurface SurfaceType = SurfaceType_Default;

	// ImpactResult.PhysMaterial이 있는지 확인
	if (!ImpactResult.PhysMaterial.IsValid())
	{
		// UC_Util::Print("PhysMaterial is NULL! Trying to fetch manually...");

		UPhysicalMaterial* PhysMaterial = nullptr;
		if (ImpactResult.Component.IsValid())
		{
			UMaterialInterface* Material = ImpactResult.Component->GetMaterial(0);
			if (Material)
			{
				PhysMaterial = Material->GetPhysicalMaterial();
			}
		}

		SurfaceType = (PhysMaterial) ? static_cast<EPhysicalSurface>(PhysMaterial->SurfaceType) : SurfaceType_Default;
		// UC_Util::Print("SurfaceType (Manual Fetch): " + FString::FromInt(SurfaceType));
	}
	else
	{
		SurfaceType = UGameplayStatics::GetSurfaceType(ImpactResult);
		// UC_Util::Print("SurfaceType: " + FString::FromInt(SurfaceType));
	}

	if (!BulletImpactSoundData) 
	{
		// UC_Util::Print("BulletImpactSoundData Is Nullptr!");
		return; 
	}

	int32 RandomIndex = 0;

	//UC_Util::Print("PlaySound");
	//UC_Util::Print(SurfaceType);

	switch (SurfaceType)
	{
	case SurfaceType_Default:
		// UC_Util::Print("PlaySound_Default, Play Sound Ground");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactGround);
		break;
	case SurfaceType1: // Body
		// UC_Util::Print("PlaySound_Body");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactBody);
		break;
	case SurfaceType2: //Helmet
		// UC_Util::Print("PlaySound_Helmet");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactBody);
		break;
	case SurfaceType3: // Vest
		// UC_Util::Print("PlaySound_Vest");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactBody);
		break;
	case SurfaceType4: // Wood
		// UC_Util::Print("PlaySound_Wood");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactWood);
		break;
	case SurfaceType5: // Concrete
		// UC_Util::Print("PlaySound_Concrete");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactConcrete);
		break;
	case SurfaceType6: // Metal
		// UC_Util::Print("PlaySound_Metal");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactMetal);
		break;
	case SurfaceType7: // Ground
		// UC_Util::Print("PlaySound_Ground");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactGround);
		break;
	case SurfaceType8: // Fly
		// UC_Util::Print("PlaySound_Fly");
		PlayRandomSoundFromArray(BulletImpactSoundData->ImpactFly);
		break;
	default:
		break;
	}
}

void AC_Bullet::PlayRandomSoundFromArray(const TArray<USoundBase*>& Sounds)
{
	if (Sounds.Num() == 0) return;

	int32 RandomIndex = FMath::RandRange(0, Sounds.Num() - 1);
	if (USoundBase* SelectedSound = Sounds[RandomIndex])
	{
		UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, GetActorLocation());
	}
}
