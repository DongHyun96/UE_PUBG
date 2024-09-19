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
	UC_Util::Print("NewBullet");
	if (!RootComponent)
		UC_Util::Print("NoRoot");
	BulletProjectileMovement->SetUpdatedComponent(RootComponent);
	BulletProjectileMovement->SetActive(false);
	BulletMesh = FindComponentByClass<UStaticMeshComponent>();
	SphereCollider = FindComponentByClass<USphereComponent>();
	
	if (IsValid(SphereCollider))
		UC_Util::Print("Found Collider");
	if (IsValid(BulletMesh))
	{
		BulletMesh->SetupAttachment(RootComponent);		
		UC_Util::Print("Found Mesh");
		
	}
}

void AC_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (BulletProjectileMovement->IsActive())
	{
		//FVector BulletSpeedCheck = BulletProjectileMovement->Velocity;
		//UC_Util::Print(BulletSpeedCheck, FColor::Green);

		//UC_Util::Print(BulletProjectileMovement->Velocity);

		float DragCoefficient = 0.3;
		FVector CurrentVelocity = BulletProjectileMovement->Velocity;
		FVector AirResistance = -DragCoefficient * CurrentVelocity;
		FVector NewVelocity = CurrentVelocity + AirResistance * DeltaTime;

		// 새 속도 적용
		BulletProjectileMovement->Velocity = NewVelocity;	
		BulletProjectileMovement->UpdateComponentVelocity();
		UC_Util::Print(float(BulletProjectileMovement->Velocity.Size()));
	}
	//UC_Util::Print(PrimaryActorTick.bCanEverTick);

	
	//UC_Util::Print(float(GetActorLocation().X));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.X));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.Y));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.Z));

}

bool AC_Bullet::Fire(AC_Gun* OwnerGun, FVector InLocation, FVector InDirection)
{
	

	if (!RootComponent)
	{
		UC_Util::Print("No Root!");
		return false;
	}
	if (BulletProjectileMovement)
	{
		if (BulletProjectileMovement->UpdatedComponent == NULL)
		{
			BulletProjectileMovement->SetUpdatedComponent(RootComponent);
		}
		SetActorHiddenInGame(false);
			//UC_Util::Print("FUCK");
		BulletProjectileMovement->SetActive(true, true);
		BulletProjectileMovement->SetComponentTickEnabled(true);
		//BulletProjectileMovement->SetUpdatedComponent(RootComponent);	
		// 
		//BulletProjectileMovement->Get
		float BulletSpeedCheck = BulletProjectileMovement->Velocity.Size();
		UC_Util::Print(BulletProjectileMovement->InitialSpeed);
		USkeletalMeshComponent* GunMesh = OwnerGun->GetGunMesh();

		SetActorLocation(InLocation);
		BulletProjectileMovement->Velocity = InDirection;
		BulletProjectileMovement->UpdateComponentVelocity();
		UC_Util::Print(BulletSpeedCheck, FColor::Blue);

		//Bullet->BulletProjectileMovement->SetActive(true);
		return true;
	}
	
	return false;
}

