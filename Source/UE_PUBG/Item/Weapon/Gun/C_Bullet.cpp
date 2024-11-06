// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_Bullet.h"
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
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/CanvasPanelSlot.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/C_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"


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
	InstancedBulletMesh = FindComponentByClass<UInstancedStaticMeshComponent>();
	SphereCollider = FindComponentByClass<USphereComponent>();
	
	//if (IsValid(SphereCollider))
	//	UC_Util::Print("Found Collider");
	if (IsValid(InstancedBulletMesh))
	{
		InstancedBulletMesh->SetupAttachment(RootComponent);
		//UC_Util::Print("Found Mesh");
		
	}
	//DeactivateInstance();
		
}

void AC_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateTravelDistanceAndDeactivate(DeltaTime);


}

void AC_Bullet::DeactivateInstance()
{

	//FString Message = "Deactivated" + FString::FromInt(InstanceNum);
	//UC_Util::Print(Message);

	InstancedBulletMesh->SetActive(false);
	InstancedBulletMesh->SetComponentTickEnabled(false);
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

}

void AC_Bullet::ActivateInstance()
{

	InstancedBulletMesh->SetActive(true);
	InstancedBulletMesh->SetVisibility(true);
	InstancedBulletMesh->SetComponentTickEnabled(true);



	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	SphereCollider->SetActive(true);
	SphereCollider->SetComponentTickEnabled(true);

	BulletProjectileMovement->SetActive(true, true);
	BulletProjectileMovement->SetComponentTickEnabled(true);
	IsActive = true;
}

bool AC_Bullet::Fire(AC_Gun* OwnerGun, FVector InLocation, FVector InDirection, bool EnableGravity, FVector InHitLocation)
{
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
		UC_Util::Print("No Root!");
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
		USkeletalMeshComponent* GunMesh = OwnerGun->GetGunMesh();

		SetActorLocation(InLocation);
		BulletProjectileMovement->Velocity = InDirection;
		//BulletProjectileMovement->UpdateComponentVelocity();
		float BulletSpeedCheck = BulletProjectileMovement->Velocity.Size();
		//UC_Util::Print(BulletSpeedCheck, FColor::Blue);

		//Bullet->BulletProjectileMovement->SetActive(true);
		InitialVelocityNormalized = InDirection.GetSafeNormal();
		return true;
	}
	else
	{
		UC_Util::Print("No BulletProjectileMovement");
	}
	
	return false;
}

void AC_Bullet::SubSteppingMovementPhysics(float SebStepDeltaTime)
{
	
	float Speed = BulletProjectileMovement->Velocity.Size() * 0.01f;


	float Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(Speed);

	FVector Gravity = FVector(0.0f, 0.0f, -9.81f); // �߷� ���ӵ� (Z�� ����)
	// 1. k1 ���
	//UC_Util::Print(Drag_Force);
	FVector k1_Velocity = BulletProjectileMovement->Velocity * 0.01f;
	FVector k1_Acceleration = -k1_Velocity * Drag_Force / Bullet_Mass;// + Gravity;

	// 2. k2 ��� (���� �ð� ����)
	FVector k2_Velocity = BulletProjectileMovement->Velocity * 0.01f + k1_Acceleration * (SebStepDeltaTime * 0.5f);
	float k2_Speed = k2_Velocity.Size() / 100.0f;
	float k2_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k2_Speed);
	FVector k2_Acceleration = -k2_Velocity * k2_Drag_Force / Bullet_Mass;// + Gravity;

	// 3. k3 ��� (�ٽ� ���� �ð� ����)
	FVector k3_Velocity = BulletProjectileMovement->Velocity * 0.01f + k2_Acceleration * (SebStepDeltaTime * 0.5f);
	float k3_Speed = k3_Velocity.Size() / 100.0f;
	float k3_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k3_Speed);
	FVector k3_Acceleration = -k3_Velocity * k3_Drag_Force / Bullet_Mass;// + Gravity;

	// 4. k4 ��� (��ü �ð� ����)
	FVector k4_Velocity = BulletProjectileMovement->Velocity * 0.01f + k3_Acceleration * SebStepDeltaTime;
	float k4_Speed = k4_Velocity.Size() / 100.0f;
	float k4_Drag_Force = 0.5 * Drag_Coefficient * Air_Density * Cross_Sectional_Area * FMath::Square(k4_Speed);
	FVector k4_Acceleration = -k4_Velocity * k4_Drag_Force / Bullet_Mass;//+Gravity;

	// ���� �ӵ� ������Ʈ (���-��Ÿ ���Ŀ� ���� ��� ���)
	FVector NewVelocity = BulletProjectileMovement->Velocity + (SebStepDeltaTime / 6.0f) * (k1_Acceleration + 2.0f * k2_Acceleration + 2.0f * k3_Acceleration + k4_Acceleration) * 100.0f;

	// ���� �ӵ� ����
	
	BulletProjectileMovement->Velocity = NewVelocity;	

	BulletProjectileMovement->UpdateComponentVelocity();



	
}

void AC_Bullet::CustomPhysics(float DeltaTime)
{
	float FixedDeltaTime = 0.005f;

	int32 NumSteps = FMath::CeilToInt(DeltaTime / FixedDeltaTime);
	for (int32 i = 0; i < NumSteps; ++i)
	{
		// ���� DeltaTime���� ���꽺�� �ð� ���ϱ�
		float SubstepTime = FMath::Min(FixedDeltaTime, DeltaTime - i * FixedDeltaTime);

		// ���� ���� �� ���� ��� ����
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
		if (TravelDistance > 1000000.0f || GetActorLocation().Z < -900)
		{
			DeactivateInstance();
		}
	}
}

